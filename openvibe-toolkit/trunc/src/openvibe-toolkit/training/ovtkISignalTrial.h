#ifndef __OpenViBEToolkit_Offline_ISignalTrial_H__
#define __OpenViBEToolkit_Offline_ISignalTrial_H__

#include "../ovtkIObject.h"

namespace OpenViBEToolkit
{
	class OVTK_API ISignalTrial : public OpenViBEToolkit::IObject
	{
	public:

		virtual OpenViBE::boolean setSamplingRate(const OpenViBE::uint32 ui32SamplingFrequency)=0;
		virtual OpenViBE::boolean setChannelCount(const OpenViBE::uint32 ui32ChannelCount)=0;
		virtual OpenViBE::boolean setChannelName(const OpenViBE::uint32 ui32ChannelIndex, const char* sChannelName)=0;
		virtual OpenViBE::boolean setLabelIdentifier(const OpenViBE::CIdentifier& rLabelIdentifier)=0;
		virtual OpenViBE::boolean setSampleCount(const OpenViBE::uint32 ui32SampleCount, const OpenViBE::boolean bPreserve)=0;

		virtual OpenViBE::uint32 getSamplingRate(void) const=0;
		virtual OpenViBE::uint32 getChannelCount(void) const=0;
		virtual const char* getChannelName(const OpenViBE::uint32 ui32ChannelIndex) const=0;
		virtual OpenViBE::CIdentifier getLabelIdentifier(void) const=0;
		virtual OpenViBE::uint32 getSampleCount(void) const=0;
		virtual OpenViBE::uint64 getDuration(void) const=0;
		virtual OpenViBE::float64* getChannelSampleBuffer(const OpenViBE::uint32 ui32ChannelIndex) const=0;

		_IsDerivedFromClass_(OpenViBEToolkit::IObject, OVTK_ClassId_);
	};

	extern OVTK_API OpenViBEToolkit::ISignalTrial* createSignalTrial(void);
	extern OVTK_API void releaseSignalTrial(OpenViBEToolkit::ISignalTrial* pSignalTrial);

	// operations
	extern OVTK_API OpenViBEToolkit::ISignalTrial& copyHeader(
		OpenViBEToolkit::ISignalTrial& rSignalTrial,
		const OpenViBEToolkit::ISignalTrial* pSourceSignalTrial=NULL);
	extern OVTK_API OpenViBEToolkit::ISignalTrial& copy(
		OpenViBEToolkit::ISignalTrial& rSignalTrial,
		const OpenViBEToolkit::ISignalTrial* pSourceSignalTrial=NULL);
	extern OVTK_API OpenViBEToolkit::ISignalTrial& selectSamples(
		OpenViBEToolkit::ISignalTrial& rSignalTrial,
		const OpenViBE::uint32 ui32SampleStart,
		const OpenViBE::uint32 ui32SampleEnd,
		const OpenViBEToolkit::ISignalTrial* pSourceSignalTrial=NULL);
	extern OVTK_API OpenViBEToolkit::ISignalTrial& selectTime(
		OpenViBEToolkit::ISignalTrial& rSignalTrial,
		const OpenViBE::uint64 ui64TimeStart,
		const OpenViBE::uint64 ui64TimeEnd,
		const OpenViBEToolkit::ISignalTrial* pSourceSignalTrial=NULL);
	extern OVTK_API OpenViBEToolkit::ISignalTrial& removeSamples(
		OpenViBEToolkit::ISignalTrial& rSignalTrial,
		const OpenViBE::uint32 ui32SampleStart,
		const OpenViBE::uint32 ui32SampleEnd,
		const OpenViBEToolkit::ISignalTrial* pSourceSignalTrial=NULL);
	extern OVTK_API OpenViBEToolkit::ISignalTrial& removeTime(
		OpenViBEToolkit::ISignalTrial& rSignalTrial,
		const OpenViBE::uint64 ui64TimeStart,
		const OpenViBE::uint64 ui64TimeEnd,
		const OpenViBEToolkit::ISignalTrial* pSourceSignalTrial=NULL);
	extern OVTK_API OpenViBEToolkit::ISignalTrial& insertBufferSamples(
		OpenViBEToolkit::ISignalTrial& rSignalTrial,
		const OpenViBE::uint32 ui32SampleStart,
		const OpenViBE::uint32 ui32SampleCount,
		const OpenViBE::float64* pBuffer,
		const OpenViBEToolkit::ISignalTrial* pSourceSignalTrial=NULL);
	extern OVTK_API OpenViBEToolkit::ISignalTrial& insertBufferTime(
		OpenViBEToolkit::ISignalTrial& rSignalTrial,
		const OpenViBE::uint64 ui64TimeStart,
		const OpenViBE::uint32 ui32SampleCount,
		const OpenViBE::float64* pBuffer,
		const OpenViBEToolkit::ISignalTrial* pSourceSignalTrial=NULL);
};

#endif // __OpenViBEToolkit_Offline_ISignalTrial_H__
