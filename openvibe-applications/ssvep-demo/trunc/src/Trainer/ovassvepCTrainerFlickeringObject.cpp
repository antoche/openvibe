#include "ovassvepCTrainerFlickeringObject.h"
#include "ovassvepCTrainerApplication.h"

using namespace Ogre;
using namespace OpenViBESSVEP;

SceneNode* CTrainerFlickeringObject::m_poParentNode = NULL;
CBasicPainter* CTrainerFlickeringObject::m_poPainter = NULL;
ColourValue CTrainerFlickeringObject::m_oLightColour = ColourValue(1.0f, 1.0f, 1.0f);
ColourValue CTrainerFlickeringObject::m_oDarkColour = ColourValue(0.0f, 0.0f, 0.0f);
OpenViBE::float32 CTrainerFlickeringObject::m_f32TargetWidth = 0.2f;
OpenViBE::float32 CTrainerFlickeringObject::m_f32TargetHeight = 0.2f;
CTrainerApplication* CTrainerFlickeringObject::m_poApplication = NULL;

void CTrainerFlickeringObject::initialize( CTrainerApplication* poApplication )
{
	m_poApplication = poApplication;
	OpenViBE::Kernel::IConfigurationManager* l_poConfigurationManager = poApplication->getConfigurationManager();

	m_poPainter = poApplication->getPainter();
	m_poParentNode = poApplication->getSceneNode();

	m_f32TargetWidth = (OpenViBE::float32)(l_poConfigurationManager->expandAsFloat("${SSVEP_TargetWidth}"));
	m_f32TargetHeight = (OpenViBE::float32)(l_poConfigurationManager->expandAsFloat("${SSVEP_TargetHeight}"));

	m_oLightColour = ColourValue(
			(float)(l_poConfigurationManager->expandAsFloat("${SSVEP_TargetLightColourRed}")),
			(float)(l_poConfigurationManager->expandAsFloat("${SSVEP_TargetLightColourGreen}")),
			(float)(l_poConfigurationManager->expandAsFloat("${SSVEP_TargetLightColourBlue}")));

	m_oDarkColour = ColourValue(
			(float)(l_poConfigurationManager->expandAsFloat("${SSVEP_TargetDarkColourRed}")),
			(float)(l_poConfigurationManager->expandAsFloat("${SSVEP_TargetDarkColourGreen}")),
			(float)(l_poConfigurationManager->expandAsFloat("${SSVEP_TargetDarkColourBlue}")));

}

CTrainerFlickeringObject* CTrainerFlickeringObject::createTrainerFlickeringObject( OpenViBE::uint32 l_i32TargetId )
{
	OpenViBE::Kernel::IConfigurationManager* l_poConfigurationManager = m_poApplication->getConfigurationManager();

	if (m_poPainter != NULL)
	{
		ColourValue l_oCurrentTargetColour = ( l_i32TargetId == 0) ? m_oDarkColour : m_oLightColour;

		char l_sTargetIdString[255];
		sprintf(l_sTargetIdString, "%d", l_i32TargetId);

		OpenViBE::CIdentifier l_oTargetId = l_poConfigurationManager->createConfigurationToken("SSVEPTarget_Id", OpenViBE::CString(l_sTargetIdString));

		OpenViBE::float32 l_f32TargetX = (OpenViBE::float32)(l_poConfigurationManager->expandAsFloat("${SSVEP_Target_X_${SSVEPTarget_Id}}"));
		OpenViBE::float32 l_f32TargetY = (OpenViBE::float32)(l_poConfigurationManager->expandAsFloat("${SSVEP_Target_Y_${SSVEPTarget_Id}}"));
		OpenViBE::uint32 l_ui32FramesL = (*(m_poApplication->getFrequencies()))[ l_i32TargetId ].first;
		OpenViBE::uint32 l_ui32FramesD = (*(m_poApplication->getFrequencies()))[ l_i32TargetId ].second;

		m_poApplication->getLogManager() << OpenViBE::Kernel::LogLevel_Info << "New trainer object : id=" << l_i32TargetId << " litFrames=" << l_ui32FramesL << " darkFrames=" << l_ui32FramesD << "\n";

		l_poConfigurationManager->releaseConfigurationToken(l_oTargetId);

		return new CTrainerFlickeringObject( l_f32TargetX, l_f32TargetY, l_oCurrentTargetColour, l_ui32FramesL, l_ui32FramesD );
	}
	else
	{
		m_poApplication->getLogManager() << OpenViBE::Kernel::LogLevel_Fatal << "TrainerTarget object was not properly initialized\n";
		return NULL;
	}
}

CTrainerFlickeringObject::CTrainerFlickeringObject( OpenViBE::float32 f32PosX, OpenViBE::float32 f32PosY, Ogre::ColourValue oColour, OpenViBE::uint8 ui8LitFrames, OpenViBE::uint8 ui8DarkFrames ) : 
	CSSVEPFlickeringObject( NULL, ui8LitFrames, ui8DarkFrames )
{
	Ogre::SceneNode* l_poPointerNode;

	Ogre::MovableObject* l_poLitObject;
	Ogre::MovableObject* l_poDarkObject;

	m_poElementNode = m_poParentNode->createChildSceneNode();
	m_poObjectNode = m_poElementNode->createChildSceneNode();
	l_poPointerNode = m_poElementNode->createChildSceneNode();

	Ogre::Rectangle l_oRectangle = { f32PosX - m_f32TargetWidth / 2, f32PosY + m_f32TargetHeight / 2, f32PosX + m_f32TargetWidth / 2, f32PosY - m_f32TargetHeight / 2};

	l_poLitObject = m_poPainter->paintRectangle( l_oRectangle, oColour );

	m_poObjectNode->attachObject( l_poLitObject );
	l_poLitObject->setVisible( true );

	l_poDarkObject = m_poPainter->paintRectangle( l_oRectangle, m_oDarkColour );
	m_poObjectNode->attachObject( l_poDarkObject );
	l_poDarkObject->setVisible( false );

	m_poPointer = m_poPainter->paintTriangle( 
			Point( f32PosX - 0.05f, f32PosY + m_f32TargetHeight ),
			Point( f32PosX, f32PosY + m_f32TargetHeight - 0.05f ), 
			Point( f32PosX + 0.05f, f32PosY + m_f32TargetHeight ),
			ColourValue(1, 1, 0));

	l_poPointerNode->attachObject( m_poPointer );
	m_poPointer->setVisible( false );


}

void CTrainerFlickeringObject::setTarget( bool bIsTarget )
{
	m_poPointer->setVisible( bIsTarget );
}


