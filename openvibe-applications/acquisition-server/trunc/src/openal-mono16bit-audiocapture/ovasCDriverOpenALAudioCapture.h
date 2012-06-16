#ifndef __OpenViBE_AcquisitionServer_CDriverOpenALAudioCapture_H__
#define __OpenViBE_AcquisitionServer_CDriverOpenALAudioCapture_H__

#if defined TARGET_HAS_ThirdPartyOpenAL
#include "../ovasIDriver.h"
#include "../ovasCHeader.h"
#include <openvibe/ov_all.h>
#if defined OVAS_OS_Windows
	#include <al.h>
	#include <alc.h>
#elif defined OVAS_OS_Linux
	#include <AL/al.h>
	#include <AL/alc.h>
#else
#endif

namespace OpenViBEAcquisitionServer
{
	/**
	 * \class CDriverOpenALAudioCapture
	 * \author Aurélien Van Langhenhove (CIC-IT Garches)
	 * \date Mon May 16 16:55:49 2011
	 * \erief The CDriverOpenALAudioCapture allows the acquisition server to acquire data from a OpenAL audio capture device.
	 *
	 * TODO: details
	 *
	 * \sa CConfigurationOpenALAudioCapture
	 */
	class CDriverOpenALAudioCapture : public OpenViBEAcquisitionServer::IDriver
	{
	public:

		CDriverOpenALAudioCapture(OpenViBEAcquisitionServer::IDriverContext& rDriverContext);
		virtual ~CDriverOpenALAudioCapture(void);
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
		
		virtual OpenViBE::boolean isFlagSet(
			const OpenViBEAcquisitionServer::EDriverFlag eFlag) const
		{
			return eFlag==DriverFlag_IsUnstable;
		}

	protected:

		OpenViBEAcquisitionServer::IDriverCallback* m_pCallback;

		// Replace this generic Header with any specific header you might have written
		OpenViBEAcquisitionServer::CHeader m_oHeader;

		OpenViBE::uint32 m_ui32SampleCountPerSentBlock;
		OpenViBE::float32* m_pSample;
		ALshort* Samples;
	
	private:

		/*
		 * Insert here all specific attributes, such as USB port number or device ID.
		 * Example :
		 */
		// OpenViBE::uint32 m_ui32ConnectionID;
		ALCdevice* Device;
		ALCcontext* Context;
		ALCdevice* CaptureDevice;
	};
};

#endif //TARGET_HAS_ThirdPartyOpenAL
#endif // __OpenViBE_AcquisitionServer_CDriverOpenALAudioCapture_H__
