#ifndef __OpenViBEApplication_CVRPNServer_H__
#define __OpenViBEApplication_CVRPNServer_H__

#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include <map>
#include <vector>
#include <string>


class vrpn_Connection;
class vrpn_Button_Server;

namespace OpenViBESSVEP
{
	class CApplication;

	class CVRPNServer
	{
		public:
			static CVRPNServer* getInstance(CApplication* poApplication);
			~CVRPNServer();
			void processFrame();

			void addButton(OpenViBE::CString sName, int iButtonCount);
			void changeButtonState(std::string sName, int iIndex, int iState);
			int getButtonState(std::string sName, int iIndex);

		private:
			static CVRPNServer* m_poVRPNServerInstance;
			static CApplication* m_poApplication;

			CVRPNServer(CApplication* poApplication);

			vrpn_Connection* m_poConnection;
			std::map<std::string, vrpn_Button_Server*> m_oButtonServer;
			std::map<std::string, std::vector<int> > m_oButtonCache;

	};
}

#endif // __OpenViBEApplication_CVRPNServer_H__
