#if defined TARGET_HAS_ThirdPartyGUSBampCAPI

#include "ovasCConfigurationGTecGUSBampMultiamp.h"

#include <windows.h>
#include <gUSBamp.h>
#include <iostream>
#include <sstream>
#define boolean OpenViBE::boolean

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBEAcquisitionServer;
using namespace std;

static void checkbutton_slave1_pressed_cb(::GtkButton* pButton, void* pUserData)
{
	CConfigurationGTecGUSBampMultiamp* l_pConfig=static_cast<CConfigurationGTecGUSBampMultiamp*>(pUserData);
	l_pConfig->checkbuttonSlavePressedCB(1);
}
static void checkbutton_slave2_pressed_cb(::GtkButton* pButton, void* pUserData)
{
	CConfigurationGTecGUSBampMultiamp* l_pConfig=static_cast<CConfigurationGTecGUSBampMultiamp*>(pUserData);
	l_pConfig->checkbuttonSlavePressedCB(2);
}
static void checkbutton_slave3_pressed_cb(::GtkButton* pButton, void* pUserData)
{
	CConfigurationGTecGUSBampMultiamp* l_pConfig=static_cast<CConfigurationGTecGUSBampMultiamp*>(pUserData);
	l_pConfig->checkbuttonSlavePressedCB(3);
}
void CConfigurationGTecGUSBampMultiamp::checkbuttonSlavePressedCB(uint32 ui32ButtonIndex)
{
	GtkWidget * l_pSlave1SerialEntry = GTK_WIDGET(gtk_builder_get_object(m_pBuilderConfigureInterface, "entry-slave1-device-serial"));
	GtkWidget * l_pSlave2SerialEntry = GTK_WIDGET(gtk_builder_get_object(m_pBuilderConfigureInterface, "entry-slave2-device-serial"));
	GtkWidget * l_pSlave3SerialEntry = GTK_WIDGET(gtk_builder_get_object(m_pBuilderConfigureInterface, "entry-slave3-device-serial"));
	GtkWidget * l_pSlave1Checkbutton = GTK_WIDGET(gtk_builder_get_object(m_pBuilderConfigureInterface, "checkbutton-slave1"));
	GtkWidget * l_pSlave2Checkbutton = GTK_WIDGET(gtk_builder_get_object(m_pBuilderConfigureInterface, "checkbutton-slave2"));
	GtkWidget * l_pSlave3Checkbutton = GTK_WIDGET(gtk_builder_get_object(m_pBuilderConfigureInterface, "checkbutton-slave3"));
	
	switch(ui32ButtonIndex)
	{
	case 1:
		gtk_widget_set_sensitive(l_pSlave1SerialEntry,!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(l_pSlave1Checkbutton)));
		break;
	case 2:
		gtk_widget_set_sensitive(l_pSlave2SerialEntry,!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(l_pSlave2Checkbutton)));
		break;
	case 3:
		gtk_widget_set_sensitive(l_pSlave3SerialEntry,!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(l_pSlave3Checkbutton)));
		break;
	default:
		break;
	}
}

CConfigurationGTecGUSBampMultiamp::CConfigurationGTecGUSBampMultiamp(const char* sGtkBuilderFileName, vector<OpenViBE::CString> & rDeviceSerials)
	: CConfigurationBuilder(sGtkBuilderFileName)
	,m_rDeviceSerials(rDeviceSerials)
{
}

