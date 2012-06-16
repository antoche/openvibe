#include "ovassvepICommandVRPNButton.h"
#include "ovassvepCApplication.h"

#include <vrpn_Connection.h>
#include <vrpn_Button.h>

using namespace OpenViBESSVEP;
using namespace OpenViBE::Kernel;


namespace
{
	void VRPN_CALLBACK ssvep_vrpn_callback_button(void *command, vrpn_BUTTONCB button)
	{
		((OpenViBESSVEP::ICommandVRPNButton*)command)->execute(button.button, button.state);
	}
}


ICommandVRPNButton::ICommandVRPNButton(CApplication* poApplication, OpenViBE::CString sName)
	: ICommand( poApplication )
{
	char l_sButtonName[1024];

	OpenViBE::Kernel::IConfigurationManager* l_poConfigurationManager = poApplication->getConfigurationManager();

	sprintf(l_sButtonName, "%s@%s", sName.toASCIIString(), (l_poConfigurationManager->expand("${SSVEP_VRPNHost}")).toASCIIString());

	m_poApplication->getLogManager() << LogLevel_Debug << "+ m_poVRPNButton = new vrpn_Button_Remote(" << OpenViBE::CString(l_sButtonName) << ")\n";

	
	m_poVRPNButton = new vrpn_Button_Remote( l_sButtonName );
	m_poVRPNButton->register_change_handler( (void*)this, ssvep_vrpn_callback_button);
}

ICommandVRPNButton::~ICommandVRPNButton()
{
	m_poApplication->getLogManager() << LogLevel_Debug << "- delete m_poVRPNButton\n";
	delete m_poVRPNButton;
}

void ICommandVRPNButton::processFrame()
{
	m_poVRPNButton->mainloop();
}
