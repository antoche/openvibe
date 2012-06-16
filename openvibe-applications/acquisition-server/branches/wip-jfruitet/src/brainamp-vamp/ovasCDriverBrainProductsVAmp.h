#ifndef __OpenViBE_AcquisitionServer_CDriverBrainProductsVAmp_H__
#define __OpenViBE_AcquisitionServer_CDriverBrainProductsVAmp_H__

#if defined TARGET_HAS_ThirdPartyUSBFirstAmpAPI

#include "../ovasIDriver.h"
#include "ovasCHeaderBrainProductsVAmp.h"
#include <openvibe/ov_all.h>

#include <windows.h>
#include <FirstAmp.h>

#include <vector>

namespace OpenViBEAcquisitionServer
{
	/**
	 * \class CDriverBrainProductsVAmp
	 * \author Laurent Bonnet (INRIA)
	 * \date 16 nov 2009
	 * \erief The CDriverBrainProductsVAmp allows the acquisition server to acquire data from a USB-VAmp-16 amplifier (BrainProducts GmbH).
	 *
	 * The driver allows 2 different acquisition modes: normal (2kHz sampling frequency - max 16 electrodes)
	 * or fast (20kHz sampling frequency, 4 monopolar or differential channels).
	 * The driver uses a dedicated Header.
	 *
	 * \sa CHeaderBrainProductsVAmp
	 */
	class CDriverBrainProductsVAmp : public OpenViBEAcquisitionServer::IDriver
	{
	public:

		CDriverBrainProductsVAmp(OpenViBEAcquisitionServer::IDriverContext& rDriverContext);
		virtual ~CDriverBrainProductsVAmp(void);
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

		OpenViBEAcquisitionServer::CHeaderBrainProductsVAmp m_oHeader;

		OpenViBE::uint32 m_ui32SampleCountPerSentBlock;
		OpenViBE::uint32 m_ui32TotalSampleCount;
		OpenViBE::float32* m_pSample;

		std::vector<OpenViBE::uint32> m_vStimulationIdentifier;
		std::vector<OpenViBE::uint64> m_vStimulationDate;
		std::vector<OpenViBE::uint64> m_vStimulationSample;

	private:
		OpenViBE::uint32 m_ui32StartTime;
	};
};

#endif // TARGET_HAS_ThirdPartyGUSBampCAPI

#endif // __OpenViBE_AcquisitionServer_CDriverBrainProductsVAmp_H__
