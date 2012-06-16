#ifndef __OpenViBE_AcquisitionServer_CConfigurationGTecGUSBamp_H__
#define __OpenViBE_AcquisitionServer_CConfigurationGTecGUSBamp_H__

#if defined TARGET_HAS_ThirdPartyGUSBampCAPI

#include "../ovasCConfigurationBuilder.h"

#include <gtk/gtk.h>

namespace OpenViBEAcquisitionServer
{
	class CConfigurationGTecGUSBamp : public OpenViBEAcquisitionServer::CConfigurationBuilder
	{
	public:
		CConfigurationGTecGUSBamp(const char* sGtkBuilderFileName, OpenViBE::uint32& rUSBIndex);

		virtual OpenViBE::boolean preConfigure(void);
		virtual OpenViBE::boolean postConfigure(void);

		void buttonCalibratePressedCB(void);
		void idleCalibrateCB(void);

	protected:
		OpenViBE::uint32& m_rUSBIndex;

	private:
		::GtkWidget* m_pCalibrateDialog;
		OpenViBE::boolean m_bCalibrationDone;
	};
};

#endif // TARGET_HAS_ThirdPartyGUSBampCAPI

#endif // __OpenViBE_AcquisitionServer_CConfigurationGTecGUSBamp_H__
