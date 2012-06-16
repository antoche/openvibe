#include "ovassvepCCommandReceiveTarget.h"
#include "ovassvepCTrainerApplication.h"

using namespace OpenViBESSVEP;

CCommandReceiveTarget::CCommandReceiveTarget(CApplication* poApplication)
	: ICommandVRPNButton(poApplication, "SSVEP_VRPN_Target")
{
}

void CCommandReceiveTarget::execute(int iButton, int iState)
{
	dynamic_cast<CTrainerApplication*>(m_poApplication)->setTarget(iButton);
}
