#ifndef __OpenViBE_AcquisitionServer_CConfigurationFieldtrip_H__
#define __OpenViBE_AcquisitionServer_CConfigurationFieldtrip_H__

#include "../ovasCConfigurationBuilder.h"

namespace OpenViBEAcquisitionServer
{
    /**
	 * \class CConfigurationFieldtrip
	 * \author Amelie Serpollet (CEA/LETI/CLINATEC)
	 * \date Mon May 23 09:48:21 2011
	 * \brief The CDriverFieldtrip allows the acquisition server to acquire data from a Fieldtrip buffer.
	 *
	 */
	class CConfigurationFieldtrip : public OpenViBEAcquisitionServer::CConfigurationBuilder
	{
	public:

		CConfigurationFieldtrip(const char* sGtkBuilderFileName);
		virtual ~CConfigurationFieldtrip(void);

		virtual OpenViBE::boolean setHostName(const OpenViBE::CString& sHostName);
		virtual OpenViBE::boolean setHostPort(const OpenViBE::uint32 ui32HostPort);
		virtual OpenViBE::boolean setMinSamples(const OpenViBE::uint32 ui32MinSamples);
		virtual OpenViBE::boolean setSRCorrection(const OpenViBE::boolean bSRCorrection);

		virtual OpenViBE::CString getHostName(void) const;
		virtual OpenViBE::uint32 getHostPort(void) const;
		virtual OpenViBE::uint32 getMinSamples(void) const;
		virtual OpenViBE::boolean getSRCorrection(void) const;

	protected:

		virtual OpenViBE::boolean preConfigure(void);
		virtual OpenViBE::boolean postConfigure(void);

	private:

		CConfigurationFieldtrip(void);

	protected:

		::GtkWidget* m_pHostName;
		::GtkWidget* m_pHostPort;
		::GtkWidget* m_pMinSamples;
		::GtkWidget* m_pSRCorrection;

		OpenViBE::CString m_sHostName;
		OpenViBE::uint32 m_ui32HostPort;
		OpenViBE::uint32 m_ui32MinSamples;
		OpenViBE::boolean m_bSRCorrection;
	};
};

#endif // __OpenViBE_AcquisitionServer_CConfigurationFieldtrip_H__
