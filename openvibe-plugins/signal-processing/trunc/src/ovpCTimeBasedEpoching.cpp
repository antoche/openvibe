#include "ovpCTimeBasedEpoching.h"

#include <system/Memory.h>

#include <iostream>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;
using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessing;
using namespace OpenViBEToolkit;
using namespace std;

//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------

class CTimeBasedEpoching::COutputHandler
{
public:

	COutputHandler(CTimeBasedEpoching& rParent, uint32 ui32OutputIndex);
	virtual ~COutputHandler(void);

protected:

	virtual void setSampleCountPerEpoch(const uint32 ui32SampleCountPerEpoch);
	virtual void setSampleCountBetweenEpoch(const uint32 ui32SampleCountBetweenEpoch);

public:

	virtual void setChannelCount(const uint32 ui32ChannelCount);
	virtual void setChannelName(const uint32 ui32ChannelIndex, const char* sChannelName);
	virtual void setSamplingRate(const uint32 ui32SamplingFrequency);

	virtual void reset(const uint64 ui64DeltaTime);
	virtual void processInput(const uint32 ui32InputSampleCount, const float64* pInputBuffer);

	virtual void write(const void* pBuffer, const EBML::uint64 ui64BufferSize);

private:

	COutputHandler(void);

public:

	float64 m_f64EpochDuration;
	float64 m_f64EpochInterval;

protected:

	uint32 m_ui32OutputIndex;
	uint32 m_ui32EpochIndex;

	uint32 m_ui32ChannelCount;
	uint32 m_ui32SamplingRate;
	uint32 m_ui32SampleIndex;
	uint32 m_ui32SampleCountPerEpoch;
	uint32 m_ui32SampleCountBetweenEpoch;

	uint64 m_ui64DeltaTime;

	float64* m_pSampleBuffer;

	EBML::TWriterCallbackProxy1<CTimeBasedEpoching::COutputHandler> m_oSignalOutputWriterCallbackProxy;
	IBoxAlgorithmSignalOutputWriter* m_pSignalOutputWriterHelper;
	EBML::IWriter* m_pSignalOutputWriter;

	CTimeBasedEpoching& m_rParent;
};

CTimeBasedEpoching::COutputHandler::COutputHandler(CTimeBasedEpoching& rParent, uint32 ui32OutputIndex)
	:m_ui32OutputIndex(ui32OutputIndex)
	,m_ui32EpochIndex(0)
	,m_ui32ChannelCount(0)
	,m_ui32SamplingRate(0)
	,m_ui32SampleIndex(0)
	,m_ui32SampleCountPerEpoch(0)
	,m_ui32SampleCountBetweenEpoch(0)
	,m_pSampleBuffer(NULL)
	,m_oSignalOutputWriterCallbackProxy(
		*this,
		&CTimeBasedEpoching::COutputHandler::write)
	,m_pSignalOutputWriterHelper(NULL)
	,m_pSignalOutputWriter(NULL)
	,m_rParent(rParent)
{
	m_pSignalOutputWriter=EBML::createWriter(m_oSignalOutputWriterCallbackProxy);
	m_pSignalOutputWriterHelper=createBoxAlgorithmSignalOutputWriter();
	m_ui64DeltaTime=0;
}

CTimeBasedEpoching::COutputHandler::~COutputHandler(void)
{
	delete [] m_pSampleBuffer;

	releaseBoxAlgorithmSignalOutputWriter(m_pSignalOutputWriterHelper);
	m_pSignalOutputWriterHelper=NULL;

	m_pSignalOutputWriter->release();
	m_pSignalOutputWriter=NULL;
}

void CTimeBasedEpoching::COutputHandler::setSampleCountPerEpoch(const uint32 ui32SampleCountPerEpoch)
{
	m_ui32SampleCountPerEpoch=ui32SampleCountPerEpoch;
	m_pSignalOutputWriterHelper->setSampleCountPerBuffer(ui32SampleCountPerEpoch);
}

void CTimeBasedEpoching::COutputHandler::setSampleCountBetweenEpoch(const uint32 ui32SampleCountBetweenEpoch)
{
	m_ui32SampleCountBetweenEpoch=ui32SampleCountBetweenEpoch;
}

