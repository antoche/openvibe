#include "ovaCAbstractVrpnPeripheral.h"

#include <vrpn_Tracker.h>
#include <vrpn_Button.h>
#include <vrpn_Analog.h>

using namespace OpenViBEVRDemos;

class CDeviceInfo
{
public:

	std::string m_sAddress;
	vrpn_Button_Remote* m_pButton;
	vrpn_Analog_Remote* m_pAnalog;
};

namespace
{
	void VRPN_CALLBACK handle_button(void* pUserData, const vrpn_BUTTONCB b)
	{
		CAbstractVrpnPeripheral* l_pAbstractVrpnPeripheral=(CAbstractVrpnPeripheral *)pUserData;

		std::pair < int, int > l_oVrpnButtonState;
		l_oVrpnButtonState.first=b.button;
		l_oVrpnButtonState.second=b.state;

		l_pAbstractVrpnPeripheral->m_vButton.push_back(l_oVrpnButtonState);
	}

	void VRPN_CALLBACK handle_analog(void* pUserData, const vrpn_ANALOGCB a)
	{
		CAbstractVrpnPeripheral* l_pAbstractVrpnPeripheral=(CAbstractVrpnPeripheral *)pUserData;

		std::list < double >  l_oVrpnAnalogState;

		for(int i=0; i<a.num_channel; i++)
		{
			l_oVrpnAnalogState.push_back(a.channel[i]*l_pAbstractVrpnPeripheral->m_dAnalogScale+l_pAbstractVrpnPeripheral->m_dAnalogOffset);
		}

		l_pAbstractVrpnPeripheral->m_vAnalog.push_back(l_oVrpnAnalogState);
	}
}

CAbstractVrpnPeripheral::CAbstractVrpnPeripheral(void)
{
	m_dAnalogScale=1;
	m_dAnalogOffset=0;
	m_sDeviceAddress = "openvibe-vrpn@localhost";
}

CAbstractVrpnPeripheral::CAbstractVrpnPeripheral(const std::string serverName)
{
	m_dAnalogScale=1;
	m_dAnalogOffset=0;
	m_sDeviceAddress = serverName;
}

CAbstractVrpnPeripheral::~CAbstractVrpnPeripheral(void)
{
}

void CAbstractVrpnPeripheral::init(void)
{
	m_pDevice=new CDeviceInfo;
	m_pDevice->m_sAddress=m_sDeviceAddress;
	m_pDevice->m_pAnalog=new vrpn_Analog_Remote(m_sDeviceAddress.c_str());
	m_pDevice->m_pButton=new vrpn_Button_Remote(m_sDeviceAddress.c_str());

	m_pDevice->m_pButton->register_change_handler(this, &handle_button);
	m_pDevice->m_pAnalog->register_change_handler(this, &handle_analog);
}

void CAbstractVrpnPeripheral::loop(void)
{
	m_pDevice->m_pButton->mainloop();
	m_pDevice->m_pAnalog->mainloop();
}
