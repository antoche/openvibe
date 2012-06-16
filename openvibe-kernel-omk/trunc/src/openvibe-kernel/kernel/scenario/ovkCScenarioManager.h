#ifndef __OpenViBEKernel_Kernel_Scenario_CScenarioManager_H__
#define __OpenViBEKernel_Kernel_Scenario_CScenarioManager_H__

#include "../ovkTKernelObject.h"

#include <map>

namespace OpenViBE
{
	namespace Kernel
	{
		class CScenario;

		class CScenarioManager : public OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IScenarioManager>
		{
		public:

			CScenarioManager(const OpenViBE::Kernel::IKernelContext& rKernelContext);
			virtual ~CScenarioManager(void);

			virtual OpenViBE::CIdentifier getNextScenarioIdentifier(
				const OpenViBE::CIdentifier& rPreviousIdentifier) const;
			virtual OpenViBE::boolean createScenario(
				OpenViBE::CIdentifier& rScenarioIdentifier);
			virtual OpenViBE::boolean releaseScenario(
				const OpenViBE::CIdentifier& rScenarioIdentifier);
			virtual OpenViBE::Kernel::IScenario& getScenario(
				const OpenViBE::CIdentifier& rScenarioIdentifier);

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IScenarioManager>, OVK_ClassId_Kernel_Scenario_ScenarioManager);

		protected:

			virtual OpenViBE::CIdentifier getUnusedIdentifier(void) const;

		protected:

			std::map<OpenViBE::CIdentifier, OpenViBE::Kernel::CScenario*> m_vScenario;

		};
	};
};

#endif // __OpenViBEKernel_Kernel_Scenario_CScenarioManager_H__
