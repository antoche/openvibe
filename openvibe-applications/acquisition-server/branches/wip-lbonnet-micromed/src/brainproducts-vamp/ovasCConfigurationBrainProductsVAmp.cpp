#include "ovasCConfigurationBrainProductsVAmp.h"
#include "ovasCHeaderBrainProductsVAmp.h"

#if defined TARGET_HAS_ThirdPartyUSBFirstAmpAPI

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBEAcquisitionServer;
using namespace std;

#define boolean OpenViBE::boolean

//____________________________________________________________________________________
//
gboolean idle_check_service(gpointer pData)
{
	::GtkBuilder* l_pInterface=(::GtkBuilder*)pData;

	SC_HANDLE l_hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	SC_HANDLE l_hService = NULL;
	if (l_hSCM != NULL && l_hSCM != INVALID_HANDLE_VALUE)
	{
		l_hService = OpenService(l_hSCM, "VampService", SERVICE_ALL_ACCESS);
		if (l_hService != NULL)
		{
			SERVICE_STATUS l_ssStatus;
			QueryServiceStatus(l_hService, &l_ssStatus);

			if (l_ssStatus.dwCurrentState == SERVICE_RUNNING)
			{
				gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(l_pInterface, "button_start_service")),false);
				gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(l_pInterface, "button_stop_service")),true);
				gtk_label_set(GTK_LABEL(gtk_builder_get_object(l_pInterface, "label_service")),"VampService is Enabled");
				gtk_image_set_from_stock(GTK_IMAGE(gtk_builder_get_object(l_pInterface, "image_service")),GTK_STOCK_YES,GTK_ICON_SIZE_BUTTON);
				return true;
			}
			else
			{
				gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(l_pInterface, "button_start_service")),true);
				gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(l_pInterface, "button_stop_service")),false);
				gtk_label_set(GTK_LABEL(gtk_builder_get_object(l_pInterface, "label_service")),"VampService is Disabled");
				gtk_image_set_from_stock(GTK_IMAGE(gtk_builder_get_object(l_pInterface, "image_service")),GTK_STOCK_NO,GTK_ICON_SIZE_BUTTON);
				return true;
			}
		}
	}

	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(l_pInterface, "button_start_service")),false);
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(l_pInterface, "button_stop_service")),false);
	gtk_label_set(GTK_LABEL(gtk_builder_get_object(l_pInterface, "label_service")), (l_hSCM!=NULL && l_hSCM!=INVALID_HANDLE_VALUE)?"VampService has not been Detected":"Service Manager not Available (you must be administrator)");
	gtk_image_set_from_stock(GTK_IMAGE(gtk_builder_get_object(l_pInterface, "image_service")),GTK_STOCK_DIALOG_ERROR,GTK_ICON_SIZE_BUTTON);
	return false;
}
//____________________________________________________________________________________
//
static void button_start_service_cb(::GtkButton* pButton, void* pUserData)
{
	static_cast<CConfigurationBrainProductsVAmp*>(pUserData)->buttonStartServiceCB();
}
static void button_stop_service_cb(::GtkButton* pButton, void* pUserData)
{
	static_cast<CConfigurationBrainProductsVAmp*>(pUserData)->buttonStopServiceCB();
}
static void button_fast_mode_settings_cb(::GtkButton* pButton, void* pUserData)
{
	static_cast<CConfigurationBrainProductsVAmp*>(pUserData)->buttonFastModeSettingsCB();
}

static void combo_box_acquisition_mode_cb(::GtkComboBox* pComboBox, void* pUserData)
{
	static_cast<CConfigurationBrainProductsVAmp*>(pUserData)->comboBoxAcquisitionModeCB(pComboBox);
}

