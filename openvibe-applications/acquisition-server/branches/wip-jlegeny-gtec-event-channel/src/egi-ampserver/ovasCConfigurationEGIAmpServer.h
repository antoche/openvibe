#ifndef __OpenViBE_AcquisitionServer_CConfigurationEGIAmpServer_H__
#define __OpenViBE_AcquisitionServer_CConfigurationEGIAmpServer_H__

#include "../ovasCConfigurationBuilder.h"

namespace OpenViBEAcquisitionServer
{
	class CConfigurationEGIAmpServer : public OpenViBEAcquisitionServer::CConfigurationBuilder
	{
	public:

		CConfigurationEGIAmpServer(const char* sGtkBuilderFileName);
		virtual ~CConfigurationEGIAmpServer(void);

		virtual OpenViBE::boolean setHostName(const OpenViBE::CString& sHostName);
		virtual OpenViBE::boolean setCommandPort(const OpenViBE::uint32 ui32CommandPort);
		virtual OpenViBE::boolean setStreamPort(const OpenViBE::uint32 ui32StreamPort);

		virtual OpenViBE::CString getHostName(void) const;
		virtual OpenViBE::uint32 getCommandPort(void) const;
		virtual OpenViBE::uint32 getStreamPort(void) const;

	protected:

		virtual OpenViBE::boolean preConfigure(void);
		virtual OpenViBE::boolean postConfigure(void);

	private:

		CConfigurationEGIAmpServer(void);

	protected:

		::GtkWidget* m_pHostName;
		::GtkWidget* m_pCommandPort;
		::GtkWidget* m_pStreamPort;

		OpenViBE::CString m_sHostName;
		OpenViBE::uint32 m_ui32CommandPort;
		OpenViBE::uint32 m_ui32StreamPort;
	};
};

#endif // __OpenViBE_AcquisitionServer_CConfigurationEGIAmpServer_H__