void CTimeBasedEpoching::COutputHandler::setChannelCount(const uint32 ui32ChannelCount)
{
	m_ui32ChannelCount=ui32ChannelCount;
	m_pSignalOutputWriterHelper->setChannelCount(ui32ChannelCount);
}

void CTimeBasedEpoching::COutputHandler::setChannelName(const uint32 ui32ChannelIndex, const char* sChannelName)
{
	m_pSignalOutputWriterHelper->setChannelName(ui32ChannelIndex, sChannelName);
}

void CTimeBasedEpoching::COutputHandler::setSamplingRate(const uint32 ui32SamplingFrequency)
{
	m_ui32SamplingRate=ui32SamplingFrequency;
	this->setSampleCountPerEpoch    ((uint32)(m_f64EpochDuration*ui32SamplingFrequency));
	this->setSampleCountBetweenEpoch((uint32)(m_f64EpochInterval*ui32SamplingFrequency));
	m_pSignalOutputWriterHelper->setSamplingRate(ui32SamplingFrequency);
}

void CTimeBasedEpoching::COutputHandler::reset(const uint64 ui64DeltaTime)
{
	m_ui64DeltaTime=ui64DeltaTime;
	m_ui32SampleIndex=0;
	m_ui32EpochIndex=0;
}

void CTimeBasedEpoching::COutputHandler::processInput(const uint32 ui32InputSampleCount, const float64* pInputBuffer)
{
	// Allocates epoch buffer when needed
	if(!m_pSampleBuffer)
	{
		m_pSampleBuffer=new float64[m_ui32SampleCountPerEpoch*m_ui32ChannelCount];
		m_pSignalOutputWriterHelper->setSampleBuffer(m_pSampleBuffer);
		m_pSignalOutputWriterHelper->writeHeader(*m_pSignalOutputWriter);
		m_rParent.getDynamicBoxContext().markOutputAsReadyToSend(m_ui32OutputIndex, 0, 0);
	}

	// Iterates on bytes to process
	uint32 i, l_ui32SamplesProcessed=0;
	while(l_ui32SamplesProcessed!=ui32InputSampleCount)
	{
		if(m_ui32SampleIndex<m_ui32SampleCountPerEpoch) // Some samples should be filled
		{
			// Copies samples to buffer
			uint32 l_ui32SamplesToFill=min(m_ui32SampleCountPerEpoch-m_ui32SampleIndex, ui32InputSampleCount-l_ui32SamplesProcessed);
			for(i=0; i<m_ui32ChannelCount; i++)
			{
				System::Memory::copy(
					m_pSampleBuffer+i*m_ui32SampleCountPerEpoch+m_ui32SampleIndex,
					pInputBuffer+i*ui32InputSampleCount+l_ui32SamplesProcessed,
					l_ui32SamplesToFill*sizeof(float64));
			}
			m_ui32SampleIndex+=l_ui32SamplesToFill;
			l_ui32SamplesProcessed+=l_ui32SamplesToFill;

			if(m_ui32SampleIndex==m_ui32SampleCountPerEpoch) // An epoch has been totally filled !
			{
				// Calculates start and end time
				uint64 l_ui64StartTime=m_ui64DeltaTime+((((uint64)m_ui32EpochIndex)*((uint64)m_ui32SampleCountBetweenEpoch)                          )<<32)/m_ui32SamplingRate;
				uint64 l_ui64EndTime=  m_ui64DeltaTime+((((uint64)m_ui32EpochIndex)*((uint64)m_ui32SampleCountBetweenEpoch)+m_ui32SampleCountPerEpoch)<<32)/m_ui32SamplingRate;
				m_ui32EpochIndex++;

				// Writes epoch
				m_pSignalOutputWriterHelper->writeBuffer(*m_pSignalOutputWriter);
				m_rParent.getDynamicBoxContext().markOutputAsReadyToSend(m_ui32OutputIndex, l_ui64StartTime, l_ui64EndTime); // $$$$$$$$$$$
				m_rParent.getLogManager() << LogLevel_Debug << "New epoch written on output " << m_ui32OutputIndex << "(" << l_ui64StartTime << ":" << l_ui64EndTime << ")\n";

				if(m_ui32SampleCountBetweenEpoch<m_ui32SampleCountPerEpoch)
				{
					// Shifts samples for next epoch when overlap
					uint32 l_ui32SamplesToSave=m_ui32SampleCountPerEpoch-m_ui32SampleCountBetweenEpoch;
					for(i=0; i<m_ui32ChannelCount; i++)
					{
						System::Memory::copy(
							m_pSampleBuffer+i*m_ui32SampleCountPerEpoch,
							m_pSampleBuffer+i*m_ui32SampleCountPerEpoch+m_ui32SampleCountPerEpoch-l_ui32SamplesToSave,
							l_ui32SamplesToSave*sizeof(float64));
					}

					// The counter can be reseted
					m_ui32SampleIndex=l_ui32SamplesToSave;
				}
			}
		}
		else
		{
			// The next few samples are useless
			uint32 l_ui32SamplesToSkip=min(m_ui32SampleCountBetweenEpoch-m_ui32SampleIndex, ui32InputSampleCount-l_ui32SamplesProcessed);
			m_ui32SampleIndex+=l_ui32SamplesToSkip;
			l_ui32SamplesProcessed+=l_ui32SamplesToSkip;

			if(m_ui32SampleIndex==m_ui32SampleCountBetweenEpoch)
			{
				// The counter can be reseted
				m_ui32SampleIndex=0;
			}
		}
	}
}