static void gtk_combo_box_set_active_text(::GtkComboBox* pComboBox, const gchar* sActiveText)
{
	::GtkTreeModel* l_pTreeModel=gtk_combo_box_get_model(pComboBox);
	::GtkTreeIter itComboEntry;
	int l_iIndex=0;
	gchar* l_sComboEntryName=NULL;
	if(gtk_tree_model_get_iter_first(l_pTreeModel, &itComboEntry))
	{
		do
		{
			gtk_tree_model_get(l_pTreeModel, &itComboEntry, 0, &l_sComboEntryName, -1);
			if(string(l_sComboEntryName)==string(sActiveText))
			{
				gtk_combo_box_set_active(pComboBox, l_iIndex);
				return;
			}
			else
			{
				l_iIndex++;
			}
		}
		while(gtk_tree_model_iter_next(l_pTreeModel, &itComboEntry));
	}
}

//____________________________________________________________________________________

// Inits the combo box, and sets the active value
// Warning : active value must be 7 8 9 10 or -1.
void initFastModeSettingsComboBox(GtkWidget * comboBox, uint32 activeValue, boolean initComboBox=true)
{
	if(initComboBox)
	{
		//-1
		char l_sPairValue[1024];
		::sprintf(l_sPairValue, "%i", (int)-1);
		gtk_combo_box_append_text(
			GTK_COMBO_BOX(comboBox),
			l_sPairValue);
		//7-10
		for(uint32 i = 7;i<11;i++)
		{
			char l_sPairValue[1024];
			::sprintf(l_sPairValue, "%i", (int)i);
			gtk_combo_box_append_text(
				GTK_COMBO_BOX(comboBox),
				l_sPairValue);
		}
	}
	char l_sPairActiveValue[1024];
	::sprintf(l_sPairActiveValue, "%i", (int)activeValue);
	gtk_combo_box_set_active_text(
		GTK_COMBO_BOX(comboBox),
		l_sPairActiveValue);
}

//____________________________________________________________________________________

CConfigurationBrainProductsVAmp::CConfigurationBrainProductsVAmp(IDriverContext& rDriverContext, const char* sGtkBuilderFileName, CHeaderBrainProductsVAmp * pHeaderBrainProductsVAmp)
	:CConfigurationBuilder(sGtkBuilderFileName)
	,m_rDriverContext(rDriverContext)
	,m_pHeaderBrainProductsVAmp(pHeaderBrainProductsVAmp)
{
	m_giIdleID = 0;
}

