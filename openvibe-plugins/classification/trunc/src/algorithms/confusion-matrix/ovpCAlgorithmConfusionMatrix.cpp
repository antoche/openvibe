#include "ovpCAlgorithmConfusionMatrix.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Classification;
using namespace std;

#define boolean OpenViBE::boolean
#define uint64 OpenViBE::uint64

boolean CAlgorithmConfusionMatrix::initialize(void)
{
	ip_pTargetStimulationSet.initialize(getInputParameter(OVP_Algorithm_ConfusionMatrixAlgorithm_InputParameterId_TargetStimulationSet));
	ip_pClassifierStimulationSet.initialize(getInputParameter(OVP_Algorithm_ConfusionMatrixAlgorithm_InputParameterId_ClassifierStimulationSet));
	ip_pClassesCodes.initialize(getInputParameter(OVP_Algorithm_ConfusionMatrixAlgorithm_InputParameterId_ClassCodes));
	ip_bPercentages.initialize(getInputParameter(OVP_Algorithm_ConfusionMatrixAlgorithm_InputParameterId_Percentage));
	ip_bSums.initialize(getInputParameter(OVP_Algorithm_ConfusionMatrixAlgorithm_InputParameterId_Sums));
	op_pConfusionMatrix.initialize(getOutputParameter(OVP_Algorithm_ConfusionMatrixAlgorithm_OutputParameterId_ConfusionMatrix));

	return true;
}

boolean CAlgorithmConfusionMatrix::uninitialize(void)
{
	ip_pTargetStimulationSet.uninitialize();
	ip_pClassifierStimulationSet.uninitialize();
	ip_pClassesCodes.uninitialize();
	ip_bPercentages.uninitialize();
	ip_bSums.uninitialize();
	op_pConfusionMatrix.uninitialize();

	return true;
}

