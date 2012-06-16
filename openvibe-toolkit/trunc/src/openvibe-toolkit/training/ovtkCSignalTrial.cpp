#include "ovtkCSignalTrial.hpp"

#include <system/Memory.h>

using namespace OpenViBE;
using namespace OpenViBEToolkit;
using namespace std;

// ________________________________________________________________________________________________________________
//

CSignalTrial::CSignalTrial(void)
	:m_ui32ChannelCount(0)
	,m_ui32SampleCount(0)
	,m_ui32SampleCountReserved(0)
	,m_ui32SamplingRate(0)
{
}

CSignalTrial::~CSignalTrial(void)
{
	map<uint32, float64*>::iterator itChannel;
	for(itChannel=m_vChannelSample.begin(); itChannel!=m_vChannelSample.end(); itChannel++)
	{
		delete [] itChannel->second;
	}
}

// ________________________________________________________________________________________________________________
//

boolean CSignalTrial::setSamplingRate(const uint32 ui32SamplingFrequency)
{
	m_ui32SamplingRate=ui32SamplingFrequency;
	return m_ui32SamplingRate!=0;
}

boolean CSignalTrial::setChannelCount(const uint32 ui32ChannelCount)
{
	uint32 i;
	for(i=0; i<ui32ChannelCount; i++)
	{
		if(m_vChannelSample.find(i)==m_vChannelSample.end())
		{
			m_vChannelSample[i]=new float64[m_ui32SampleCountReserved];
		}
	}
	for(i=ui32ChannelCount; i<m_ui32ChannelCount; i++)
	{
		delete [] m_vChannelSample[i];
		m_vChannelSample.erase(m_vChannelSample.find(i));
	}

	m_ui32ChannelCount=ui32ChannelCount;
	m_ui32SampleCount=0;
	m_vChannelName.clear();
	return m_ui32ChannelCount!=0;
}

boolean CSignalTrial::setChannelName(const uint32 ui32ChannelIndex, const char* sChannelName)
{
	if(ui32ChannelIndex<m_ui32ChannelCount)
	{
		m_vChannelName[ui32ChannelIndex]=sChannelName;
		return true;
	}
	return false;
}

boolean CSignalTrial::setLabelIdentifier(const CIdentifier& rLabelIdentifier)
{
	m_oLabelIdentifier=rLabelIdentifier;
	return true;
}
boolean CSignalTrial::setSampleCount(const uint32 ui32SampleCount, const boolean bPreserve)
{
	const uint32 l_ui32SampleCountRounding=0x00000fff;

	if(ui32SampleCount>m_ui32SampleCountReserved)
	{
		uint32 l_ui32SampleCountReserved=(ui32SampleCount+l_ui32SampleCountRounding+1)&(~l_ui32SampleCountRounding);
		map<uint32, float64*>::iterator itChannelSample;
		for(itChannelSample=m_vChannelSample.begin(); itChannelSample!=m_vChannelSample.end(); itChannelSample++)
		{
			float64* l_pSample=new float64[l_ui32SampleCountReserved];
			if(bPreserve)
			{
				System::Memory::copy(l_pSample, itChannelSample->second, (ui32SampleCount<m_ui32SampleCount?ui32SampleCount:m_ui32SampleCount)*sizeof(float64));
			}
			delete [] itChannelSample->second;
			itChannelSample->second=l_pSample;
		}
		m_ui32SampleCountReserved=l_ui32SampleCountReserved;
	}
	m_ui32SampleCount=ui32SampleCount;
	return true;
}

// ________________________________________________________________________________________________________________
//

uint32 CSignalTrial::getSamplingRate(void) const
{
	return m_ui32SamplingRate;
}

uint32 CSignalTrial::getChannelCount(void) const
{
	return m_ui32ChannelCount;
}

const char* CSignalTrial::getChannelName(const uint32 ui32ChannelIndex) const
{
	map<uint32, string>::const_iterator itChannelName;
	itChannelName=m_vChannelName.find(ui32ChannelIndex);
	if(itChannelName!=m_vChannelName.end())
	{
		return itChannelName->second.c_str();
	}
	return "";
}

CIdentifier CSignalTrial::getLabelIdentifier(void) const
{
	return m_oLabelIdentifier;
}

uint32 CSignalTrial::getSampleCount(void) const
{
	return m_ui32SampleCount;
}

uint64 CSignalTrial::getDuration(void) const
{
	return m_ui32SamplingRate?(((uint64)m_ui32SampleCount)<<32)/m_ui32SamplingRate:0;
}

float64* CSignalTrial::getChannelSampleBuffer(const uint32 ui32ChannelIndex) const
{
	map<uint32, float64*>::const_iterator itChannelSample;
	itChannelSample=m_vChannelSample.find(ui32ChannelIndex);
	if(itChannelSample!=m_vChannelSample.end())
	{
		return itChannelSample->second;
	}
	return NULL;
}

// ________________________________________________________________________________________________________________
//

ISignalTrial* OpenViBEToolkit::createSignalTrial(void)
{
	return new CSignalTrial();
}

void OpenViBEToolkit::releaseSignalTrial(ISignalTrial* pSignalTrial)
{
	delete pSignalTrial;
}