boolean CConfigurationBrainProductsVAmp::preConfigure(void)
{
	if(!CConfigurationBuilder::preConfigure())
	{
		return false;
	}

	// Finds all the widgets
	m_pDialogFastModeSettings=GTK_WIDGET(gtk_builder_get_object(m_pBuilderConfigureInterface, "dialog_fast_mode_settings"));

	// the acquisition mode combo box in the main interface
	m_pAcquisitionMode=GTK_WIDGET(gtk_builder_get_object(m_pBuilderConfigureInterface, "combobox_acquisition_mode"));

	// the device combo box autofilled with all connected device
	m_pDevice=GTK_WIDGET(gtk_builder_get_object(m_pBuilderConfigureInterface, "combobox_device"));

	// the 8 spin buttons for the settings in the "fast mode settings" interface
	m_pPair1PositiveInputs=GTK_WIDGET(gtk_builder_get_object(m_pBuilderConfigureInterface, "combobox_pair1_positive_input"));
	m_pPair1NegativeInputs=GTK_WIDGET(gtk_builder_get_object(m_pBuilderConfigureInterface, "combobox_pair1_negative_input"));

	m_pPair2PositiveInputs=GTK_WIDGET(gtk_builder_get_object(m_pBuilderConfigureInterface, "combobox_pair2_positive_input"));
	m_pPair2NegativeInputs=GTK_WIDGET(gtk_builder_get_object(m_pBuilderConfigureInterface, "combobox_pair2_negative_input"));

	m_pPair3PositiveInputs=GTK_WIDGET(gtk_builder_get_object(m_pBuilderConfigureInterface, "combobox_pair3_positive_input"));
	m_pPair3NegativeInputs=GTK_WIDGET(gtk_builder_get_object(m_pBuilderConfigureInterface, "combobox_pair3_negative_input"));

	m_pPair4PositiveInputs=GTK_WIDGET(gtk_builder_get_object(m_pBuilderConfigureInterface, "combobox_pair4_positive_input"));
	m_pPair4NegativeInputs=GTK_WIDGET(gtk_builder_get_object(m_pBuilderConfigureInterface, "combobox_pair4_negative_input"));

	// connects callbacks to buttons
	g_signal_connect(gtk_builder_get_object(m_pBuilderConfigureInterface,"button_fast_mode_settings"), "pressed", G_CALLBACK(button_fast_mode_settings_cb), this);
	g_signal_connect(gtk_builder_get_object(m_pBuilderConfigureInterface,"combobox_acquisition_mode"), "changed", G_CALLBACK(combo_box_acquisition_mode_cb), this);
	g_signal_connect(gtk_builder_get_object(m_pBuilderConfigureInterface,"button_start_service"), "pressed", G_CALLBACK(button_start_service_cb), this);
	g_signal_connect(gtk_builder_get_object(m_pBuilderConfigureInterface,"button_stop_service"), "pressed", G_CALLBACK(button_stop_service_cb), this);

	// start the idle function that checks the VampService
	m_giIdleID = g_idle_add(idle_check_service, m_pBuilderConfigureInterface);

	// Configures interface with given values
	//Data mode
	gtk_combo_box_set_active(GTK_COMBO_BOX(m_pAcquisitionMode), m_pHeaderBrainProductsVAmp->getAcquisitionMode());

	//Device(s)
	uint32 l_uint32DeviceCount = 0;
	uint32 l_uint32RetriesCount = 0;

	// We try to get the last opened device, (max 5 tries, 500ms sleep between tries)
	while (l_uint32RetriesCount++ < 5)
	{
		l_uint32DeviceCount = faGetCount();	// Get the last opened Device id.
		if (l_uint32DeviceCount < 1)
		{
			Sleep(500);
		}
		else
		{
			break;
		}
	}

	m_rDriverContext.getLogManager() << LogLevel_Trace << "VAmp Configuration: " << l_uint32DeviceCount << " device(s) connected.\n";

	if(l_uint32DeviceCount != 0 )
	{
		m_rDriverContext.getLogManager() << LogLevel_Trace << "VAmp Configuration: Device(s) information :\n";

		t_faInformation l_oInformation;
		char l_sBuffer[1024];
		::GtkComboBox* l_pComboBox=GTK_COMBO_BOX(gtk_builder_get_object(m_pBuilderConfigureInterface, "combobox_device"));

		for(uint32 i = 0; i<l_uint32DeviceCount; i++)
		{
			int32 l_int32DeviceId = faGetId(i);
			faGetInformation(l_int32DeviceId,  &l_oInformation);
			m_rDriverContext.getLogManager() << LogLevel_Trace << "     device#" << i << " > Model(" << CString(l_oInformation.Model == 1 ? "VAmp 16" : "VAmp 8") << ") | SN(" <<  l_oInformation.SerialNumber << ")\n";
			::sprintf(l_sBuffer, "device#%u", i);
			gtk_combo_box_append_text(l_pComboBox, l_sBuffer);
		}
		// active = the last opened device
		gtk_combo_box_set_active(l_pComboBox, l_uint32DeviceCount-1);
	}

	m_iDeviceCount = l_uint32DeviceCount;

	// acquisition mode
	gtk_combo_box_set_active(GTK_COMBO_BOX(m_pAcquisitionMode), m_pHeaderBrainProductsVAmp->getAcquisitionMode());

	//SETTINGS:
	//Adding all possible settings : 7/8/9/10 and -1
	// and setting active text
	//_______________________________________________________

	initFastModeSettingsComboBox(m_pPair1PositiveInputs, m_pHeaderBrainProductsVAmp->getFastModeSettings().Mode20kHz4Channels.ChannelsPos[0]);
	initFastModeSettingsComboBox(m_pPair1NegativeInputs, m_pHeaderBrainProductsVAmp->getFastModeSettings().Mode20kHz4Channels.ChannelsNeg[0]);
	initFastModeSettingsComboBox(m_pPair2PositiveInputs, m_pHeaderBrainProductsVAmp->getFastModeSettings().Mode20kHz4Channels.ChannelsPos[1]);
	initFastModeSettingsComboBox(m_pPair2NegativeInputs, m_pHeaderBrainProductsVAmp->getFastModeSettings().Mode20kHz4Channels.ChannelsNeg[1]);
	initFastModeSettingsComboBox(m_pPair3PositiveInputs, m_pHeaderBrainProductsVAmp->getFastModeSettings().Mode20kHz4Channels.ChannelsPos[2]);
	initFastModeSettingsComboBox(m_pPair3NegativeInputs, m_pHeaderBrainProductsVAmp->getFastModeSettings().Mode20kHz4Channels.ChannelsNeg[2]);
	initFastModeSettingsComboBox(m_pPair4PositiveInputs, m_pHeaderBrainProductsVAmp->getFastModeSettings().Mode20kHz4Channels.ChannelsPos[3]);
	initFastModeSettingsComboBox(m_pPair4NegativeInputs, m_pHeaderBrainProductsVAmp->getFastModeSettings().Mode20kHz4Channels.ChannelsNeg[3]);

	return true;
}

