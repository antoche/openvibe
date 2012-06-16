#if defined TARGET_HAS_ThirdPartyGUSBampCAPI

#include "ovasCConfigurationGTecGUSBamp.h"

#include <windows.h>
#include <gUSBamp.h>
#include <iostream>
#define boolean OpenViBE::boolean

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBEAcquisitionServer;

static void button_calibrate_pressed_cb(::GtkButton* pButton, void* pUserData)
{
	CConfigurationGTecGUSBamp* l_pConfig=static_cast<CConfigurationGTecGUSBamp*>(pUserData);
	l_pConfig->buttonCalibratePressedCB();
}

static gboolean idle_calibrate_cb(void* pUserData)
{
	CConfigurationGTecGUSBamp* l_pConfig=static_cast<CConfigurationGTecGUSBamp*>(pUserData);
	l_pConfig->idleCalibrateCB();
	return FALSE;
}

CConfigurationGTecGUSBamp::CConfigurationGTecGUSBamp(const char* sGtkBuilderFileName, OpenViBE::uint32& rUSBIndex)
	:CConfigurationBuilder(sGtkBuilderFileName)
	,m_rUSBIndex(rUSBIndex)
{
}

boolean CConfigurationGTecGUSBamp::preConfigure(void)
{
	if(!CConfigurationBuilder::preConfigure())
	{
		return false;
	}

	::GtkComboBox* l_pComboBox=GTK_COMBO_BOX(gtk_builder_get_object(m_pBuilderConfigureInterface, "combobox_device"));

	char l_sBuffer[1024];
	int l_iCount=0;
	boolean l_bSelected=false;

	// autodetection of the connected device
	for(uint32 i=1; i<11; i++)
	{
		::HANDLE l_pHandle=::GT_OpenDevice(i);
		if(l_pHandle)
		{
			::GT_CloseDevice(&l_pHandle);

			sprintf(l_sBuffer, "USB port %i", i);
			::gtk_combo_box_append_text(l_pComboBox, l_sBuffer);
			if(m_rUSBIndex==i)
			{
				::gtk_combo_box_set_active(l_pComboBox, l_iCount);
				l_bSelected=true;
			}
			l_iCount++;
		}
	}

	g_signal_connect(gtk_builder_get_object(m_pBuilderConfigureInterface, "button_calibrate"), "pressed", G_CALLBACK(button_calibrate_pressed_cb), this);

	if(!l_bSelected && l_iCount!=0)
	{
		::gtk_combo_box_set_active(l_pComboBox, 0);
	}

	return true;
}

boolean CConfigurationGTecGUSBamp::postConfigure(void)
{
	::GtkComboBox* l_pComboBox=GTK_COMBO_BOX(gtk_builder_get_object(m_pBuilderConfigureInterface, "combobox_device"));

	if(m_bApplyConfiguration)
	{
		int l_iUSBIndex=0;
		const char* l_sUSBIndex=::gtk_combo_box_get_active_text(l_pComboBox);
		if(l_sUSBIndex)
		{
			if(::sscanf(l_sUSBIndex, "USB port %i", &l_iUSBIndex)==1)
			{
				m_rUSBIndex=(uint32)l_iUSBIndex;
			}
		}
	}

	if(!CConfigurationBuilder::postConfigure())
	{
		return false;
	}
	return true;
}

void CConfigurationGTecGUSBamp::buttonCalibratePressedCB(void)
{
	::g_idle_add(idle_calibrate_cb, this);

	m_pCalibrateDialog=::gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_NONE, "Calibrating...");
	::gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(m_pCalibrateDialog), "Please wait a few seconds...");
	::gtk_dialog_run(GTK_DIALOG(m_pCalibrateDialog));
	::gtk_widget_destroy(m_pCalibrateDialog);

	if(m_bCalibrationDone)
	{
		::GtkWidget* l_pDialog=::gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK, "Calibration finished !");
		::gtk_dialog_run(GTK_DIALOG(l_pDialog));
		::gtk_widget_destroy(l_pDialog);
	}
	else
	{
		::GtkWidget* l_pDialog=::gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK, "Calibration failed !");
		::gtk_dialog_run(GTK_DIALOG(l_pDialog));
		::gtk_widget_destroy(l_pDialog);
	}
}

void CConfigurationGTecGUSBamp::idleCalibrateCB(void)
{
	::GtkComboBox* l_pComboBox=GTK_COMBO_BOX(gtk_builder_get_object(m_pBuilderConfigureInterface, "combobox_device"));

	m_bCalibrationDone=false;
	int l_iUSBIndex=0;
	const char* l_sUSBIndex=::gtk_combo_box_get_active_text(l_pComboBox);

	if(l_sUSBIndex)
	{
		if(::sscanf(l_sUSBIndex, "USB port %i", &l_iUSBIndex)==1)
		{
			::HANDLE l_pHandle=::GT_OpenDevice(l_iUSBIndex);
			if(l_pHandle)
			{
				m_bCalibrationDone=true;

				::SCALE l_oCalibration;
				if(!::GT_Calibrate(l_pHandle, &l_oCalibration)) { std::cout << "err GT_Calibrate\n", m_bCalibrationDone=false; }
				if(!::GT_SetScale(l_pHandle, &l_oCalibration)) { std::cout << "err GT_SetScale\n"; m_bCalibrationDone=false; }
				::GT_CloseDevice(&l_pHandle);
			}
		}
	}

	gtk_dialog_response(GTK_DIALOG(m_pCalibrateDialog), 0);
}

#endif // TARGET_HAS_ThirdPartyGUSBampCAPI
