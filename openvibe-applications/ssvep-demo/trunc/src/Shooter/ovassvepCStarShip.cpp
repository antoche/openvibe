#include "ovassvepCStarShip.h"

#include "../ovassvepCApplication.h"
#include "../ovassvepCSSVEPFlickeringObject.h"

#define SSVEP_SHIP_HULL_COLOUR Ogre::ColourValue(0.0f, 0.5f, 0.5f)
#define SIGN(x) ( x / abs(x) )

using namespace OpenViBESSVEP;
using namespace Ogre;

CStarShip::CStarShip(CApplication* poApplication, Ogre::SceneNode* poParentNode, Ogre::Real rRadius, std::vector<std::pair<OpenViBE::uint32, OpenViBE::uint32> >* pFrequencies) :
	m_rCurrentAngle( 0 ),
	m_iCurrentRotationCount( 0 ),
	m_bIsShooting( false )
{
	// get configuration and controlling objects
	OpenViBE::Kernel::IConfigurationManager* l_poConfigurationManager = poApplication->getConfigurationManager();
	CBasicPainter* l_poPainter = poApplication->getPainter();
	m_rAngularSpeed = l_poConfigurationManager->expandAsFloat("${SSVEP_ShipAngularSpeed}", Math::PI / 60.0f);
	poApplication->getLogManager() << OpenViBE::Kernel::LogLevel_Info << "Ship Angluar Speed : " << m_rAngularSpeed << "\n";

	ColourValue l_oLightColour = ColourValue(
				(float)(l_poConfigurationManager->expandAsFloat("${SSVEP_TargetLightColourRed}", 1.0f)),
				(float)(l_poConfigurationManager->expandAsFloat("${SSVEP_TargetLightColourGreen}", 1.0f)),
				(float)(l_poConfigurationManager->expandAsFloat("${SSVEP_TargetLightColourBlue}", 1.0f)));

	ColourValue l_oDarkColour = ColourValue(
				(float)(l_poConfigurationManager->expandAsFloat("${SSVEP_TargetDarkColourRed}", 0.0f)),
				(float)(l_poConfigurationManager->expandAsFloat("${SSVEP_TargetDarkColourGreen}", 0.0f)),
				(float)(l_poConfigurationManager->expandAsFloat("${SSVEP_TargetDarkColourBlue}", 0.0f)));


	// draw the ship scene objects
	m_poShipNode = poParentNode->createChildSceneNode();
	m_poShotNode = m_poShipNode->createChildSceneNode();

	MovableObject* l_poShipHull;
	l_poShipHull = l_poPainter->paintCircle( 0.0f, 0.0f, rRadius, SSVEP_SHIP_HULL_COLOUR, false, 2);
	m_poShipNode->attachObject( l_poShipHull );
	l_poShipHull->setVisible( true );


	SceneNode* l_poDrawnObjectNode;
	MovableObject* l_poDrawnObject;
	Ogre::Rectangle l_oWingRectangle = { - rRadius * 0.4f, rRadius * 0.4f, rRadius * 0.4f, -rRadius * 0.4f };

	// paint the cannon

	l_poDrawnObjectNode = m_poShipNode->createChildSceneNode();

	l_poDrawnObject = l_poPainter->paintTriangle(
				Point( 0.0f, rRadius * 0.4f),
				Point( - rRadius * 0.4f, -rRadius * 0.4f),
				Point( rRadius * 0.4f, -rRadius * 0.4f),
				l_oLightColour
				);
	l_poDrawnObject->setVisible( true );
	l_poDrawnObjectNode->attachObject( l_poDrawnObject );

	l_poDrawnObject = l_poPainter->paintTriangle(
				Point( 0.0f, rRadius * 0.4f),
				Point( - rRadius * 0.4f, -rRadius * 0.4f),
				Point( rRadius * 0.4f, -rRadius * 0.4f),
				l_oDarkColour
				);
	l_poDrawnObject->setVisible( false );
	l_poDrawnObjectNode->attachObject( l_poDrawnObject );

	l_poDrawnObjectNode->setPosition( 0.0, rRadius, 0.0);

	m_poShipCannon = new CSSVEPFlickeringObject( l_poDrawnObjectNode, (*pFrequencies)[1].first, (*pFrequencies)[1].second);

	// paint the left wing

	l_poDrawnObjectNode = m_poShipNode->createChildSceneNode();

	l_poDrawnObject = l_poPainter->paintRectangle( l_oWingRectangle, l_oLightColour );
	l_poDrawnObject->setVisible( true );
	l_poDrawnObjectNode->attachObject( l_poDrawnObject );

	l_poDrawnObject = l_poPainter->paintRectangle( l_oWingRectangle, l_oDarkColour );
	l_poDrawnObject->setVisible( false );
	l_poDrawnObjectNode->attachObject( l_poDrawnObject );

	l_poDrawnObjectNode->setPosition( -rRadius * 0.875f, -rRadius * 0.875f, 0.0 );

	m_poShipLeftWing = new CSSVEPFlickeringObject( l_poDrawnObjectNode, (*pFrequencies)[2].first, (*pFrequencies)[2].second);

	// paint the right wing

	l_poDrawnObjectNode = m_poShipNode->createChildSceneNode();

	l_poDrawnObject = l_poPainter->paintRectangle( l_oWingRectangle, l_oLightColour );
	l_poDrawnObject->setVisible( true );
	l_poDrawnObjectNode->attachObject( l_poDrawnObject );

	l_poDrawnObject = l_poPainter->paintRectangle( l_oWingRectangle, l_oDarkColour );
	l_poDrawnObject->setVisible( false );
	l_poDrawnObjectNode->attachObject( l_poDrawnObject );

	l_poDrawnObjectNode->setPosition( rRadius * 0.875f, -rRadius * 0.875f, 0.0f );

	m_poShipRightWing = new CSSVEPFlickeringObject( l_poDrawnObjectNode, (*pFrequencies)[3].first, (*pFrequencies)[3].second);

	// create the shot

	l_poDrawnObject = l_poPainter->paintTriangle(
				Point( 0.0f, rRadius * 0.25f ),
				Point( -rRadius * 0.125f, 0.0f ),
				Point( rRadius * 0.125f, 0.0f ),
				ColourValue( 1.0f, 1.0f, 0.0f));
	l_poDrawnObject->setVisible( false );

	m_poShotNode->attachObject( l_poDrawnObject );

	l_poDrawnObject = l_poPainter->paintTriangle(
				Point( -rRadius * 0.125f, 0.0f ),
				Point( 0.0f, -rRadius * 0.75f ),
				Point( rRadius * 0.125f, 0.0f ),
				ColourValue( 1.0f, 1.0f, 0.0f));
	l_poDrawnObject->setVisible( false );

	m_poShotNode->attachObject( l_poDrawnObject );


}

