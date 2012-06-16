#ifndef __OpenViBEKernel_Kernel_Player_CPlayer_H__
#define __OpenViBEKernel_Kernel_Player_CPlayer_H__

#include "../ovkTKernelObject.h"
#include "ovkCScheduler.h"

#include "../ovkCKernelContext.h"

#include <system/CChrono.h>

#include <map>

namespace OpenViBE
{
	namespace Kernel
	{
		class CPlayer : public OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IPlayer >
		{
		public:

			CPlayer(const OpenViBE::Kernel::IKernelContext& rKernelContext);
			virtual ~CPlayer(void);

			virtual OpenViBE::boolean setScenario(
				const OpenViBE::CIdentifier& rScenarioIdentifier);

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);

			virtual OpenViBE::boolean stop(void);
			virtual OpenViBE::boolean pause(void);
			virtual OpenViBE::boolean step(void);
			virtual OpenViBE::boolean play(void);
			virtual OpenViBE::boolean forward(void);

			virtual OpenViBE::Kernel::EPlayerStatus getStatus(void) const;
			virtual OpenViBE::float64 getCPUUsage(const OpenViBE::CIdentifier& rProcessingUnitIdentifier) const;

			virtual OpenViBE::boolean loop(
				const OpenViBE::uint64 ui64ElapsedTime);

			virtual OpenViBE::uint64 getCurrentSimulatedTime(void) const;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IPlayer >, OVK_ClassId_Kernel_Player_Player);

		protected:

			OpenViBE::Kernel::CKernelContextBridge m_oKernelContextBridge;
			OpenViBE::Kernel::IConfigurationManager* m_pLocalConfigurationManager;

			OpenViBE::Kernel::CScheduler m_oScheduler;

			OpenViBE::uint64 m_ui64CurrentTimeToReach;
			OpenViBE::uint64 m_ui64Lateness;
			OpenViBE::Kernel::EPlayerStatus m_eStatus;

		private:

			System::CChrono m_oBenchmarkChrono;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_Player_CPlayer_H__
