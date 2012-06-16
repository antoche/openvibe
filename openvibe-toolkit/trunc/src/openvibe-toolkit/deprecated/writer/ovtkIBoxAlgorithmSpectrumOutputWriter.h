#ifndef __OpenViBEToolkit_Writer_IBoxAlgorithmSpectrumOutputWriter_H__
#define __OpenViBEToolkit_Writer_IBoxAlgorithmSpectrumOutputWriter_H__

#include "ovtkIBoxAlgorithmStreamedMatrixOutputWriter.h"

namespace OpenViBEToolkit
{
	class OVTK_API IBoxAlgorithmSpectrumOutputWriter : public OpenViBEToolkit::IBoxAlgorithmEBMLOutputWriter
	{
	public:

		virtual OpenViBE::boolean setChannelCount(const OpenViBE::uint32 ui32ChannelCount)=0;
		virtual OpenViBE::boolean setChannelName(const OpenViBE::uint32 ui32ChannelIndex, const char* sChannelName)=0;
		virtual OpenViBE::boolean setFrequencyBandCount(const OpenViBE::uint32 ui32FrequencyBandCount)=0;
		virtual OpenViBE::boolean setFrequencyBandName(const OpenViBE::uint32 ui32FrequencyBandIndex, const char* sFrequencyBandName)=0;
		virtual OpenViBE::boolean setFrequencyBandStart(const OpenViBE::uint32 ui32FrequencyBandIndex, const OpenViBE::float64 f64FrequencyBandStart)=0;
		virtual OpenViBE::boolean setFrequencyBandStop(const OpenViBE::uint32 ui32FrequencyBandIndex, const OpenViBE::float64 f64FrequencyBandStop)=0;
		virtual OpenViBE::boolean setBuffer(const OpenViBE::float64* pBuffer)=0;

		_IsDerivedFromClass_(OpenViBEToolkit::IBoxAlgorithmEBMLOutputWriter, OVTK_ClassId_);
	};

	extern OVTK_API OpenViBEToolkit::IBoxAlgorithmSpectrumOutputWriter* createBoxAlgorithmSpectrumOutputWriter(void);
	extern OVTK_API void releaseBoxAlgorithmSpectrumOutputWriter(OpenViBEToolkit::IBoxAlgorithmSpectrumOutputWriter* pOutputWriter);
};

#endif // __OpenViBEToolkit_Writer_IBoxAlgorithmSpectrumOutputWriter_H__
