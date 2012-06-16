#include "ovpCChannelSelector.h"

#include <system/Memory.h>

#include <iostream>
#include <sstream>
#include <algorithm>

using namespace OpenViBE;
using namespace OpenViBE::Plugins;
using namespace OpenViBE::Kernel;
using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessing;
using namespace OpenViBEToolkit;
using namespace std;

void CChannelSelector::setChannelCount(const uint32 ui32ChannelCount)
{
	//gets the channel count
	m_pSignalDescription->m_ui32ChannelCount = ui32ChannelCount;
}

void CChannelSelector::setChannelName(const uint32 ui32ChannelIndex, const char* sChannelName)
{
	//adds this name to the list
	std::string l_sChannelName(sChannelName);
	std::string::size_type i=l_sChannelName.length();
	boolean l_bFinished=false;
	while(i!=0 && !l_bFinished)
	{
		if(l_sChannelName[i-1]==' ')
		{
			i--;
		}
		else
		{
			l_bFinished=true;
		}
	}

	m_vChannelNames.push_back(l_sChannelName.substr(0, i));
}

void CChannelSelector::setSampleCountPerBuffer(const uint32 ui32SampleCountPerBuffer)
{
	//if the channels are selected by their indexes
	if(m_bSelectionbyIndex)
	{
		uint32 l_ui32CurrentChannel=0;
		//for all the channels selected by the user
		for(unsigned int i=0 ; i<m_vSelectedChannelIndexes.size() ; i++)
		{
			l_ui32CurrentChannel = m_vSelectedChannelIndexes[i];
			//check if it exists
			if(l_ui32CurrentChannel>=0 && l_ui32CurrentChannel<m_pSignalDescription->m_ui32ChannelCount)
			{
				//if it does, adds it to the list of kept channels
				m_vChannelsToKeep.push_back(m_vSelectedChannelIndexes[i]);
				m_pSignalDescription->m_oChannelName.push_back(m_vChannelNames[m_vSelectedChannelIndexes[i]]);
			}
		}
	}
	//if the selection is done by names
	else
	{
		for(unsigned int i=0 ; i<m_vSelectedChannelNames.size() ; i++)
		{
			//check if the current name is indeed a channel's name
			for(uint32 j=0 ; j<m_vChannelNames.size() ; j++)
			{
				if(m_vSelectedChannelNames[i] == m_vChannelNames[j])
				{
					//if is is, adds it to the list of kept channels
					m_pSignalDescription->m_oChannelName.push_back(m_vSelectedChannelNames[i].c_str());
					m_vChannelsToKeep.push_back(j);
				}
			}
		}
	}

	//the new channel count is the number of selected channels found in the input stream
	m_pSignalDescription->m_ui32ChannelCount = m_vChannelsToKeep.size();

	//check if we have at least one channel
	if(m_pSignalDescription->m_ui32ChannelCount == 0)
	{
		getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Warning <<"0 channels selected!\n";
		//stop output in this case
	}
	else
	{
		//gets the sample count
		m_pSignalDescription->m_ui32SampleCount = ui32SampleCountPerBuffer;

		//the matrix buffer hasn't been allocated yet, allocate it
		if(!m_pMatrixBuffer)
		{
			m_ui64MatrixBufferSize = m_pSignalDescription -> m_ui32SampleCount * m_pSignalDescription -> m_ui32ChannelCount;

			m_pMatrixBuffer = new EBML::float64[(size_t)m_ui64MatrixBufferSize];
		}

		//we have everything needed to send the stream header
		m_pSignalOutputWriterHelper->setSamplingRate(m_pSignalDescription->m_ui32SamplingRate);
		m_pSignalOutputWriterHelper->setChannelCount(m_pSignalDescription->m_ui32ChannelCount);

		for(uint32 i=0 ; i<m_pSignalDescription->m_ui32ChannelCount ; i++)
		{
			m_pSignalOutputWriterHelper->setChannelName(i, m_pSignalDescription->m_oChannelName[i].c_str());
		}

		m_pSignalOutputWriterHelper->setSampleCountPerBuffer(m_pSignalDescription->m_ui32SampleCount);
		m_pSignalOutputWriterHelper->setSampleBuffer(m_pMatrixBuffer);

		m_pSignalOutputWriterHelper->writeHeader(*m_pWriter);

		getBoxAlgorithmContext()->getDynamicBoxContext()->markOutputAsReadyToSend(0, m_ui64LastChunkStartTime, m_ui64LastChunkEndTime);
	}
}

void CChannelSelector::setSamplingRate(const uint32 ui32SamplingFrequency)
{
	m_pSignalDescription->m_ui32SamplingRate = ui32SamplingFrequency;
}

