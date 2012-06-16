#ifndef __OpenViBEKernel_Kernel_Plugins_CPluginModule_H__
#define __OpenViBEKernel_Kernel_Plugins_CPluginModule_H__

#include "../ovkTKernelObject.h"

#include <openvibe/ov_all.h>

namespace OpenViBE
{
	namespace Kernel
	{
		class CPluginModule : public OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IPluginModule>
		{
		public:

			CPluginModule(const OpenViBE::Kernel::IKernelContext& rKernelContext);
			virtual ~CPluginModule(void);

			virtual OpenViBE::boolean load(
				const OpenViBE::CString& sName,
				OpenViBE::CString* pError);
			virtual OpenViBE::boolean unload(
				OpenViBE::CString* pError);
			virtual OpenViBE::boolean getFileName(
				OpenViBE::CString& rFileName) const;

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean getPluginObjectDescription(
				OpenViBE::uint32 ui32Index,
				OpenViBE::Plugins::IPluginObjectDesc*& rpPluginObjectDescription);
			virtual OpenViBE::boolean uninitialize(void);

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::IPluginModule, OVK_ClassId_Kernel_Plugins_PluginModule)

		protected:

			OpenViBE::Kernel::IPluginModule* m_pImplementation;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_Plugins_CPluginModule_H__
