#include "ovpCBoxAlgorithmBCI2000Reader.h"

#include <iostream>
#include <sstream>
#include <cstdio>
#include <cmath>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::FileIO;
using namespace OpenViBEToolkit;

using namespace BCI2000;

boolean CBoxAlgorithmBCI2000Reader::initialize(void)
{
	CString l_sFilename=FSettingValueAutoCast(*this->getBoxAlgorithmContext(),0);
	m_pB2KReaderHelper=new CBCI2000ReaderHelper(l_sFilename);
	if (!m_pB2KReaderHelper->isGood())
	{
		this->getLogManager() << LogLevel_ImportantWarning << "Could not open file [" << l_sFilename << "]\n";
		m_pB2KReaderHelper = NULL;
		return false;
	}
	else
	{
		std::stringstream l_sStream;
		m_pB2KReaderHelper->printInfo(l_sStream);
		this->getLogManager() << LogLevel_Trace << "Metadata from [" << l_sFilename << "] :\n" << l_sStream.str().c_str() << "\n";
	}

	m_bHeaderSent=false;
	m_ui32ChannelCount=m_pB2KReaderHelper->getChannels();
	m_ui32SampleCountPerBuffer=static_cast<uint32>((uint64)FSettingValueAutoCast(*this->getBoxAlgorithmContext(),1));
	m_ui32SamplesSent=0;
	m_pBuffer=new float64[m_ui32ChannelCount*m_ui32SampleCountPerBuffer];
	m_pStates=new uint32[m_pB2KReaderHelper->getStateVectorSize()*m_ui32SampleCountPerBuffer];
	m_ui32Rate=(uint32)m_pB2KReaderHelper->getRate();
	if(m_ui32Rate == -1)
	{
		this->getLogManager() << LogLevel_Error << "Sampling rate could not be extracted from the file.\n";
		return false;
	}


	m_oSignalEncoder.initialize(*this);
	m_pSignalOutputMatrix=m_oSignalEncoder.getInputMatrix();
	m_pSignalOutputMatrix->setDimensionCount(2);
	m_pSignalOutputMatrix->setDimensionSize(0,m_ui32ChannelCount);
	m_pSignalOutputMatrix->setDimensionSize(1,m_ui32SampleCountPerBuffer);
	for (uint32 i=0; i<m_ui32ChannelCount; i++)
	{
		m_pSignalOutputMatrix->setDimensionLabel(0,i,m_pB2KReaderHelper->getChannelName(i));
	}

	m_oStateEncoder.initialize(*this);
	m_pStateOutputMatrix=m_oStateEncoder.getInputMatrix();
	m_pStateOutputMatrix->setDimensionCount(2);
	m_pStateOutputMatrix->setDimensionSize(0,m_pB2KReaderHelper->getStateVectorSize());
	m_pStateOutputMatrix->setDimensionSize(1,m_ui32SampleCountPerBuffer);
	for (int i=0; i<m_pB2KReaderHelper->getStateVectorSize(); i++)
	{
		m_pStateOutputMatrix->setDimensionLabel(0,i,m_pB2KReaderHelper->getStateName(i));
	}
	m_oSignalEncoder.getInputSamplingRate()=m_ui32Rate;
	m_oStateEncoder.getInputSamplingRate()=m_ui32Rate;
	for (int i=0; i<m_pB2KReaderHelper->getStateVectorSize(); i++)
	{
		this->getLogManager() << LogLevel_Trace << "BCI2000 state var "<< i << " is : " << m_pB2KReaderHelper->getStateName(i) << "\n";
	}
	return true;
}

boolean CBoxAlgorithmBCI2000Reader::uninitialize(void)
{
	if(m_pB2KReaderHelper != NULL)
	{
		delete m_pB2KReaderHelper;
		delete[] m_pBuffer;
		delete[] m_pStates;
	}
	
	m_oSignalEncoder.uninitialize();
	m_oStateEncoder.uninitialize();

	return true;
}

