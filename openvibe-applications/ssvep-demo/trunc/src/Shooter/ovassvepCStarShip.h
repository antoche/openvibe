#ifndef __OpenViBEApplication_CStarShip_H__
#define __OpenViBEApplication_CStarShip_H__

#include <Ogre.h>

#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

namespace OpenViBESSVEP
{
	class CApplication;
	class CSSVEPFlickeringObject;

	class CStarShip
	{
		public:
			CStarShip( CApplication* poApplication, Ogre::SceneNode* poParentNode, Ogre::Real rRadius, std::vector<std::pair<OpenViBE::uint32, OpenViBE::uint32> >* pFrequencies);
			void processFrame( OpenViBE::uint32 ui32CurrentFrame );

			void rotate( int iRotationCount );

			bool isShooting()
			{
				return m_bIsShooting;
			}

			void shoot();
			std::pair<Ogre::Real, Ogre::Real> getShotCoordinates();

		private:
			Ogre::SceneNode* m_poShipNode;

			CSSVEPFlickeringObject* m_poShipCannon;
			CSSVEPFlickeringObject* m_poShipLeftWing;
			CSSVEPFlickeringObject* m_poShipRightWing;

			Ogre::Real m_rAngularSpeed;
			Ogre::Radian m_rCurrentAngle;
			int m_iCurrentRotationCount;

			Ogre::SceneNode* m_poShotNode;
			bool m_bIsShooting;
			Ogre::Real m_rShotDistance;


	};
}

#endif // __OpenViBEApplication_CStarShip_H__
