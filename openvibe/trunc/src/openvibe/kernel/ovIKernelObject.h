#ifndef __OpenViBE_Kernel_IKernelObject_H__
#define __OpenViBE_Kernel_IKernelObject_H__

#include "../ovIObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		/**
		 * \class IKernelObject
		 * \brief Base class for all kernel objects
		 * \author Yann Renard (INRIA/IRISA)
		 * \date 2006-09-26
		 * \ingroup Group_Kernel
		 */
		class OV_API IKernelObject : public OpenViBE::IObject
		{
		public:

			_IsDerivedFromClass_(OpenViBE::IObject, OV_ClassId_Kernel_KernelObject)

		};
	};
};

#endif // __OpenViBE_Kernel_IKernelObject_H__
