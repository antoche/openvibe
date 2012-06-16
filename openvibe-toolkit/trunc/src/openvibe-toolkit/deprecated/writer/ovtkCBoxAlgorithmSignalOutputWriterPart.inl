#ifndef __OpenViBEToolkit_Writer_CBoxAlgorithmSignalOutputWriterPart_INL__
#define __OpenViBEToolkit_Writer_CBoxAlgorithmSignalOutputWriterPart_INL__

#include "ovtkIBoxAlgorithmEBMLOutputWriter.h"
#include "ovtkCBoxAlgorithmEBMLOutputWriterPartBase.inl"

#include <ebml/CWriterHelper.h>

namespace OpenViBEToolkit
{
	class CBoxAlgorithmSignalOutputWriterPart : public OpenViBEToolkit::CBoxAlgorithmEBMLOutputWriterPartBase
	{
	public:

		CBoxAlgorithmSignalOutputWriterPart(void);

		virtual OpenViBE::boolean setSamplingRate(const OpenViBE::uint32 ui32SamplingRate);

		_IsDerivedFromClass_Final_(OpenViBEToolkit::CBoxAlgorithmEBMLOutputWriterPartBase, OVTK_ClassId_);

	protected:

		virtual OpenViBE::boolean writeHeaderStart(EBML::IWriter& rWriter);
		virtual OpenViBE::boolean writeHeaderStop(EBML::IWriter& rWriter);
		// virtual OpenViBE::boolean writeBufferStart(EBML::IWriter& rWriter);
		// virtual OpenViBE::boolean writeBufferStop(EBML::IWriter& rWriter);
		// virtual OpenViBE::boolean writeEndStart(EBML::IWriter& rWriter);
		// virtual OpenViBE::boolean writeEndStop(EBML::IWriter& rWriter);

	protected:

		OpenViBE::uint32 m_ui32SamplingRate;

		EBML::CWriterHelper m_oWriterHelper;
	};
};

#endif // __OpenViBEToolkit_Writer_CBoxAlgorithmSignalOutputWriterPart_INL__
