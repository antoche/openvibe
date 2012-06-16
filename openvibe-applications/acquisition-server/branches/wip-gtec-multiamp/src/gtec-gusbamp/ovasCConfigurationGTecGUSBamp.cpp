#if defined TARGET_HAS_ThirdPartyGUSBampCAPI

#include "ovasCConfigurationGTecGUSBamp.h"

#include <windows.h>
#include <gUSBamp.h>
#include <iostream>
#include <sstream>
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

static void button_common_gnd_ref_pressed_cb(::GtkButton* pButton, void* pUserData)
{
	CConfigurationGTecGUSBamp* l_pConfig=static_cast<CConfigurationGTecGUSBamp*>(pUserData);
	l_pConfig->buttonCommonGndRefPressedCB();
}

static void button_filters_pressed_cb(::GtkButton* pButton, void* pUserData)
{
	CConfigurationGTecGUSBamp* l_pConfig=static_cast<CConfigurationGTecGUSBamp*>(pUserData);
	l_pConfig->buttonFiltersPressedCB();
}

CConfigurationGTecGUSBamp::CConfigurationGTecGUSBamp(const char* sGtkBuilderFileName,uint32& rUSBIndex,uint8& rCommonGndAndRefBitmap, int32& rNotchFilterIndex, int32& rBandPassFilterIndex)
	: CConfigurationBuilder(sGtkBuilderFileName)
	,m_rUSBIndex(rUSBIndex)
	,m_rCommonGndAndRefBitmap(rCommonGndAndRefBitmap)
	,m_rNotchFilterIndex(rNotchFilterIndex)
	,m_rBandPassFilterIndex(rBandPassFilterIndex)
	
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

	g_signal_connect(gtk_builder_get_object(m_pBuilderConfigureInterface, "button-common-gnd-ref"), "pressed", G_CALLBACK(button_common_gnd_ref_pressed_cb), this);
	g_signal_connect(gtk_builder_get_object(m_pBuilderConfigureInterface, "button-filters"), "pressed", G_CALLBACK(button_filters_pressed_cb), this);

	::GtkWidget * l_pDialog = GTK_WIDGET(gtk_builder_get_object(m_pBuilderConfigureInterface, "dialog-common-gnd-ref"));
	gtk_dialog_add_button (GTK_DIALOG (l_pDialog),
		GTK_STOCK_APPLY,
		GTK_RESPONSE_APPLY);

	gtk_dialog_add_button (GTK_DIALOG (l_pDialog),
		GTK_STOCK_CANCEL,
		GTK_RESPONSE_CANCEL);

	l_pDialog = GTK_WIDGET(gtk_builder_get_object(m_pBuilderConfigureInterface, "dialog-filters"));
	gtk_dialog_add_button (GTK_DIALOG (l_pDialog),
		GTK_STOCK_APPLY,
		GTK_RESPONSE_APPLY);

	gtk_dialog_add_button (GTK_DIALOG (l_pDialog),
		GTK_STOCK_CANCEL,
		GTK_RESPONSE_CANCEL);


	::GtkToggleButton * l_pCheckBox = GTK_TOGGLE_BUTTON(gtk_builder_get_object(m_pBuilderConfigureInterface, "checkbutton-gnd-blockA"));
	gtk_toggle_button_set_active(l_pCheckBox,(m_rCommonGndAndRefBitmap & 1));
	l_pCheckBox = GTK_TOGGLE_BUTTON(gtk_builder_get_object(m_pBuilderConfigureInterface, "checkbutton-gnd-blockB"));
	gtk_toggle_button_set_active(l_pCheckBox,(m_rCommonGndAndRefBitmap & (1<<1)));
	l_pCheckBox = GTK_TOGGLE_BUTTON(gtk_builder_get_object(m_pBuilderConfigureInterface, "checkbutton-gnd-blockC"));
	gtk_toggle_button_set_active(l_pCheckBox,(m_rCommonGndAndRefBitmap & (1<<2)));
	l_pCheckBox = GTK_TOGGLE_BUTTON(gtk_builder_get_object(m_pBuilderConfigureInterface, "checkbutton-gnd-blockD"));
	gtk_toggle_button_set_active(l_pCheckBox,(m_rCommonGndAndRefBitmap & (1<<3)));

	l_pCheckBox = GTK_TOGGLE_BUTTON(gtk_builder_get_object(m_pBuilderConfigureInterface, "checkbutton-ref-blockA"));
	gtk_toggle_button_set_active(l_pCheckBox,(m_rCommonGndAndRefBitmap & (1<<4)));
	l_pCheckBox = GTK_TOGGLE_BUTTON(gtk_builder_get_object(m_pBuilderConfigureInterface, "checkbutton-ref-blockB"));
	gtk_toggle_button_set_active(l_pCheckBox,(m_rCommonGndAndRefBitmap & (1<<5)));
	l_pCheckBox = GTK_TOGGLE_BUTTON(gtk_builder_get_object(m_pBuilderConfigureInterface, "checkbutton-ref-blockC"));
	gtk_toggle_button_set_active(l_pCheckBox,(m_rCommonGndAndRefBitmap & (1<<6)));
	l_pCheckBox = GTK_TOGGLE_BUTTON(gtk_builder_get_object(m_pBuilderConfigureInterface, "checkbutton-ref-blockD"));
	gtk_toggle_button_set_active(l_pCheckBox,(m_rCommonGndAndRefBitmap & (1<<7)));

	// To check for available filters in the amplifier, we must connect to it.
	if(l_iCount != 0)
	{
		::HANDLE l_pHandle=::GT_OpenDevice(l_iCount);

		int l_iNbBandPassFilters, l_iNbNotchFilters;
		if(!GT_GetNumberOfFilter(&l_iNbBandPassFilters)) { std::cout << "err GT_GetNumberOfFilter\n"; }
		if(!GT_GetNumberOfNotch(&l_iNbNotchFilters)) { std::cout << "err GT_GetNumberOfNotch\n"; }
		
		::FILT * l_pBandPassFilterSpec = new ::FILT[l_iNbBandPassFilters];
		::FILT * l_pNotchFilterSpec = new ::FILT[l_iNbNotchFilters];

		if(!::GT_GetFilterSpec(l_pBandPassFilterSpec)) { std::cout << "err GT_GetFilterSpec\n"; }
		if(!::GT_GetNotchSpec(l_pNotchFilterSpec)) { std::cout << "err GT_GetNotchSpec\n"; }

		GtkComboBox * l_pComboBoxBandPass = GTK_COMBO_BOX(gtk_builder_get_object(m_pBuilderConfigureInterface, "combobox-band-pass"));
		GtkTreeModel * l_pBandPassListStore = gtk_combo_box_get_model(l_pComboBoxBandPass);
		std::stringstream l_sFilterDescription;  l_sFilterDescription << "no band pass filter.";
		GtkTreeIter l_iter;
		gtk_list_store_append(GTK_LIST_STORE(l_pBandPassListStore), &l_iter);
		gtk_list_store_set (GTK_LIST_STORE(l_pBandPassListStore), &l_iter, 0, l_sFilterDescription.str().c_str(),-1);
		l_sFilterDescription.clear();
		l_sFilterDescription.str("");
		for(int32 i = 0 ; i < l_iNbBandPassFilters ; i++)
		{
			if(l_pBandPassFilterSpec[i].type == 1) l_sFilterDescription << "Butterworth - ";
			if(l_pBandPassFilterSpec[i].type == 2) l_sFilterDescription << "Chebyshev   - ";
			l_sFilterDescription << l_pBandPassFilterSpec[i].order << " - [";
			l_sFilterDescription << l_pBandPassFilterSpec[i].fu << "; ";
			l_sFilterDescription << l_pBandPassFilterSpec[i].fo << "] - ";
			l_sFilterDescription << l_pBandPassFilterSpec[i].fs;
			GtkTreeIter l_iter;
			gtk_list_store_append(GTK_LIST_STORE(l_pBandPassListStore), &l_iter);
			gtk_list_store_set (GTK_LIST_STORE(l_pBandPassListStore), &l_iter, 0, l_sFilterDescription.str().c_str(),-1);
			l_sFilterDescription.clear();
			l_sFilterDescription.str("");
		}
		gtk_combo_box_set_active(l_pComboBoxBandPass,m_rBandPassFilterIndex+1); // +1 because -1 is for "no filter".

		GtkComboBox * l_pComboBoxNotch = GTK_COMBO_BOX(gtk_builder_get_object(m_pBuilderConfigureInterface, "combobox-notch"));
		GtkTreeModel * l_pNotchListStore = gtk_combo_box_get_model(l_pComboBoxNotch);
		l_sFilterDescription << "no notch filter.";
		gtk_list_store_append(GTK_LIST_STORE(l_pNotchListStore), &l_iter);
		gtk_list_store_set (GTK_LIST_STORE(l_pNotchListStore), &l_iter, 0, l_sFilterDescription.str().c_str(),-1);
		l_sFilterDescription.clear();
		l_sFilterDescription.str("");
		for(int32 i = 0 ; i < l_iNbNotchFilters ; i++)
		{
			if(l_pNotchFilterSpec[i].type == 1) l_sFilterDescription << "Butterworth - ";
			if(l_pNotchFilterSpec[i].type == 2) l_sFilterDescription << "Chebyshev   - ";
			l_sFilterDescription << l_pNotchFilterSpec[i].order << " - [";
			l_sFilterDescription << l_pNotchFilterSpec[i].fu << "; ";
			l_sFilterDescription << l_pNotchFilterSpec[i].fo << "] - ";
			l_sFilterDescription << l_pNotchFilterSpec[i].fs;
			GtkTreeIter l_iter;
			gtk_list_store_append(GTK_LIST_STORE(l_pNotchListStore), &l_iter);
			gtk_list_store_set (GTK_LIST_STORE(l_pNotchListStore), &l_iter, 0, l_sFilterDescription.str().c_str(),-1);
			l_sFilterDescription.clear();
			l_sFilterDescription.str("");
		}
		gtk_combo_box_set_active(l_pComboBoxNotch,m_rNotchFilterIndex+1);

		delete l_pBandPassFilterSpec;
		delete l_pNotchFilterSpec;
		::GT_CloseDevice(&l_pHandle);
	}
	else
	{
		// deactivate the buttons
		::GtkWidget * l_pButton = GTK_WIDGET(gtk_builder_get_object(m_pBuilderConfigureInterface, "button-filters"));
		gtk_widget_set_sensitive(l_pButton,false);
		l_pButton = GTK_WIDGET(gtk_builder_get_object(m_pBuilderConfigureInterface, "button-common-gnd-ref"));
		gtk_widget_set_sensitive(l_pButton,false);
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

		::GtkToggleButton * l_pCheckBox = GTK_TOGGLE_BUTTON(gtk_builder_get_object(m_pBuilderConfigureInterface, "checkbutton-gnd-blockA"));
		m_rCommonGndAndRefBitmap = (gtk_toggle_button_get_active(l_pCheckBox)?1:0);
		l_pCheckBox = GTK_TOGGLE_BUTTON(gtk_builder_get_object(m_pBuilderConfigureInterface, "checkbutton-gnd-blockB"));
		m_rCommonGndAndRefBitmap = m_rCommonGndAndRefBitmap + (gtk_toggle_button_get_active(l_pCheckBox)?(1<<1):0);
		l_pCheckBox = GTK_TOGGLE_BUTTON(gtk_builder_get_object(m_pBuilderConfigureInterface, "checkbutton-gnd-blockC"));
		m_rCommonGndAndRefBitmap = m_rCommonGndAndRefBitmap + (gtk_toggle_button_get_active(l_pCheckBox)?(1<<2):0);
		l_pCheckBox = GTK_TOGGLE_BUTTON(gtk_builder_get_object(m_pBuilderConfigureInterface, "checkbutton-gnd-blockD"));
		m_rCommonGndAndRefBitmap = m_rCommonGndAndRefBitmap + (gtk_toggle_button_get_active(l_pCheckBox)?(1<<3):0);
	
		l_pCheckBox = GTK_TOGGLE_BUTTON(gtk_builder_get_object(m_pBuilderConfigureInterface, "checkbutton-ref-blockA"));
		m_rCommonGndAndRefBitmap = m_rCommonGndAndRefBitmap + (gtk_toggle_button_get_active(l_pCheckBox)?(1<<4):0);
		l_pCheckBox = GTK_TOGGLE_BUTTON(gtk_builder_get_object(m_pBuilderConfigureInterface, "checkbutton-ref-blockB"));
		m_rCommonGndAndRefBitmap = m_rCommonGndAndRefBitmap + (gtk_toggle_button_get_active(l_pCheckBox)?(1<<5):0);
		l_pCheckBox = GTK_TOGGLE_BUTTON(gtk_builder_get_object(m_pBuilderConfigureInterface, "checkbutton-ref-blockC"));
		m_rCommonGndAndRefBitmap = m_rCommonGndAndRefBitmap + (gtk_toggle_button_get_active(l_pCheckBox)?(1<<6):0);
		l_pCheckBox = GTK_TOGGLE_BUTTON(gtk_builder_get_object(m_pBuilderConfigureInterface, "checkbutton-ref-blockD"));
		m_rCommonGndAndRefBitmap = m_rCommonGndAndRefBitmap + (gtk_toggle_button_get_active(l_pCheckBox)?(1<<7):0);

		GtkComboBox * l_pComboBoxNotch = GTK_COMBO_BOX(gtk_builder_get_object(m_pBuilderConfigureInterface, "combobox-notch"));
		m_rNotchFilterIndex = gtk_combo_box_get_active(l_pComboBoxNotch) - 1;
		GtkComboBox * l_pComboBoxBandPass = GTK_COMBO_BOX(gtk_builder_get_object(m_pBuilderConfigureInterface, "combobox-band-pass"));
		m_rBandPassFilterIndex = gtk_combo_box_get_active(l_pComboBoxBandPass) - 1;
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

void CConfigurationGTecGUSBamp::buttonCommonGndRefPressedCB(void)
{
	::GtkWidget * l_pDialog = GTK_WIDGET(gtk_builder_get_object(m_pBuilderConfigureInterface, "dialog-common-gnd-ref"));
	
	gint resp = gtk_dialog_run(GTK_DIALOG(l_pDialog));
	
	gtk_widget_hide(l_pDialog);
}

void CConfigurationGTecGUSBamp::buttonFiltersPressedCB(void)
{
	::GtkWidget * l_pDialog = GTK_WIDGET(gtk_builder_get_object(m_pBuilderConfigureInterface, "dialog-filters"));
	
	gint resp = gtk_dialog_run(GTK_DIALOG(l_pDialog));

	gtk_widget_hide(l_pDialog);
}

#endif // TARGET_HAS_ThirdPartyGUSBampCAPI
