#ifndef __OpenViBEKernel_Kernel_CKernelObjectFactory_H__
#define __OpenViBEKernel_Kernel_CKernelObjectFactory_H__

#include "ovkTKernelObject.h"

#include <vector>

namespace OpenViBE
{
	namespace Kernel
	{
		class CKernelObjectFactory : public OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IKernelObjectFactory>
		{
		public:

			CKernelObjectFactory(const OpenViBE::Kernel::IKernelContext& rKernelContext);

			virtual OpenViBE::IObject* createObject(
				const OpenViBE::CIdentifier& rClassIdentifier);
			virtual OpenViBE::boolean releaseObject(
				OpenViBE::IObject* pObject);

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::IKernelObjectFactory, OVK_ClassId_Kernel_KernelObjectFactory)

		protected:

			std::vector<OpenViBE::IObject*> m_oCreatedObjects;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_CObjectFactory_H__
