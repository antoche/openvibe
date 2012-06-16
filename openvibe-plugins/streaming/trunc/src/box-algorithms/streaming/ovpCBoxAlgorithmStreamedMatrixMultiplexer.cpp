#include "ovpCBoxAlgorithmStreamedMatrixMultiplexer.h"

#include <system/Memory.h>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Streaming;

boolean CBoxAlgorithmStreamedMatrixMultiplexer::initialize(void)
{
/*
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	uint32 i, l_ui32InputCount=l_rStaticBoxContext.getInputCount();

	m_vStreamDecoder.resize(l_ui32InputCount);

	CIdentifier l_oOutputTypeIdentifier;
	l_rStaticBoxContext.getOutputType(0, l_oOutputTypeIdentifier);
	if(l_oOutputTypeIdentifier==OV_TypeId_StreamedMatrix)
	{
		for(i=0; i<l_ui32InputCount; i++)
		{
			m_vStreamDecoder[i]=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamedMatrixStreamDecoder));
		}
		m_pStreamEncoder=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamedMatrixStreamEncoder));
	}
	else if(l_oOutputTypeIdentifier==OV_TypeId_FeatureVector)
	{
		for(i=0; i<l_ui32InputCount; i++)
		{
			m_vStreamDecoder[i]=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_FeatureVectorStreamDecoder));
		}
		m_pStreamEncoder=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_FeatureVectorStreamEncoder));
	}
	else if(l_oOutputTypeIdentifier==OV_TypeId_Signal)
	{
		for(i=0; i<l_ui32InputCount; i++)
		{
			m_vStreamDecoder[i]=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamDecoder));
		}
		m_pStreamEncoder=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamEncoder));
	}
	else if(l_oOutputTypeIdentifier==OV_TypeId_Spectrum)
	{
		for(i=0; i<l_ui32InputCount; i++)
		{
			m_vStreamDecoder[i]=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SpectrumStreamDecoder));
		}
		m_pStreamEncoder=&getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SpectrumStreamEncoder));
	}
	else
	{
		return false;
	}

	for(i=0; i<l_ui32InputCount; i++)
	{
		m_vStreamDecoder[i]->initialize();
	}
	m_pStreamEncoder->initialize();
*/

	m_ui64LastStartTime=0;
	m_ui64LastEndTime=0;
	m_bIncoherentChunkDating=false;
	m_bHeaderSent=false;

	return true;
}

boolean CBoxAlgorithmStreamedMatrixMultiplexer::uninitialize(void)
{
/*
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	uint32 i, l_ui32InputCount=l_rStaticBoxContext.getInputCount();

	m_pStreamEncoder->uninitialize();
	for(i=0; i<l_ui32InputCount; i++)
	{
		m_vStreamDecoder[i]->uninitialize();
	}

	this->getAlgorithmManager().releaseAlgorithm(*m_pStreamEncoder);
	for(i=0; i<l_ui32InputCount; i++)
	{
		this->getAlgorithmManager().releaseAlgorithm(*m_vStreamDecoder[i]);
	}
*/
	return true;
}

boolean CBoxAlgorithmStreamedMatrixMultiplexer::processInput(uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CBoxAlgorithmStreamedMatrixMultiplexer::process(void)
{
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

/*
	if(l_oOutputTypeIdentifier==OV_TypeId_StreamedMatrix)
	{
	}
	else if(l_oOutputTypeIdentifier==OV_TypeId_FeatureVector)
	{
	}
	else if(l_oOutputTypeIdentifier==OV_TypeId_Signal)
	{
		m_pStreamEncoder->getInputParameter(OVP_GD_Algorithm_SignalStreamEncoder_InputParameterId_SamplingRate)->setReferenceTarget(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamDecoder_OutputParameterId_SamplingRate));
	}
	else if(l_oOutputTypeIdentifier==OV_TypeId_Spectrum)
	{
		m_pStreamEncoder->getInputParameter(OVP_GD_Algorithm_SpectrumStreamEncoder_InputParameterId_MinMaxFrequencyBands)->setReferenceTarget(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SpectrumStreamDecoder_OutputParameterId_MinMaxFrequencyBands));
	}
*/

	for(uint32 i=0; i<l_rStaticBoxContext.getInputCount(); i++)
	{
		for(uint32 j=0; j<l_rDynamicBoxContext.getInputChunkCount(i); j++)
		{
			if(!m_bIncoherentChunkDating)
			{
				const IMemoryBuffer* l_pInputMemoryBuffer=l_rDynamicBoxContext.getInputChunk(i, j);
				uint64 l_ui64StartTime=l_rDynamicBoxContext.getInputChunkStartTime(i, j);
				uint64 l_ui64EndTime=l_rDynamicBoxContext.getInputChunkEndTime(i, j);
				if((!m_bHeaderSent && l_ui64StartTime==l_ui64EndTime) || (m_bHeaderSent && l_ui64StartTime!=l_ui64EndTime))
				{
					IMemoryBuffer* l_pOutputMemoryBuffer=l_rDynamicBoxContext.getOutputChunk(0);
					l_pOutputMemoryBuffer->setSize(l_pInputMemoryBuffer->getSize(), true);
					System::Memory::copy(
						l_pOutputMemoryBuffer->getDirectPointer(),
						l_pInputMemoryBuffer->getDirectPointer(),
						l_pInputMemoryBuffer->getSize());

					if(l_ui64StartTime<m_ui64LastStartTime || l_ui64EndTime<m_ui64LastEndTime)
					{
						this->getLogManager() << LogLevel_ImportantWarning << "Stream chunk dating is incoherent, check inputs !\n";
						m_bIncoherentChunkDating=true;
					}
					else
					{
						l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_ui64StartTime, l_ui64EndTime);
						m_bHeaderSent=true;
					}
				}
			}

			l_rDynamicBoxContext.markInputAsDeprecated(i, j);
		}
	}

	return true;
}