void CTimeBasedEpoching::COutputHandler::write(const void* pBuffer, const EBML::uint64 ui64BufferSize)
{
	m_rParent.appendOutputChunkData(m_ui32OutputIndex, pBuffer, ui64BufferSize);
}

//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------

CTimeBasedEpoching::CTimeBasedEpoching(void)
	:m_ui32InputSampleCountPerBuffer(0)
	,m_pSignalInputReader(NULL)
	,m_pSignalInputReaderCallback(NULL)
	,m_oSignalInputReaderCallbackProxy(
		*this,
		&CTimeBasedEpoching::setChannelCount,
		&CTimeBasedEpoching::setChannelName,
		&CTimeBasedEpoching::setSampleCountPerBuffer,
		&CTimeBasedEpoching::setSamplingRate,
		&CTimeBasedEpoching::setSampleBuffer)
{
}

void CTimeBasedEpoching::release(void)
{
	delete this;
}

boolean CTimeBasedEpoching::initialize(void)
{
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();

	for(uint32 i=0; i<getBoxAlgorithmContext()->getStaticBoxContext()->getOutputCount(); i++)
	{
		CTimeBasedEpoching::COutputHandler* l_pOutputHandler=new CTimeBasedEpoching::COutputHandler(*this, i);
		CString l_sEpochDuration;
		CString l_sEpochInterval;

		float64 l_f64EpochDuration=0;
		float64 l_f64EpochInterval=0;

		l_rStaticBoxContext.getSettingValue(i*2+0, l_sEpochDuration);
		l_rStaticBoxContext.getSettingValue(i*2+1, l_sEpochInterval);

		sscanf(l_sEpochDuration, "%lf", &l_f64EpochDuration);
		sscanf(l_sEpochInterval, "%lf", &l_f64EpochInterval);

		if(l_f64EpochDuration>0 && l_f64EpochInterval>0)
		{
			l_pOutputHandler->m_f64EpochDuration=l_f64EpochDuration;
			l_pOutputHandler->m_f64EpochInterval=l_f64EpochInterval;
		}
		else
		{
			l_pOutputHandler->m_f64EpochDuration=1.0;
			l_pOutputHandler->m_f64EpochInterval=0.5;

			this->getLogManager() << LogLevel_Warning << "Epocher settings for output " << i << " are invalid (duration:" << l_f64EpochDuration << "|" << "interval:" << l_f64EpochInterval << ")... falling back to default 1 second duration and 0.5 second interval.\n";
		}
		m_vOutputHandler.push_back(l_pOutputHandler);
	}

	m_ui64LastStartTime=0;
	m_ui64LastEndTime=0;

	m_pSignalInputReaderCallback=createBoxAlgorithmSignalInputReaderCallback(m_oSignalInputReaderCallbackProxy);

	m_pSignalInputReader=EBML::createReader(*m_pSignalInputReaderCallback);

	return true;
}