boolean CConfigurationBrainProductsVAmp::postConfigure(void)
{
	if(m_bApplyConfiguration)
	{
		m_pHeaderBrainProductsVAmp->setAcquisitionMode(gtk_combo_box_get_active(GTK_COMBO_BOX(m_pAcquisitionMode)));
		int l_iUSBIndex=0;

		// Device number
		if(m_iDeviceCount != 0)
		{
			int l_iDeviceNumber;
			::GtkComboBox* l_pComboBox=GTK_COMBO_BOX(gtk_builder_get_object(m_pBuilderConfigureInterface, "combobox_device"));
			if(::sscanf(gtk_combo_box_get_active_text(l_pComboBox), "device#%i", &l_iDeviceNumber)==1)
			{
				m_pHeaderBrainProductsVAmp->setDeviceId(faGetId((uint32)l_iDeviceNumber));
			}
		}
	}

#if 0
	// Code from CConfigurationBuilder::postConfigure() in order to have the channel names before making the pairs !
	if(m_bApplyConfiguration)
	{
		m_pHeader->setChannelCount(gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(m_pNumberOfChannels)));

		for(uint32 i=0; i!=m_pHeader->getChannelCount(); i++)
		{
			if(m_vChannelName[i]!="")
			{
				m_pHeader->setChannelName(i, m_vChannelName[i].c_str());
			}
		}
	}
