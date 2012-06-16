#ifndef __OpenViBEApplication_CShooterTarget_H__
#define __OpenViBEApplication_CShooterTarget_H__

#include <Ogre.h>
#include <iostream>

#include "../ovassvepCBasicPainter.h"

#define SSVEP_SHOOTER_TARGET_SIZE 0.15f

namespace OpenViBESSVEP
{
	class CShooterTarget
	{
		public:
			static CShooterTarget* createTarget( Ogre::Radian rAngle );
			static void initialize( CBasicPainter* poPainter, Ogre::SceneNode* poParentNode );
			~CShooterTarget();

			bool collidesWith(std::pair<Ogre::Real, Ogre::Real>);

		private:
			static Ogre::SceneNode* m_poParentNode;
			static CBasicPainter* m_poPainter;

			Ogre::SceneNode* m_poTargetNode;

			CShooterTarget( Ogre::Radian rAngle );

	};
}


#endif // __OpenViBEApplication_CShooterTarget_H__
