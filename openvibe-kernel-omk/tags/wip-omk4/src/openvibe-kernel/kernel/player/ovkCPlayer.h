#ifndef __OpenViBEKernel_Kernel_Player_CPlayer_H__
#define __OpenViBEKernel_Kernel_Player_CPlayer_H__

#include "../ovkTKernelObject.h"

#include <openvibe/ov_all.h>

#include <system/CChrono.h>

namespace OMK
{	
	class Controller;
	class ReferenceObjectHandle;
	class ObjectDescriptor;
};

namespace OpenViBE
{
	namespace Kernel
	{
		class CPlayer : public OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IPlayer>
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

			virtual OpenViBE::boolean loop(
				const OpenViBE::uint64 ui64ElapsedTime);

			virtual OpenViBE::uint64 getCurrentSimulatedTime(void) const;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::IPlayer, OVK_ClassId_Kernel_Player_Player);

		protected:

			OMK::Controller* m_pController;
			OMK::ReferenceObjectHandle* m_pControllerHandle;
			OMK::ObjectDescriptor* m_pSimulation;

			OpenViBE::CIdentifier m_oScenarioIdentifier;

			OpenViBE::uint32 m_ui32ControllerSteps;
			OpenViBE::uint32 m_ui32SecondsLate;

			OpenViBE::uint64 m_ui64LastSimulatedTime;
			OpenViBE::uint64 m_ui64CurrentSimulatedTime;
			OpenViBE::Kernel::EPlayerStatus m_eStatus;
			OpenViBE::boolean m_bIsInitialized;

		private:

			System::CChrono m_oBenchmarkChrono;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_Player_CPlayer_H__
