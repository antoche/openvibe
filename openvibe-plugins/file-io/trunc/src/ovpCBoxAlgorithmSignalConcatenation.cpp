#include "ovpCBoxAlgorithmSignalConcatenation.h"

#include <system/Memory.h>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::FileIO;

using namespace std;

boolean CBoxAlgorithmSignalConcatenation::initialize(void)
{
	m_vSignalChunkBuffers.resize(this->getStaticBoxContext().getInputCount() >> 1);
	
	m_ui64TimeOut = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	m_ui64TimeOut = m_ui64TimeOut << 32;
	this->getLogManager() << LogLevel_Info << "Timeout set to "<< time64(m_ui64TimeOut) <<".\n";
	for(uint32 i = 0; i < this->getStaticBoxContext().getInputCount(); i+=2)
	{
		m_vEndOfFileStimulations.push_back(FSettingValueAutoCast(*this->getBoxAlgorithmContext(), (i>>2)+1));
		m_vEndOfFileReached.push_back(false);
		m_vFileEndTimes.push_back(0);
		
	}

	for(uint32 i = 0; i < this->getStaticBoxContext().getInputCount(); i+=2)
	{
		OpenViBEToolkit::TStimulationDecoder < CBoxAlgorithmSignalConcatenation > * l_pStimDecoder = new OpenViBEToolkit::TStimulationDecoder < CBoxAlgorithmSignalConcatenation >(*this);
		OpenViBEToolkit::TSignalDecoder < CBoxAlgorithmSignalConcatenation > * l_pSignalDecoder = new OpenViBEToolkit::TSignalDecoder < CBoxAlgorithmSignalConcatenation >(*this);
		
		m_vSignalDecoders.push_back(l_pSignalDecoder);
		m_vStimulationDecoders.push_back(l_pStimDecoder);
		CStimulationSet * l_pStimSet = new CStimulationSet();
		m_vStimulationSets.push_back(l_pStimSet);
	}

	m_oStimulationEncoder.initialize(*this);
	m_oStimulationEncoder.getInputStimulationSet().setReferenceTarget(m_vStimulationDecoders[0]->getOutputStimulationSet());

	m_oSignalEncoder.initialize(*this);
	m_oSignalEncoder.getInputSamplingRate().setReferenceTarget(m_vSignalDecoders[0]->getOutputSamplingRate());
	m_oSignalEncoder.getInputMatrix().setReferenceTarget(m_vSignalDecoders[0]->getOutputMatrix());

	m_oTriggerEncoder.initialize(*this);
	m_oTriggerEncoder.getInputStimulationSet().setReferenceTarget(m_vStimulationDecoders[0]->getOutputStimulationSet());

	m_ui32HeaderReceivedCount = 0;
	m_bHeaderSent = false;
	m_bEndSent = false;
	m_bStimHeaderSent = false;
	m_bConcatenationFinished = false;
	m_bResynchroDone = false;

	m_sState.ui32CurrentFileIndex        = 0;
	m_sState.ui32CurrentChunkIndex       = 0;
	m_sState.ui32CurrentStimulationIndex = 0;

	m_ui64TriggerDate = 0;
	m_ui64LastChunkStartTime = 0;
	m_ui64LastChunkEndTime = 0;

	return true;
}
/*******************************************************************************/

boolean CBoxAlgorithmSignalConcatenation::uninitialize(void)
{
	m_oStimulationEncoder.uninitialize();
	m_oSignalEncoder.uninitialize();
	m_oTriggerEncoder.uninitialize();

	for(uint32 i = 0; i < m_vSignalDecoders.size(); i++)
	{
		m_vSignalDecoders[i]->uninitialize();
		m_vStimulationDecoders[i]->uninitialize();
		delete m_vSignalDecoders[i];
		delete m_vStimulationDecoders[i];
	}

	for(uint32 i = 0; i < m_vSignalChunkBuffers.size(); i++)
	{
		for(uint32 j = 0; j < m_vSignalChunkBuffers[i].size(); j++)
		{
			delete m_vSignalChunkBuffers[i][j].m_pMemoryBuffer;
		}
	}
	for(uint32 i = 0; i < m_vStimulationSets.size(); i++)
	{
		delete m_vStimulationSets[i];
	}
	
	return true;
}
/*******************************************************************************/

