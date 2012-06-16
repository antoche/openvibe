#ifndef __OpenViBEToolkit_IObject_H__
#define __OpenViBEToolkit_IObject_H__

#include "ovtk_base.h"

namespace OpenViBEToolkit
{
	class OVTK_API IObject : public OpenViBE::IObject
	{
	public:

		_IsDerivedFromClass_(OpenViBE::IObject, OVTK_ClassId_);
	};
};

#endif // __OpenViBEToolkit_IObject_H__
