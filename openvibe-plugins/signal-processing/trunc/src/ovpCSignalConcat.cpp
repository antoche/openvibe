#include "ovpCSignalConcat.h"
#include <system/Memory.h>

#include <cmath>
#include <cstdio>
#include <iostream>
#include <sstream>

using namespace OpenViBE;
using namespace OpenViBE::Plugins;
using namespace OpenViBE::Kernel;
using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessing;
using namespace OpenViBEToolkit;
using namespace std;

#define STIMULATION_END_OF_SESSION 0x3F2

void CSignalConcatenation::setChannelCount(const uint32 ui32ChannelCount)
{
	if(m_ui32CurrentInput==0)
	{
		//gets the channel count
		m_pSignalDescription->m_ui32ChannelCount = ui32ChannelCount;
		m_pSignalDescription->m_pChannelName.resize(ui32ChannelCount);
	}
}

void CSignalConcatenation::setChannelName(const uint32 ui32ChannelIndex, const char* sChannelName)
{
	if(m_ui32CurrentInput == 0)
	{
		m_pSignalDescription->m_pChannelName[ui32ChannelIndex]=sChannelName;
	}
}

void CSignalConcatenation::setSampleCountPerBuffer(const uint32 ui32SampleCountPerBuffer)
{
	if(m_ui32CurrentInput == 0)
	{
		//gets the sample count
		m_pSignalDescription->m_ui32SampleCount = ui32SampleCountPerBuffer;

		//the matrix buffer hasn't been allocated yet, allocate it
		if(!m_pMatrixBuffer)
		{
			m_ui64MatrixBufferSize = m_pSignalDescription -> m_ui32ChannelCount * ui32SampleCountPerBuffer;

			m_pMatrixBuffer = new EBML::float64[(size_t)m_ui64MatrixBufferSize];
		}

		//we have everything needed to send the stream header
		m_pSignalOutputWriterHelper->setSamplingRate(m_pSignalDescription->m_ui32SamplingRate);

		m_pSignalOutputWriterHelper->setChannelCount(m_pSignalDescription->m_ui32ChannelCount);

		for(uint32 i=0 ; i<m_pSignalDescription->m_ui32ChannelCount ; i++)
		{
			m_pSignalOutputWriterHelper->setChannelName(i, m_pSignalDescription->m_pChannelName[i].c_str());
		}

		//just one sample per buffer since we made an average of the values in each buffer
		m_pSignalOutputWriterHelper->setSampleCountPerBuffer(ui32SampleCountPerBuffer);
		m_pSignalOutputWriterHelper->setSampleBuffer(m_pMatrixBuffer);

		m_pSignalOutputWriterHelper->writeHeader(*m_pSignalWriter);

		getBoxAlgorithmContext()->getDynamicBoxContext()->markOutputAsReadyToSend(0, m_ui64LastChunkStartTime, m_ui64LastChunkEndTime);
	}
}

void CSignalConcatenation::setSamplingRate(const uint32 ui32SamplingFrequency)
{
	if(m_ui32CurrentInput==0)
	{
		m_pSignalDescription->m_ui32SamplingRate = ui32SamplingFrequency;
	}
}

void CSignalConcatenation::setSampleBuffer(const float64* pBuffer)
{
	System::Memory::copy(m_pMatrixBuffer, pBuffer, m_ui64MatrixBufferSize*sizeof(float64));

	//sends the current signal matrix
	m_pSignalOutputWriterHelper->writeBuffer(*m_pSignalWriter);

	getBoxAlgorithmContext()->getDynamicBoxContext()->markOutputAsReadyToSend(0, m_ui64LastChunkStartTime+m_ui64TimeOffset, m_ui64LastChunkEndTime+m_ui64TimeOffset);
}

void CSignalConcatenation::setStimulationCount(const OpenViBE::uint32 ui32StimulationCount)
{
	m_ui32StimulationCount=ui32StimulationCount;
	m_oStimulations.clear();
}