boolean CBoxAlgorithmSignalConcatenation::processInput(uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}
/*******************************************************************************/

boolean CBoxAlgorithmSignalConcatenation::processClock(CMessageClock& rMessageClock)
{
	if(!m_bHeaderSent || m_bConcatenationFinished)
	{
		return true;
	}

	uint64 l_ui64CurrentPlayerTime = this->getPlayerContext().getCurrentTime();
	
	for(uint32 i = 0; i < m_vFileEndTimes.size(); i++)
	{
		if(!m_vEndOfFileReached[i] && l_ui64CurrentPlayerTime > m_vFileEndTimes[i] + m_ui64TimeOut)
		{
			m_vEndOfFileReached[i] = true;
			this->getLogManager() << LogLevel_Info << "File #" << i+1 << "/" << (this->getStaticBoxContext().getInputCount()/2) << " has timed out (effective end time: "<< time64(m_vFileEndTimes[i]) <<").\n";
		}
	}

	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	return true;
}
/*******************************************************************************/

boolean CBoxAlgorithmSignalConcatenation::process(void)
{
	
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

	//SIGNAL INPUTS
	uint64 l_ui64SamplingFrequency;
	uint32 l_ui32ChannelCount;
	for(uint32 input = 0 ; input < l_rStaticBoxContext.getInputCount() ; input+=2)
	{
		if(! m_vEndOfFileReached[input>>1])
		{
			for(uint32 chunk = 0; chunk < l_rDynamicBoxContext.getInputChunkCount(input); chunk++)
			{
				if(m_ui32HeaderReceivedCount < l_rStaticBoxContext.getInputCount()>>1)
				{
					m_vSignalDecoders[input>>1]->decode(input,chunk);
					
					if(m_vSignalDecoders[input>>1]->isHeaderReceived())
					{
						m_ui32HeaderReceivedCount++;

						l_ui64SamplingFrequency = m_vSignalDecoders[input>>1]->getOutputSamplingRate();
						l_ui32ChannelCount = m_vSignalDecoders[input>>1]->getOutputMatrix()->getDimensionSize(0);
						m_ui32SampleCountPerBuffer = m_vSignalDecoders[input>>1]->getOutputMatrix()->getDimensionSize(1);
						if(input == 0)
						{
							this->getLogManager() << LogLevel_Info << "Common sampling rate is " << l_ui64SamplingFrequency << ", channel count is " << l_ui32ChannelCount << " and sample count per buffer is " << m_ui32SampleCountPerBuffer <<".\n";
							m_oSignalEncoder.encodeHeader(0);
							l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_rDynamicBoxContext.getInputChunkStartTime(input,chunk), l_rDynamicBoxContext.getInputChunkEndTime(input,chunk));
							m_bHeaderSent = true;
						}
						else
						{
							if(m_vSignalDecoders[input>>1]->getOutputSamplingRate() != l_ui64SamplingFrequency)
							{
								this->getLogManager() << LogLevel_Error << "File #" << (input>>1)+1 << "/" << (l_rStaticBoxContext.getInputCount()/2) << " has a different sampling rate ("<< m_vSignalDecoders[input>>1]->getOutputSamplingRate() <<"Hz) than previous file(s) ("<< l_ui64SamplingFrequency <<"Hz).\n";
								return false;
							}
							if(m_vSignalDecoders[input>>1]->getOutputMatrix()->getDimensionSize(0) != l_ui32ChannelCount)
							{
								this->getLogManager() << LogLevel_Error << "File #" << (input>>1)+1 << "/" << (l_rStaticBoxContext.getInputCount()/2) << " has a different channel count ("<< m_vSignalDecoders[input>>1]->getOutputMatrix()->getDimensionSize(0) <<") than previous file(s) ("<< l_ui32ChannelCount <<").\n";
								return false;
							}
							if(m_vSignalDecoders[input>>1]->getOutputMatrix()->getDimensionSize(1) != m_ui32SampleCountPerBuffer)
							{
								this->getLogManager() << LogLevel_Error << "File #" << (input>>1)+1 << "/" << (l_rStaticBoxContext.getInputCount()/2) << " has a different sample count per buffer ("<< m_vSignalDecoders[input>>1]->getOutputMatrix()->getDimensionSize(1) <<") than previous file(s) ("<< m_ui32SampleCountPerBuffer <<").\n";
								return false;
							}
						}
					}
					else if(m_vSignalDecoders[input>>1]->isBufferReceived())
					{
						IMemoryBuffer * l_pBuffer = new CMemoryBuffer();
						l_pBuffer->setSize(l_rDynamicBoxContext.getInputChunk(input,chunk)->getSize(),true);
						System::Memory::copy(
							l_pBuffer->getDirectPointer(),
							l_rDynamicBoxContext.getInputChunk(input,chunk)->getDirectPointer(),
							l_pBuffer->getSize());
						Chunk l_oChunk;
						l_oChunk.m_pMemoryBuffer = l_pBuffer;
						l_oChunk.m_ui64StartTime = l_rDynamicBoxContext.getInputChunkStartTime(input,chunk);
						l_oChunk.m_ui64EndTime = l_rDynamicBoxContext.getInputChunkEndTime(input,chunk);
						m_vSignalChunkBuffers[input>>1].push_back(l_oChunk);

						m_vFileEndTimes[input>>1] = l_rDynamicBoxContext.getInputChunkEndTime(input,chunk);
						
						l_rDynamicBoxContext.markInputAsDeprecated(input, chunk);
					}
				}
				else
				{
					if(m_vEndOfFileReached[input>>1])
					{
						//just discard it
						l_rDynamicBoxContext.markInputAsDeprecated(input, chunk);
					}
					else
					{
						IMemoryBuffer * l_pBuffer = new CMemoryBuffer();
						l_pBuffer->setSize(l_rDynamicBoxContext.getInputChunk(input,chunk)->getSize(),true);
						System::Memory::copy(
							l_pBuffer->getDirectPointer(),
							l_rDynamicBoxContext.getInputChunk(input,chunk)->getDirectPointer(),
							l_pBuffer->getSize());
						Chunk l_oChunk;
						l_oChunk.m_pMemoryBuffer = l_pBuffer;
						l_oChunk.m_ui64StartTime = l_rDynamicBoxContext.getInputChunkStartTime(input,chunk);
						l_oChunk.m_ui64EndTime = l_rDynamicBoxContext.getInputChunkEndTime(input,chunk);
						m_vSignalChunkBuffers[input>>1].push_back(l_oChunk);


						m_vFileEndTimes[input>>1] = l_rDynamicBoxContext.getInputChunkEndTime(input,chunk);
						
						l_rDynamicBoxContext.markInputAsDeprecated(input, chunk);
					}
				}
			}
		}
	}

	//STIMULATION INPUTS
	for(uint32 input = 1 ; input < l_rStaticBoxContext.getInputCount() ; input+=2)
	{
		for(uint32 chunk = 0; chunk < l_rDynamicBoxContext.getInputChunkCount(input); chunk++)
		{
			m_vStimulationDecoders[input>>1]->decode(input,chunk);
			if(m_vStimulationDecoders[input>>1]->isHeaderReceived() && !m_bStimHeaderSent)
			{
				m_oStimulationEncoder.encodeHeader(1);
				l_rDynamicBoxContext.markOutputAsReadyToSend(1,l_rDynamicBoxContext.getInputChunkStartTime(input,chunk),l_rDynamicBoxContext.getInputChunkEndTime(input,chunk));
				m_oTriggerEncoder.encodeHeader(2);
				l_rDynamicBoxContext.markOutputAsReadyToSend(2,l_rDynamicBoxContext.getInputChunkStartTime(input,chunk),l_rDynamicBoxContext.getInputChunkEndTime(input,chunk));
				m_bStimHeaderSent = true;
			}
			if(m_vStimulationDecoders[input>>1]->isEndReceived() && !m_bEndSent)
			{
				m_oStimulationEncoder.encodeEnd(1);
				l_rDynamicBoxContext.markOutputAsReadyToSend(1,l_rDynamicBoxContext.getInputChunkStartTime(input,chunk),l_rDynamicBoxContext.getInputChunkEndTime(input,chunk));
				m_oTriggerEncoder.encodeEnd(2);
				l_rDynamicBoxContext.markOutputAsReadyToSend(2,l_rDynamicBoxContext.getInputChunkStartTime(input,chunk),l_rDynamicBoxContext.getInputChunkEndTime(input,chunk));
				m_oSignalEncoder.encodeEnd(0);
				l_rDynamicBoxContext.markOutputAsReadyToSend(0,l_rDynamicBoxContext.getInputChunkStartTime(input,chunk),l_rDynamicBoxContext.getInputChunkEndTime(input,chunk));
				m_bEndSent = true;
			}
			if(m_vStimulationDecoders[input>>1]->isBufferReceived())
			{
				IStimulationSet * l_pStimSet = m_vStimulationDecoders[input>>1]->getOutputStimulationSet();
				for(uint32 stim = 0; stim < l_pStimSet->getStimulationCount(); stim++)
				{
					if(l_pStimSet->getStimulationIdentifier(stim) == m_vEndOfFileStimulations[input>>1])
					{
						this->getLogManager() << LogLevel_Info << "File #" << (input>>1)+1 << "/" << (l_rStaticBoxContext.getInputCount()/2) << " is finished (end time: "<< time64(m_vFileEndTimes[input>>1]) <<"). Any more signal chunk will be discarded.\n";
						m_vEndOfFileReached[input>>1] = true;
					}
					
					m_vStimulationSets[input>>1]->appendStimulation(
						l_pStimSet->getStimulationIdentifier(stim),
						l_pStimSet->getStimulationDate(stim),
						l_pStimSet->getStimulationDuration(stim));
							
				}
			}
		}
	}

	boolean l_bShouldConcatenate = true;
	for(uint32 i = 0; i < m_vEndOfFileReached.size(); i++)
	{
		l_bShouldConcatenate &= m_vEndOfFileReached[i];
	}

	if(l_bShouldConcatenate && !m_bConcatenationFinished)
	{
		if(!this->concate())
		{
			// concatenation not finished, we will resume on next process
			return true;
		}
		else
		{
			m_oTriggerEncoder.getInputStimulationSet()->appendStimulation(OVTK_StimulationId_EndOfFile, this->getPlayerContext().getCurrentTime(), 0);
			m_oTriggerEncoder.encodeBuffer(2);
			l_rDynamicBoxContext.markOutputAsReadyToSend(2,this->getPlayerContext().getCurrentTime(),this->getPlayerContext().getCurrentTime());
			m_bConcatenationFinished = true;
		}
	}

	return true;
}


