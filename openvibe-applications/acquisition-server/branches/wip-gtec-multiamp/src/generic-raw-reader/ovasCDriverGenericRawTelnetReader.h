#ifndef __OpenViBE_AcquisitionServer_CDriverGenericRawTelnetReader_H__
#define __OpenViBE_AcquisitionServer_CDriverGenericRawTelnetReader_H__

#include "ovasCDriverGenericRawReader.h"

#include <socket/IConnectionClient.h>

namespace OpenViBEAcquisitionServer
{
	class CDriverGenericRawTelnetReader : public OpenViBEAcquisitionServer::CDriverGenericRawReader
	{
	public:

		CDriverGenericRawTelnetReader(OpenViBEAcquisitionServer::IDriverContext& rDriverContext);

		const char* getName(void) { return "Generic Raw Telnet Reader"; }
		virtual OpenViBE::boolean isConfigurable(void) { return true; }
		virtual OpenViBE::boolean configure(void);

	protected:

		virtual OpenViBE::boolean open(void);
		virtual OpenViBE::boolean close(void);
		virtual OpenViBE::boolean read(void);

	protected:

		Socket::IConnectionClient* m_pConnection;
		OpenViBE::CString m_sHostName;
		OpenViBE::uint32 m_ui32HostPort;
	};
};

#endif // __OpenViBE_AcquisitionServer_CDriverGenericRawTelnetReader_H__
