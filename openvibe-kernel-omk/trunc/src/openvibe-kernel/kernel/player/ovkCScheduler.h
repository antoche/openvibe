#ifndef __OpenViBEKernel_Kernel_Scheduler_CScheduler_H__
#define __OpenViBEKernel_Kernel_Scheduler_CScheduler_H__

#include "../ovkTKernelObject.h"

#include <system/CChrono.h>

#include <map>
#include <list>

namespace OpenViBE
{
	namespace Kernel
	{
		class CSimulatedBox;
		class CChunk;
		class CPlayer;

		class CScheduler : public OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IKernelObject >
		{
		public:

			CScheduler(const OpenViBE::Kernel::IKernelContext& rKernelContext, OpenViBE::Kernel::CPlayer& rPlayer);
			virtual ~CScheduler(void);

			virtual OpenViBE::boolean setScenario(
				const OpenViBE::CIdentifier& rScenarioIdentifier);
			virtual OpenViBE::boolean setFrequency(
				const OpenViBE::uint64 ui64Frequency);

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);

			virtual OpenViBE::boolean loop(void);

			virtual OpenViBE::boolean sendInput(const OpenViBE::Kernel::CChunk& rChunk, const OpenViBE::CIdentifier& rBoxIdentifier, const OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::uint64 getCurrentTime(void) const;
			virtual OpenViBE::uint64 getFrequency(void) const;
			virtual OpenViBE::float64 getCPUUsage(void) const;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IKernelObject >, OVK_ClassId_Kernel_Player_Scheduler);

			CPlayer& getPlayer(void)
			{
				return m_rPlayer;
			}

		protected:

			OpenViBE::Kernel::CPlayer& m_rPlayer;
			OpenViBE::CIdentifier m_oScenarioIdentifier;
			OpenViBE::Kernel::IScenario* m_pScenario;
			OpenViBE::uint64 m_ui64Steps;
			OpenViBE::uint64 m_ui64Frequency;
			OpenViBE::uint64 m_ui64CurrentTime;
			OpenViBE::boolean m_bIsInitialized;

			std::map < std::pair < OpenViBE::int32, OpenViBE::CIdentifier>, OpenViBE::Kernel::CSimulatedBox* > m_vSimulatedBox;
			std::map < OpenViBE::CIdentifier, System::CChrono > m_vSimulatedBoxChrono;
			std::map < OpenViBE::CIdentifier, std::map < OpenViBE::uint32, std::list < OpenViBE::Kernel::CChunk > > > m_vSimulatedBoxInput;

		private:

			System::CChrono m_oBenchmarkChrono;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_Scheduler_CScheduler_H__
