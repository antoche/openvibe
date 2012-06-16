#ifndef __OpenViBEToolkit_Writer_IBoxAlgorithmEBMLOutputWriter_H__
#define __OpenViBEToolkit_Writer_IBoxAlgorithmEBMLOutputWriter_H__

#include "../../ovtkIObject.h"

namespace OpenViBEToolkit
{
	class OVTK_API IBoxAlgorithmEBMLOutputWriter : public OpenViBEToolkit::IObject
	{
	public:

		virtual OpenViBE::boolean writeHeader(EBML::IWriter& rWriter)=0;
		virtual OpenViBE::boolean writeBuffer(EBML::IWriter& rWriter)=0;
		virtual OpenViBE::boolean writeEnd(EBML::IWriter& rWriter)=0;

		_IsDerivedFromClass_(OpenViBEToolkit::IObject, OVTK_ClassId_);
	};
};

#endif // __OpenViBEToolkit_Writer_IBoxAlgorithmEBMLOutputWriter_H__
