#include "ovassvepCCommandTargetControl.h"
#include "ovassvepCShooterApplication.h"

using namespace OpenViBESSVEP;
using namespace OpenViBE::Kernel;

CCommandTargetControl::CCommandTargetControl(CShooterApplication* poApplication)
	: ICommandVRPNButton(poApplication, "SSVEP_VRPN_ShooterTarget")
{
	m_poVRPNServer = CVRPNServer::getInstance( poApplication );
	m_poVRPNServer->addButton("SSVEP_VRPN_TargetRequest", 1);
}

void CCommandTargetControl::processFrame()
{
	CShooterApplication* l_poShooterApplication = dynamic_cast<CShooterApplication*>(m_poApplication);

	ICommandVRPNButton::processFrame();

	if (l_poShooterApplication->m_bTargetRequest)
	{
		m_poApplication->getLogManager() << LogLevel_Info << "Requesting target\n";
		m_poVRPNServer->changeButtonState("SSVEP_VRPN_TargetRequest", 0, 1);
		l_poShooterApplication->m_bTargetRequest = false;
	}
	else
	{
		m_poVRPNServer->changeButtonState("SSVEP_VRPN_TargetRequest", 0, 0);
	}

	m_poVRPNServer->processFrame();
}

void CCommandTargetControl::execute(int iButton, int iState)
{
	if (iState == 1)
	{
		m_poApplication->getLogManager() << LogLevel_Info << "Target created at position " << iButton << "\n";
		dynamic_cast<CShooterApplication*>(m_poApplication)->addTarget(iButton);
	}
}
