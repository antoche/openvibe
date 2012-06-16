#ifndef __OpenViBE_AcquisitionServer_CConfigurationBrainProductsVAmp_H__
#define __OpenViBE_AcquisitionServer_CConfigurationBrainProductsVAmp_H__

#if defined TARGET_HAS_ThirdPartyUSBFirstAmpAPI

#include "../ovasCConfigurationBuilder.h"
#include "../ovasIDriver.h"
#include "ovasCHeaderBrainProductsVAmp.h"

#include <gtk/gtk.h>

#include <windows.h>
#include <FirstAmp.h>

namespace OpenViBEAcquisitionServer
{
	enum
	{
		AcquisitionMode_VAmp16 = 0,
		AcquisitionMode_VAmp8 = 1,
		AcquisitionMode_VAmp4Fast = 2,
	};

	/**
	 * \class CConfigurationBrainProductsVAmp
	 * \author Laurent Bonnet (INRIA)
	 * \date 16 nov 2009
	 * \erief The CConfigurationBrainProductsVAmp handles the configuration dialog specific to the VAmp device.
	 *
	 * User can configure the acquisition mode (normal/fast) and the fast mode settings (monopolar or differential pair).
	 *
	 * \sa CDriverBrainProductsVAmp
	 */
	class CConfigurationBrainProductsVAmp : public OpenViBEAcquisitionServer::CConfigurationBuilder
	{
	public:

		CConfigurationBrainProductsVAmp(OpenViBEAcquisitionServer::IDriverContext& rDriverContext, const char* sGtkBuilderFileName, OpenViBEAcquisitionServer::CHeaderBrainProductsVAmp * pHeaderBrainProductsVAmp);

		virtual OpenViBE::boolean preConfigure(void);
		virtual OpenViBE::boolean postConfigure(void);

		virtual void buttonFastModeSettingsCB(void);
		virtual void buttonStartServiceCB(void);
		virtual void buttonStopServiceCB(void);
		virtual void comboBoxAcquisitionModeCB(::GtkComboBox* pComboBox);

	protected:

		OpenViBEAcquisitionServer::IDriverContext& m_rDriverContext;
		OpenViBE::int32 m_iDeviceCount;

		CHeaderBrainProductsVAmp* m_pHeaderBrainProductsVAmp;

		//widgets
		::GtkWidget* m_pDialogFastModeSettings;

		::GtkWidget* m_pDevice;

		::GtkWidget* m_pAcquisitionMode;

		::GtkWidget* m_pPair1PositiveInputs;
		::GtkWidget* m_pPair1NegativeInputs;

		::GtkWidget* m_pPair2PositiveInputs;
		::GtkWidget* m_pPair2NegativeInputs;

		::GtkWidget* m_pPair3PositiveInputs;
		::GtkWidget* m_pPair3NegativeInputs;

		::GtkWidget* m_pPair4PositiveInputs;
		::GtkWidget* m_pPair4NegativeInputs;

	private:
		OpenViBE::boolean controlVampService(OpenViBE::boolean state);
		//DWORD startWindowsService(SC_HANDLE hService);
		//gboolean idleCheckVampService(gpointer data);
		gint m_giIdleID;
	};
};

#endif // TARGET_HAS_ThirdPartyGUSBampCAPI

#endif // __OpenViBE_AcquisitionServer_CConfigurationBrainProductsVAmp_H__