void CSignalConcatenation::setStimulation(const OpenViBE::uint32 ui32StimulationIndex, const OpenViBE::uint64 ui64StimulationIdentifier, const OpenViBE::uint64 ui64StimulationDate)
{
	//when stimulation is not end stimulation
	if(ui64StimulationIdentifier != m_ui64EndStimulation)
	{
		//stores stimulation
		m_oStimulations.push_back(pair<uint64, uint64>(ui64StimulationIdentifier, ui64StimulationDate));
	}
	else
	{
		//goto next input
		m_bChangeInput = true;
	}

	//if the last stimulation of the current chunk is received
	if(ui32StimulationIndex==m_ui32StimulationCount-1)
	{
		//set the number of stimulation to send
		m_pStimulationOutputWriterHelper->setStimulationCount(m_oStimulations.size());

		//adds them
		for(uint32 i = 0 ; i<m_oStimulations.size() ; i++)
		{
			m_pStimulationOutputWriterHelper->setStimulation(i, m_oStimulations[i].first, m_oStimulations[i].second + m_ui64TimeOffset);
		}

		//sends them
		m_pStimulationOutputWriterHelper->writeBuffer(*m_pStimulationWriter);

		getBoxAlgorithmContext()->getDynamicBoxContext()->markOutputAsReadyToSend(1, m_ui64TimeOffset+m_ui64LastChunkStartTime, m_ui64LastChunkEndTime+m_ui64TimeOffset);
	}
}

void CSignalConcatenation::writeSignalOutput(const void* pBuffer, const EBML::uint64 ui64BufferSize)
{
	appendOutputChunkData<0>(pBuffer, ui64BufferSize);
}

void CSignalConcatenation::writeStimulationOutput(const void* pBuffer, const EBML::uint64 ui64BufferSize)
{
	appendOutputChunkData<1>(pBuffer, ui64BufferSize);
}

CSignalConcatenation::CSignalConcatenation(void)
	:m_pSignalReaderCallBack(NULL),
	m_ui64LastChunkStartTime(0),
	m_ui64LastChunkEndTime(0),
	m_oSignalOutputWriterCallbackProxy(*this, &CSignalConcatenation::writeSignalOutput),
	m_oStimulationOutputWriterCallbackProxy(*this, &CSignalConcatenation::writeStimulationOutput),
	m_pSignalOutputWriterHelper(NULL),
	m_pSignalDescription(NULL),
	m_ui64MatrixBufferSize(0),
	m_pMatrixBuffer(NULL),
	m_ui32CurrentInput(0),
	m_ui64TimeOffset(0),
	m_ui64EndStimulation(0),
	m_bChangeInput(false)
{
}

void CSignalConcatenation::release(void)
{
}

boolean CSignalConcatenation::initialize()
{
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();

	CString l_sSettingValue;
	l_rStaticBoxContext.getSettingValue(0, l_sSettingValue);
	m_ui64EndStimulation=this->getTypeManager().getEnumerationEntryValueFromName(OV_TypeId_Stimulation, l_sSettingValue);

	m_oSignalReader.resize(l_rStaticBoxContext.getInputCount()/2);
	m_oStimulationReader.resize(l_rStaticBoxContext.getInputCount()/2);

	//initialises the signal description structure
	m_pSignalDescription = new CSignalDescription();

	m_pSignalReaderCallBack = createBoxAlgorithmSignalInputReaderCallback(*this);

	// Prepares EBML reader
	for(size_t i=0 ; i<m_oSignalReader.size() ; i++)
	{
		m_oSignalReader[i]=EBML::createReader(*m_pSignalReaderCallBack);
	}

	m_pStimulationReaderCallBack = createBoxAlgorithmStimulationInputReaderCallback(*this);

	for(size_t i=0 ; i<m_oStimulationReader.size() ; i++)
	{
		m_oStimulationReader[i]=EBML::createReader(*m_pStimulationReaderCallBack);
	}

	// Prepares EBML writer
	m_pSignalOutputWriterHelper=createBoxAlgorithmSignalOutputWriter();
	m_pSignalWriter=EBML::createWriter(m_oSignalOutputWriterCallbackProxy);

	m_pStimulationOutputWriterHelper=createBoxAlgorithmStimulationOutputWriter();
	m_pStimulationWriter=EBML::createWriter(m_oStimulationOutputWriterCallbackProxy);

	//send stims header
	m_pStimulationOutputWriterHelper->writeHeader(*m_pStimulationWriter);
	getBoxAlgorithmContext()->getDynamicBoxContext()->markOutputAsReadyToSend(1, 0, 0);

	return true;
}

