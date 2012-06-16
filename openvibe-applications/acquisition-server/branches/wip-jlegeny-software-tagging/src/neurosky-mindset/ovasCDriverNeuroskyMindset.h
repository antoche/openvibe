#ifndef __OpenViBE_AcquisitionServer_CDriverNeuroskyMindset_H__
#define __OpenViBE_AcquisitionServer_CDriverNeuroskyMindset_H__

#if defined TARGET_HAS_ThirdPartyThinkGearAPI

#include "../ovasIDriver.h"
#include "../ovasCHeader.h"
#include <openvibe/ov_all.h>

namespace OpenViBEAcquisitionServer
{
	/**
	 * \class CDriverNeuroskyMindset
	 * \author Laurent Bonnet (INRIA)
	 * \date 03 may 2010
	 * \erief The CDriverNeuroskyMindset allows the acquisition server to acquire data from a MindSet device (Neurosky)).
	 *
	 * The driver opens a connection to the device through a dedicated API called ThinkGear, part of the MindSet Development Tools (MDT).
	 * The MDT are available for free on the official Neurosky website (http://store.neurosky.com/products/mindset-development-tools).
	 *
	 */
	class CDriverNeuroskyMindset : public OpenViBEAcquisitionServer::IDriver
	{
	public:

		CDriverNeuroskyMindset(OpenViBEAcquisitionServer::IDriverContext& rDriverContext);
		virtual ~CDriverNeuroskyMindset(void);
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

		OpenViBE::uint32 m_ui32SampleCountPerSentBlock;
		OpenViBE::uint32 m_ui32TotalSampleCount;
		OpenViBE::float32* m_pSample;

	private:

		OpenViBE::int32 m_i32ConnectionID;
		OpenViBE::uint32 m_ui32ComPort;
		OpenViBE::boolean m_bESenseChannels;
		OpenViBE::boolean m_bBandPowerChannels;
		OpenViBE::boolean m_bBlinkStimulations;
		OpenViBE::boolean m_bBlinkStrenghtChannel;

		OpenViBE::uint32 m_ui32WarningCount;
	};
};

#endif // TARGET_HAS_ThirdPartyThinkGearAPI

#endif // __OpenViBE_AcquisitionServer_CDriverNeuroskyMindset_H__
