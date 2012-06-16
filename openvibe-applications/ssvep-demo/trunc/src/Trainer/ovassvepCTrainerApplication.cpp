#include "ovassvepCTrainerApplication.h"

#include "../ovassvepCCommandStartStop.h"
#include "../ovassvepCCommandStimulatorControl.h"
#include "ovassvepCCommandReceiveTarget.h"

using namespace Ogre;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBESSVEP;

	CTrainerApplication::CTrainerApplication() : 
	CApplication(),
	m_bActive( false ),
	m_poInstructionsReady( NULL )
{
}

bool CTrainerApplication::setup(OpenViBE::Kernel::IKernelContext* poKernelContext)
{
	CApplication::setup(poKernelContext);

	(*m_poLogManager) << LogLevel_Debug << "  * CTrainerApplication::setup()\n";

	IConfigurationManager* l_poConfigurationManager = &(m_poKernelContext->getConfigurationManager());

	CTrainerFlickeringObject::initialize( this );

	// paint targets
	
	OpenViBE::uint32 l_ui32TargetCount = (OpenViBE::uint32)(l_poConfigurationManager->expandAsUInteger("${SSVEP_TargetCount}"));
	
	for (OpenViBE::uint32 i = 0; i < l_ui32TargetCount; i++)
	{
		this->addObject(CTrainerFlickeringObject::createTrainerFlickeringObject(i));
	}

	// draw the initial text
	m_poInstructionsReady = m_poGUIWindowManager->createWindow("TaharezLook/StaticImage", "InstructionsReady");
	m_poInstructionsReady->setPosition(CEGUI::UVector2(cegui_reldim(0.0f), cegui_reldim(0.0f)) );
	m_poInstructionsReady->setSize(CEGUI::UVector2(CEGUI::UDim(0.0f, 640.f), CEGUI::UDim(0.0f, 32.f)));

	m_poSheet->addChildWindow(m_poInstructionsReady);

	CEGUI::ImagesetManager::getSingleton().createFromImageFile("InstructionsReady","InstructionText-Start.png");

	m_poInstructionsReady->setProperty("Image","set:InstructionsReady image:full_image");
	m_poInstructionsReady->setProperty("FrameEnabled","False");
	m_poInstructionsReady->setProperty("BackgroundEnabled","False");
	m_poInstructionsReady->setVisible(true);

	
	// initialize commands
	(*m_poLogManager) << LogLevel_Debug << "+ addCommand(new CCommandStartStop(...)\n";
	this->addCommand(new CCommandStartStop( this ));

	(*m_poLogManager) << LogLevel_Debug << "+ addCommand(new CCommandStimulatorControl(...))\n";
	this->addCommand(new CCommandStimulatorControl( this ));

	(*m_poLogManager) << LogLevel_Debug << "+ addCommand(new CC(...))\n";
	this->addCommand(new CCommandReceiveTarget( this ));

	(*m_poLogManager) << LogLevel_Debug << "  * CTrainerApplication::setup() completed successfully\n";
	
	return true;
}


void CTrainerApplication::processFrame(OpenViBE::uint32 ui32CurrentFrame)
{
	CApplication::processFrame(ui32CurrentFrame);

	if (!m_bActive)
	{
		return;
	}

	for (OpenViBE::uint32 i = 0; i < m_oObjects.size(); i++)
	{
		m_oObjects[i]->processFrame();
	}
}

void CTrainerApplication::addObject(CTrainerFlickeringObject* poObject)
{
	m_oObjects.push_back(poObject);
	poObject->setVisible( true );
}

void CTrainerApplication::setTarget(OpenViBE::int32 i32Target)
{
	OpenViBE::uint32 l_ui32CurrentTime = (OpenViBE::int32)(time(NULL) - m_ttStartTime);
	(*m_poLogManager) << LogLevel_Info << l_ui32CurrentTime << "    > Target set to " << i32Target << "\n";

	for (OpenViBE::int32 i = 0; i < int(m_oObjects.size()); i++)
	{
		m_oObjects[i]->setTarget( i32Target == i );
	}
}

void CTrainerApplication::startExperiment()
{
	CApplication::startExperiment();

	m_ttStartTime = time(NULL);

	this->stopFlickering();
	m_poInstructionsReady->setVisible( false );
}

void CTrainerApplication::startFlickering()
{
	OpenViBE::uint32 l_ui32CurrentTime = (OpenViBE::uint32)(time(NULL) - m_ttStartTime);
	(*m_poLogManager) << LogLevel_Info << l_ui32CurrentTime << "    > Starting Visual Stimulation\n";
	m_bActive = true;
}

void CTrainerApplication::stopFlickering()
{
	OpenViBE::uint32 l_ui32CurrentTime = (OpenViBE::uint32)(time(NULL) - m_ttStartTime);
	(*m_poLogManager) << LogLevel_Info << l_ui32CurrentTime << "    > Stopping Visual Stimulation\n";
	m_bActive = false;

	for (OpenViBE::uint32 i = 0; i < m_oObjects.size(); i++)
	{
		m_oObjects[i]->setVisible(true);
	}

	this->setTarget( -1 );
}