#endif

	//releasing ressources as we dont need it anymore
	gtk_widget_hide(m_pDialogFastModeSettings);

	// making the pairs names
	if(m_pHeaderBrainProductsVAmp->getAcquisitionMode() == AcquisitionMode_VAmp4Fast)
	{
		uint32 l_uint32PairCount = 0;
		vector<uint32> l_vPairIndex; // if the user has n<4 pairs but not in the n first settings, we need the indexes
		for (uint32 i=0; i < 4; i++)
		{
			if(m_pHeaderBrainProductsVAmp->getFastModeSettings().Mode20kHz4Channels.ChannelsPos[i] != -1
			|| m_pHeaderBrainProductsVAmp->getFastModeSettings().Mode20kHz4Channels.ChannelsNeg[i] != -1)
			{
				l_uint32PairCount++;
				l_vPairIndex.push_back(i);
			}
		}

		m_pHeaderBrainProductsVAmp->setPairCount(l_uint32PairCount);
		m_rDriverContext.getLogManager() << LogLevel_Trace << l_uint32PairCount << " channel pairs used.\n";

		// we need to rename the 4 first channels, in order to match the pairs
		vector<string> l_vPairNames;

		for (uint32 i=0; i < l_uint32PairCount; i++)
		{
			string l_sPositiveChannelName = "";
			string l_sNegativeChannelName = "";
			string l_sPairName = "";

			boolean l_bPositiveChannelSet = false;
			boolean l_bNegativeChannelSet = false;

			if(m_pHeaderBrainProductsVAmp->getFastModeSettings().Mode20kHz4Channels.ChannelsPos[l_vPairIndex[i]] != -1)
			{
				l_sPositiveChannelName = string(m_pHeader->getChannelName(m_pHeaderBrainProductsVAmp->getFastModeSettings().Mode20kHz4Channels.ChannelsPos[l_vPairIndex[i]]));
				if(l_sPositiveChannelName == "")
				{
					char l_sBuffer[8];
					int l_iVal = m_pHeaderBrainProductsVAmp->getFastModeSettings().Mode20kHz4Channels.ChannelsPos[l_vPairIndex[i]];
					::sprintf(l_sBuffer, "%i", l_iVal);
					l_sPositiveChannelName = string(l_sBuffer);
				}
				l_bPositiveChannelSet = true;
			}

			if(m_pHeaderBrainProductsVAmp->getFastModeSettings().Mode20kHz4Channels.ChannelsNeg[l_vPairIndex[i]] != -1)
			{
				l_sNegativeChannelName = string(m_pHeader->getChannelName(m_pHeaderBrainProductsVAmp->getFastModeSettings().Mode20kHz4Channels.ChannelsNeg[l_vPairIndex[i]]));
				if(l_sNegativeChannelName == "")
				{
					char l_sBuffer[8];
					int l_iVal = m_pHeaderBrainProductsVAmp->getFastModeSettings().Mode20kHz4Channels.ChannelsNeg[l_vPairIndex[i]];
					::sprintf(l_sBuffer, "%i", l_iVal);
					l_sNegativeChannelName = string(l_sBuffer);
				}
				l_bNegativeChannelSet = true;
			}

			if(l_bNegativeChannelSet && l_bPositiveChannelSet)
			{
				l_sPairName = string("Differential (")+l_sPositiveChannelName.c_str() + " - " + l_sNegativeChannelName.c_str()+")";
			}

			if(!l_bNegativeChannelSet && l_bPositiveChannelSet)
			{
				l_sPairName = string("Monopolar(+) ")+l_sPositiveChannelName.c_str();
			}

			if(l_bNegativeChannelSet && !l_bPositiveChannelSet)
			{
				l_sPairName = string("Monopolar(-) ")+l_sNegativeChannelName.c_str();
			}

			l_vPairNames.push_back(l_sPairName);
		}

		for (uint32 i=0; i < l_uint32PairCount; i++)
		{
			m_pHeaderBrainProductsVAmp->setPairName(i,l_vPairNames[i].c_str());
		}
	}

	//remove the idle function for the service check
	if(m_giIdleID != 0) g_source_remove(m_giIdleID);

	if(!CConfigurationBuilder::postConfigure()) // normal header is filled, ressources are realesed
	{
		return false;
	}

	// Force sampling frequency
	m_pHeaderBrainProductsVAmp->setSamplingFrequency(m_pHeaderBrainProductsVAmp->getAcquisitionMode()==AcquisitionMode_VAmp4Fast?20000:2000);

	return true;
}

//____________________________________________________________________________________//

