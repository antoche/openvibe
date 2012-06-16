#ifndef __OpenViBE_AcquisitionServer_CConfigurationNetworkBuilder_H__
#define __OpenViBE_AcquisitionServer_CConfigurationNetworkBuilder_H__

#include "ovasCConfigurationBuilder.h"

namespace OpenViBEAcquisitionServer
{
	class CConfigurationNetworkBuilder : public OpenViBEAcquisitionServer::CConfigurationBuilder
	{
	public:

		CConfigurationNetworkBuilder(const char* sGtkBuilderFileName);
		virtual ~CConfigurationNetworkBuilder(void);

		virtual OpenViBE::boolean setHostName(const OpenViBE::CString& sHostName);
		virtual OpenViBE::boolean setHostPort(const OpenViBE::uint32 ui32HostPort);

		virtual OpenViBE::CString getHostName(void) const;
		virtual OpenViBE::uint32 getHostPort(void) const;

	protected:

		virtual OpenViBE::boolean preConfigure(void);
		virtual OpenViBE::boolean postConfigure(void);

	private:

		CConfigurationNetworkBuilder(void);

	protected:

		::GtkWidget* m_pHostName;
		::GtkWidget* m_pHostPort;

		OpenViBE::CString m_sHostName;
		OpenViBE::uint32 m_ui32HostPort;
	};
};

#endif // __OpenViBE_AcquisitionServer_CConfigurationNetworkBuilder_H__