boolean CBoxAlgorithmSignalConcatenation::concate(void)
{
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();
	if(!m_bResynchroDone)
	{
		this->getLogManager() << LogLevel_Info << "Concatenation in progress...\n";
		
		
		this->getLogManager() << LogLevel_Trace << "Resynchronizing Chunks ...\n";

		uint64 l_ui64Offset = m_vFileEndTimes[0];
		// m_vStimulationSets and m_vSignalChunkBuffers should have the same size (file #)
		for(uint32 i = 1; i < m_vStimulationSets.size(); i++)
		{
			for(uint32 j = 0; j < m_vStimulationSets[i]->getStimulationCount(); j++)
			{
				uint64 l_ui64SynchronizedDate = m_vStimulationSets[i]->getStimulationDate(j) + l_ui64Offset;
				//this->getLogManager() << LogLevel_Info << "Resynchronizing stim ["<<m_vStimulations[i][j].first<<"] from time ["<<m_vStimulations[i][j].second<<"] to ["<<l_ui64SynchronizedDate<<"]\n";
				m_vStimulationSets[i]->setStimulationDate(j,l_ui64SynchronizedDate);
			}
			for(uint32 j = 0; j < m_vSignalChunkBuffers[i].size(); j++)
			{
				m_vSignalChunkBuffers[i][j].m_ui64StartTime += l_ui64Offset;
				m_vSignalChunkBuffers[i][j].m_ui64EndTime += l_ui64Offset;
			}
			l_ui64Offset = l_ui64Offset + m_vFileEndTimes[i];
		}
		
		this->getLogManager() << LogLevel_Trace << "Resynchronization finished.\n";
		m_bResynchroDone = true;
	}
	
	for(uint32 i = m_sState.ui32CurrentFileIndex; i < m_vSignalChunkBuffers.size(); i++)
	{
		for(uint32 j = m_sState.ui32CurrentChunkIndex; j < m_vSignalChunkBuffers[i].size(); j++)
		{

			// we write the signal memory buffer
			IMemoryBuffer * l_pBuffer = m_vSignalChunkBuffers[i][j].m_pMemoryBuffer;
			IMemoryBuffer * l_pOutputMemoryBuffer = l_rDynamicBoxContext.getOutputChunk(0);
			l_pOutputMemoryBuffer->setSize(l_pBuffer->getSize(), true);
			System::Memory::copy(
				l_pOutputMemoryBuffer->getDirectPointer(),
				l_pBuffer->getDirectPointer(),
				l_pBuffer->getSize());
			l_rDynamicBoxContext.markOutputAsReadyToSend(0,m_vSignalChunkBuffers[i][j].m_ui64StartTime,m_vSignalChunkBuffers[i][j].m_ui64EndTime);

			IStimulationSet * l_pStimSet = m_oStimulationEncoder.getInputStimulationSet();
			l_pStimSet->clear();
			boolean l_bStimFound = false;
			boolean l_bOverTime = false;
			for(uint32 s = m_sState.ui32CurrentStimulationIndex; s < m_vStimulationSets[i]->getStimulationCount() && !l_bOverTime; s++)
			{
				if(m_vSignalChunkBuffers[i][j].m_ui64StartTime <= m_vStimulationSets[i]->getStimulationDate(s))
				{
					if(m_vStimulationSets[i]->getStimulationDate(s) < m_vSignalChunkBuffers[i][j].m_ui64EndTime)
					{
						l_pStimSet->appendStimulation(m_vStimulationSets[i]->getStimulationIdentifier(s),
							m_vStimulationSets[i]->getStimulationDate(s),
							m_vStimulationSets[i]->getStimulationDuration(s));
						l_bStimFound = true;
						
						m_sState.ui32CurrentFileIndex           = i;   // file may be unfinished
						m_sState.ui32CurrentChunkIndex          = j+1; // chunk is finished, goto next
						m_sState.ui32CurrentStimulationIndex    = s+1; // goto next stim

						//this->getLogManager() << LogLevel_Info << "Adding stimulation at date [" << time64(m_vStimulationSets[i]->getStimulationDate(s)) << "]\n";
					}
					else
					{
						l_bOverTime = true;
					}
				}
					
				// we will release the box so the kernel can send every pending chunks
				// saving the current state :
				
				
				
				//	//this->getLogManager() << LogLevel_Info << "Appending stim ["<<m_vStimulations[i][s].first<<"] with date ["<<m_vStimulations[i][s].second<<"]\n";
				//	m_vStimulations[i][s].second = -1; // should not be needed...
				//	l_bStimFound = true;

				//	m_sState.ui32CurrentBufferIndex      = i;
				//	m_sState.ui32CurrentChunkIndex       = j+1;
				//	m_sState.ui32CurrentStimulationIndex = s+1;
				//	m_sState.ui32CurrentStartTime        = l_ui64CurrentEndTime;
				//	m_sState.ui32CurrentOffset           = l_ui64Offset;
				//}
			}

			// then the stim memory buffer even if it is empty
			m_oStimulationEncoder.encodeBuffer(1);
			l_rDynamicBoxContext.markOutputAsReadyToSend(1,m_vSignalChunkBuffers[i][j].m_ui64StartTime,m_vSignalChunkBuffers[i][j].m_ui64EndTime);
			//this->getLogManager() << LogLevel_Info << "sending signal chunk from ["<<time64(l_ui64CurrentStartTime)<<"] to ["<<time64(l_ui64CurrentEndTime)<<"]\n";
			
			if(l_bStimFound)
			{
				// we stop the concatenation here, to let the kernel send the chunks.
				// we will resume at next process()
				// this behaviour is implemented to avoid the Generic Stream R/W bug (all stims written at the end of the concatenation file)
				return false;
			}
		}
		this->getLogManager() << LogLevel_Info << "File #" << i+1 <<" Finished.\n";
		m_sState.ui32CurrentChunkIndex = 0;
		m_sState.ui32CurrentStimulationIndex = 0;
	}
		
	this->getLogManager() << LogLevel_Info << "Concatenation finished !\n";
	
	return true;

}
