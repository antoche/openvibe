#ifndef __OpenViBE_Kernel_Player_IPlayerContext_H__
#define __OpenViBE_Kernel_Player_IPlayerContext_H__

#include "../ovIKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class IConfigurationManager;
		class ILogManager;
		class IMessageClock;
		class IMessageEvent;
		class IMessageSignal;
		class IScenarioManager;

		/**
		 * \class IPlayerContext
		 * \author Yann Renard (INRIA/IRISA)
		 * \date 2006-07-07
		 * \brief Player interface for plugin objects
		 * \ingroup Group_Player
		 * \ingroup Group_Kernel
		 *
		 * Instances of this class are given to plugin object
		 * so they can communicate with the platform kernel,
		 * providing services such as message sending etc...
		 */
		class OV_API IPlayerContext : public OpenViBE::Kernel::IKernelObject
		{
		public:

			/** \name Messages sending */
			//@{

			/**
			 * \brief Sends signal to the kernel
			 * \param rMessageSignal [in] : the signal message to send
			 * \return \e true in case of success, \e false in other cases.
			 */
			virtual OpenViBE::boolean sendSignal(
				const OpenViBE::Kernel::IMessageSignal& rMessageSignal)=0;
			/**
			 * \brief Sends a message to another box
			 * \param rMessageEvent [in] : the message to send to the other box
			 * \param rTargetIdentifier [in] : the identifier of the other box
			 * \return \e true in case of success, \e false in other cases.
			 */
			virtual OpenViBE::boolean sendMessage(
				const OpenViBE::Kernel::IMessageEvent& rMessageEvent,
				const OpenViBE::CIdentifier& rTargetIdentifier)=0;
			/**
			 * \brief Broadcasts a message to several other boxes
			 * \param rMessageEvent [in] : the message to send
			 * \param pTargetIdentifier [in] : a pointer containing all
			 *        the targets which should receive the sent message.
			 * \param ui32TargetIdentifierCount [in] : the number of
			 *        targets contained in the \c pTargetIdentifier array.
			 * \return \e true in case of success, \e false in other cases.
			 */
			virtual OpenViBE::boolean sendMessage(
				const OpenViBE::Kernel::IMessageEvent& rMessageEvent,
				const OpenViBE::CIdentifier* pTargetIdentifier,
				const OpenViBE::uint32 ui32TargetIdentifierCount)=0;

			//@}
			/** \name Time management */
			//@{

			/**
			 * \brief Gets the current player time
			 * \return the current player time.
			 * \note The time value is fixed point 32:32 representated in seconds
			 */
			virtual OpenViBE::uint64 getCurrentTime(void)=0;

			//@}
			/** \name Player control */
			//@{

			/**
			 * \brief Stops player execution
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 * \warning Once the player is stopped, there's no way to restart it
			 */
			virtual OpenViBE::boolean stop(void)=0;
			/**
			 * \brief Pauses player execution
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean pause(void)=0;
			/**
			 * \brief Makes player run normal speed
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean play(void)=0;
			/**
			 * \brief Makes player run as fast as possible
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean forward(void)=0;
			/**
			 * \brief Gets current player status
			 * \return current player status
			 */
			virtual OpenViBE::Kernel::EPlayerStatus getStatus(void) const=0;

			//@}
			/** \name Give access to some managers */
			//@{

			/**
			 * \brief Gets the current player' configuration manager
			 * \return The current player' configuration manager
			 */
			virtual OpenViBE::Kernel::IConfigurationManager& getConfigurationManager(void)=0;
			/**
			 * \brief Gets the current player' algorithm manager
			 * \return The current player' algorithm manager
			 */
			virtual OpenViBE::Kernel::IAlgorithmManager& getAlgorithmManager(void)=0;
			/**
			 * \brief Gets the current player' log manager
			 * \return The current player' log manager
			 */
			virtual OpenViBE::Kernel::ILogManager& getLogManager(void)=0;
			/**
			 * \brief Gets the current player' scenario manager
			 * \return The current player' scenario manager
			 */
			virtual OpenViBE::Kernel::IScenarioManager& getScenarioManager(void)=0;
			/**
			 * \brief Gets the current player' type manager
			 * \return The current player' type manager
			 */
			virtual OpenViBE::Kernel::ITypeManager& getTypeManager(void)=0;

			//@}

			_IsDerivedFromClass_(OpenViBE::Kernel::IKernelObject, OV_ClassId_Kernel_Player_PlayerContext)
		};
	};
};

#endif // __OpenViBE_Kernel_Player_IPlayerContext_H__
