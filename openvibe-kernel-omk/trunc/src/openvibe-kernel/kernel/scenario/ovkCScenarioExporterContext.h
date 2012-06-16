#ifndef __OpenViBEKernel_Kernel_Scenario_CScenarioExporterContext_H__
#define __OpenViBEKernel_Kernel_Scenario_CScenarioExporterContext_H__

#include "../ovkTKernelObject.h"

#include <openvibe/ov_all.h>

namespace OpenViBE
{
	namespace Kernel
	{
		class CScenarioExporterContext : public OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IScenarioExporterContext>
		{
		public:

			CScenarioExporterContext(
				const OpenViBE::Kernel::IKernelContext& rKernelContext,
				const OpenViBE::CString& sFileName,
				const OpenViBE::Kernel::IScenario& rScenario);

			virtual const OpenViBE::CString getFileName(void) const;
			virtual const OpenViBE::Kernel::IScenario& getScenario(void) const;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IScenarioExporterContext>, OVK_ClassId_Kernel_Scenario_ScenarioExporterContext);

		protected:

			const OpenViBE::CString m_sFileName;
			const OpenViBE::Kernel::IScenario& m_rScenario;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_Scenario_CScenarioExporterContext_H__
