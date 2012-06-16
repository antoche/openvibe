#ifndef __OpenViBE_Kernel_Player_IPlayerManager_H__
#define __OpenViBE_Kernel_Player_IPlayerManager_H__

#include "../ovIKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class IPlayer;

		/**
		 * \class IPlayerManager
		 * \author Yann Renard (INRIA/IRISA)
		 * \date 2006-07-21
		 * \brief The manager responsible for scenario playback
		 * \ingroup Group_Player
		 * \ingroup Group_Kernel
		 *
		 * The player manager is responsible for creating the different player
		 * instances. Each player is responsible for the playback of a specific
		 * scenario.
		 */
		class OV_API IPlayerManager : public OpenViBE::Kernel::IKernelObject
		{
		public:

			/**
			 * \brief Creates a new player
			 * \param rPlayerIdentifier [out] : the identifier of the created player
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean createPlayer(
				OpenViBE::CIdentifier& rPlayerIdentifier)=0;
			/**
			 * \brief Releases a specific player
			 * \param rPlayerIdentifier [in] : the identifier of the player to release
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 *
			 * Releasing a player means the player instance won't be used any more,
			 * thus the player manager can release any allocated resource / memory
			 * for this player.
			 */
			virtual OpenViBE::boolean releasePlayer(
				const OpenViBE::CIdentifier& rPlayerIdentifier)=0;
			/**
			 * \brief Gets a specific player instance, provided its identifier
			 * \param rPlayerIdentifier [in] : the identifier of the player instance to return
			 * \return A reference on the player instance
			 * \warning Using a non player identifier will cause a crash
			 */
			virtual OpenViBE::Kernel::IPlayer& getPlayer(
				const OpenViBE::CIdentifier& rPlayerIdentifier)=0;
			/**
			 * \brief Gets next player identifier
			 * \param rPreviousIdentifier [in] : The identifier
			 *        for the preceeding player
			 * \return The identifier of the next player in case of success.
			 * \return \c OV_UndefinedIdentifier on error.
			 * \note Giving \c OV_UndefinedIdentifier as \c rPreviousIdentifier
			 *       will cause this function to return the first player
			 *       identifier.
			 */
			virtual OpenViBE::CIdentifier getNextPlayerIdentifier(
				const OpenViBE::CIdentifier& rPreviousIdentifier) const=0;

			_IsDerivedFromClass_(OpenViBE::Kernel::IKernelObject, OV_ClassId_Kernel_Player_PlayerManager);
		};
	};
};

#endif // __OpenViBE_Kernel_Player_IPlayerManager_H__
