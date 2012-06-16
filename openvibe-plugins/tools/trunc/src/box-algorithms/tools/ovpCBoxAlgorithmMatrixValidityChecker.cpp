#include "ovpCBoxAlgorithmMatrixValidityChecker.h"

#include <cmath>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Tools;
using namespace std;

boolean CBoxAlgorithmMatrixValidityChecker::initialize(void)
{
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	for(uint32 i=0; i<l_rStaticBoxContext.getInputCount(); i++)
	{
		IAlgorithmProxy* m_pStreamDecoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamedMatrixStreamDecoder));
		m_pStreamDecoder->initialize();
		m_vStreamDecoder.push_back(m_pStreamDecoder);
	}

	CString l_sSettingValue;
	l_rStaticBoxContext.getSettingValue(0, l_sSettingValue);
	m_eLogLevel=static_cast<ELogLevel>(getTypeManager().getEnumerationEntryValueFromName(OV_TypeId_LogLevel, l_sSettingValue));

	return true;
}

boolean CBoxAlgorithmMatrixValidityChecker::uninitialize(void)
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

boolean CBoxAlgorithmMatrixValidityChecker::processInput(uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CBoxAlgorithmMatrixValidityChecker::process(void)
{
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

	for(uint32 i=0; i<l_rStaticBoxContext.getInputCount(); i++)
	{
		for(uint32 j=0; j<l_rDynamicBoxContext.getInputChunkCount(i); j++)
		{
			TParameterHandler < const IMemoryBuffer* > ip_pMemoryBufferToDecode(m_vStreamDecoder[i]->getInputParameter(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_InputParameterId_MemoryBufferToDecode));
			TParameterHandler < const IMatrix* > op_pMatrix(m_vStreamDecoder[i]->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputParameterId_Matrix));
			ip_pMemoryBufferToDecode=l_rDynamicBoxContext.getInputChunk(i, j);

			m_vStreamDecoder[i]->process();
			if(m_vStreamDecoder[i]->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedHeader))
			{
			}
			if(m_vStreamDecoder[i]->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedBuffer))
			{
				if(!OpenViBEToolkit::Tools::Matrix::isContentValid(*op_pMatrix))
				{
					getLogManager() << m_eLogLevel << "Matrix on input " << i << " either contains NAN or Infinity in its buffer (" << l_rDynamicBoxContext.getInputChunkStartTime(i, j) << "," << l_rDynamicBoxContext.getInputChunkEndTime(i, j) << ")\n";
				}
			}
			if(m_vStreamDecoder[i]->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedEnd))
			{
			}
			l_rDynamicBoxContext.markInputAsDeprecated(i, j);
		}
	}

	return true;
}