boolean CBoxAlgorithmBCI2000Reader::processClock(IMessageClock& rMessageClock)
{
	if (m_pB2KReaderHelper->getSamplesLeft()>0)
	{
		getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
		return true;
	}

	return false;
}

uint64 CBoxAlgorithmBCI2000Reader::getClockFrequency(void)
{
	return ((uint64)m_ui32Rate<<32)/m_ui32SampleCountPerBuffer;
}

void CBoxAlgorithmBCI2000Reader::sendHeader(void)
{
	m_oSignalEncoder.encodeHeader(0);
	m_oStateEncoder.encodeHeader(1);
	m_bHeaderSent=true;
	getDynamicBoxContext().markOutputAsReadyToSend(0,0,0);
	getDynamicBoxContext().markOutputAsReadyToSend(1,0,0);
}

boolean CBoxAlgorithmBCI2000Reader::process(void)
{
	if (!m_bHeaderSent)
	{
		sendHeader();
	}

	//prepare data
	int32 l_ui32SamplesRead=m_pB2KReaderHelper->readSamples(m_pBuffer,m_pStates,m_ui32SampleCountPerBuffer);
	if (l_ui32SamplesRead>0)
	{
		// padding. TODO: is it necessary ? or even dangerous ?
		for (uint32 i=l_ui32SamplesRead; i<m_ui32SampleCountPerBuffer; i++)
		{
			for (uint32 j=0; j<m_ui32ChannelCount; j++)
			{
				m_pBuffer[i*m_ui32ChannelCount+j]=0.0;
			}
		}
		// transpose (yeah, I know... ugly)
		for (uint32 i=0; i<m_ui32SampleCountPerBuffer; i++)
		{
			for (uint32 j=0; j<m_ui32ChannelCount; j++)
			{
				m_pSignalOutputMatrix->getBuffer()[j*m_ui32SampleCountPerBuffer+i]=m_pBuffer[i*m_ui32ChannelCount+j];
			}
		}
		m_oSignalEncoder.encodeBuffer(0);
		uint64 StartTime;
		uint64 EndTime;
		StartTime=(((uint64)(m_ui32SamplesSent))<<32)/m_ui32Rate;
		EndTime=(((uint64)(m_ui32SamplesSent+m_ui32SampleCountPerBuffer))<<32)/m_ui32Rate;
		m_ui32SamplesSent+=l_ui32SamplesRead;
		if (m_pB2KReaderHelper->getSamplesLeft()==0)
		{
			m_oSignalEncoder.encodeEnd(0);
		}

		getDynamicBoxContext().markOutputAsReadyToSend(0,StartTime,EndTime);

		// padding. TODO: is it necessary ? or even dangerous ?
		for (uint32 i=l_ui32SamplesRead; i<m_ui32SampleCountPerBuffer; i++)
		{
			for (int j=0; j<m_pB2KReaderHelper->getStateVectorSize(); j++)
			{
				m_pStates[i*m_pB2KReaderHelper->getStateVectorSize()+j]=0;
			}
		}
		// transpose (yeah, I know... ugly)
		for (uint32 i=0; i<m_ui32SampleCountPerBuffer; i++)
		{
			for (int j=0; j<m_pB2KReaderHelper->getStateVectorSize(); j++)
			{
				m_pStateOutputMatrix->getBuffer()[j*m_ui32SampleCountPerBuffer+i]=
					m_pStates[i*m_pB2KReaderHelper->getStateVectorSize()+j];
			}
		}
		m_oStateEncoder.encodeBuffer(1);

		if (m_pB2KReaderHelper->getSamplesLeft()==0)
		{
			m_oSignalEncoder.encodeEnd(1);
		}

		getDynamicBoxContext().markOutputAsReadyToSend(1,StartTime,EndTime);
	}
	else
	{
		this->getLogManager() << LogLevel_Error << "An error occured while trying to get new samples from file. The file may be corrupted.\n";
		return false;
	}
	return true;
}
