#ifndef __OpenViBE_Kernel_IKernel_H__
#define __OpenViBE_Kernel_IKernel_H__

#include "ovIKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class IKernelContext;

		/**
		 * \class IKernel
		 * \brief Kernel main class, mostly provides access to the kernel context
		 * \author Yann Renard (INRIA/IRISA)
		 * \date 2006-09-26
		 * \ingroup Group_Kernel
		 */
		class OV_API IKernel : public OpenViBE::Kernel::IKernelObject
		{
		public:

			/**
			 * \brief Releases this kernel
			 *
			 * This function tells the kernel it won't be used any more. When called,
			 * the kernel should release any memory / resource it owns. The client code
			 * will not be allowed to access any component of the kernel after this call.
			 */
			virtual void release(void)=0;
			/**
			 * \brief Gets the kernel context for this kernel
			 * \return the kernel context for this kernel.
			 *
			 * The kernel context is used to access the kernel's managers.
			 */
			virtual const OpenViBE::Kernel::IKernelContext* getContext(void) const { return NULL; }

			_IsDerivedFromClass_(OpenViBE::Kernel::IKernelObject, OV_ClassId_Kernel_Kernel)
		};
	};
};

#endif // __OpenViBE_Kernel_IKernel_H__