boolean CSignalConcatenation::uninitialize()
{
	if(m_pMatrixBuffer)
	{
		delete[] m_pMatrixBuffer;
	}

	// Cleans up EBML writer
	m_pSignalWriter->release();
	m_pSignalWriter=NULL;

	m_pStimulationWriter->release();
	m_pStimulationWriter=NULL;

	releaseBoxAlgorithmSignalOutputWriter(m_pSignalOutputWriterHelper);
	m_pSignalOutputWriterHelper=NULL;

	// Cleans up EBML reader
	releaseBoxAlgorithmSignalInputReaderCallback(m_pSignalReaderCallBack);

	for(size_t i=0 ; i<m_oSignalReader.size() ; i++)
	{
		m_oSignalReader[i]->release();
		m_oSignalReader[i]=NULL;
	}

	for(size_t i=0 ; i<m_oStimulationReader.size() ; i++)
	{
		m_oStimulationReader[i]->release();
		m_oStimulationReader[i]=NULL;
	}

	delete m_pSignalDescription;
	m_pSignalDescription = NULL;

	return true;
}

boolean CSignalConcatenation::processInput( uint32 ui32InputIndex)
{
	if(m_bChangeInput)
	{
		m_ui64TimeOffset = m_ui64TimeOffset + m_ui64LastChunkEndTime;
		m_ui32CurrentInput+=2;
		m_bChangeInput = false;
		// getBoxAlgorithmContext()->getPlayerContext()->getLogManager()<<Kernel::LogLevel_Trace<<"Changed input\n";

		if(m_ui32CurrentInput>=getBoxAlgorithmContext()->getStaticBoxContext()->getInputCount())
		{
			getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << Kernel::LogLevel_Info << "Concatenation finished !\n";
			return false;
		}
	}

	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CSignalConcatenation::process()
{
	IDynamicBoxContext* l_pDynamicBoxContext=getBoxAlgorithmContext()->getDynamicBoxContext();

	uint32 l_ui32ChunkCount1=l_pDynamicBoxContext->getInputChunkCount(m_ui32CurrentInput);
	uint32 l_ui32ChunkCount2=l_pDynamicBoxContext->getInputChunkCount(m_ui32CurrentInput+1);
	uint32 l_ui32ChunkCount=l_ui32ChunkCount1<l_ui32ChunkCount2?l_ui32ChunkCount1:l_ui32ChunkCount2;

	// only parse when input arrived on both signal & stim
	for(uint32 i=0; i<l_ui32ChunkCount; i++)
	{
		{
			// Process input data
			uint64 l_ui64ChunkSize;
			const uint8* l_pBuffer;
			if(l_pDynamicBoxContext->getInputChunk(m_ui32CurrentInput, i, m_ui64LastChunkStartTime, m_ui64LastChunkEndTime, l_ui64ChunkSize, l_pBuffer))
			{
				l_pDynamicBoxContext->markInputAsDeprecated(m_ui32CurrentInput, i);
				m_oSignalReader[m_ui32CurrentInput>>1]->processData(l_pBuffer, l_ui64ChunkSize);
				// getBoxAlgorithmContext()->getPlayerContext()->getLogManager()<<Kernel::LogLevel_Trace<<"Processed data on input "<<m_ui32CurrentInput<<" ["<<m_ui64LastChunkStartTime<<":"<<m_ui64LastChunkEndTime<<"]\n";
			}
		}

		{
			//process stimulations
			uint64 l_ui64ChunkSize;
			const uint8* l_pBuffer;
			if(l_pDynamicBoxContext->getInputChunk(m_ui32CurrentInput+1, i, m_ui64LastChunkStartTime, m_ui64LastChunkEndTime, l_ui64ChunkSize, l_pBuffer))
			{
				l_pDynamicBoxContext->markInputAsDeprecated(m_ui32CurrentInput+1, i);
				m_oStimulationReader[m_ui32CurrentInput>>1]->processData(l_pBuffer, l_ui64ChunkSize);
				// getBoxAlgorithmContext()->getPlayerContext()->getLogManager()<<Kernel::LogLevel_Trace<<"Processed data on input "<<m_ui32CurrentInput+1<<" ["<<m_ui64LastChunkStartTime<<":"<<m_ui64LastChunkEndTime<<"]\n";
			}
		}
	}

	return true;
}