void CStarShip::processFrame( OpenViBE::uint32 ui32CurrentFrame )
{
	m_poShipCannon->processFrame();
	m_poShipLeftWing->processFrame();
	m_poShipRightWing->processFrame();

	if (m_iCurrentRotationCount != 0)
	{
		m_poShipNode->roll( Ogre::Radian( m_rAngularSpeed * SIGN( m_iCurrentRotationCount )));

		m_rCurrentAngle += Ogre::Radian( m_rAngularSpeed * SIGN( m_iCurrentRotationCount ));

		m_iCurrentRotationCount -= SIGN( m_iCurrentRotationCount );
	}

	if (m_bIsShooting)
	{
		m_rShotDistance += 0.07f;
		m_poShotNode->setPosition( 0.0, m_rShotDistance, 0.0 );

		if (m_rShotDistance > 1.5f)
		{
			m_bIsShooting = false;
			m_poShotNode->setVisible( false );
		}
	}

}

void CStarShip::rotate( int iRotationCount )
{
	m_iCurrentRotationCount += iRotationCount;
}

void CStarShip::shoot()
{
	if (m_bIsShooting)
	{
		return;
	}

	m_bIsShooting = true;
	m_rShotDistance = 0.0f;

	m_poShotNode->setPosition( 0.0f, 0.0f, 0.0f );
	m_poShotNode->setVisible( true );
}

std::pair<Ogre::Real, Ogre::Real> CStarShip::getShotCoordinates()
{
	return std::make_pair( -Ogre::Math::Sin(m_rCurrentAngle) * m_rShotDistance, Ogre::Math::Cos(m_rCurrentAngle) * m_rShotDistance);
}
