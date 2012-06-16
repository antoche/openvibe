#include "ovpCGenericNetworkAcquisition.h"

#include <system/Memory.h>

#include <iostream>
#include <cstdlib>

using namespace OpenViBE;
using namespace OpenViBE::Plugins;
using namespace OpenViBE::Kernel;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Acquisition;

using namespace OpenViBEToolkit;

using namespace std;

namespace OpenViBEPlugins
{
	namespace Acquisition
	{

EBML::boolean CGenericNetworkAcquisition::readerIsMasterChild(const EBML::CIdentifier& rIdentifier)
{
	if(rIdentifier== OVTK_NodeId_Acquisition_Header
	||rIdentifier == OVTK_NodeId_Acquisition_AcquisitionInformation
	||rIdentifier == OVTK_NodeId_Acquisition_ChannelNames
	||rIdentifier == OVTK_NodeId_Acquisition_Buffer
	||rIdentifier == OVTK_NodeId_Acquisition_Samples)
	{
		return true;
	}
	return false;
}

void CGenericNetworkAcquisition::readerOpenChild(const EBML::CIdentifier& rIdentifier)
{
	m_oCurrentIdentifier=rIdentifier;
}

void CGenericNetworkAcquisition::readerProcessChildData(const void* pBuffer, const EBML::uint64 ui64BufferSize)
{
	//Experiment description
	if(m_oCurrentIdentifier==OVTK_NodeId_Acquisition_ExperimentId)
	{
		m_pExperimentInfoHeader->m_ui64ExperimentId = m_pReaderHelper->getUIntegerFromChildData(pBuffer, ui64BufferSize);
	}

	if(m_oCurrentIdentifier==OVTK_NodeId_Acquisition_SubjectAge)
	{
		m_pExperimentInfoHeader->m_ui64SubjectAge = m_pReaderHelper->getUIntegerFromChildData(pBuffer, ui64BufferSize);
	}

	if(m_oCurrentIdentifier==OVTK_NodeId_Acquisition_SubjectSex)
	{
		m_pExperimentInfoHeader->m_ui64SubjectSex = m_pReaderHelper->getUIntegerFromChildData(pBuffer, ui64BufferSize);

		//The header is ready to send
		m_pExperimentInfoHeader -> m_bReadyToSend = true;
	}

	//Signal description
	if(m_oCurrentIdentifier==OVTK_NodeId_Acquisition_ChannelCount)
	{
		m_pSignalDescription -> m_ui32ChannelCount = (uint32)m_pReaderHelper->getUIntegerFromChildData(pBuffer, ui64BufferSize);

		m_pSignalDescription -> m_pChannelName.resize(m_pSignalDescription -> m_ui32ChannelCount);
	}

	if(m_oCurrentIdentifier==OVTK_NodeId_Acquisition_SamplingFrequency)
	{
		m_pSignalDescription -> m_ui32SamplingRate = (uint32)m_pReaderHelper->getUIntegerFromChildData(pBuffer, ui64BufferSize);
	}

	if(m_oCurrentIdentifier==OVTK_NodeId_Acquisition_ChannelName)
	{
		//copy the channel name
		m_pSignalDescription->m_pChannelName[m_pSignalDescription->m_ui32CurrentChannel] = m_pReaderHelper->getASCIIStringFromChildData(pBuffer, ui64BufferSize);

		m_pSignalDescription->m_ui32CurrentChannel++;

	}

	if(m_oCurrentIdentifier==OVTK_NodeId_Acquisition_SamplesPerChannelCount)
	{
		m_pSignalDescription -> m_ui32SampleCount = (uint32)m_pReaderHelper->getUIntegerFromChildData(pBuffer, ui64BufferSize);

		//if the matrix buffer hasn't been allocated yet
		if(!m_pMatrixBuffer)
		{
			m_ui64MatrixBufferSize = m_pSignalDescription -> m_ui32SampleCount * m_pSignalDescription->m_ui32ChannelCount;

			//we now have all the information we needed to allocate the matrix buffer
			m_pMatrixBuffer = new EBML::float64[(size_t)m_ui64MatrixBufferSize];
		}

		m_ui32CurrentChannel=0;

		if(!m_bSignalDescriptionSent)
		{
			IDynamicBoxContext * l_pDynamicBoxContext = getBoxAlgorithmContext()->getDynamicBoxContext();

			m_pSignalOutputWriterHelper->setSamplingRate(m_pSignalDescription->m_ui32SamplingRate);
			m_pSignalOutputWriterHelper->setChannelCount(m_pSignalDescription->m_ui32ChannelCount);

			for(uint32 i=0 ; i<m_pSignalDescription->m_ui32ChannelCount ; i++)
			{
				m_pSignalOutputWriterHelper->setChannelName(i, m_pSignalDescription->m_pChannelName[i].c_str());
			}

			m_pSignalOutputWriterHelper->setSampleCountPerBuffer(m_pSignalDescription->m_ui32SampleCount);
			m_pSignalOutputWriterHelper->setSampleBuffer(m_pMatrixBuffer);

			m_pSignalOutputWriterHelper->writeHeader(*m_pWriter[GenericNetworkAcquisition_SignalOutput]);

			m_bSignalDescriptionSent=true;
			l_pDynamicBoxContext->markOutputAsReadyToSend(GenericNetworkAcquisition_SignalOutput, 0, 0);
		}

	}

	//matrix
	if(m_oCurrentIdentifier==OVTK_NodeId_Acquisition_SampleBlock)
	{
		const EBML::float32  * l_pSourceMatrix = static_cast<const EBML::float32*>(pBuffer);

		//copy the buffer
		for(uint64 i=0 ; i<(ui64BufferSize/sizeof(EBML::float32)) ; i++)
		{
			m_pMatrixBuffer[(m_ui32CurrentChannel*m_pSignalDescription->m_ui32SampleCount)+i] = static_cast<EBML::float64>(l_pSourceMatrix[i]);
		}

		m_ui32CurrentChannel++;

		//If a whole matrix has been read
		if(m_ui32CurrentChannel == m_pSignalDescription->m_ui32ChannelCount)
		{

			m_ui32CurrentChannel = 0;

			IDynamicBoxContext * l_pDynamicBoxContext = getBoxAlgorithmContext()->getDynamicBoxContext();

			m_pSignalOutputWriterHelper->writeBuffer(*m_pWriter[GenericNetworkAcquisition_SignalOutput]);

			//increases total sample counter
			m_ui32SentSampleCount += m_pSignalDescription->m_ui32SampleCount;

			OpenViBE::uint64 l_ui64StartTime=(((uint64)(m_ui32SentSampleCount - m_pSignalDescription->m_ui32SampleCount))<<32)/m_pSignalDescription->m_ui32SamplingRate;

			OpenViBE::uint64 l_ui64EndTime  =(((uint64)(m_ui32SentSampleCount))<<32)/m_pSignalDescription->m_ui32SamplingRate;

			l_pDynamicBoxContext->markOutputAsReadyToSend(GenericNetworkAcquisition_SignalOutput, l_ui64StartTime, l_ui64EndTime);

			static uint64 l_ui64CurrentDate=0;
			if(l_ui64CurrentDate>>32 != m_ui64CurrentDate>>32)
			{
				l_ui64CurrentDate=m_ui64CurrentDate;
				uint32 l_ui32SentSampleCount=(uint32)((m_pSignalDescription->m_ui32SamplingRate*m_ui64CurrentDate)>>32);
				// m_ui32SentSampleCount
				getBoxAlgorithmContext()->getPlayerContext()->getLogManager()
					<< LogLevel_Trace
					<< "Sample count jittering : " << (int32)(m_ui32SentSampleCount-l_ui32SentSampleCount)
					<< " (exact values are " << m_ui32SentSampleCount << ":" << l_ui32SentSampleCount << ")\n";
			}
		}
	}

	/*
	//Stimulations
	if(m_oCurrentIdentifier==)
	{
	}
	*/

}

void CGenericNetworkAcquisition::readerCloseChild()
{

}

boolean CGenericNetworkAcquisitionDesc::getBoxPrototype(Kernel::IBoxProto& rPrototype) const
{
	// Adds box inputs

	// Adds box outputs
	rPrototype.addOutput("Experiment information",	OV_TypeId_ExperimentationInformation);
	rPrototype.addOutput("EEG stream",           OV_TypeId_Signal);
	rPrototype.addOutput("Stimulations",           OV_TypeId_Stimulations);

	// Adds box settings
	rPrototype.addSetting("EEG server hostname", OV_TypeId_String,  "localhost");
	rPrototype.addSetting("EEG server port",     OV_TypeId_Integer, "1024");

	rPrototype.addFlag(BoxFlag_IsDeprecated);

	return true;
}

CGenericNetworkAcquisition::CGenericNetworkAcquisition(void)
	:m_ui32ServerHostPort(0),
	m_pConnectionClient(NULL),
	m_oInputReaderCallbackProxy(
		*this,
		&CGenericNetworkAcquisition::readerIsMasterChild,
		&CGenericNetworkAcquisition::readerOpenChild,
		&CGenericNetworkAcquisition::readerProcessChildData,
		&CGenericNetworkAcquisition::readerCloseChild),
	m_pReader(NULL),
	m_pReaderHelper(NULL),
	m_ui32CurrentChannel(0),
	m_pSignalOutputWriterHelper(NULL),
	m_pExperimentInformationOutputWriterHelper(NULL),
	m_ui64CurrentBufferSize(0),
	m_pExperimentInfoHeader(NULL),
	m_bHeaderSent(false),
	m_pSignalDescription(NULL),
	m_bSignalDescriptionSent(false),
	m_pMatrixBuffer(NULL),
	m_ui64MatrixBufferSize(0),
	m_ui32SentSampleCount(0),
	m_ui64CurrentDate(0)
{
	//just in case ...
	for(int i=0 ; i<3 ; i++)
	{
		m_pWriter[i]=NULL;
	}
}

void CGenericNetworkAcquisition::writeExperimentOutput(const void* pBuffer, const EBML::uint64 ui64BufferSize)
{
	appendOutputChunkData<GenericNetworkAcquisition_ExperimentInfoOutput>(pBuffer, ui64BufferSize);
}

void CGenericNetworkAcquisition::writeSignalOutput(const void* pBuffer, const EBML::uint64 ui64BufferSize)
{
	appendOutputChunkData<GenericNetworkAcquisition_SignalOutput>(pBuffer, ui64BufferSize);
}

void CGenericNetworkAcquisition::writeStimulationOutput(const void* pBuffer, const EBML::uint64 ui64BufferSize)
{
	appendOutputChunkData<GenericNetworkAcquisition_StimulationOutput>(pBuffer, ui64BufferSize);
}

boolean CGenericNetworkAcquisition::initialize()
{
	const IBox* l_pStaticBoxContext=getBoxAlgorithmContext()->getStaticBoxContext();

	// Builds up client connection
	m_pConnectionClient=Socket::createConnectionClient();

	// Prepares EBML reader
	m_pReader=EBML::createReader(m_oInputReaderCallbackProxy);
	m_pReaderHelper=EBML::createReaderHelper();

	//Prepares the proxies
	m_pOutputWriterCallbackProxy[GenericNetworkAcquisition_ExperimentInfoOutput] = new EBML::TWriterCallbackProxy1<OpenViBEPlugins::Acquisition::CGenericNetworkAcquisition>(*this, &CGenericNetworkAcquisition::writeExperimentOutput);

	m_pOutputWriterCallbackProxy[GenericNetworkAcquisition_SignalOutput] = new EBML::TWriterCallbackProxy1<OpenViBEPlugins::Acquisition::CGenericNetworkAcquisition>(*this, &CGenericNetworkAcquisition::writeSignalOutput);

	m_pOutputWriterCallbackProxy[GenericNetworkAcquisition_StimulationOutput] = new EBML::TWriterCallbackProxy1<OpenViBEPlugins::Acquisition::CGenericNetworkAcquisition>(*this, &CGenericNetworkAcquisition::writeStimulationOutput);

	// Prepares experiment information output writer
	m_pWriter[GenericNetworkAcquisition_ExperimentInfoOutput]=EBML::createWriter(*m_pOutputWriterCallbackProxy[GenericNetworkAcquisition_ExperimentInfoOutput]);
	m_pExperimentInformationOutputWriterHelper=createBoxAlgorithmExperimentInformationOutputWriter();

	//Prepares the signal output writer
	m_pWriter[GenericNetworkAcquisition_SignalOutput]=EBML::createWriter(*m_pOutputWriterCallbackProxy[GenericNetworkAcquisition_SignalOutput]);
	m_pSignalOutputWriterHelper=createBoxAlgorithmSignalOutputWriter();

	//Prepares the stimulation output writer
	//m_pStimulationOutputWriterHelper=createBoxAlgorithmStimulationOutputWriter();
	m_pWriter[GenericNetworkAcquisition_StimulationOutput]=EBML::createWriter(*m_pOutputWriterCallbackProxy[GenericNetworkAcquisition_StimulationOutput]);

	// Prepares the data storage structure -
	m_pExperimentInfoHeader = new CExperimentInfoHeader;
	m_pExperimentInfoHeader -> m_bReadyToSend = false;

	m_pSignalDescription = new CSignalDescription;

	// Parses box settings to try connecting to server
	CString l_sServerHostPort;
	l_pStaticBoxContext->getSettingValue(0, m_sServerHostName);
	l_pStaticBoxContext->getSettingValue(1, l_sServerHostPort);
	m_ui32ServerHostPort=atoi(l_sServerHostPort);

	// Tries to connect to server
	m_pConnectionClient->connect(m_sServerHostName, m_ui32ServerHostPort);

	return m_pConnectionClient->isConnected();
}

boolean CGenericNetworkAcquisition::uninitialize()
{
	// Cleans up EBML writers
	for(int i=0 ; i<3 ; i++)
	{
		delete m_pOutputWriterCallbackProxy[i];
		m_pOutputWriterCallbackProxy[i] = NULL;
		m_pWriter[i]->release();
		m_pWriter[i] = NULL;
	}

	releaseBoxAlgorithmExperimentInformationOutputWriter(m_pExperimentInformationOutputWriterHelper);
	m_pExperimentInformationOutputWriterHelper=NULL;

	//desallocate the signal output writer helper
	releaseBoxAlgorithmSignalOutputWriter(m_pSignalOutputWriterHelper);
	m_pSignalOutputWriterHelper=NULL;
;
	//desallocate the stimulation output writer helper
	//releaseBoxAlgorithmSignalOutputWriter(m_pStimulationOutputWriterHelper);
	//m_pStimulationOutputWriterHelper=NULL;

	// Deletes the signal description structure
	delete m_pSignalDescription;

	//delete the matrix buffer
	delete[] m_pMatrixBuffer;

	// Cleans up EBML reader
	m_pReaderHelper->release();
	m_pReaderHelper=NULL;

	m_pReader->release();
	m_pReader=NULL;

	// Cleans up client connection
	if(m_pConnectionClient->isConnected())
	{
		m_pConnectionClient->close();
	}
	m_pConnectionClient->release();
	m_pConnectionClient=NULL;

	return true;
}

boolean CGenericNetworkAcquisition::processClock(CMessageClock& rMessageClock)
{
	m_ui64CurrentDate=rMessageClock.getTime();

	// Checks if connection is correctly established
	if(!m_pConnectionClient->isConnected())
	{
		// In case it is not, abort
		return false;
	}
	else
	{
		// Checks if connection has pending data
		if(m_pConnectionClient->isReadyToReceive())
		{
			// Data are waiting, mark box algorithm as ready to go :)
			getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
		}
	}

	return true;
}

boolean CGenericNetworkAcquisition::process()
{
	uint8 l_pBuffer[1024];
	uint32 l_ui32Received;
	m_ui64CurrentBufferSize=0;

	IBoxIO * l_pDynamicBoxContext = getBoxAlgorithmContext()->getDynamicBoxContext();

	//reset the OutPut chunks
	l_pDynamicBoxContext->setOutputChunkSize(GenericNetworkAcquisition_SignalOutput, 0);
	l_pDynamicBoxContext->setOutputChunkSize(GenericNetworkAcquisition_ExperimentInfoOutput, 0);

	//if the header hasn't been sent yet
	if(!m_bHeaderSent)
	{
		//if the header is ready, send it
		if(m_pExperimentInfoHeader -> m_bReadyToSend)
		{
			m_pExperimentInformationOutputWriterHelper->setValue(IBoxAlgorithmExperimentInformationOutputWriter::Value_ExperimentIdentifier, (uint32)m_pExperimentInfoHeader->m_ui64ExperimentId);

			m_pExperimentInformationOutputWriterHelper->setValue(IBoxAlgorithmExperimentInformationOutputWriter::Value_SubjectAge, (uint32)m_pExperimentInfoHeader->m_ui64SubjectAge);
			m_pExperimentInformationOutputWriterHelper->setValue(IBoxAlgorithmExperimentInformationOutputWriter::Value_SubjectSex, (uint32)m_pExperimentInfoHeader->m_ui64SubjectSex);

			m_pExperimentInformationOutputWriterHelper->writeHeader(*m_pWriter[GenericNetworkAcquisition_ExperimentInfoOutput]);

			// The header has been sent
			m_bHeaderSent = true;
			l_pDynamicBoxContext->markOutputAsReadyToSend(GenericNetworkAcquisition_ExperimentInfoOutput, 0, 0);

			//not needed anymore
			delete m_pExperimentInfoHeader;
			m_pExperimentInfoHeader=NULL;
		}
	}

	while(m_pConnectionClient->isReadyToReceive())
	{
		// Receives data from the connection
		l_ui32Received=m_pConnectionClient->receiveBuffer(l_pBuffer, sizeof(l_pBuffer));

		// Sends them to the EBML processor
		m_pReader->processData(l_pBuffer, l_ui32Received);
	}

	return true;
}

	}
}
