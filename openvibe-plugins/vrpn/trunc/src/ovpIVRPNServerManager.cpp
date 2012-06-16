#if defined TARGET_HAS_ThirdPartyVRPN

#include "ovpIVRPNServerManager.h"

#include <vrpn_Connection.h>
#include <vrpn_Analog.h>
#include <vrpn_Button.h>

#include <vector>
#include <string>
#include <map>
#include <iostream>

using namespace OpenViBE;
using namespace OpenViBE::Plugins;
using namespace OpenViBE::Kernel;
using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::VRPN;
using namespace std;

#define boolean OpenViBE::boolean

namespace OpenViBEPlugins
{
	namespace VRPN
	{
		namespace
		{
			class CVRPNServerManager : public IVRPNServerManager
			{
			public:

				CVRPNServerManager(void);
				virtual ~CVRPNServerManager(void);

				virtual boolean initialize(void);
				virtual boolean uninitialize(void);

				virtual boolean process(void);
				virtual boolean reportAnalog(
					const CIdentifier& rServerIdentifier);
				virtual boolean reportButton(
					const CIdentifier& rServerIdentifier);

				virtual boolean addServer(
					const CString& sServerName,
					CIdentifier& rServerIdentifier);
				virtual boolean isServer(
					const CIdentifier& rServerIdentifier) const;
				virtual boolean isServer(
					const CString& sServerName) const;
				virtual boolean getServerIdentifier(
					const CString& sServerName,
					CIdentifier& rServerIdentifier) const;
				virtual boolean getServerName(
					const CIdentifier& rServerIdentifier,
					CString& sServerName) const;
				virtual boolean removeServer(
					const CIdentifier& rServerIdentifier);

				virtual boolean setButtonCount(
					const CIdentifier& rServerIdentifier,
					const uint32 ui32ButtonCount);
				virtual boolean setButtonState(
					const CIdentifier& rServerIdentifier,
					const uint32 ui32ButtonIndex,
					const boolean bButtonStatus);
				virtual boolean getButtonState(
					const CIdentifier& rServerIdentifier,
					const uint32 ui32ButtonIndex) const;

				virtual boolean setAnalogCount(
					const CIdentifier& rServerIdentifier,
					const uint32 ui32AnalogCount);
				virtual boolean setAnalogState(
					const CIdentifier& rServerIdentifier,
					const uint32 ui32AnalogIndex,
					const float64 f64AnalogStatus);
				virtual float64 getAnalogState(
					const CIdentifier& rServerIdentifier,
					const uint32 ui32AnalogIndex) const;

			protected:

				vrpn_Connection* m_pConnection;

				map<CIdentifier, CString> m_vServerName;
				map<CIdentifier, vrpn_Button_Server*> m_vButtonServer;
				map<CIdentifier, vrpn_Analog_Server*> m_vAnalogServer;
				map<CIdentifier, vector<boolean> > m_vButtonCache;

				uint32 m_ui32InitializeCount;
			};
		};

		IVRPNServerManager& IVRPNServerManager::getInstance()
		{
			static CVRPNServerManager g_oVRPNServerManager;
			return g_oVRPNServerManager;
		}
	};
};

CVRPNServerManager::CVRPNServerManager(void)
	:m_pConnection(NULL)
	,m_ui32InitializeCount(0)
{
}

CVRPNServerManager::~CVRPNServerManager(void)
{
}

boolean CVRPNServerManager::initialize(void)
{
	if(!m_ui32InitializeCount)
	{
		//m_pConnection=new vrpn_Connection;
		m_pConnection=vrpn_create_server_connection();
	}
	m_ui32InitializeCount++;
	return true;
}

