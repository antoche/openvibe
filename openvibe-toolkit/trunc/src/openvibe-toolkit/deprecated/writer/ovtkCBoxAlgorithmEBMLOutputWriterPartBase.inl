#ifndef __OpenViBEToolkit_Writer_CBoxAlgorithmEBMLOutputWriterPartBase_INL__
#define __OpenViBEToolkit_Writer_CBoxAlgorithmEBMLOutputWriterPartBase_INL__

#include "ovtkIBoxAlgorithmEBMLOutputWriter.h"

#include <vector>

namespace OpenViBEToolkit
{
	class CBoxAlgorithmEBMLOutputWriterPartBase : public OpenViBEToolkit::IBoxAlgorithmEBMLOutputWriter
	{
	public:

		virtual OpenViBE::boolean registerBoxAlgorithmEBMLOutputWriterPart(OpenViBEToolkit::IBoxAlgorithmEBMLOutputWriter& rBoxAlgorithmEBMLOutputWriter);
		virtual OpenViBE::boolean unregisterBoxAlgorithmEBMLOutputWriterPart(OpenViBEToolkit::IBoxAlgorithmEBMLOutputWriter& rBoxAlgorithmEBMLOutputWriter);

		virtual OpenViBE::boolean writeHeader(EBML::IWriter& rWriter);
		virtual OpenViBE::boolean writeBuffer(EBML::IWriter& rWriter);
		virtual OpenViBE::boolean writeEnd(EBML::IWriter& rWriter);

		_IsDerivedFromClass_(OpenViBEToolkit::IBoxAlgorithmEBMLOutputWriter, OVTK_ClassId_);

	protected:

		virtual OpenViBE::boolean writeHeaderStart(EBML::IWriter& rWriter);
		virtual OpenViBE::boolean writeHeaderStop(EBML::IWriter& rWriter);
		virtual OpenViBE::boolean writeBufferStart(EBML::IWriter& rWriter);
		virtual OpenViBE::boolean writeBufferStop(EBML::IWriter& rWriter);
		virtual OpenViBE::boolean writeEndStart(EBML::IWriter& rWriter);
		virtual OpenViBE::boolean writeEndStop(EBML::IWriter& rWriter);

	protected:

		std::vector<OpenViBEToolkit::IBoxAlgorithmEBMLOutputWriter*> m_vBoxAlgorithmEBMLOutputWriter;
	};
};

#endif // __OpenViBEToolkit_Writer_CBoxAlgorithmEBMLOutputWriterPartBase_INL__
