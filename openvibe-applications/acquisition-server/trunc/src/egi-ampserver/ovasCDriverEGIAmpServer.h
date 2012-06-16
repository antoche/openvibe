#ifndef __OpenViBE_AcquisitionServer_CDriverEGIAmpServer_H__
#define __OpenViBE_AcquisitionServer_CDriverEGIAmpServer_H__

#include "../ovasIDriver.h"
#include "../ovasCHeader.h"

#include <socket/IConnectionClient.h>

namespace OpenViBEAcquisitionServer
{
	class CCommandConnectionHandler;

	class CDriverEGIAmpServer : public OpenViBEAcquisitionServer::IDriver
	{
	public:

		friend class CCommandConnectionHandler;

		CDriverEGIAmpServer(OpenViBEAcquisitionServer::IDriverContext& rDriverContext);
		virtual void release(void);
		virtual const char* getName(void);

		virtual OpenViBE::boolean isFlagSet(
			const OpenViBEAcquisitionServer::EDriverFlag eFlag) const
		{
			return eFlag==DriverFlag_IsUnstable;
		}

		virtual OpenViBE::boolean initialize(
			const OpenViBE::uint32 ui32SampleCountPerSentBlock,
			OpenViBEAcquisitionServer::IDriverCallback& rCallback);
		virtual OpenViBE::boolean uninitialize(void);

		virtual OpenViBE::boolean start(void);
		virtual OpenViBE::boolean stop(void);
		virtual OpenViBE::boolean loop(void);

		virtual OpenViBE::boolean isConfigurable(void);
		virtual OpenViBE::boolean configure(void);
		virtual const OpenViBEAcquisitionServer::IHeader* getHeader(void) { return &m_oHeader; }

	protected:

		virtual OpenViBE::boolean exectue(const char* sScriptFilename);

	protected:

		OpenViBEAcquisitionServer::IDriverCallback* m_pCallback;
		OpenViBEAcquisitionServer::CHeader m_oHeader;

		Socket::IConnectionClient* m_pCommand;
		Socket::IConnectionClient* m_pStream;
		OpenViBE::uint32 m_ui32SampleCountPerSentBlock;
		OpenViBE::uint32 m_ui32SampleIndex;
		OpenViBE::uint32 m_ui32ChannelCount;
		OpenViBE::float32* m_pBuffer;

		OpenViBE::CString m_sAmpServerHostName;
		OpenViBE::uint32 m_ui32CommandPort;
		OpenViBE::uint32 m_ui32StreamPort;
	};
};

#endif // __OpenViBE_AcquisitionServer_CDriverEGIAmpServer_H__
