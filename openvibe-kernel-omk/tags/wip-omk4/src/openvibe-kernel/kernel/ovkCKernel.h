#ifndef __OpenViBEKernel_CKernel_H__
#define __OpenViBEKernel_CKernel_H__

#include "ovkTKernelObject.h"
#include "ovkCKernelContext.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class CKernel : public OpenViBE::Kernel::IKernel
		{
		public:

			CKernel(void);

			virtual void release(void);

			// $$$$$$$$$$$$$$$$$$
			virtual const OpenViBE::Kernel::IKernelContext* getContext(void) const
			{
				return &m_oContext;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::IKernel, OVK_ClassId_Kernel_Kernel)

		protected:

			OpenViBE::Kernel::CKernelContext m_oContext;
		};
	};
};

#endif // __OpenViBEKernel_CKernel_H__
