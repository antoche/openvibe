#include "ovpCBoxAlgorithmVRPNAnalogClient.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::VRPN;

#define boolean OpenViBE::boolean

namespace
{
	static void VRPN_CALLBACK vrpn_analog_cb(void* pUserData, const vrpn_ANALOGCB a)
	{
		OpenViBEPlugins::VRPN::CBoxAlgorithmVRPNAnalogClient* l_pClient=static_cast < OpenViBEPlugins::VRPN::CBoxAlgorithmVRPNAnalogClient* > (pUserData);
		l_pClient->setAnalog(a.num_channel, a.channel);
	}
}

uint64 CBoxAlgorithmVRPNAnalogClient::getClockFrequency(void)
{
	return 128LL<<32; // the box clock frequency
}

boolean CBoxAlgorithmVRPNAnalogClient::initialize(void)
{
	m_pVRPNAnalogRemote=NULL;

	m_sPeripheralName=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	m_ui64SamplingRate=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);
	m_ui32ChannelCount=uint32((uint64)FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2));
	m_ui32SampleCountPerSentBlock=uint32((uint64)FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 3));

	m_pStreamEncoder=&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamEncoder));
	m_pStreamEncoder->initialize();

	ip_pMatrix.initialize(m_pStreamEncoder->getInputParameter(OVP_GD_Algorithm_SignalStreamEncoder_InputParameterId_Matrix));
	ip_ui64SamplingRate.initialize(m_pStreamEncoder->getInputParameter(OVP_GD_Algorithm_SignalStreamEncoder_InputParameterId_SamplingRate));
	op_pMemoryBuffer.initialize(m_pStreamEncoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamEncoder_OutputParameterId_EncodedMemoryBuffer));

	m_ui64LastChunkEndTime=uint64(-1);
	m_ui64ChunkDuration=(uint64(m_ui32SampleCountPerSentBlock)<<32)/m_ui64SamplingRate;
	m_ui32SampleIndex=0;

	m_vSwapAnalog.resize(m_ui32ChannelCount);

	return true;
}

boolean CBoxAlgorithmVRPNAnalogClient::uninitialize(void)
{
	if(m_pVRPNAnalogRemote)
	{
		delete m_pVRPNAnalogRemote;
		m_pVRPNAnalogRemote=NULL;
	}

	if(m_pStreamEncoder)
	{
		m_pStreamEncoder->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*m_pStreamEncoder);
		m_pStreamEncoder=NULL;
	}

	m_vSwapAnalog.clear();
	m_vAnalog.clear();

	return true;
}

boolean CBoxAlgorithmVRPNAnalogClient::processClock(IMessageClock& rMessageClock)
{
	this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CBoxAlgorithmVRPNAnalogClient::process(void)
{
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();
	uint32 i, j;

	if(m_pVRPNAnalogRemote)
	{
		m_pVRPNAnalogRemote->mainloop();
	}
	else
	{
		m_pVRPNAnalogRemote=new vrpn_Analog_Remote(m_sPeripheralName.toASCIIString());
		m_pVRPNAnalogRemote->register_change_handler(this, &vrpn_analog_cb);
	}

	if(m_ui64LastChunkEndTime==uint64(-1))
	{
		ip_ui64SamplingRate=m_ui64SamplingRate;
		ip_pMatrix->setDimensionCount(2);
		ip_pMatrix->setDimensionSize(0, m_ui32ChannelCount);
		ip_pMatrix->setDimensionSize(1, m_ui32SampleCountPerSentBlock);

		// do labels ?

		op_pMemoryBuffer=l_rDynamicBoxContext.getOutputChunk(0);
		m_pStreamEncoder->process(OVP_GD_Algorithm_SignalStreamEncoder_InputTriggerId_EncodeHeader);
		l_rDynamicBoxContext.markOutputAsReadyToSend(0, 0, 0);

		m_ui64LastChunkEndTime=0;
	}

	uint64 l_ui64CurrentTime=this->getPlayerContext().getCurrentTime();
	uint32 l_ui32SampleCountToFill=((l_ui64CurrentTime-m_ui64LastChunkEndTime)*m_ui64SamplingRate)>>32;

	if(l_ui32SampleCountToFill>m_ui32SampleIndex)
	{
		for(i=m_ui32SampleIndex; i<m_ui32SampleIndex+l_ui32SampleCountToFill && i<m_ui32SampleCountPerSentBlock; i++)
		{
			if(m_vAnalog.size()!=0)
			{
				m_vSwapAnalog=m_vAnalog[0];
				m_vAnalog.pop_front();
			}
			for(j=0; j<m_ui32ChannelCount; j++)
			{
				ip_pMatrix->getBuffer()[j*m_ui32SampleCountPerSentBlock+i]=m_vSwapAnalog[j];
			}
		}

		m_ui32SampleIndex+=l_ui32SampleCountToFill;
		if(m_ui32SampleIndex>m_ui32SampleCountPerSentBlock)
		{
			op_pMemoryBuffer=l_rDynamicBoxContext.getOutputChunk(0);
			m_pStreamEncoder->process(OVP_GD_Algorithm_SignalStreamEncoder_InputTriggerId_EncodeBuffer);
			l_rDynamicBoxContext.markOutputAsReadyToSend(0, m_ui64LastChunkEndTime, m_ui64LastChunkEndTime+m_ui64ChunkDuration);
			m_ui32SampleIndex=0;
			m_ui64LastChunkEndTime+=m_ui64ChunkDuration;
		}
	}

	return true;
}

void CBoxAlgorithmVRPNAnalogClient::setAnalog(uint32 ui32AnalogCount, const float64* pAnalog)
{
	for(uint32 i=0; i<ui32AnalogCount && i<m_ui32ChannelCount; i++)
	{
		m_vSwapAnalog[i]=pAnalog[i];
	}
	m_vAnalog.push_back(m_vSwapAnalog);
}
