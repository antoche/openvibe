#ifndef __OpenViBE_Kernel_Player_IPlayer_H__
#define __OpenViBE_Kernel_Player_IPlayer_H__

#include "../ovIKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class IScenario;

		enum EPlayerStatus
		{
			PlayerStatus_Stop,
			PlayerStatus_Pause,
			PlayerStatus_Step,
			PlayerStatus_Play,
			PlayerStatus_Forward,
		};

		/**
		 * \class IPlayer
		 * \author Yann Renard (INRIA/IRISA)
		 * \date 2006-09-26
		 * \brief The main player class
		 * \ingroup Group_Player
		 * \ingroup Group_Kernel
		 *
		 * A player is responsible for the playback of a specific scenario.
		 * This player scenario is provided at initialisation stage and should
		 * not be changed until the player terminates. The player idea of calling
		 * a "play" function forces the use of threads in order to avoid CPU locking.
		 * Thus we prefer the idea of having a "short-time" function that is to be
		 * called repeatedly until the player terminates. This is the role of the
		 * \c IPlayer::loop function, that should be called repeatedly by the outside
		 * application.
		 *
		 * \todo Refactor this base class to propose an interface
		 *       more §OpenViBE§ compliant (use IDs, intialize/unitialize
		 *       pairs etc...)
		 */
		class OV_API IPlayer : public OpenViBE::Kernel::IKernelObject
		{
		public:

			virtual OpenViBE::boolean setScenario(
				const OpenViBE::CIdentifier& rScenarioIdentifier)=0;

			virtual OpenViBE::boolean stop(void)=0;
			virtual OpenViBE::boolean pause(void)=0;
			virtual OpenViBE::boolean step(void)=0;
			virtual OpenViBE::boolean play(void)=0;
			virtual OpenViBE::boolean forward(void)=0;

			virtual OpenViBE::Kernel::EPlayerStatus getStatus(void) const=0;

			/**
			 * \brief "short time" function to be called repeatedly by the outstide application
			 * \param ui64ElapsedTime [in] : real elapsed time given in seconds 32:32
			 * \return \e true if the execution went successfully.
			 * \return \e false in case their was a problem or the execution terminated.
			 */
			virtual OpenViBE::boolean loop(
				const OpenViBE::uint64 ui64ElapsedTime)=0;

			virtual OpenViBE::uint64 getCurrentSimulatedTime(void) const=0;

			_IsDerivedFromClass_(OpenViBE::Kernel::IKernelObject, OV_ClassId_Kernel_Player_Player);
		};
	};
};

#endif // __OpenViBE_Kernel_Player_IPlayer_H__
