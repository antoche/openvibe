#ifndef __OpenViBEToolkit_Reader_IBoxAlgorithmEBMLInputReaderCallback_H__
#define __OpenViBEToolkit_Reader_IBoxAlgorithmEBMLInputReaderCallback_H__

#include "../../ovtkIObject.h"

#include <ebml/IReader.h>

namespace OpenViBEToolkit
{
	class OVTK_API IBoxAlgorithmEBMLInputReaderCallback : public OpenViBEToolkit::IObject, virtual public EBML::IReaderCallback
	{
	public:

		_IsDerivedFromClass_(OpenViBEToolkit::IObject, OVTK_ClassId_);
	};
};

#endif // __OpenViBEToolkit_Reader_IBoxAlgorithmEBMLInputReaderCallback_H__
