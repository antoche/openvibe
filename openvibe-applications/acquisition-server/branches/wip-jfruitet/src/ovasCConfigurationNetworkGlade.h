#ifndef __OpenViBE_AcquisitionServer_CConfigurationNetworkGlade_H__
#define __OpenViBE_AcquisitionServer_CConfigurationNetworkGlade_H__

#include "ovasCConfigurationGlade.h"

namespace OpenViBEAcquisitionServer
{
	class CConfigurationNetworkGlade : public OpenViBEAcquisitionServer::CConfigurationGlade
	{
	public:

		CConfigurationNetworkGlade(const char* sGladeXMLFileName);
		virtual ~CConfigurationNetworkGlade(void);

		virtual OpenViBE::boolean setHostName(const OpenViBE::CString& sHostName);
		virtual OpenViBE::boolean setHostPort(const OpenViBE::uint32 ui32HostPort);

		virtual OpenViBE::CString getHostName(void) const;
		virtual OpenViBE::uint32 getHostPort(void) const;

	protected:

		virtual OpenViBE::boolean preConfigure(void);
		virtual OpenViBE::boolean postConfigure(void);

	private:

		CConfigurationNetworkGlade(void);

	protected:

		::GtkWidget* m_pHostName;
		::GtkWidget* m_pHostPort;

		OpenViBE::CString m_sHostName;
		OpenViBE::uint32 m_ui32HostPort;
	};
};

#endif // __OpenViBE_AcquisitionServer_CConfigurationNetworkGlade_H__
