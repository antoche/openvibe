#ifndef __OpenViBE_Kernel_Scenario_IScenarioImporterContext_H__
#define __OpenViBE_Kernel_Scenario_IScenarioImporterContext_H__

#include "../ovIKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class OV_API IScenarioImporterContext : public OpenViBE::Kernel::IKernelObject
		{
		public:

			virtual OpenViBE::CString getFileName(void) const=0;
			virtual OpenViBE::Kernel::IScenario& getScenario(void) const=0;

			_IsDerivedFromClass_(OpenViBE::Kernel::IKernelObject, OV_ClassId_Kernel_Scenario_ScenarioImporterContext);

		};
	};
};

#endif // __OpenViBE_Kernel_Scenario_IScenarioImporterContext_H__
