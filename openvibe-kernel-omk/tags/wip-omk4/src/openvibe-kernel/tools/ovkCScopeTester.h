#ifndef __OpenViBEKernel_Tools_CScopeTester_H__
#define __OpenViBEKernel_Tools_CScopeTester_H__

#include "../ovk_base.h"

#include <openvibe/ov_all.h>

namespace OpenViBE
{
	namespace Tools
	{
		class CScopeTester : virtual OpenViBE::IObject
		{
		public:

			CScopeTester(const OpenViBE::Kernel::IKernelContext& rKernelCotnext, const OpenViBE::CString& sPrefix);
			virtual ~CScopeTester(void);

			_IsDerivedFromClass_Final_(IObject, OVK_ClassId_Tools_ScopeTester);

		protected:

			OpenViBE::CString m_sPrefix;
			const OpenViBE::Kernel::IKernelContext& m_rKernelContext;
		};
	};
};

#endif // __OpenViBEKernel_Tools_CScopeTester_H__
