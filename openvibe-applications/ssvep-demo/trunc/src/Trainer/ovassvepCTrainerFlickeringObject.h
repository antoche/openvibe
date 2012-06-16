#ifndef __OpenViBEApplication_CTrainerFlickeringObject_H__
#define __OpenViBEApplication_CTrainerFlickeringObject_H__

#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>
#include <Ogre.h>

#include "../ovassvepCSSVEPFlickeringObject.h"
#include "../ovassvepCBasicPainter.h"

/**
 */
namespace OpenViBESSVEP
{
	class CTrainerApplication;

	class CTrainerFlickeringObject : public CSSVEPFlickeringObject
	{
		public:
			static CTrainerFlickeringObject* createTrainerFlickeringObject( OpenViBE::uint32 );
			static void initialize( CTrainerApplication* poApplication );

			void connectToNode( Ogre::SceneNode* poSceneNode );
			void setTarget( OpenViBE::boolean bIsTarget );

			OpenViBE::boolean isTarget()
			{
				return m_poPointer->getVisible();
			}

		private:
			static CTrainerApplication* m_poApplication;
			static Ogre::SceneNode* m_poParentNode;
			static CBasicPainter* m_poPainter;
			static OpenViBE::float32 m_f32TargetWidth;
			static OpenViBE::float32 m_f32TargetHeight;
			static Ogre::ColourValue m_oLightColour;
			static Ogre::ColourValue m_oDarkColour;

			CTrainerFlickeringObject( OpenViBE::float32 f32PosX, OpenViBE::float32 f32PosY, Ogre::ColourValue oColour, OpenViBE::uint8 ui8LitFrames, OpenViBE::uint8 ui8DarkFrames );


			Ogre::SceneNode* m_poElementNode;
			Ogre::MovableObject* m_poPointer;
	};
}


#endif // __OpenViBEApplication_CTrainerFlickeringObject_H__
