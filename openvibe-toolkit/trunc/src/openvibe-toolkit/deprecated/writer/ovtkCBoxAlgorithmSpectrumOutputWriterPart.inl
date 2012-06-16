#ifndef __OpenViBEToolkit_Writer_CBoxAlgorithmSpectrumOutputWriterPart_INL__
#define __OpenViBEToolkit_Writer_CBoxAlgorithmSpectrumOutputWriterPart_INL__

#include "ovtkIBoxAlgorithmEBMLOutputWriter.h"
#include "ovtkCBoxAlgorithmEBMLOutputWriterPartBase.inl"

#include <ebml/CWriterHelper.h>

#include <map>

namespace OpenViBEToolkit
{
	class CBoxAlgorithmSpectrumOutputWriterPart : public OpenViBEToolkit::CBoxAlgorithmEBMLOutputWriterPartBase
	{
	public:

		CBoxAlgorithmSpectrumOutputWriterPart(void);

		virtual OpenViBE::boolean setFrequencyBandCount(const OpenViBE::uint32 ui32FrequencyBandCount);
		virtual OpenViBE::boolean setFrequencyBandStart(const OpenViBE::uint32 ui32FrequencyBandIndex, const OpenViBE::float64 f64FrequencyBandStart);
		virtual OpenViBE::boolean setFrequencyBandStop(const OpenViBE::uint32 ui32FrequencyBandIndex, const OpenViBE::float64 f64FrequencyBandStop);

		_IsDerivedFromClass_Final_(OpenViBEToolkit::CBoxAlgorithmEBMLOutputWriterPartBase, OVTK_ClassId_);

	protected:

		virtual OpenViBE::boolean writeHeaderStart(EBML::IWriter& rWriter);
		virtual OpenViBE::boolean writeHeaderStop(EBML::IWriter& rWriter);
		// virtual OpenViBE::boolean writeBufferStart(EBML::IWriter& rWriter);
		// virtual OpenViBE::boolean writeBufferStop(EBML::IWriter& rWriter);
		// virtual OpenViBE::boolean writeEndStart(EBML::IWriter& rWriter);
		// virtual OpenViBE::boolean writeEndStop(EBML::IWriter& rWriter);

	protected:

		std::map<OpenViBE::uint32, OpenViBE::float64> m_vFrequencyBandStart;
		std::map<OpenViBE::uint32, OpenViBE::float64> m_vFrequencyBandStop;
		OpenViBE::uint32 m_ui32FrequencyBandCount;

		EBML::CWriterHelper m_oWriterHelper;
	};
};

#endif // __OpenViBEToolkit_Writer_CBoxAlgorithmSpectrumOutputWriterPart_INL__