boolean CConfigurationGTecGUSBampMultiamp::preConfigure(void)
{
	if(!CConfigurationBuilder::preConfigure())
	{
		return false;
	}

	g_signal_connect(gtk_builder_get_object(m_pBuilderConfigureInterface, "checkbutton-slave1"), "pressed", G_CALLBACK(checkbutton_slave1_pressed_cb), this);
	g_signal_connect(gtk_builder_get_object(m_pBuilderConfigureInterface, "checkbutton-slave2"), "pressed", G_CALLBACK(checkbutton_slave2_pressed_cb), this);
	g_signal_connect(gtk_builder_get_object(m_pBuilderConfigureInterface, "checkbutton-slave3"), "pressed", G_CALLBACK(checkbutton_slave3_pressed_cb), this);

	GtkEntry * l_pSlave1SerialEntry = GTK_ENTRY(gtk_builder_get_object(m_pBuilderConfigureInterface, "entry-slave1-device-serial"));
	GtkEntry * l_pSlave2SerialEntry = GTK_ENTRY(gtk_builder_get_object(m_pBuilderConfigureInterface, "entry-slave2-device-serial"));
	GtkEntry * l_pSlave3SerialEntry = GTK_ENTRY(gtk_builder_get_object(m_pBuilderConfigureInterface, "entry-slave3-device-serial"));
	GtkEntry * l_pMasterSerialEntry = GTK_ENTRY(gtk_builder_get_object(m_pBuilderConfigureInterface, "entry-master-device-serial"));
	GtkWidget * l_pSlave1Checkbutton = GTK_WIDGET(gtk_builder_get_object(m_pBuilderConfigureInterface, "checkbutton-slave1"));
	GtkWidget * l_pSlave2Checkbutton = GTK_WIDGET(gtk_builder_get_object(m_pBuilderConfigureInterface, "checkbutton-slave2"));
	GtkWidget * l_pSlave3Checkbutton = GTK_WIDGET(gtk_builder_get_object(m_pBuilderConfigureInterface, "checkbutton-slave3"));
	
	GtkEntry * l_pEntries[] = {l_pSlave1SerialEntry,l_pSlave2SerialEntry,l_pSlave3SerialEntry,l_pMasterSerialEntry};
	GtkWidget * l_pCheckboxes[] = {l_pSlave1Checkbutton,l_pSlave2Checkbutton,l_pSlave3Checkbutton};

	gtk_widget_set_sensitive(GTK_WIDGET(l_pSlave1SerialEntry),false);
	gtk_widget_set_sensitive(GTK_WIDGET(l_pSlave2SerialEntry),false);
	gtk_widget_set_sensitive(GTK_WIDGET(l_pSlave3SerialEntry),false);

	for(uint32 i = 1; i < m_rDeviceSerials.size(); i++)
	{
		gtk_entry_set_text(l_pEntries[i-1], (const char*) m_rDeviceSerials[i-1]);
		gtk_widget_set_sensitive(GTK_WIDGET(l_pEntries[i-1]),true);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(l_pCheckboxes[i-1]),true);
	}

	if(m_rDeviceSerials.size() > 0) gtk_entry_set_text(l_pMasterSerialEntry, (const char*) m_rDeviceSerials[m_rDeviceSerials.size()-1]);
	
	return true;
}

boolean CConfigurationGTecGUSBampMultiamp::postConfigure(void)
{
	if(m_bApplyConfiguration)
	{
		GtkEntry * l_pSlave1SerialEntry = GTK_ENTRY(gtk_builder_get_object(m_pBuilderConfigureInterface, "entry-slave1-device-serial"));
		GtkEntry * l_pSlave2SerialEntry = GTK_ENTRY(gtk_builder_get_object(m_pBuilderConfigureInterface, "entry-slave2-device-serial"));
		GtkEntry * l_pSlave3SerialEntry = GTK_ENTRY(gtk_builder_get_object(m_pBuilderConfigureInterface, "entry-slave3-device-serial"));
		GtkEntry * l_pMasterSerialEntry = GTK_ENTRY(gtk_builder_get_object(m_pBuilderConfigureInterface, "entry-master-device-serial"));
		GtkEntry * l_pEntries[] = {l_pSlave1SerialEntry,l_pSlave2SerialEntry,l_pSlave3SerialEntry,l_pMasterSerialEntry};

		GtkWidget * l_pSlave1Checkbutton = GTK_WIDGET(gtk_builder_get_object(m_pBuilderConfigureInterface, "checkbutton-slave1"));
		GtkWidget * l_pSlave2Checkbutton = GTK_WIDGET(gtk_builder_get_object(m_pBuilderConfigureInterface, "checkbutton-slave2"));
		GtkWidget * l_pSlave3Checkbutton = GTK_WIDGET(gtk_builder_get_object(m_pBuilderConfigureInterface, "checkbutton-slave3"));
		GtkWidget * l_pcheckbuttons[] = {l_pSlave1Checkbutton,l_pSlave2Checkbutton,l_pSlave3Checkbutton};

		m_rDeviceSerials.clear();
		// getting slaves serials
		for(int32 i = 2; i >= 0; i--)
		{
			if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(l_pcheckbuttons[i]))) m_rDeviceSerials.push_back(CString(gtk_entry_get_text(l_pEntries[i])));
		}
		// and master serial
		m_rDeviceSerials.push_back(CString(gtk_entry_get_text(l_pEntries[3])));
	}
	
	// The order in the device vector must be the following :
	// {SLAVE_3; SLAVE_2; SLAVE_1; MASTER}
	// Slaves are optionnal. This order is the start order (master must be started last).
	
	if(!CConfigurationBuilder::postConfigure())
	{
		return false;
	}
	return true;
}


#endif // TARGET_HAS_ThirdPartyGUSBampCAPI