void CConfigurationBrainProductsVAmp::buttonFastModeSettingsCB(void)
{
	::GtkDialog* l_pDialog=GTK_DIALOG(gtk_builder_get_object(m_pBuilderConfigureInterface, "dialog_fast_mode_settings"));
	int32 l_iDialogResponse;
	do
	{
		l_iDialogResponse=gtk_dialog_run(l_pDialog);
		switch(l_iDialogResponse)
		{
			case GTK_RESPONSE_APPLY:
			{
				t_faDataModeSettings l_tFastModeSettings;
				l_tFastModeSettings.Mode20kHz4Channels.ChannelsPos[0] = atoi(gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_pPair1PositiveInputs)));
				l_tFastModeSettings.Mode20kHz4Channels.ChannelsNeg[0] = atoi(gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_pPair1NegativeInputs)));

				l_tFastModeSettings.Mode20kHz4Channels.ChannelsPos[1] = atoi(gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_pPair2PositiveInputs)));
				l_tFastModeSettings.Mode20kHz4Channels.ChannelsNeg[1] = atoi(gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_pPair2NegativeInputs)));

				l_tFastModeSettings.Mode20kHz4Channels.ChannelsPos[2] = atoi(gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_pPair3PositiveInputs)));
				l_tFastModeSettings.Mode20kHz4Channels.ChannelsNeg[2] = atoi(gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_pPair3NegativeInputs)));

				l_tFastModeSettings.Mode20kHz4Channels.ChannelsPos[3] = atoi(gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_pPair4PositiveInputs)));
				l_tFastModeSettings.Mode20kHz4Channels.ChannelsNeg[3] = atoi(gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_pPair4NegativeInputs)));

				m_pHeaderBrainProductsVAmp->setFastModeSettings(l_tFastModeSettings);
				gtk_combo_box_set_active_text(GTK_COMBO_BOX(m_pAcquisitionMode), "Fast");

				break;
			}
			case GTK_RESPONSE_CANCEL:
			{
				//Data mode
				gtk_combo_box_set_active(GTK_COMBO_BOX(m_pAcquisitionMode), m_pHeaderBrainProductsVAmp->getAcquisitionMode());

				//Settings

				initFastModeSettingsComboBox(m_pPair1PositiveInputs, m_pHeaderBrainProductsVAmp->getFastModeSettings().Mode20kHz4Channels.ChannelsPos[0],false);
				initFastModeSettingsComboBox(m_pPair1NegativeInputs, m_pHeaderBrainProductsVAmp->getFastModeSettings().Mode20kHz4Channels.ChannelsNeg[0],false);
				initFastModeSettingsComboBox(m_pPair2PositiveInputs, m_pHeaderBrainProductsVAmp->getFastModeSettings().Mode20kHz4Channels.ChannelsPos[1],false);
				initFastModeSettingsComboBox(m_pPair2NegativeInputs, m_pHeaderBrainProductsVAmp->getFastModeSettings().Mode20kHz4Channels.ChannelsNeg[1],false);
				initFastModeSettingsComboBox(m_pPair3PositiveInputs, m_pHeaderBrainProductsVAmp->getFastModeSettings().Mode20kHz4Channels.ChannelsPos[2],false);
				initFastModeSettingsComboBox(m_pPair3NegativeInputs, m_pHeaderBrainProductsVAmp->getFastModeSettings().Mode20kHz4Channels.ChannelsNeg[2],false);
				initFastModeSettingsComboBox(m_pPair4PositiveInputs, m_pHeaderBrainProductsVAmp->getFastModeSettings().Mode20kHz4Channels.ChannelsPos[3],false);
				initFastModeSettingsComboBox(m_pPair4NegativeInputs, m_pHeaderBrainProductsVAmp->getFastModeSettings().Mode20kHz4Channels.ChannelsNeg[3],false);

				break;
			}
		}
	}
	while(l_iDialogResponse!=GTK_RESPONSE_APPLY && l_iDialogResponse!=GTK_RESPONSE_CANCEL);

	gtk_widget_hide(GTK_WIDGET(l_pDialog));
}

void CConfigurationBrainProductsVAmp::comboBoxAcquisitionModeCB(::GtkComboBox* pComboBox)
{
	gint l_iAcquisitionMode=gtk_combo_box_get_active(pComboBox);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_pNumberOfChannels),
		m_pHeaderBrainProductsVAmp->getEEGChannelCount(l_iAcquisitionMode)+
		m_pHeaderBrainProductsVAmp->getAuxiliaryChannelCount(l_iAcquisitionMode)+
		m_pHeaderBrainProductsVAmp->getTriggerChannelCount(l_iAcquisitionMode));
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderConfigureInterface, "button_change_channel_names")), l_iAcquisitionMode!=AcquisitionMode_VAmp4Fast);
	gtk_widget_set_sensitive(GTK_WIDGET(gtk_builder_get_object(m_pBuilderConfigureInterface, "button_fast_mode_settings")), l_iAcquisitionMode==AcquisitionMode_VAmp4Fast);
}