boolean CTimeBasedEpoching::uninitialize(void)
{
	m_pSignalInputReader->release();
	m_pSignalInputReader=NULL;

	releaseBoxAlgorithmSignalInputReaderCallback(m_pSignalInputReaderCallback);
	m_pSignalInputReaderCallback=NULL;

	vector<CTimeBasedEpoching::COutputHandler*>::iterator itOutputHandler;
	for(itOutputHandler=m_vOutputHandler.begin(); itOutputHandler!=m_vOutputHandler.end(); itOutputHandler++)
	{
		delete *itOutputHandler;
	}
	m_vOutputHandler.clear();

	return true;
}

boolean CTimeBasedEpoching::processInput(uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CTimeBasedEpoching::process(void)
{
	IDynamicBoxContext& l_rDynamicBoxContext=this->getDynamicBoxContext();

	uint64 l_ui64ChunkStartTime;
	uint64 l_ui64ChunkEndTime;
	uint64 l_ui64ChunkSize;
	const uint8* l_pChunkBuffer;
	for(uint32 i=0; i<l_rDynamicBoxContext.getInputChunkCount(0); i++)
	{
		if(l_rDynamicBoxContext.getInputChunk(0, i, l_ui64ChunkStartTime, l_ui64ChunkEndTime, l_ui64ChunkSize, l_pChunkBuffer))
		{
			if(m_ui64LastEndTime!=l_ui64ChunkStartTime)
			{
				this->getLogManager() << LogLevel_Debug << "Consecutive chunk start/end time differ (" << m_ui64LastEndTime << ":" << l_ui64ChunkStartTime << "), the epocher will restart\n";
				vector<CTimeBasedEpoching::COutputHandler*>::iterator itOutputHandler;
				for(itOutputHandler=m_vOutputHandler.begin(); itOutputHandler!=m_vOutputHandler.end(); itOutputHandler++)
				{
					(*itOutputHandler)->reset(l_ui64ChunkStartTime);
				}
			}
			else
			{
				this->getLogManager() << LogLevel_Debug << "Consecutive chunk start/end time match (" << m_ui64LastEndTime << ":" << l_ui64ChunkStartTime << ")\n";
			}

			l_rDynamicBoxContext.markInputAsDeprecated(0, i);
			m_pSignalInputReader->processData(l_pChunkBuffer, l_ui64ChunkSize);

			m_ui64LastStartTime=l_ui64ChunkStartTime;
			m_ui64LastEndTime=l_ui64ChunkEndTime;
		}
	}
	return true;
}

//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------

void CTimeBasedEpoching::setChannelCount(const OpenViBE::uint32 ui32ChannelCount)
{
	for(size_t i=0; i<m_vOutputHandler.size(); i++)
	{
		m_vOutputHandler[i]->setChannelCount(ui32ChannelCount);
	}
}

void CTimeBasedEpoching::setChannelName(const OpenViBE::uint32 ui32ChannelIndex, const char* sChannelName)
{
	for(size_t i=0; i<m_vOutputHandler.size(); i++)
	{
		m_vOutputHandler[i]->setChannelName(ui32ChannelIndex, sChannelName);
	}
}

void CTimeBasedEpoching::setSampleCountPerBuffer(const OpenViBE::uint32 ui32SampleCountPerBuffer)
{
	m_ui32InputSampleCountPerBuffer=ui32SampleCountPerBuffer;
}

void CTimeBasedEpoching::setSamplingRate(const OpenViBE::uint32 ui32SamplingFrequency)
{
	for(size_t i=0; i<m_vOutputHandler.size(); i++)
	{
		m_vOutputHandler[i]->setSamplingRate(ui32SamplingFrequency);
	}
}

void CTimeBasedEpoching::setSampleBuffer(const OpenViBE::float64* pBuffer)
{
	for(size_t i=0; i<m_vOutputHandler.size(); i++)
	{
		m_vOutputHandler[i]->processInput(m_ui32InputSampleCountPerBuffer, pBuffer);
	}
}
