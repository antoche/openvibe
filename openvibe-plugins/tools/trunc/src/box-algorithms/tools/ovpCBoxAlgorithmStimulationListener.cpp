#include "ovpCBoxAlgorithmStimulationListener.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Tools;

boolean CBoxAlgorithmStimulationListener::initialize(void)
{
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	for(uint32 i=0; i<l_rStaticBoxContext.getInputCount(); i++)
	{
		IAlgorithmProxy* m_pStreamDecoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamDecoder));
		m_pStreamDecoder->initialize();
		m_vStreamDecoder.push_back(m_pStreamDecoder);
	}

	CString l_sSettingValue;
	l_rStaticBoxContext.getSettingValue(0, l_sSettingValue);
	m_eLogLevel=static_cast<ELogLevel>(getTypeManager().getEnumerationEntryValueFromName(OV_TypeId_LogLevel, l_sSettingValue));

	return true;
}

boolean CBoxAlgorithmStimulationListener::uninitialize(void)
{
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	for(uint32 i=0; i<l_rStaticBoxContext.getInputCount(); i++)
	{
		m_vStreamDecoder[i]->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*m_vStreamDecoder[i]);
	}
	m_vStreamDecoder.clear();

	return true;
}

boolean CBoxAlgorithmStimulationListener::processInput(uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CBoxAlgorithmStimulationListener::process(void)
{
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

	for(uint32 i=0; i<l_rStaticBoxContext.getInputCount(); i++)
	{
		for(uint32 j=0; j<l_rDynamicBoxContext.getInputChunkCount(i); j++)
		{
			TParameterHandler < const IMemoryBuffer* > ip_pMemoryBufferToDecode(m_vStreamDecoder[i]->getInputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_InputParameterId_MemoryBufferToDecode));
			TParameterHandler < const IStimulationSet* > op_pStimulationSet(m_vStreamDecoder[i]->getOutputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_OutputParameterId_StimulationSet));
			ip_pMemoryBufferToDecode=l_rDynamicBoxContext.getInputChunk(i, j);

			m_vStreamDecoder[i]->process();
			if(m_vStreamDecoder[i]->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedHeader))
			{
			}
			if(m_vStreamDecoder[i]->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedBuffer))
			{
				CString l_sInputName;
				l_rStaticBoxContext.getInputName(i, l_sInputName);
				for(uint64 k=0; k<op_pStimulationSet->getStimulationCount(); k++)
				{
					this->getLogManager() << m_eLogLevel
						<< "For input " << i << " with name " << l_sInputName
						<< " got stimulation " << op_pStimulationSet->getStimulationIdentifier(k)
						<< "[" << this->getTypeManager().getEnumerationEntryNameFromValue(OV_TypeId_Stimulation, op_pStimulationSet->getStimulationIdentifier(k)) << "]"
						<< " at date " << time64(op_pStimulationSet->getStimulationDate(k))
						<< " and duration " << time64(op_pStimulationSet->getStimulationDuration(k))
						<< "\n";
					if(op_pStimulationSet->getStimulationDate(k) < l_rDynamicBoxContext.getInputChunkStartTime(i, j) || op_pStimulationSet->getStimulationDate(k) > l_rDynamicBoxContext.getInputChunkEndTime(i, j))
					{
						this->getLogManager() << LogLevel_Warning
							<< "The stimulation date is out of chunk range ! "
							<< " Stimulation date is " << time64(op_pStimulationSet->getStimulationDate(k))
							<< " and chunk range is [" << time64(l_rDynamicBoxContext.getInputChunkStartTime(i, j)) << ", " << time64(l_rDynamicBoxContext.getInputChunkEndTime(i, j)) << "]\n";
					}
				}
			}
			if(m_vStreamDecoder[i]->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedEnd))
			{
			}
			l_rDynamicBoxContext.markInputAsDeprecated(i, j);
		}
	}

	return true;
}
