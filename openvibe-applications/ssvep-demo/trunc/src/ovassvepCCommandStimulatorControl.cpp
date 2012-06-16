#include "ovassvepCCommandStimulatorControl.h"
#include "ovassvepCApplication.h"

using namespace OpenViBESSVEP;

CCommandStimulatorControl::CCommandStimulatorControl(CApplication* poApplication)
	: ICommandVRPNButton(poApplication, "SSVEP_VRPN_StimulatorControl")
{
}

void CCommandStimulatorControl::execute(int iButton, int iState)
{
	// only run the commands once, skip

	switch (iButton)
	{

		case 0:
			m_poApplication->startExperiment();
			break;
			
		case 1:
			m_poApplication->stopExperiment();
			break;
			
		case 2:
			m_poApplication->startFlickering();
			break;
			
		case 3:
			m_poApplication->stopFlickering();
			break;
			
		default:
			m_poApplication->getLogManager() << OpenViBE::Kernel::LogLevel_Error << "[ERROR] Unknown command\n";
			break;

	}

}
