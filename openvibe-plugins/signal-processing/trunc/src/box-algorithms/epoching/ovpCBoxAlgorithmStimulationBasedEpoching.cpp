#include "ovpCBoxAlgorithmStimulationBasedEpoching.h"
#include <cstdio>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessing;

using namespace std;

boolean CBoxAlgorithmStimulationBasedEpoching::initialize(void)
{
	// IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

	m_pStimulationStreamDecoder=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamDecoder));
	m_pStimulationStreamEncoder=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamEncoder));
	m_pSignalStreamDecoder=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamDecoder));
	m_pSignalStreamEncoder=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamEncoder));

	m_pStimulationStreamDecoder->initialize();
	m_pStimulationStreamEncoder->initialize();
	m_pSignalStreamDecoder->initialize();
	m_pSignalStreamEncoder->initialize();

	CString l_sSettingValue;

	float64 l_f64EpochDuration;
	getStaticBoxContext().getSettingValue(0, l_sSettingValue);
	if(sscanf(l_sSettingValue.toASCIIString(), "%lf", &l_f64EpochDuration)==0)
	{
		getLogManager() << LogLevel_Error << "Epoch duration could not be parsed as float64\n";
		return false;
	}
	getLogManager() << LogLevel_Debug << "Epoch duration : " << l_f64EpochDuration << "\n";
	m_ui64EpochDuration=(int64)(l_f64EpochDuration*(1LL<<32)); // $$$ Casted in (int64) because of Ubuntu 7.10 crash !

	float64 l_f64EpochOffset;
	getStaticBoxContext().getSettingValue(1, l_sSettingValue);
	if(sscanf(l_sSettingValue.toASCIIString(), "%lf", &l_f64EpochOffset)==0)
	{
		getLogManager() << LogLevel_Error << "Epoch offset could not be parsed as float64\n";
		return false;
	}
	getLogManager() << LogLevel_Debug << "Epoch offset : " << l_f64EpochOffset << "\n";
	m_i64EpochOffset=(int64)(l_f64EpochOffset*(1LL<<32));

	for(uint32 i=2; i<getStaticBoxContext().getSettingCount(); i++)
	{
		getStaticBoxContext().getSettingValue(i, l_sSettingValue);
		uint64 l_ui64StimulationId=getTypeManager().getEnumerationEntryValueFromName(OV_TypeId_Stimulation, l_sSettingValue);
		getLogManager() << LogLevel_Debug << "Stimulation Id : " << l_ui64StimulationId << " with name " << l_sSettingValue << "\n";
		m_vStimulationId[l_ui64StimulationId]=true;
	}

	m_ui64LastStimulationInputStartTime=0;
	m_ui64LastStimulationInputEndTime=0;
	m_ui64LastStimulationOutputEndTime=0;

	op_ui64SamplingRate.initialize(m_pSignalStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamDecoder_OutputParameterId_SamplingRate));
	op_pStimulationSet.initialize(m_pStimulationStreamDecoder->getOutputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_OutputParameterId_StimulationSet));
	ip_pStimulationSet.initialize(m_pStimulationStreamEncoder->getInputParameter(OVP_GD_Algorithm_StimulationStreamEncoder_InputParameterId_StimulationSet));
	ip_pSignal.initialize(m_pSignalStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamDecoder_OutputParameterId_Matrix));
	op_pSignal.initialize(m_pSignalStreamEncoder->getInputParameter(OVP_GD_Algorithm_SignalStreamEncoder_InputParameterId_Matrix));

	op_pStimulationMemoryBuffer.initialize(m_pStimulationStreamEncoder->getOutputParameter(OVP_GD_Algorithm_StimulationStreamEncoder_OutputParameterId_EncodedMemoryBuffer));
	ip_pStimulationMemoryBuffer.initialize(m_pStimulationStreamDecoder->getInputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_InputParameterId_MemoryBufferToDecode));
	op_SignalMemoryBuffer.initialize(m_pSignalStreamEncoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamEncoder_OutputParameterId_EncodedMemoryBuffer));
	ip_SignalMemoryBuffer.initialize(m_pSignalStreamDecoder->getInputParameter(OVP_GD_Algorithm_SignalStreamDecoder_InputParameterId_MemoryBufferToDecode));

	getLogManager() << LogLevel_Debug << "Parameters existence : " << op_ui64SamplingRate.exists() << ip_pStimulationSet.exists() << op_pStimulationSet.exists() << ip_pSignal.exists() << op_pSignal.exists() << "\n";

	m_pSignalStreamEncoder->getInputParameter(OVP_GD_Algorithm_SignalStreamEncoder_InputParameterId_SamplingRate)->setReferenceTarget(m_pSignalStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamDecoder_OutputParameterId_SamplingRate));

	op_pStimulationMemoryBuffer=l_rDynamicBoxContext.getOutputChunk(1);
	m_pStimulationStreamEncoder->process(OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeHeader);
	l_rDynamicBoxContext.markOutputAsReadyToSend(1, 0, 0);

	m_pOutputSignalDescription=new CMatrix();

	return true;
}

