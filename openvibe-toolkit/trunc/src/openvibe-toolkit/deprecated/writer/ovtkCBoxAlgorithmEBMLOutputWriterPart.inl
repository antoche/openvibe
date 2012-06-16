#ifndef __OpenViBEToolkit_Writer_CBoxAlgorithmEBMLOutputWriterPart_INL__
#define __OpenViBEToolkit_Writer_CBoxAlgorithmEBMLOutputWriterPart_INL__

#include "ovtkCBoxAlgorithmEBMLOutputWriterPartBase.inl"

#include <ebml/CWriterHelper.h>

namespace OpenViBEToolkit
{
	class CBoxAlgorithmEBMLOutputWriterPart : public OpenViBEToolkit::CBoxAlgorithmEBMLOutputWriterPartBase
	{
	public:

		_IsDerivedFromClass_Final_(OpenViBEToolkit::CBoxAlgorithmEBMLOutputWriterPartBase, OVTK_ClassId_);

	protected:

		virtual OpenViBE::boolean writeHeaderStart(EBML::IWriter& rWriter);
		virtual OpenViBE::boolean writeHeaderStop(EBML::IWriter& rWriter);
		virtual OpenViBE::boolean writeBufferStart(EBML::IWriter& rWriter);
		virtual OpenViBE::boolean writeBufferStop(EBML::IWriter& rWriter);
		virtual OpenViBE::boolean writeEndStart(EBML::IWriter& rWriter);
		virtual OpenViBE::boolean writeEndStop(EBML::IWriter& rWriter);

	protected:

		EBML::CWriterHelper m_oWriterHelper;
	};
};

#endif // __OpenViBEToolkit_Writer_CBoxAlgorithmEBMLOutputWriterPart_INL__
