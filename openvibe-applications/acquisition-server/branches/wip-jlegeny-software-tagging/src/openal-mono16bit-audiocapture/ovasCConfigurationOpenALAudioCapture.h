#ifndef __OpenViBE_AcquisitionServer_CConfigurationOpenALAudioCapture_H__
#define __OpenViBE_AcquisitionServer_CConfigurationOpenALAudioCapture_H__

#if defined TARGET_HAS_ThirdPartyOpenAL
#include "../ovasCConfigurationBuilder.h"
#include "../ovasIDriver.h"

#include <gtk/gtk.h>

namespace OpenViBEAcquisitionServer
{
	/**
	 * \class CConfigurationOpenALAudioCapture
	 * \author Aurélien Van Langhenhove (CIC-IT Garches)
	 * \date Mon May 16 16:55:49 2011
	 * \erief The CConfigurationOpenALAudioCapture handles the configuration dialog specific to the OpenAL audio capture device.
	 *
	 * TODO: details
	 *
	 * \sa CDriverOpenALAudioCapture
	 */
	class CConfigurationOpenALAudioCapture : public OpenViBEAcquisitionServer::CConfigurationBuilder
	{
	public:

		// you may have to add to your constructor some reference parameters
		// for example, a connection ID:
		//CConfigurationOpenALAudioCapture(OpenViBEAcquisitionServer::IDriverContext& rDriverContext, const char* sGtkBuilderFileName, OpenViBE::uint32& rConnectionId);
		CConfigurationOpenALAudioCapture(OpenViBEAcquisitionServer::IDriverContext& rDriverContext, const char* sGtkBuilderFileName);

		virtual OpenViBE::boolean preConfigure(void);
		virtual OpenViBE::boolean postConfigure(void);

	protected:

		OpenViBEAcquisitionServer::IDriverContext& m_rDriverContext;

	private:

		/*
		 * Insert here all specific attributes, such as a connection ID.
		 * use references to directly modify the corresponding attribute of the driver
		 * Example:
		 */
		// OpenViBE::uint32& m_ui32ConnectionID;
	};
};

#endif //TARGET_HAS_ThirdPartyOpenAL
#endif // __OpenViBE_AcquisitionServer_CConfigurationOpenALAudioCapture_H__
