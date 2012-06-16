#ifndef __OpenViBE_AcquisitionServer_CConfigurationNeuroskyMindset_H__
#define __OpenViBE_AcquisitionServer_CConfigurationNeuroskyMindset_H__

#if defined TARGET_HAS_ThirdPartyThinkGearAPI

#include "../ovasCConfigurationBuilder.h"
#include "../ovasIDriver.h"

#include <gtk/gtk.h>

#define OVAS_MINDSET_INVALID_COM_PORT 0xffff

namespace OpenViBEAcquisitionServer
{
	/**
	 * \class CConfigurationNeuroskyMindset
	 * \author Laurent Bonnet (INRIA)
	 * \date 05 may 2010
	 * \erief The CConfigurationNeuroskyMindset handles the configuration dialog specific to the MindSet device.
	 *
	 * User can configure ... (TODO).
	 *
	 * \sa CDriverNeuroskyMindset
	 */
	class CConfigurationNeuroskyMindset : public OpenViBEAcquisitionServer::CConfigurationBuilder
	{
	public:

		CConfigurationNeuroskyMindset(OpenViBEAcquisitionServer::IDriverContext& rDriverContext, const char* sGtkBuilderFileName,OpenViBE::uint32& rComPort);

		virtual OpenViBE::boolean preConfigure(void);
		virtual OpenViBE::boolean postConfigure(void);

		//virtual void buttonCheckSignalQualityCB(void);
		//virtual void buttonRefreshCB(void);
	
	protected:

		OpenViBEAcquisitionServer::IDriverContext& m_rDriverContext;
		OpenViBE::int32 m_iDeviceCount;

		OpenViBE::uint32& m_rComPort;

		//widgets
		::GtkWidget* m_pComPortSpinButton;

		OpenViBE::boolean m_bCheckSignalQuality;

	private:

		OpenViBE::uint32 m_ui32CurrentConnectionId;
	};
};

#endif // TARGET_HAS_ThirdPartyThinkGearAPI

#endif // __OpenViBE_AcquisitionServer_CConfigurationNeuroskyMindset_H__