boolean CVRPNServerManager::uninitialize(void)
{
	m_ui32InitializeCount--;
	if(!m_ui32InitializeCount)
	{
		map<CIdentifier, vrpn_Analog_Server*>::iterator itAnalogServer;
		for(itAnalogServer=m_vAnalogServer.begin(); itAnalogServer!=m_vAnalogServer.end(); itAnalogServer++)
		{
			delete itAnalogServer->second;
		}
		m_vAnalogServer.clear();
		map<CIdentifier, vrpn_Button_Server*>::iterator itButtonServer;
		for(itButtonServer=m_vButtonServer.begin(); itButtonServer!=m_vButtonServer.end(); itButtonServer++)
		{
			delete itButtonServer->second;
		}
		m_vButtonServer.clear();

		// $$$ UGLY !
		// The following function should destroy correctly the connection, but does not.
		//vrpn_ConnectionManager::instance().deleteConnection(m_pConnection);
		delete m_pConnection;
		m_pConnection=NULL;
	}
	return true;
}

boolean CVRPNServerManager::process(void)
{
	map<CIdentifier, vrpn_Analog_Server*>::iterator itAnalogServer;
	for(itAnalogServer=m_vAnalogServer.begin(); itAnalogServer!=m_vAnalogServer.end(); itAnalogServer++)
	{
		if(itAnalogServer->second)
		{
			itAnalogServer->second->mainloop();
		}
	}

	map<CIdentifier, vrpn_Button_Server*>::iterator itButtonServer;
	for(itButtonServer=m_vButtonServer.begin(); itButtonServer!=m_vButtonServer.end(); itButtonServer++)
	{
		if(itButtonServer->second)
		{
			itButtonServer->second->mainloop();
		}
	}

	if(m_pConnection)
	{
		m_pConnection->mainloop();
	}
	return true;
}

boolean CVRPNServerManager::reportAnalog(
	const CIdentifier& rServerIdentifier)
{
	map<CIdentifier, vrpn_Analog_Server*>::iterator itAnalogServer=m_vAnalogServer.find(rServerIdentifier);
	if(itAnalogServer!=m_vAnalogServer.end())
	{
		if(itAnalogServer->second)
		{
			// This is public function and mainloop won't call it for me
			// Thank you VRPN for this to be similar to button behavior ;o)
			itAnalogServer->second->report();
		}
	}
	return true;
}

boolean CVRPNServerManager::reportButton(
	const CIdentifier& rServerIdentifier)
{
	map<CIdentifier, vrpn_Button_Server*>::iterator itButtonServer=m_vButtonServer.find(rServerIdentifier);
	if(itButtonServer!=m_vButtonServer.end())
	{
		if(itButtonServer->second)
		{
			// This is not public function, however, mainloop calls it for me
			// Thank you VRPN for this to be similar to analog behavior ;o)
			// itButtonServer->second->report_changes();
		}
	}
	return true;
}

boolean CVRPNServerManager::addServer(
	const CString& sServerName,
	CIdentifier& rServerIdentifier)
{
	if(this->isServer(sServerName))
	{
		return this->getServerIdentifier(sServerName, rServerIdentifier);
	}

	rServerIdentifier=CIdentifier::random();
	while(m_vServerName.find(rServerIdentifier)!=m_vServerName.end())
	{
		++rServerIdentifier;
	}

	m_vServerName[rServerIdentifier]=sServerName;
	return true;
}

boolean CVRPNServerManager::isServer(
	const CIdentifier& rServerIdentifier) const
{
	return m_vServerName.find(rServerIdentifier)!=m_vServerName.end();
}

boolean CVRPNServerManager::isServer(
	const CString& sServerName) const
{
	map<CIdentifier, CString>::const_iterator itServerName;
	for(itServerName=m_vServerName.begin(); itServerName!=m_vServerName.end(); itServerName++)
	{
		if(itServerName->second==sServerName)
		{
			return true;
		}
	}
	return false;
}

boolean CVRPNServerManager::getServerIdentifier(
	const CString& sServerName,
	CIdentifier& rServerIdentifier) const
{
	map<CIdentifier, CString>::const_iterator itServerName;
	for(itServerName=m_vServerName.begin(); itServerName!=m_vServerName.end(); itServerName++)
	{
		if(itServerName->second==sServerName)
		{
			rServerIdentifier=itServerName->first;
			return true;
		}
	}
	return false;
}

