#ifndef __OpenViBE_AcquisitionServer_CConfigurationEmokit_H__
#define __OpenViBE_AcquisitionServer_CConfigurationEmokit_H__

#if defined TARGET_HAS_ThirdPartyEmokit

#include "../ovasCConfigurationBuilder.h"
#include "../ovasIDriver.h"
#include "../ovasCHeader.h"

#include <gtk/gtk.h>

namespace OpenViBEAcquisitionServer
{
	/**
	 * \class CConfigurationEmokit
	 * \author Antoine Bouthors
	 * \date 10 June 2012
	 * \brief The CConfigurationEmokit handles the configuration dialog specific to the Emotiv EPOC headset.
	 *
	 * \sa CDriverEmokit
	 */
	class CConfigurationEmokit : public OpenViBEAcquisitionServer::CConfigurationBuilder
	{
	public:

		CConfigurationEmokit(OpenViBEAcquisitionServer::IDriverContext& rDriverContext, const char* sGtkBuilderFileName, OpenViBE::boolean& rUseGyroscope, OpenViBE::uint32&  rUserID);

		virtual OpenViBE::boolean preConfigure(void);
		virtual OpenViBE::boolean postConfigure(void);

	protected:

		OpenViBEAcquisitionServer::IDriverContext& m_rDriverContext;
		OpenViBE::boolean& m_rUseGyroscope;
		OpenViBE::uint32&  m_rUserID;
	};

};

#endif // TARGET_HAS_ThirdPartyEmokit

#endif // __OpenViBE_AcquisitionServer_CConfigurationEmokit_H__
