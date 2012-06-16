#include "ovassvepCVRPNServer.h"
#include "ovassvepCApplication.h"

#include <vrpn_Connection.h>
#include <vrpn_Button.h>

using namespace OpenViBESSVEP;
using namespace OpenViBE::Kernel;

CVRPNServer* CVRPNServer::m_poVRPNServerInstance = NULL;
CApplication* CVRPNServer::m_poApplication = NULL;

CVRPNServer::CVRPNServer(CApplication* poApplication)
{
	m_poApplication = poApplication;

	int l_iPort = (OpenViBE::int32)(m_poApplication->getConfigurationManager()->expandAsInteger("${VRPN_ExternalServerPort}"));

	m_poApplication->getLogManager() << LogLevel_Debug << "VRPN SERVER PORT :" << l_iPort << "\n";
	m_poConnection = vrpn_create_server_connection(l_iPort);
}

CVRPNServer* CVRPNServer::getInstance(CApplication* poApplication)
{
	if (m_poVRPNServerInstance == NULL)
	{
		m_poVRPNServerInstance = new CVRPNServer(poApplication);
	}

	return m_poVRPNServerInstance;
}

void CVRPNServer::addButton(OpenViBE::CString sName, int iButtonCount)
{
	m_oButtonServer.insert(std::pair<std::string, vrpn_Button_Server*>(sName.toASCIIString(), new vrpn_Button_Server(sName.toASCIIString(), m_poConnection, iButtonCount)));
	m_oButtonCache[sName.toASCIIString()].clear();
	m_oButtonCache[sName.toASCIIString()].resize(iButtonCount);
}

void CVRPNServer::processFrame()
{
	for (std::map<std::string, vrpn_Button_Server*>::iterator it = m_oButtonServer.begin(); it != m_oButtonServer.end(); ++it)
	{
		it->second->mainloop();
	}

	m_poConnection->mainloop();
}

void CVRPNServer::changeButtonState(std::string sName, int iIndex, int iState)
{
	m_oButtonServer[sName]->set_button(iIndex, iState);
	m_oButtonCache[sName][iIndex] = iState;
}

int CVRPNServer::getButtonState(std::string sName, int iIndex)
{
	return m_oButtonCache[sName][iIndex];
}
