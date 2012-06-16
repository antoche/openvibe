#ifndef __OpenViBEToolkit_Writer_CBoxAlgorithmFeatureVectorOutputWriterPart_INL__
#define __OpenViBEToolkit_Writer_CBoxAlgorithmFeatureVectorOutputWriterPart_INL__

#include "ovtkIBoxAlgorithmEBMLOutputWriter.h"
#include "ovtkCBoxAlgorithmEBMLOutputWriterPartBase.inl"

#include <iostream>

namespace OpenViBEToolkit
{
	class CBoxAlgorithmFeatureVectorOutputWriterPart : public OpenViBEToolkit::CBoxAlgorithmEBMLOutputWriterPartBase
	{
	public:

		_IsDerivedFromClass_Final_(OpenViBEToolkit::CBoxAlgorithmEBMLOutputWriterPartBase, OVTK_ClassId_);

	protected:

		// virtual OpenViBE::boolean writeHeaderStart(EBML::IWriter& rWriter);
		// virtual OpenViBE::boolean writeHeaderStop(EBML::IWriter& rWriter);
		// virtual OpenViBE::boolean writeBufferStart(EBML::IWriter& rWriter);
		// virtual OpenViBE::boolean writeBufferStop(EBML::IWriter& rWriter);
		// virtual OpenViBE::boolean writeEndStart(EBML::IWriter& rWriter);
		// virtual OpenViBE::boolean writeEndStop(EBML::IWriter& rWriter);

	};
};

#endif // __OpenViBEToolkit_Writer_CBoxAlgorithmFeatureVectorOutputWriterPart_INL__
