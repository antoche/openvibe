#ifndef __OpenViBE_Kernel_Scenario_IScenarioExporterContext_H__
#define __OpenViBE_Kernel_Scenario_IScenarioExporterContext_H__

#include "../ovIKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class OV_API IScenarioExporterContext : public OpenViBE::Kernel::IKernelObject
		{
		public:

			virtual const OpenViBE::CString getFileName(void) const=0;
			virtual const OpenViBE::Kernel::IScenario& getScenario(void) const=0;

			_IsDerivedFromClass_(OpenViBE::Kernel::IKernelObject, OV_ClassId_Kernel_Scenario_ScenarioExporterContext);

		};
	};
};

#endif // __OpenViBE_Kernel_Scenario_IScenarioExporterContext_H__