void CChannelSelector::setSampleBuffer(const float64* pBuffer)
{
	//Don't send anything if no channels are selected
	if(m_pSignalDescription->m_ui32ChannelCount!=0)
	{
		const EBML::uint8* l_pSourceMatrix = reinterpret_cast<const EBML::uint8 *>(pBuffer);
		uint8* l_pDestinationMatrix = reinterpret_cast<EBML::uint8 *>(m_pMatrixBuffer);

		//size of a sample block (for one channel) in bytes
		uint64 l_ui64SampleBlockSize = m_pSignalDescription->m_ui32SampleCount * sizeof(EBML::float64);

		//for each selected channel found in the input stream
		for(uint32 i=0 ; i<m_vChannelsToKeep.size() ; i++)
		{
			//gets its "index" in the input stream's channel list
			uint32 l_ui32CurrentChannelIndex = m_vChannelsToKeep[i];

			//copy the sample block of the current selected channel
			System::Memory::copy(l_pDestinationMatrix + (i*l_ui64SampleBlockSize), l_pSourceMatrix+(l_ui32CurrentChannelIndex*l_ui64SampleBlockSize), l_ui64SampleBlockSize);
		}

		//the matrix is ready to be sent
		m_pSignalOutputWriterHelper->writeBuffer(*m_pWriter);
		getBoxAlgorithmContext()->getDynamicBoxContext()->markOutputAsReadyToSend(0, m_ui64LastChunkStartTime, m_ui64LastChunkEndTime);
	}
}

void CChannelSelector::writeSignalOutput(const void* pBuffer, const EBML::uint64 ui64BufferSize)
{
	appendOutputChunkData<0>(pBuffer, ui64BufferSize);
}

CChannelSelector::CChannelSelector(void)
	: m_pReader(NULL),
	m_pSignalReaderCallBack(NULL),
	m_ui64LastChunkStartTime(0),
	m_ui64LastChunkEndTime(0),
	m_bCurrentDimension(false),
	m_pWriter(NULL),
	m_oSignalOutputWriterCallbackProxy(
		*this,
		&CChannelSelector::writeSignalOutput),
	m_pSignalOutputWriterHelper(NULL),
	m_bSelectionbyIndex(false),
	m_pSignalDescription(NULL),
	m_ui64MatrixBufferSize(0),
	m_pMatrixBuffer(NULL)
{
}

void CChannelSelector::release(void)
{
}

boolean CChannelSelector::initialize()
{
	//reads the plugin settings
	CString l_sSettings;
	getBoxAlgorithmContext()->getStaticBoxContext()->getSettingValue(1, l_sSettings);
	if(l_sSettings == CString("true"))
	{
		m_bSelectionbyIndex = true;
	}
	else
	{
		m_bSelectionbyIndex = false;
	}

	//reads the plugin settings
	getBoxAlgorithmContext()->getStaticBoxContext()->getSettingValue(0, l_sSettings);

	istringstream l_sChannelList(static_cast<const char *>(l_sSettings));

	string l_sCurrentChannelId;

	//parses the configuration string
	while(!l_sChannelList.eof())
	{
		getline(l_sChannelList, l_sCurrentChannelId, OV_Value_EnumeratedStringSeparator);

		//if the channels are selected by their names
		if(!m_bSelectionbyIndex)
		{
			//We don't want empty strings (causes problems when comparing with input stream's channel's labels)
			if(l_sCurrentChannelId.length() != 0)
			{
				m_vSelectedChannelNames.push_back(l_sCurrentChannelId);
			}
		}
		else
		{
			m_vSelectedChannelIndexes.push_back(atol(l_sCurrentChannelId.c_str()));
		}
	}

	//initialises the signal description structure
	m_pSignalDescription = new CSignalDescription();

	// Prepares EBML reader
	m_pSignalReaderCallBack = createBoxAlgorithmSignalInputReaderCallback(*this);
	m_pReader=EBML::createReader(*m_pSignalReaderCallBack);

	// Prepares EBML writer
	m_pSignalOutputWriterHelper=createBoxAlgorithmSignalOutputWriter();
	m_pWriter=EBML::createWriter(m_oSignalOutputWriterCallbackProxy);

	return true;
}

boolean CChannelSelector::uninitialize()
{
	if(m_pMatrixBuffer)
	{
		delete[] m_pMatrixBuffer;
	}

	// Cleans up EBML writer
	m_pWriter->release();
	m_pWriter=NULL;

	releaseBoxAlgorithmSignalOutputWriter(m_pSignalOutputWriterHelper);
	m_pSignalOutputWriterHelper=NULL;

	// Cleans up EBML reader
	releaseBoxAlgorithmSignalInputReaderCallback(m_pSignalReaderCallBack);

	m_pReader->release();
	m_pReader=NULL;

	delete m_pSignalDescription;
	m_pSignalDescription = NULL;

	return true;
}

boolean CChannelSelector::processInput( uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CChannelSelector::process()
{
	IBoxIO* l_pBoxIO = getBoxAlgorithmContext()->getDynamicBoxContext();

	// Process input data
	for(uint32 i=0; i<l_pBoxIO->getInputChunkCount(0); i++)
	{

		uint64 l_ui64ChunkSize;
		const uint8* l_pBuffer;
		l_pBoxIO->getInputChunk(0, i, m_ui64LastChunkStartTime, m_ui64LastChunkEndTime, l_ui64ChunkSize, l_pBuffer);
		l_pBoxIO->markInputAsDeprecated(0, i);
		m_pReader->processData(l_pBuffer, l_ui64ChunkSize);
	}

	return true;
}
