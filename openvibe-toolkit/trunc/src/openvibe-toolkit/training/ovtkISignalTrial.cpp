#include "ovtkISignalTrial.h"

#include <system/Memory.h>

using namespace OpenViBEToolkit;
using namespace OpenViBE;

ISignalTrial& OpenViBEToolkit::copyHeader(
		ISignalTrial& rSignalTrial,
		const ISignalTrial* pSourceSignalTrial)
{
	if(pSourceSignalTrial==NULL)
	{
		pSourceSignalTrial=&rSignalTrial;
	}

	if(pSourceSignalTrial!=&rSignalTrial)
	{
		uint32 l_ui32ChannelCount=pSourceSignalTrial->getChannelCount();
		rSignalTrial.setChannelCount(l_ui32ChannelCount);
		for(uint32 i=0; i<l_ui32ChannelCount; i++)
		{
			rSignalTrial.setChannelName(i, pSourceSignalTrial->getChannelName(i));
		}
		rSignalTrial.setSamplingRate(pSourceSignalTrial->getSamplingRate());
	}

	return rSignalTrial;
}

ISignalTrial& OpenViBEToolkit::copy(
		ISignalTrial& rSignalTrial,
		const ISignalTrial* pSourceSignalTrial)
{
	if(pSourceSignalTrial==NULL)
	{
		pSourceSignalTrial=&rSignalTrial;
	}

	// copyHeader(rSignalTrial, pSourceSignalTrial);

	if(pSourceSignalTrial!=&rSignalTrial)
	{
		uint32 l_ui32ChannelCount=pSourceSignalTrial->getChannelCount();
		uint32 l_ui32SampleCount=pSourceSignalTrial->getSampleCount();
		rSignalTrial.setSampleCount(l_ui32SampleCount, false);
		for(uint32 i=0; i<l_ui32ChannelCount; i++)
		{
			System::Memory::copy(
				rSignalTrial.getChannelSampleBuffer(i),
				pSourceSignalTrial->getChannelSampleBuffer(i),
				l_ui32SampleCount*sizeof(float64));
		}
	}

	return rSignalTrial;
}

ISignalTrial& OpenViBEToolkit::selectSamples(
		ISignalTrial& rSignalTrial,
		const uint32 ui32SampleStart,
		const uint32 ui32SampleEnd,
		const ISignalTrial* pSourceSignalTrial)
{
	if(pSourceSignalTrial==NULL)
	{
		pSourceSignalTrial=&rSignalTrial;
	}

	if(pSourceSignalTrial==&rSignalTrial)
	{
		// $$$ NOT YET IMPLEMENTED
		return rSignalTrial;
	}

	// copyHeader(rSignalTrial, pSourceSignalTrial);

	uint32 l_ui32ChannelCount=pSourceSignalTrial->getChannelCount();
	uint32 l_ui32SampleCount=ui32SampleEnd-ui32SampleStart;

	rSignalTrial.setSampleCount(l_ui32SampleCount, false);
	for(uint32 i=0; i<l_ui32ChannelCount; i++)
	{
		System::Memory::copy(
			rSignalTrial.getChannelSampleBuffer(i),
			pSourceSignalTrial->getChannelSampleBuffer(i)+ui32SampleStart,
			l_ui32SampleCount*sizeof(float64));
	}

	return rSignalTrial;
}

ISignalTrial& OpenViBEToolkit::selectTime(
		ISignalTrial& rSignalTrial,
		const uint64 ui64TimeStart,
		const uint64 ui64TimeEnd,
		const ISignalTrial* pSourceSignalTrial)
{
	if(pSourceSignalTrial==NULL)
	{
		pSourceSignalTrial=&rSignalTrial;
	}

	if(pSourceSignalTrial==&rSignalTrial)
	{
		// $$$ NOT YET IMPLEMENTED
		return rSignalTrial;
	}

	uint32 l_ui32SampleStart=(uint32)((ui64TimeStart*pSourceSignalTrial->getSamplingRate())>>32);
	uint32 l_ui32SampleEnd=(uint32)((ui64TimeEnd*pSourceSignalTrial->getSamplingRate())>>32);
	return selectSamples(rSignalTrial, l_ui32SampleStart, l_ui32SampleEnd, pSourceSignalTrial);
}