void CConfigurationBrainProductsVAmp::buttonStartServiceCB(void)
{
	controlVampService(true);
}
void CConfigurationBrainProductsVAmp::buttonStopServiceCB(void)
{
	controlVampService(false);
}

boolean CConfigurationBrainProductsVAmp::controlVampService(boolean bStartService)
{
	SC_HANDLE l_handleSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	SC_HANDLE l_handleService = NULL;
	//bool bReturn = false;
	//DWORD dwReturn = NO_ERROR;

	if (l_handleSCM == NULL || l_handleSCM == INVALID_HANDLE_VALUE)
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << " [VampService] The driver was unable to create the handler to the SCManager (err code "<<(uint32)GetLastError()<<".\n";
		return false;
	}
	try
	{
		l_handleService = OpenService(l_handleSCM, "VampService", SERVICE_ALL_ACCESS);
		if (l_handleService != NULL)
		{
			SERVICE_STATUS Status;
			QueryServiceStatus(l_handleService, &Status);
			if (bStartService)
			{
				if (Status.dwCurrentState != SERVICE_RUNNING)
				{
					m_rDriverContext.getLogManager() << LogLevel_Trace << " [VampService] Starting VampService...\n";
					if (!StartService(
							l_handleService, // handle to service
							0,               // number of arguments
							NULL))           // no arguments
					{
						m_rDriverContext.getLogManager() << LogLevel_Error << " [VampService] The driver was unable to restart the service (err code "<<(uint32)GetLastError()<<".\n";
						return false;
					}
					else
					{
						m_rDriverContext.getLogManager() << LogLevel_Trace << " [VampService] VampService started successfully.\n";
						return true;
					}
				}
			}
			else // try to stop service
			{
				if (Status.dwCurrentState != SERVICE_STOPPED)
				{
					ControlService(l_handleService, SERVICE_CONTROL_STOP, &Status);
					boolean l_bStopped=false;
					for (int i = 0; i < 5 && !l_bStopped; i++) // about 5 seconds
					{
						m_rDriverContext.getLogManager() << LogLevel_Trace << " [VampService] Checking the VampService...\n";
						Sleep(1000);
						ControlService(l_handleService, SERVICE_CONTROL_INTERROGATE, &Status);
						if (Status.dwCurrentState == SERVICE_STOPPED)
						{
							m_rDriverContext.getLogManager() << LogLevel_Trace << " [VampService] VampService stopped successfully.\n";
							l_bStopped = true;
						}
					}
					if(!l_bStopped) m_rDriverContext.getLogManager() << LogLevel_Warning << " [VampService] After 5 seconds check, VampService did not stop.\n";
				}
			}
			m_rDriverContext.getLogManager() << LogLevel_Trace << " [VampService] Closing handlers.\n";
			CloseServiceHandle(l_handleService);
			l_handleService = NULL;
			CloseServiceHandle(l_handleSCM);
			l_handleSCM = NULL;
		}
		else
		{
			m_rDriverContext.getLogManager() << LogLevel_Error << " [VampService] The driver was unable to create the handler to VampService (err code "<<(uint32)GetLastError()<<".\n";
			return false;
		}
	}
	catch (...)
	{
		if (l_handleService != NULL)
		{
			m_rDriverContext.getLogManager() << LogLevel_Error << " [VampService] An error occured with the VampService handler (err code "<<(uint32)GetLastError()<<".\n";
			CloseServiceHandle(l_handleService);
			l_handleService = NULL;
		}
		if (l_handleSCM != NULL)
		{
			m_rDriverContext.getLogManager() << LogLevel_Error << " [VampService] An error occured with the SCManager handler (err code "<<(uint32)GetLastError()<<".\n";
			CloseServiceHandle(l_handleSCM);
			l_handleSCM = NULL;
		}
		return false;
	}
	return true;
}

#endif // TARGET_HAS_ThirdPartyUSBFirstAmpAPI
