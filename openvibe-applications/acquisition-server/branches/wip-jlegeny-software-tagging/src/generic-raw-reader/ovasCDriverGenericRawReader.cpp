#include "ovasCDriverGenericRawReader.h"
#include "../ovasCConfigurationBuilder.h"

#include <openvibe-toolkit/ovtk_all.h>

#include <system/Memory.h>
#include <system/Time.h>

#include <cmath>

using namespace OpenViBEAcquisitionServer;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;

namespace
{
	template <class T>
	float32 decode_little_endian(const uint8* pBuffer)
	{
		T t;
		System::Memory::littleEndianToHost(pBuffer, &t);
		return float32(t);
	}

	template <class T>
	float32 decode_big_endian(const uint8* pBuffer)
	{
		T t;
		System::Memory::bigEndianToHost(pBuffer, &t);
		return float32(t);
	}
};

//___________________________________________________________________//
//                                                                   //

CDriverGenericRawReader::CDriverGenericRawReader(IDriverContext& rDriverContext)
	:IDriver(rDriverContext)
	,m_pCallback(NULL)
	,m_ui32SampleCountPerSentBlock(0)
	,m_ui32SampleFormat(Format_SignedInteger32)
	,m_ui32SampleEndian(Endian_Little)
	,m_ui32HeaderSkip(0)
	,m_ui32FrameSkip(20)
	,m_bLimitSpeed(false)
	,m_pDataFrame(NULL)
	,m_pSample(NULL)
{
	m_oHeader.setSamplingFrequency(512);
	m_oHeader.setChannelCount(16);
}

void CDriverGenericRawReader::release(void)
{
	delete this;
}

//___________________________________________________________________//
//                                                                   //

boolean CDriverGenericRawReader::initialize(
	const uint32 ui32SampleCountPerSentBlock,
	IDriverCallback& rCallback)
{
	if(m_rDriverContext.isConnected()) { return false; }

	switch(m_ui32SampleFormat)
	{
		case Format_UnsignedInteger8:
		case Format_SignedInteger8:
			m_ui32SampleSize=1;
			break;
		case Format_UnsignedInteger16:
		case Format_SignedInteger16:
			m_ui32SampleSize=2;
			break;
		case Format_UnsignedInteger32:
		case Format_SignedInteger32:
		case Format_Float32:
			m_ui32SampleSize=4;
			break;
		case Format_Float64:
			m_ui32SampleSize=8;
			break;
		default:
			m_rDriverContext.getLogManager() << LogLevel_Error << "Unsupported data format " << m_ui32SampleFormat << "\n";
			return false;
	}

	m_ui32DataFrameSize=m_ui32SampleSize*m_oHeader.getChannelCount();
	m_ui32DataFrameSize+=m_ui32FrameSkip;
	m_ui32DataFrameSize+=m_ui32HeaderSkip;

	m_pSample=new float32[m_oHeader.getChannelCount()];
	m_pDataFrame=new uint8[m_ui32DataFrameSize];
	if(!m_pSample || !m_pDataFrame)
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "Could not allocate memory !\n";
		return false;
	}

	if(!this->open())
	{
		return false;
	}

	m_pCallback=&rCallback;
	m_ui32SampleCountPerSentBlock=ui32SampleCountPerSentBlock;

	return true;
}

boolean CDriverGenericRawReader::start(void)
{
	if(!m_rDriverContext.isConnected()) { return false; }
	if(m_rDriverContext.isStarted()) { return false; }
	m_ui64TotalSampleCount=0;
	m_ui64StartTime=System::Time::zgetTime();
	return true;
}

boolean CDriverGenericRawReader::loop(void)
{
	if(!m_rDriverContext.isConnected()) { return false; }
	// if(!m_rDriverContext.isStarted()) { return true; }

	if(m_bLimitSpeed && ((m_ui64TotalSampleCount << 32) / m_oHeader.getSamplingFrequency() > System::Time::zgetTime() - m_ui64StartTime))
	{
		return true;
	}

	uint32 i, j;
	for(j=0; j<m_ui32SampleCountPerSentBlock; j++)
	{
		if(!this->read())
		{
			return false;
		}

		for(i=0; i<m_oHeader.getChannelCount(); i++)
		{
			uint8* l_pDataFrame=m_pDataFrame+i*m_ui32SampleSize;
			switch(m_ui32SampleEndian)
			{
				case Endian_Little:
					switch(m_ui32SampleFormat)
					{
						case Format_UnsignedInteger8:  m_pSample[i]=*l_pDataFrame; break;
						case Format_UnsignedInteger16: m_pSample[i]=decode_little_endian<uint16>(l_pDataFrame); break;
						case Format_UnsignedInteger32: m_pSample[i]=decode_little_endian<uint32>(l_pDataFrame); break;
						case Format_SignedInteger8:    m_pSample[i]=*l_pDataFrame; break;
						case Format_SignedInteger16:   m_pSample[i]=decode_little_endian<int16>(l_pDataFrame); break;
						case Format_SignedInteger32:   m_pSample[i]=decode_little_endian<int32>(l_pDataFrame); break;
						case Format_Float32:           m_pSample[i]=decode_little_endian<float32>(l_pDataFrame); break;
						case Format_Float64:           m_pSample[i]=decode_little_endian<float64>(l_pDataFrame); break;
						default: break;
					}
					break;

				case Endian_Big:
					switch(m_ui32SampleFormat)
					{
						case Format_UnsignedInteger8:  m_pSample[i]=*l_pDataFrame; break;
						case Format_UnsignedInteger16: m_pSample[i]=decode_big_endian<uint16>(l_pDataFrame); break;
						case Format_UnsignedInteger32: m_pSample[i]=decode_big_endian<uint32>(l_pDataFrame); break;
						case Format_SignedInteger8:    m_pSample[i]=*l_pDataFrame; break;
						case Format_SignedInteger16:   m_pSample[i]=decode_big_endian<int16>(l_pDataFrame); break;
						case Format_SignedInteger32:   m_pSample[i]=decode_big_endian<int32>(l_pDataFrame); break;
						case Format_Float32:           m_pSample[i]=decode_big_endian<float32>(l_pDataFrame); break;
						case Format_Float64:           m_pSample[i]=decode_big_endian<float64>(l_pDataFrame); break;
						default: break;
					}
					break;
				default:
					break;
			}
		}

		if(m_rDriverContext.isStarted())
		{
			m_pCallback->setSamples(m_pSample, 1);
		}
	}

	if(m_rDriverContext.isStarted())
	{
		m_rDriverContext.correctDriftSampleCount(m_rDriverContext.getSuggestedDriftCorrectionSampleCount());
	}

	m_ui64TotalSampleCount+=m_ui32SampleCountPerSentBlock;

	return true;
}

boolean CDriverGenericRawReader::stop(void)
{
	if(!m_rDriverContext.isConnected()) { return false; }
	if(!m_rDriverContext.isStarted()) { return false; }

	return true;
}

boolean CDriverGenericRawReader::uninitialize(void)
{
	if(!m_rDriverContext.isConnected()) { return false; }
	if(m_rDriverContext.isStarted()) { return false; }

	if(!this->close())
	{
		return false;
	}

	delete [] m_pSample;
	delete [] m_pDataFrame;
	m_pSample=NULL;
	m_pDataFrame=NULL;
	m_pCallback=NULL;

	return true;
}