boolean CBoxAlgorithmStimulationBasedEpoching::uninitialize(void)
{
	delete m_pOutputSignalDescription;
	m_pOutputSignalDescription=NULL;

	m_pSignalStreamEncoder->uninitialize();
	m_pSignalStreamDecoder->uninitialize();
	m_pStimulationStreamEncoder->uninitialize();
	m_pStimulationStreamDecoder->uninitialize();

	getAlgorithmManager().releaseAlgorithm(*m_pSignalStreamEncoder);
	getAlgorithmManager().releaseAlgorithm(*m_pSignalStreamDecoder);
	getAlgorithmManager().releaseAlgorithm(*m_pStimulationStreamEncoder);
	getAlgorithmManager().releaseAlgorithm(*m_pStimulationStreamDecoder);

	m_vStimulationId.clear();

	std::vector < SStimulationBasedEpoching >::iterator itStimulationBasedEpoching;
	for(itStimulationBasedEpoching=m_vStimulationBasedEpoching.begin(); itStimulationBasedEpoching!=m_vStimulationBasedEpoching.end(); itStimulationBasedEpoching++)
	{
		getAlgorithmManager().releaseAlgorithm(*itStimulationBasedEpoching->m_pEpocher);
	}

	return true;
}

boolean CBoxAlgorithmStimulationBasedEpoching::processInput(uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	return true;
}

