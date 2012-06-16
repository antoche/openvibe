#ifndef __OpenViBEApplication_CBasicPainter_H__
#define __OpenViBEApplication_CBasicPainter_H__

#include <Ogre.h>

#define SSVEP_DEFAULT_COLOUR Ogre::ColourValue(1.0f, 1.0f, 1.0f)

namespace OpenViBESSVEP
{
	class CApplication;

	class Point
	{
		public:
			Point(float _x, float _y) :
				x(_x), y(_y)
		{};

			float x, y;
	};

	class CBasicPainter
	{
		public:
			CBasicPainter( CApplication* poApplication );
			~CBasicPainter() {};

			Ogre::ManualObject* paintRectangle( Ogre::Rectangle oRectangle, Ogre::ColourValue oColour = SSVEP_DEFAULT_COLOUR, int iPlane = 1 );
			Ogre::ManualObject* paintTriangle( Point oP1, Point oP2, Point oP3, Ogre::ColourValue oColour = SSVEP_DEFAULT_COLOUR, int iPlane = 1 );
			Ogre::ManualObject* paintCircle( Ogre::Real rX, Ogre::Real rY, Ogre::Real rR, Ogre::ColourValue = SSVEP_DEFAULT_COLOUR, bool bFilled = true, int iPlane = 1);
			
			void paintText( 
					const std::string& sID,
					const std::string& sText,
					Ogre::Real rX, Ogre::Real rY,
					Ogre::Real rWidth, Ogre::Real rHeight,
					const Ogre::ColourValue& oColour );

		protected:
			CApplication* m_poApplication;
			Ogre::OverlayManager* m_poOverlayManager;
			Ogre::OverlayContainer* m_poOverlayContainer;
			Ogre::SceneManager* m_poSceneManager;
			Ogre::AxisAlignedBox m_oAABInf;


	};

}


#endif // __OpenViBEApplication_CPainter_H__
