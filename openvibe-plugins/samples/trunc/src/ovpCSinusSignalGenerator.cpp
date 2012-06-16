#include "ovpCSinusSignalGenerator.h"

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <cstdio>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;
using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Samples;
using namespace OpenViBEToolkit;
using namespace std;

CSinusSignalGenerator::CSinusSignalGenerator(void)
	:m_oSignalOutputWriterCallbackProxy(
		*this,
		&CSinusSignalGenerator::writeSignalOutput)
	,m_pSignalOutputWriterHelper(NULL)
	,m_pSignalOutputWriter(NULL)
	,m_bHeaderSent(false)
	,m_ui32ChannelCount(0)
	,m_ui32SamplingFrequency(0)
	,m_ui32GeneratedEpochSampleCount(0)
	,m_pSampleBuffer(NULL)
	,m_ui32SentSampleCount(0)
{
}

void CSinusSignalGenerator::release(void)
{
	delete this;
}

boolean CSinusSignalGenerator::initialize(void)
{
	m_pSignalOutputWriterHelper=createBoxAlgorithmSignalOutputWriter();
	m_pSignalOutputWriter=EBML::createWriter(m_oSignalOutputWriterCallbackProxy);

	// Parses box settings to try connecting to server
	CString l_sChannelCount;
	CString l_sSamplingFrequency;
	CString l_sGeneratedEpochSampleCount;
	getBoxAlgorithmContext()->getStaticBoxContext()->getSettingValue(0, l_sChannelCount);
	getBoxAlgorithmContext()->getStaticBoxContext()->getSettingValue(1, l_sSamplingFrequency);
	getBoxAlgorithmContext()->getStaticBoxContext()->getSettingValue(2, l_sGeneratedEpochSampleCount);
	m_ui32ChannelCount=atoi(l_sChannelCount);
	m_ui32SamplingFrequency=atoi(l_sSamplingFrequency);
	m_ui32GeneratedEpochSampleCount=atoi(l_sGeneratedEpochSampleCount);
	m_bHeaderSent=false;

	// Allocates sample block
	m_pSampleBuffer=new float64[m_ui32ChannelCount*m_ui32GeneratedEpochSampleCount];

	return true;
}

boolean CSinusSignalGenerator::uninitialize(void)
{
	delete [] m_pSampleBuffer;
	m_pSampleBuffer=NULL;

	m_pSignalOutputWriter->release();
	m_pSignalOutputWriter=NULL;

	releaseBoxAlgorithmSignalOutputWriter(m_pSignalOutputWriterHelper);
	m_pSignalOutputWriterHelper=NULL;

	return true;
}

boolean CSinusSignalGenerator::processClock(CMessageClock& rMessageClock)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CSinusSignalGenerator::process(void)
{
	IBoxIO* l_pDynamicBoxContext=getBoxAlgorithmContext()->getDynamicBoxContext();

	uint32 i,j;
	if(!m_bHeaderSent)
	{
		m_pSignalOutputWriterHelper->setSamplingRate(m_ui32SamplingFrequency);
		m_pSignalOutputWriterHelper->setChannelCount(m_ui32ChannelCount);
		m_pSignalOutputWriterHelper->setSampleCountPerBuffer(m_ui32GeneratedEpochSampleCount);
		m_pSignalOutputWriterHelper->setSampleBuffer(m_pSampleBuffer);
		for(i=0; i<m_ui32ChannelCount; i++)
		{
			char l_sChannelName[1024];
			sprintf(l_sChannelName, "Channel %i", (int)i);
			m_pSignalOutputWriterHelper->setChannelName(i, l_sChannelName);
		}
		m_pSignalOutputWriterHelper->writeHeader(*m_pSignalOutputWriter);
		m_bHeaderSent=true;

		uint64 l_ui64Time=(((uint64)(m_ui32SentSampleCount))<<32)/m_ui32SamplingFrequency;
		l_pDynamicBoxContext->markOutputAsReadyToSend(0, l_ui64Time, l_ui64Time);
	}
	else
	{
		uint32 l_ui32SentSampleCount=m_ui32SentSampleCount;
		for(i=0; i<m_ui32ChannelCount; i++)
		{
			for(j=0; j<m_ui32GeneratedEpochSampleCount; j++)
			{
				m_pSampleBuffer[i*m_ui32GeneratedEpochSampleCount+j]=
					sin(((j+m_ui32SentSampleCount)*(i+1)*12.3)/m_ui32SamplingFrequency)+
					sin(((j+m_ui32SentSampleCount)*(i+1)* 4.5)/m_ui32SamplingFrequency)+
					sin(((j+m_ui32SentSampleCount)*(i+1)*67.8)/m_ui32SamplingFrequency);
			}
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

void CSinusSignalGenerator::writeSignalOutput(const void* pBuffer, const EBML::uint64 ui64BufferSize)
{
	appendOutputChunkData<0>(pBuffer, ui64BufferSize);
}
