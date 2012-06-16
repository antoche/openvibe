#if defined TARGET_HAS_ThirdPartyVRPN

#include "ovpCVRPNAnalogServer.h"
#include "ovpIVRPNServerManager.h"

using namespace OpenViBE;
using namespace OpenViBE::Plugins;
using namespace OpenViBE::Kernel;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::VRPN;

CVRPNAnalogServer::CVRPNAnalogServer()
	:m_bAnalogSet(false)
{
}

boolean CVRPNAnalogServer::initialize()
{
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();

	// Gets server name, and creates an analog server for this server
	CString l_oServerName;
	l_rStaticBoxContext.getSettingValue(0, l_oServerName);

	// Creates the stream decoders
	for(uint32 i=0; i<l_rStaticBoxContext.getInputCount(); i++)
	{
		m_vStreamDecoder[i]=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamedMatrixStreamDecoder));
		m_vStreamDecoder[i]->initialize();
	}

	// Creates the peripheral
	IVRPNServerManager::getInstance().initialize();
	IVRPNServerManager::getInstance().addServer(l_oServerName, m_oServerIdentifier);

	return true;
}

boolean CVRPNAnalogServer::uninitialize()
{
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();

	// Releases decoders
	for(uint32 i=0; i<l_rStaticBoxContext.getInputCount(); i++)
	{
		m_vStreamDecoder[i]->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*m_vStreamDecoder[i]);
	}
	m_vStreamDecoder.clear();

	// Releases the peripheral
	IVRPNServerManager::getInstance().uninitialize();

	return true;
}

boolean CVRPNAnalogServer::processClock(IMessageClock& rMessageClock)
{
	IVRPNServerManager::getInstance().process();
	return true;
}

boolean CVRPNAnalogServer::processInput(uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CVRPNAnalogServer::process()
{
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

	uint32 k;

	for(uint32 i=0; i<l_rStaticBoxContext.getInputCount(); i++)
	{
		for(uint32 j=0; j<l_rDynamicBoxContext.getInputChunkCount(i); j++)
		{
			TParameterHandler < const IMemoryBuffer* > ip_pMemoryBuffer(m_vStreamDecoder[i]->getInputParameter(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_InputParameterId_MemoryBufferToDecode));
			TParameterHandler < IMatrix* > op_pMatrix(m_vStreamDecoder[i]->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputParameterId_Matrix));

			ip_pMemoryBuffer=l_rDynamicBoxContext.getInputChunk(i, j);
			m_vStreamDecoder[i]->process();

			IMatrix* l_pMatrix=op_pMatrix;

			if(m_vStreamDecoder[i]->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedHeader))
			{
				m_vAnalogCount[i]=l_pMatrix->getBufferElementCount();
				if(m_vAnalogCount.size()==l_rStaticBoxContext.getInputCount())
				{
					uint32 l_ui32AnalogCount=0;
					for(k=0; k<l_rStaticBoxContext.getInputCount(); k++)
					{
						l_ui32AnalogCount+=m_vAnalogCount[k];
					}

					IVRPNServerManager::getInstance().setAnalogCount(m_oServerIdentifier, l_ui32AnalogCount);
					this->getLogManager() << LogLevel_Trace << "Created VRPN analog server for " << l_ui32AnalogCount << " channel(s)\n";

					m_bAnalogSet=true;
				}
				l_rDynamicBoxContext.markInputAsDeprecated(i, j);
			}
			if(m_bAnalogSet)
			{
				if(m_vStreamDecoder[i]->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedBuffer))
				{
					if(m_bAnalogSet)
					{
						uint32 l_ui32AnalogOffset=0;
						for(k=0; k<i; k++)
						{
							l_ui32AnalogOffset+=m_vAnalogCount[k];
						}
						for(k=0; k<l_pMatrix->getBufferElementCount(); k++)
						{
							if(!IVRPNServerManager::getInstance().setAnalogState(m_oServerIdentifier, l_ui32AnalogOffset+k, l_pMatrix->getBuffer()[k]))
							{
								getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Warning << "Could not set analog state !\n";
							}
						}
						IVRPNServerManager::getInstance().reportAnalog(m_oServerIdentifier);
					}
				}
				if(m_vStreamDecoder[i]->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedEnd))
				{
				}
				l_rDynamicBoxContext.markInputAsDeprecated(i, j);
			}
		}
	}

	return true;
}

#endif // OVP_HAS_Vrpn
