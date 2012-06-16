#ifndef __OpenViBE_AcquisitionServer_CConfigurationEmotivEPOC_H__
#define __OpenViBE_AcquisitionServer_CConfigurationEmotivEPOC_H__

#if defined TARGET_HAS_ThirdPartyEmotivAPI

#include "../ovasCConfigurationBuilder.h"
#include "../ovasIDriver.h"
#include "../ovasCHeader.h"

#include <gtk/gtk.h>

#include <windows.h>

namespace OpenViBEAcquisitionServer
{
	/**
	 * \class CConfigurationEmotivEPOC
	 * \author Laurent Bonnet (INRIA)
	 * \date 21 july 2010
	 * \erief The CConfigurationEmotivEPOC handles the configuration dialog specific to the Emotiv EPOC headset.
	 *
	 * \sa CDriverEmotivEPOC
	 */
	class CConfigurationEmotivEPOC : public OpenViBEAcquisitionServer::CConfigurationBuilder
	{
	public:

		CConfigurationEmotivEPOC(OpenViBEAcquisitionServer::IDriverContext& rDriverContext, const char* sGtkBuilderFileName);

		virtual OpenViBE::boolean preConfigure(void);
		virtual OpenViBE::boolean postConfigure(void);

	protected:

		OpenViBEAcquisitionServer::IDriverContext& m_rDriverContext;
	};

};

#endif // TARGET_HAS_ThirdPartyEmotivAPI

#endif // __OpenViBE_AcquisitionServer_CConfigurationEmotivEPOC_H__
