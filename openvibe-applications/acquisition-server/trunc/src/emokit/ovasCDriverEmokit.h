#ifndef __OpenViBE_AcquisitionServer_CDriverEmokit_H__
#define __OpenViBE_AcquisitionServer_CDriverEmokit_H__

#if defined TARGET_HAS_ThirdPartyEmokit

#include "../ovasIDriver.h"
#include "../ovasCHeader.h"

#include <openvibe/ov_all.h>

#include <emokit/emokit.h>

#include <vector>

namespace OpenViBEAcquisitionServer
{
	/**
	 * \class CDriverEmokit
	 * \author Antoine Bouthors
	 * \date 10 June 2012
	 * \brief The CDriverEmokit allows the acquisition server to acquire data from a Emotiv EPOC amplifier via the emokit library.
	 *
	 */
	class CDriverEmokit : public OpenViBEAcquisitionServer::IDriver
	{
	public:

		CDriverEmokit(OpenViBEAcquisitionServer::IDriverContext& rDriverContext);
		virtual ~CDriverEmokit(void);
		virtual const char* getName(void);

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

		OpenViBEAcquisitionServer::IDriverCallback* m_pCallback;

		OpenViBEAcquisitionServer::CHeader m_oHeader;

		OpenViBE::float32* m_pSample;

	private:

		OpenViBE::uint32 m_ui32EDK_LastErrorCode;

		emokit_device* m_device;
		OpenViBE::uint32 m_ui32UserID;
		OpenViBE::boolean m_bUseGyroscope;
	};
};

#endif // TARGET_HAS_ThirdPartyEmokit

#endif // __OpenViBE_AcquisitionServer_CDriverEmokit_H__