boolean CVRPNServerManager::getServerName(
	const CIdentifier& rServerIdentifier,
	CString& sServerName) const
{
	map<CIdentifier, CString>::const_iterator itServerName=m_vServerName.find(rServerIdentifier);
	if(itServerName==m_vServerName.end())
	{
		return false;
	}
	sServerName=itServerName->second;
	return true;
}

boolean CVRPNServerManager::removeServer(
	const CIdentifier& rServerIdentifier)
{
	if(!this->isServer(rServerIdentifier))
	{
		return false;
	}

	// TODO
	return true;
}

boolean CVRPNServerManager::setButtonCount(
	const CIdentifier& rServerIdentifier,
	const uint32 ui32ButtonCount)
{
	if(!this->isServer(rServerIdentifier))
	{
		return false;
	}
	delete m_vButtonServer[rServerIdentifier];
	m_vButtonServer[rServerIdentifier]=new vrpn_Button_Server(m_vServerName[rServerIdentifier], m_pConnection, ui32ButtonCount);
	m_vButtonCache[rServerIdentifier].clear();
	m_vButtonCache[rServerIdentifier].resize(ui32ButtonCount);
	return true;
}

boolean CVRPNServerManager::setButtonState(
	const CIdentifier& rServerIdentifier,
	const uint32 ui32ButtonIndex,
	const boolean bButtonStatus)
{
	if(!this->isServer(rServerIdentifier))
	{
		return false;
	}
	if(m_vButtonServer.find(rServerIdentifier)==m_vButtonServer.end())
	{
		return false;
	}
	m_vButtonServer[rServerIdentifier]->set_button(ui32ButtonIndex, bButtonStatus?1:0);
	m_vButtonCache[rServerIdentifier][ui32ButtonIndex]=bButtonStatus;
	return true;
}

boolean CVRPNServerManager::getButtonState(
	const CIdentifier& rServerIdentifier,
	const uint32 ui32ButtonIndex) const
{
	if(!this->isServer(rServerIdentifier))
	{
		return false;
	}
	map<CIdentifier, vrpn_Button_Server*>::const_iterator itButtonServer=m_vButtonServer.find(rServerIdentifier);
	if(itButtonServer==m_vButtonServer.end())
	{
		return false;
	}
	map<CIdentifier, vector<boolean> >::const_iterator itButtonCache=m_vButtonCache.find(rServerIdentifier);
	return itButtonCache->second[ui32ButtonIndex];
}

boolean CVRPNServerManager::setAnalogCount(
	const CIdentifier& rServerIdentifier,
	const uint32 ui32AnalogCount)
{
	if(!this->isServer(rServerIdentifier))
	{
		return false;
	}
	delete m_vAnalogServer[rServerIdentifier];
	m_vAnalogServer[rServerIdentifier]=new vrpn_Analog_Server(m_vServerName[rServerIdentifier], m_pConnection);
	m_vAnalogServer[rServerIdentifier]->setNumChannels(ui32AnalogCount);
	return true;
}

boolean CVRPNServerManager::setAnalogState(
	const CIdentifier& rServerIdentifier,
	const uint32 ui32AnalogIndex,
	const float64 f64AnalogStatus)
{
	if(!this->isServer(rServerIdentifier))
	{
		return false;
	}
	if(m_vAnalogServer.find(rServerIdentifier)==m_vAnalogServer.end())
	{
		return false;
	}
	m_vAnalogServer[rServerIdentifier]->channels()[ui32AnalogIndex]=f64AnalogStatus;
	return true;
}

float64 CVRPNServerManager::getAnalogState(
	const CIdentifier& rServerIdentifier,
	const uint32 ui32AnalogIndex) const
{
	if(!this->isServer(rServerIdentifier))
	{
		return 0;
	}
	map<CIdentifier, vrpn_Analog_Server*>::const_iterator itAnalogServer=m_vAnalogServer.find(rServerIdentifier);
	if(itAnalogServer==m_vAnalogServer.end())
	{
		return 0;
	}
	return itAnalogServer->second->channels()[ui32AnalogIndex];
}

#endif // OVP_HAS_Vrpn
