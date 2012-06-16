#include "ovassvepCCommandShipControl.h"
#include "ovassvepCShooterApplication.h"

using namespace OpenViBESSVEP;

CCommandShipControl::CCommandShipControl(CShooterApplication* poApplication)
	: ICommandVRPNButton(poApplication, "SSVEP_VRPN_ShipControl")
{
}

void CCommandShipControl::execute(int iButton, int iState)
{
	CShooterApplication* l_poShooterApplication = dynamic_cast<CShooterApplication*>(m_poApplication);

	switch (iButton)
	{
		case 0:
			l_poShooterApplication->getShip()->shoot();
			break;
		case 1:
			l_poShooterApplication->getShip()->rotate( -6 );
			break;
		case 2:
			l_poShooterApplication->getShip()->rotate( 6 );
			break;
	}
}
