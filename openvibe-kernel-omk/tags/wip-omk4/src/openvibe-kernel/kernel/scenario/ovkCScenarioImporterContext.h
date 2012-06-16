#ifndef __OpenViBEKernel_Kernel_Scenario_CScenarioImporterContext_H__
#define __OpenViBEKernel_Kernel_Scenario_CScenarioImporterContext_H__

#include "../ovkTKernelObject.h"

#include <openvibe/ov_all.h>

namespace OpenViBE
{
	namespace Kernel
	{
		class CScenarioImporterContext : public OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IScenarioImporterContext>
		{
		public:

			CScenarioImporterContext(
				const OpenViBE::Kernel::IKernelContext& rKernelContext,
				const OpenViBE::CString& sFileName,
				OpenViBE::Kernel::IScenario& rScenario);

			virtual OpenViBE::CString getFileName(void) const;
			virtual OpenViBE::Kernel::IScenario& getScenario(void) const;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IScenarioImporterContext>, OVK_ClassId_Kernel_Scenario_ScenarioImporterContext);

		protected:

			const OpenViBE::CString m_sFileName;
			mutable OpenViBE::Kernel::IScenario& m_rScenario;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_Scenario_CScenarioImporterContext_H__
