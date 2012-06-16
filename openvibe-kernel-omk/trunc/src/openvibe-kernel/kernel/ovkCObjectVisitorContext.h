#ifndef __OpenViBEKernel_Kernel_CObjectVisitorContext_H__
#define __OpenViBEKernel_Kernel_CObjectVisitorContext_H__

#include "ovkTKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class CObjectVisitorContext : public OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IObjectVisitorContext >
		{
		public:

			CObjectVisitorContext(const OpenViBE::Kernel::IKernelContext& rKernelContext);
			virtual ~CObjectVisitorContext(void);

			virtual OpenViBE::Kernel::IAlgorithmManager& getAlgorithmManager(void);
			virtual OpenViBE::Kernel::IConfigurationManager& getConfigurationManager(void);
			virtual OpenViBE::Kernel::ITypeManager& getTypeManager(void);
			virtual OpenViBE::Kernel::ILogManager& getLogManager(void);

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IObjectVisitorContext >, OVK_ClassId_Kernel_ObjectVisitorContext)

		protected:

			OpenViBE::Kernel::ILogManager* m_pLogManager;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_CObjectVisitorContext_H__
