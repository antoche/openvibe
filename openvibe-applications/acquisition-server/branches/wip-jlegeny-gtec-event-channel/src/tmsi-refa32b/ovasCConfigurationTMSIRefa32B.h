#ifndef __OpenViBE_AcquisitionServer_CConfigurationTMSIRefa32B_H__
#define __OpenViBE_AcquisitionServer_CConfigurationTMSIRefa32B_H__

#include "../ovasCConfigurationBuilder.h"
#include <gtk/gtk.h>
#include <string.h>

namespace OpenViBEAcquisitionServer
{

	class CConfigurationTMSIRefa32B : public OpenViBEAcquisitionServer::CConfigurationBuilder
	{
	public:
		CConfigurationTMSIRefa32B(const char* sGtkBuilderFileName);

		virtual OpenViBE::boolean preConfigure(void);
		virtual OpenViBE::boolean postConfigure(void);
		OpenViBE::boolean setDeviceList(const std::vector <std::string> deviceList,std::string *deviceMaster,std::vector<std::string> *deviceSlaves);
		virtual void buttonRemoveSlaveDevice(void);
		virtual void buttonAddSlaveDevice(void);

	protected:
		std::string *m_sDeviceMaster;
		std::vector <std::string> m_vDeviceList;
		std::vector <std::string> *m_vDeviceSlaves;
		std::vector <std::string> m_vDeviceSlavesTemp;
	};
};

#endif // __OpenViBE_AcquisitionServer_CConfigurationTMSIRefa32B_H__
