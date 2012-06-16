#include "ovpCTimeSignalGenerator.h"

#include <iostream>
#include <cmath>
#include <cstdlib>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;
using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Samples;
using namespace OpenViBEToolkit;
using namespace std;

CTimeSignalGenerator::CTimeSignalGenerator(void)
	:m_oSignalOutputWriterCallbackProxy(
		*this,
		&CTimeSignalGenerator::writeSignalOutput)
	,m_pSignalOutputWriterHelper(NULL)
	,m_pSignalOutputWriter(NULL)
	,m_bHeaderSent(false)
	,m_ui32SamplingFrequency(0)
	,m_ui32GeneratedEpochSampleCount(0)
	,m_pSampleBuffer(NULL)
	,m_ui32SentSampleCount(0)
{
}

void CTimeSignalGenerator::release(void)
{
	delete this;
}

boolean CTimeSignalGenerator::initialize(void)
{
	m_pSignalOutputWriterHelper=createBoxAlgorithmSignalOutputWriter();
	m_pSignalOutputWriter=EBML::createWriter(m_oSignalOutputWriterCallbackProxy);

	// Parses box settings to try connecting to server
	CString l_sSamplingFrequency;
	CString l_sGeneratedEpochSampleCount;
	getBoxAlgorithmContext()->getStaticBoxContext()->getSettingValue(0, l_sSamplingFrequency);
	getBoxAlgorithmContext()->getStaticBoxContext()->getSettingValue(1, l_sGeneratedEpochSampleCount);
	m_ui32SamplingFrequency=atoi(l_sSamplingFrequency);
	m_ui32GeneratedEpochSampleCount=atoi(l_sGeneratedEpochSampleCount);
	m_bHeaderSent=false;

	// Allocates sample block
	m_pSampleBuffer=new float64[m_ui32GeneratedEpochSampleCount];
	m_ui32SentSampleCount=0;

	return true;
}

boolean CTimeSignalGenerator::uninitialize(void)
{
	delete [] m_pSampleBuffer;
	m_pSampleBuffer=NULL;

	m_pSignalOutputWriter->release();
	m_pSignalOutputWriter=NULL;

	releaseBoxAlgorithmSignalOutputWriter(m_pSignalOutputWriterHelper);
	m_pSignalOutputWriterHelper=NULL;

	return true;
}

boolean CTimeSignalGenerator::processClock(CMessageClock& rMessageClock)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CTimeSignalGenerator::process(void)
{
	IBoxIO* l_pDynamicBoxContext=getBoxAlgorithmContext()->getDynamicBoxContext();

	if(!m_bHeaderSent)
	{
		m_pSignalOutputWriterHelper->setSamplingRate(m_ui32SamplingFrequency);
		m_pSignalOutputWriterHelper->setChannelCount(1);
		m_pSignalOutputWriterHelper->setSampleCountPerBuffer(m_ui32GeneratedEpochSampleCount);
		m_pSignalOutputWriterHelper->setSampleBuffer(m_pSampleBuffer);
		m_pSignalOutputWriterHelper->setChannelName(0, "Time signal");
		m_pSignalOutputWriterHelper->writeHeader(*m_pSignalOutputWriter);
		m_bHeaderSent=true;

		l_pDynamicBoxContext->markOutputAsReadyToSend(0, 0, 0);
	}
	else
	{
		uint32 l_ui32SentSampleCount=m_ui32SentSampleCount;
		float64 l_f64SamplingFrequency=static_cast<float64>(m_ui32SamplingFrequency);
		for(uint32 i=0; i<m_ui32GeneratedEpochSampleCount; i++)
		{
			m_pSampleBuffer[i]=(i+m_ui32SentSampleCount)/l_f64SamplingFrequency;
		}
		m_pSignalOutputWriterHelper->writeBuffer(*m_pSignalOutputWriter);
		m_ui32SentSampleCount+=m_ui32GeneratedEpochSampleCount;

		uint64 l_ui64StartTime;
		uint64 l_ui64EndTime;
		l_ui64StartTime=(((uint64)(l_ui32SentSampleCount))<<32)/m_ui32SamplingFrequency;
		l_ui64EndTime  =(((uint64)(m_ui32SentSampleCount))<<32)/m_ui32SamplingFrequency;
		l_pDynamicBoxContext->markOutputAsReadyToSend(0, l_ui64StartTime, l_ui64EndTime);
	}

	return true;
}

void CTimeSignalGenerator::writeSignalOutput(const void* pBuffer, const EBML::uint64 ui64BufferSize)
{
	appendOutputChunkData<0>(pBuffer, ui64BufferSize);
}