boolean CBoxAlgorithmStimulationBasedEpoching::process(void)
{
	// IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();
	uint32 i, j, k;

	// Stimulation input parsing
	for(i=0; i<l_rDynamicBoxContext.getInputChunkCount(1); i++)
	{
		CStimulationSet l_oOutputStimulationSet;
		ip_pStimulationSet=&l_oOutputStimulationSet;

		op_pStimulationMemoryBuffer=l_rDynamicBoxContext.getOutputChunk(1);
		ip_pStimulationMemoryBuffer=l_rDynamicBoxContext.getInputChunk(1, i);

		m_pStimulationStreamDecoder->process();
		if(m_pStimulationStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedBuffer))
		{
			for(j=0; j<op_pStimulationSet->getStimulationCount(); j++)
			{
				if(m_vStimulationId.find(op_pStimulationSet->getStimulationIdentifier(j))!=m_vStimulationId.end())
				{
					if((int64)op_pStimulationSet->getStimulationDate(j)+m_i64EpochOffset>=0)
					{
						SStimulationBasedEpoching l_oEpocher;
						l_oEpocher.m_pEpocher=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_StimulationBasedEpoching));
						l_oEpocher.m_pEpocher->initialize();
						l_oEpocher.m_ui64StimulationTime=op_pStimulationSet->getStimulationDate(j);
						l_oEpocher.m_ui64StartTime=op_pStimulationSet->getStimulationDate(j)+m_i64EpochOffset;
						l_oEpocher.m_ui64EndTime=op_pStimulationSet->getStimulationDate(j)+m_i64EpochOffset+m_ui64EpochDuration;
						l_oEpocher.m_bNeedsReset=true;
						m_vStimulationBasedEpoching.push_back(l_oEpocher);
						getLogManager() << LogLevel_Debug << "Created new epocher at time "
							<< time64(l_oEpocher.m_ui64StimulationTime) << ":"
							<< time64(l_oEpocher.m_ui64StartTime) << ":"
							<< time64(l_oEpocher.m_ui64EndTime) << "\n";

						ip_pStimulationSet->appendStimulation(
							op_pStimulationSet->getStimulationIdentifier(j),
							op_pStimulationSet->getStimulationDate(j)+m_i64EpochOffset,
							m_ui64EpochDuration);
					}
					else
					{
						getLogManager() << LogLevel_Warning << "Skipped epocher that should have started at a negative time\n";
					}
				}
			}

			if((int64)l_rDynamicBoxContext.getInputChunkEndTime(1, i)+m_i64EpochOffset>=0)
			{
				m_pStimulationStreamEncoder->process(OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeBuffer);
				l_rDynamicBoxContext.markOutputAsReadyToSend(1, m_ui64LastStimulationOutputEndTime, l_rDynamicBoxContext.getInputChunkEndTime(1, i)+m_i64EpochOffset);
				m_ui64LastStimulationOutputEndTime=l_rDynamicBoxContext.getInputChunkEndTime(1, i)+m_i64EpochOffset;
			}
		}

		m_ui64LastStimulationInputStartTime=l_rDynamicBoxContext.getInputChunkStartTime(1, i);
		m_ui64LastStimulationInputEndTime=l_rDynamicBoxContext.getInputChunkEndTime(1, i);
		l_rDynamicBoxContext.markInputAsDeprecated(1, i);
	}

	// Signal input parsing

	boolean l_bLastChunkIsHeader = false;

	for(i=0; i<l_rDynamicBoxContext.getInputChunkCount(0); i++)
	{
		ip_SignalMemoryBuffer=l_rDynamicBoxContext.getInputChunk(0, i);
		op_SignalMemoryBuffer=l_rDynamicBoxContext.getOutputChunk(0);

		m_pSignalStreamDecoder->process();

		if(m_pSignalStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SignalStreamDecoder_OutputTriggerId_ReceivedHeader))
		{
			m_pOutputSignalDescription->setDimensionCount(2);
			m_pOutputSignalDescription->setDimensionSize(0, ip_pSignal->getDimensionSize(0));
			m_pOutputSignalDescription->setDimensionSize(1, (uint32)(((m_ui64EpochDuration+1)*op_ui64SamplingRate-1)>>32)); // http://stickyvibe.tuxfamily.org/blog/?p=121

			for(k=0; k<ip_pSignal->getDimensionSize(0); k++)
			{
				m_pOutputSignalDescription->setDimensionLabel(0, k, ip_pSignal->getDimensionLabel(0, k));
			}
			op_pSignal.setReferenceTarget(m_pOutputSignalDescription);
			m_pSignalStreamEncoder->process(OVP_GD_Algorithm_SignalStreamEncoder_InputTriggerId_EncodeHeader);
			l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_rDynamicBoxContext.getInputChunkStartTime(0, i), l_rDynamicBoxContext.getInputChunkEndTime(0, i));
			l_bLastChunkIsHeader = true;
		}

		if(m_pSignalStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SignalStreamDecoder_OutputTriggerId_ReceivedBuffer))
		{
			vector < SStimulationBasedEpoching >::iterator j;
			for(j=m_vStimulationBasedEpoching.begin(); j!=m_vStimulationBasedEpoching.end(); )
			{
				SStimulationBasedEpoching& l_oEpocher=*j;

				TParameterHandler < IMatrix* > l_pEpocherInputSignal(l_oEpocher.m_pEpocher->getInputParameter(OVP_Algorithm_StimulationBasedEpoching_InputParameterId_InputSignal));
				TParameterHandler < IMatrix* > l_pEpocherOutputSignal(l_oEpocher.m_pEpocher->getOutputParameter(OVP_Algorithm_StimulationBasedEpoching_OutputParameterId_OutputSignal));
				l_pEpocherInputSignal.setReferenceTarget(ip_pSignal);
				op_pSignal.setReferenceTarget(l_pEpocherOutputSignal);

				/*
				 * solving the negative offset bug: two lines of code added so that the epoching algorithm will not add signal chunks to its epoch that it has already added
				 * this is necessary so that we can remove the first if statement (in the original code) in the for loop for signal chunk processing which in
				 * turn enables us to output an epoch with negative offset without an additional delay
				*/
				TParameterHandler < uint64 > l_ui64TimeChunk(l_oEpocher.m_pEpocher->getInputParameter(OVP_Algorithm_StimulationBasedEpoching_InputParameterId_EndTimeChunkToProcess));
				l_ui64TimeChunk = l_rDynamicBoxContext.getInputChunkEndTime(0, i);

				if(l_oEpocher.m_bNeedsReset)
				{
					OpenViBEToolkit::Tools::Matrix::copyDescription(*l_pEpocherOutputSignal, *m_pOutputSignalDescription);
					TParameterHandler < int64 > l_ui64OffsetSampleCount(l_oEpocher.m_pEpocher->getInputParameter(OVP_Algorithm_StimulationBasedEpoching_InputParameterId_OffsetSampleCount));
					l_ui64OffsetSampleCount=(op_ui64SamplingRate*((l_oEpocher.m_ui64StartTime-l_rDynamicBoxContext.getInputChunkStartTime(0, i))>>16))>>16;
					l_oEpocher.m_pEpocher->process(OVP_Algorithm_StimulationBasedEpoching_InputTriggerId_Reset);
					l_oEpocher.m_bNeedsReset=false;
				}

				l_oEpocher.m_pEpocher->process(OVP_Algorithm_StimulationBasedEpoching_InputTriggerId_PerformEpoching);

				if(l_oEpocher.m_pEpocher->isOutputTriggerActive(OVP_Algorithm_StimulationBasedEpoching_OutputTriggerId_EpochingDone))
				{
					m_pSignalStreamEncoder->process(OVP_GD_Algorithm_SignalStreamEncoder_InputTriggerId_EncodeBuffer);
					l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_oEpocher.m_ui64StartTime, l_oEpocher.m_ui64EndTime);

					getAlgorithmManager().releaseAlgorithm(*l_oEpocher.m_pEpocher);
					j=m_vStimulationBasedEpoching.erase(j);
				}
				else
				{
					j++;
				}
			}
		}

		if(m_pSignalStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SignalStreamDecoder_OutputTriggerId_ReceivedEnd))
		{
			m_pSignalStreamEncoder->process(OVP_GD_Algorithm_SignalStreamEncoder_InputTriggerId_EncodeEnd);
			l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_rDynamicBoxContext.getInputChunkStartTime(0, i), l_rDynamicBoxContext.getInputChunkEndTime(0, i));
		}

		/*
		 * solving the negative offset bug: the fact that this statement is not at the top means that all epochs in the waiting queue can already be used for completing an epoch
		 * without waiting for the next input chunk. This enables the algorithm to output an epoch with negative offset without additional delay.
		 * Nevertheless we still need to preserve some history and thus we can not mark all processed chunks as deprecated yet.
		 * lbonnet 09.05.2012: added a clause to avoid saving the header chunk. We need to deprecate it so we don't send headers continuously on the output.
		*/
		if((int64)l_rDynamicBoxContext.getInputChunkEndTime(0, i) <= (int64)m_ui64LastStimulationInputEndTime + m_i64EpochOffset || l_bLastChunkIsHeader) // preserve enough history
		{
			l_rDynamicBoxContext.markInputAsDeprecated(0, i);
		}
	}

	return true;
}