boolean CAlgorithmConfusionMatrix::process(void)
{
	uint64 l_ui64ClassCount = ip_pClassesCodes->getStimulationCount();

	if(this->isInputTriggerActive(OVP_Algorithm_ConfusionMatrixAlgorithm_InputTriggerId_ResetTarget))
	{
		for(uint32 i = 0 ; i<ip_pClassesCodes->getStimulationCount(); i++)
		{
			this->getLogManager() << LogLevel_Trace << "class code " << i << ": " << ip_pClassesCodes->getStimulationIdentifier(i) << "\n";
		}

		m_mapClassificationAttemptCountPerClass.clear();
		for(uint32 i = 0;i<ip_pClassesCodes->getStimulationCount();i++)
		{
			m_mapClassificationAttemptCountPerClass.insert(make_pair(ip_pClassesCodes->getStimulationIdentifier(i),0));
		}

		op_pConfusionMatrix->setDimensionCount(2);
		if(ip_bSums)
		{
			op_pConfusionMatrix->setDimensionSize(0,l_ui64ClassCount+1);
			op_pConfusionMatrix->setDimensionSize(1,l_ui64ClassCount+1);
		}
		else
		{
			op_pConfusionMatrix->setDimensionSize(0,l_ui64ClassCount);
			op_pConfusionMatrix->setDimensionSize(1,l_ui64ClassCount);
		}

		for(uint32 i = 0 ; i<l_ui64ClassCount; i++)
		{
			stringstream ss;
			//ss << "Target\nClass " << i+1;
			ss << "Target Class\n" << this->getTypeManager().getEnumerationEntryNameFromValue(OV_TypeId_Stimulation,ip_pClassesCodes->getStimulationIdentifier(i));
			op_pConfusionMatrix->setDimensionLabel(0,i,ss.str().c_str());
			stringstream ss_;
			//ss_ << "Result\nClass "<< i+1;
			ss_ << "Result Class\n" << this->getTypeManager().getEnumerationEntryNameFromValue(OV_TypeId_Stimulation,ip_pClassesCodes->getStimulationIdentifier(i));
			op_pConfusionMatrix->setDimensionLabel(1,i,ss_.str().c_str());
		}

		if(ip_bSums)
		{
			op_pConfusionMatrix->setDimensionLabel(0, l_ui64ClassCount, "Sums");
			op_pConfusionMatrix->setDimensionLabel(1, l_ui64ClassCount, "Sums");
		}

		m_oConfusionMatrix.setDimensionCount(2);
		m_oConfusionMatrix.setDimensionSize(0, l_ui64ClassCount);
		m_oConfusionMatrix.setDimensionSize(1, l_ui64ClassCount);

		// initialization
		for(uint32 i = 0; i<op_pConfusionMatrix->getDimensionSize(0); i++)
		{
			for(uint32 j = 0; j<op_pConfusionMatrix->getDimensionSize(1); j++)
			{
				op_pConfusionMatrix->getBuffer()[i*op_pConfusionMatrix->getDimensionSize(1)+j] = 0.f;
				if( i < m_oConfusionMatrix.getDimensionSize(0) && j < m_oConfusionMatrix.getDimensionSize(1))
				{
					m_oConfusionMatrix.getBuffer()[i*m_oConfusionMatrix.getDimensionSize(1)+j] = 0.f;
				}
			}
		}
	}

	if(this->isInputTriggerActive(OVP_Algorithm_ConfusionMatrixAlgorithm_InputTriggerId_ResetClassifier))
	{
	}

	if(this->isInputTriggerActive(OVP_Algorithm_ConfusionMatrixAlgorithm_InputTriggerId_FeedTarget))
	{
		for(uint32 s=0; s<ip_pTargetStimulationSet->getStimulationCount(); s++)
		{
			uint64 l_ui64StimulationIdentifier = ip_pTargetStimulationSet->getStimulationIdentifier(s);
			if(isClass(l_ui64StimulationIdentifier))
			{
				uint64 l_ui64StimulationDate = ip_pTargetStimulationSet->getStimulationDate(s);
				m_mTargetsTimeLine.insert(std::pair<uint64,uint64>(l_ui64StimulationDate,l_ui64StimulationIdentifier));
				getLogManager() << LogLevel_Trace << "Current target is " << m_mTargetsTimeLine.rbegin()->second << "\n";
			}
			else
			{
				getLogManager() << LogLevel_Warning << "The target received is not a valid class: " << l_ui64StimulationIdentifier << "\n";
			}
		}
	}

	if(this->isInputTriggerActive(OVP_Algorithm_ConfusionMatrixAlgorithm_InputTriggerId_FeedClassifier))
	{
		for(uint32 s=0; s<ip_pClassifierStimulationSet->getStimulationCount(); s++)
		{
			//We need to locate the stimulation on the timeline
			uint64 l_ui64StimulationFromClassifierIdentifier = ip_pClassifierStimulationSet->getStimulationIdentifier(s);
			uint64 l_ui64StimulationTargeted = 0;
			uint64 l_ui64StimulationFromClassifierDate = ip_pClassifierStimulationSet->getStimulationDate(s);

			std::map<uint64,uint64>::iterator l_itTarget = m_mTargetsTimeLine.begin();
			std::map<uint64,uint64>::iterator l_itNextTarget;
			boolean l_bFound = false;
			for(;l_itTarget != m_mTargetsTimeLine.end() && !l_bFound;l_itTarget++)
			{
				l_itNextTarget = l_itTarget;
				l_itNextTarget++;
				if( (l_itNextTarget == m_mTargetsTimeLine.end() || l_ui64StimulationFromClassifierDate < l_itNextTarget->first)
					&& l_ui64StimulationFromClassifierDate > l_itTarget->first)
				{
					l_ui64StimulationTargeted = l_itTarget->second;
					l_bFound = true;
				}
			}
			if(l_bFound)
			{
				// now we found the target, let's update the confusion matrix
				// we need to update the whole line vector for the targeted class
				if(isClass(l_ui64StimulationFromClassifierIdentifier))
				{
					uint32 l_ui32OldAttemptCount = m_mapClassificationAttemptCountPerClass[l_ui64StimulationTargeted];
					m_mapClassificationAttemptCountPerClass[l_ui64StimulationTargeted]++; // the confusion matrix can treat this result

					uint32 i = getClassIndex(l_ui64StimulationTargeted);// the good line index
					uint32 l_ui32ResultIndex = getClassIndex(l_ui64StimulationFromClassifierIdentifier);
					for(uint32 j=0;j<l_ui64ClassCount;j++)
					{
						float64 l_f64NewValue = 0.f;
						float64 l_f64OldValue = op_pConfusionMatrix->getBuffer()[i*op_pConfusionMatrix->getDimensionSize(0)+j];
						if(j == l_ui32ResultIndex)
						{
							l_f64NewValue = (l_f64OldValue*l_ui32OldAttemptCount +1) / (m_mapClassificationAttemptCountPerClass[l_ui64StimulationTargeted]);
							m_oConfusionMatrix.getBuffer()[i*l_ui64ClassCount+j]++;
						}
						else
						{
							l_f64NewValue = (l_f64OldValue*l_ui32OldAttemptCount) / (m_mapClassificationAttemptCountPerClass[l_ui64StimulationTargeted]);
						}
						if(ip_bPercentages)
						{
							op_pConfusionMatrix->getBuffer()[i*op_pConfusionMatrix->getDimensionSize(0)+j] = l_f64NewValue;
						}
						else // the count value
						{
							op_pConfusionMatrix->getBuffer()[i*op_pConfusionMatrix->getDimensionSize(0)+j] = m_oConfusionMatrix.getBuffer()[i*l_ui64ClassCount+j];
						}
					}

					//we compute the sums if needed
					if(ip_bSums)
					{
						uint64 l_ui64Size = op_pConfusionMatrix->getDimensionSize(0);
						float64 l_f64Total = 0.f;
						for(uint32 i = 0; i<l_ui64ClassCount; i++)
						{
							float64 l_f64LineSum = 0.f;
							float64 l_f64ColumnSum = 0.f;
							for(uint32 j = 0; j<l_ui64ClassCount; j++)
							{
								l_f64LineSum += op_pConfusionMatrix->getBuffer()[i*l_ui64Size+j];
								l_f64ColumnSum += op_pConfusionMatrix->getBuffer()[j*l_ui64Size+i];
							}
							op_pConfusionMatrix->getBuffer()[i*l_ui64Size+l_ui64Size-1] = l_f64LineSum;
							op_pConfusionMatrix->getBuffer()[(l_ui64Size-1)*l_ui64Size+i] = l_f64ColumnSum;
							l_f64Total+=l_f64LineSum;
						}
						op_pConfusionMatrix->getBuffer()[(l_ui64Size-1)*l_ui64Size + l_ui64Size -1] = l_f64Total; // the lower-right entry, i.e. the last in the buffer
					}
				}
				else
				{
					getLogManager() << LogLevel_Warning << "The classification result received is not a valid class: " << l_ui64StimulationFromClassifierIdentifier << "\n";
				}
			}
			else
			{
				getLogManager() << LogLevel_Warning << " No target available.\n";
			}
		}
		this->activateOutputTrigger(OVP_Algorithm_ConfusionMatrixAlgorithm_OutputTriggerId_ConfusionPerformed, true);
	}

	return true;
}

boolean CAlgorithmConfusionMatrix::isClass(uint64 StimulationIdentifier)
{
	for(uint32 i = 0; i<ip_pClassesCodes->getStimulationCount();i++)
	{
		if(ip_pClassesCodes->getStimulationIdentifier(i) == StimulationIdentifier)
		{
			return true;
		}
	}
	return false;
}

uint32 CAlgorithmConfusionMatrix::getClassIndex(uint64 StimulationIdentifier)
{
	for(uint32 i = 0; i<ip_pClassesCodes->getStimulationCount();i++)
	{
		if(ip_pClassesCodes->getStimulationIdentifier(i) == StimulationIdentifier)
		{
			return i;
		}
	}
	return -1;
}