ISignalTrial& OpenViBEToolkit::removeSamples(
		ISignalTrial& rSignalTrial,
		const uint32 ui32SampleStart,
		const uint32 ui32SampleEnd,
		const ISignalTrial* pSourceSignalTrial)
{
	if(pSourceSignalTrial==NULL)
	{
		pSourceSignalTrial=&rSignalTrial;
	}

	if(pSourceSignalTrial==&rSignalTrial)
	{
		// $$$ NOT YET IMPLEMENTED
		return rSignalTrial;
	}

	// copyHeader(rSignalTrial, pSourceSignalTrial);

	uint32 l_ui32ChannelCount=pSourceSignalTrial->getChannelCount();
	uint32 l_ui32SampleCount=pSourceSignalTrial->getSampleCount()-(ui32SampleEnd-ui32SampleStart);

	rSignalTrial.setSampleCount(l_ui32SampleCount, false);
	for(uint32 i=0; i<l_ui32ChannelCount; i++)
	{
		System::Memory::copy(
			rSignalTrial.getChannelSampleBuffer(i),
			pSourceSignalTrial->getChannelSampleBuffer(i),
			ui32SampleStart*sizeof(float64));
		System::Memory::copy(
			rSignalTrial.getChannelSampleBuffer(i)+ui32SampleStart,
			pSourceSignalTrial->getChannelSampleBuffer(i)+ui32SampleEnd,
			(l_ui32SampleCount-ui32SampleStart)*sizeof(float64));
	}

	return rSignalTrial;
}

ISignalTrial& OpenViBEToolkit::removeTime(
		ISignalTrial& rSignalTrial,
		const uint64 ui64TimeStart,
		const uint64 ui64TimeEnd,
		const ISignalTrial* pSourceSignalTrial)
{
	if(pSourceSignalTrial==NULL)
	{
		pSourceSignalTrial=&rSignalTrial;
	}

	if(pSourceSignalTrial==&rSignalTrial)
	{
		// $$$ NOT YET IMPLEMENTED
		return rSignalTrial;
	}

	uint32 l_ui32SampleStart=(uint32)((ui64TimeStart*pSourceSignalTrial->getSamplingRate())>>32);
	uint32 l_ui32SampleEnd=(uint32)((ui64TimeEnd*pSourceSignalTrial->getSamplingRate())>>32);
	return removeSamples(rSignalTrial, l_ui32SampleStart, l_ui32SampleEnd, pSourceSignalTrial);
}

ISignalTrial& OpenViBEToolkit::insertBufferSamples(
		ISignalTrial& rSignalTrial,
		const uint32 ui32SampleStart,
		const uint32 ui32SampleCount,
		const float64* pBuffer,
		const ISignalTrial* pSourceSignalTrial)
{
	if(pSourceSignalTrial==NULL)
	{
		pSourceSignalTrial=&rSignalTrial;
	}

	// copyHeader(rSignalTrial, pSourceSignalTrial);

	uint32 l_ui32ChannelCount=pSourceSignalTrial->getChannelCount();
	uint32 l_ui32SampleCount=pSourceSignalTrial->getSampleCount();

	rSignalTrial.setSampleCount(l_ui32SampleCount+ui32SampleCount, true);
	for(uint32 i=0; i<l_ui32ChannelCount; i++)
	{
		if(&rSignalTrial!=pSourceSignalTrial)
		{
			System::Memory::copy(
				rSignalTrial.getChannelSampleBuffer(i),
				pSourceSignalTrial->getChannelSampleBuffer(i),
				ui32SampleStart*sizeof(float64));
		}

		System::Memory::copy(
			rSignalTrial.getChannelSampleBuffer(i)+ui32SampleStart+ui32SampleCount,
			pSourceSignalTrial->getChannelSampleBuffer(i),
			(l_ui32SampleCount-ui32SampleStart)*sizeof(float64));

		System::Memory::copy(
			rSignalTrial.getChannelSampleBuffer(i)+ui32SampleStart,
			pBuffer+ui32SampleCount*i,
			ui32SampleCount*sizeof(float64));
	}

	return rSignalTrial;
}

ISignalTrial& OpenViBEToolkit::insertBufferTime(
		ISignalTrial& rSignalTrial,
		const uint64 ui64TimeStart,
		const uint32 ui32SampleCount,
		const float64* pBuffer,
		const ISignalTrial* pSourceSignalTrial)
{
	if(pSourceSignalTrial==NULL)
	{
		pSourceSignalTrial=&rSignalTrial;
	}

	uint32 l_ui32SampleStart=(uint32)((ui64TimeStart*pSourceSignalTrial->getSamplingRate())>>32);
	return insertBufferSamples(rSignalTrial, l_ui32SampleStart, ui32SampleCount, pBuffer, pSourceSignalTrial);
}
