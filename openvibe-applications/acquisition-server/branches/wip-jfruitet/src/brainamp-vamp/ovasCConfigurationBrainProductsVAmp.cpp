#include "ovasCConfigurationBrainProductsVAmp.h"
#include "ovasCHeaderBrainProductsVAmp.h"

#if defined TARGET_HAS_ThirdPartyUSBFirstAmpAPI

//Path to the glade xml file used for the fast mode settings GUI
#define OVAS_ConfigureGUIFastModeSettings   "../share/openvibe-applications/acquisition-server/interface-BrainProducts-VAmp-FastModeSettings.glade"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBEAcquisitionServer;
using namespace std;

#define boolean OpenViBE::boolean

//____________________________________________________________________________________
static void button_fast_mode_settings_cb(::GtkButton* pButton, void* pUserData)
{
	static_cast<CConfigurationBrainProductsVAmp*>(pUserData)->buttonFastModeSettingsCB();
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

CConfigurationBrainProductsVAmp::CConfigurationBrainProductsVAmp(IDriverContext& rDriverContext, const char* sGladeXMLFileName, CHeaderBrainProductsVAmp * pHeaderBrainProductsVAmp)
	:CConfigurationGlade(sGladeXMLFileName)
	,m_rDriverContext(rDriverContext)
	,m_sGladeXMLFastModeSettingsFileName(OVAS_ConfigureGUIFastModeSettings)
	,m_pHeaderBrainProductsVAmp(pHeaderBrainProductsVAmp)
{
}

boolean CConfigurationBrainProductsVAmp::preConfigure(void)
{
	if(! CConfigurationGlade::preConfigure())
	{
		return false;
	}

	// prepares interface
	m_pGladeConfigureFastModeSettingsInterface=glade_xml_new(m_sGladeXMLFastModeSettingsFileName.c_str(), NULL, NULL);

	// Finds all the widgets
	m_pDialogFastModeSettings=glade_xml_get_widget(m_pGladeConfigureFastModeSettingsInterface, "dialog_fast_mode_settings");

	// the acquisition mode combo box in the main interface
	m_pAcquisitionMode=glade_xml_get_widget(m_pGladeConfigureInterface, "combobox_acquisition_mode");

	// the device combo box autofilled with all connected device
	m_pDevice=glade_xml_get_widget(m_pGladeConfigureInterface, "combobox_device");

	// the 8 spin buttons for the settings in the "fast mode settings" interface
	m_pPair1PositiveInputs=glade_xml_get_widget(m_pGladeConfigureFastModeSettingsInterface, "combobox_pair1_positive_input");
	m_pPair1NegativeInputs=glade_xml_get_widget(m_pGladeConfigureFastModeSettingsInterface, "combobox_pair1_negative_input");

	m_pPair2PositiveInputs=glade_xml_get_widget(m_pGladeConfigureFastModeSettingsInterface, "combobox_pair2_positive_input");
	m_pPair2NegativeInputs=glade_xml_get_widget(m_pGladeConfigureFastModeSettingsInterface, "combobox_pair2_negative_input");

	m_pPair3PositiveInputs=glade_xml_get_widget(m_pGladeConfigureFastModeSettingsInterface, "combobox_pair3_positive_input");
	m_pPair3NegativeInputs=glade_xml_get_widget(m_pGladeConfigureFastModeSettingsInterface, "combobox_pair3_negative_input");

	m_pPair4PositiveInputs=glade_xml_get_widget(m_pGladeConfigureFastModeSettingsInterface, "combobox_pair4_positive_input");
	m_pPair4NegativeInputs=glade_xml_get_widget(m_pGladeConfigureFastModeSettingsInterface, "combobox_pair4_negative_input");

	// connects callbacks to buttons
	g_signal_connect(glade_xml_get_widget(m_pGladeConfigureInterface,"button_fast_mode_settings"),	"pressed", G_CALLBACK(button_fast_mode_settings_cb), this);

	// Configures interface with given values

	//Data mode
	gtk_combo_box_set_active_text(
		GTK_COMBO_BOX(m_pAcquisitionMode), (m_pHeaderBrainProductsVAmp->getDataMode() == dmNormal ? "Normal" : "Fast"));

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
		::GtkComboBox* l_pComboBox=GTK_COMBO_BOX(glade_xml_get_widget(m_pGladeConfigureInterface, "combobox_device"));

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
	gtk_combo_box_set_active_text(
		GTK_COMBO_BOX(m_pAcquisitionMode), (m_pHeaderBrainProductsVAmp->getDataMode() == dmNormal ? "Normal" : "Fast"));

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
		string l_sMode = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_pAcquisitionMode));
		m_pHeaderBrainProductsVAmp->setDataMode((l_sMode == "Normal" ? dmNormal : dm20kHz4Channels));
		int l_iUSBIndex=0;

		// Device number
		if(m_iDeviceCount != 0){
			int l_iDeviceNumber;
			::GtkComboBox* l_pComboBox=GTK_COMBO_BOX(glade_xml_get_widget(m_pGladeConfigureInterface, "combobox_device"));
			if(::sscanf(gtk_combo_box_get_active_text(l_pComboBox), "device#%i", &l_iDeviceNumber)==1)
			{
				m_pHeaderBrainProductsVAmp->setDeviceId(faGetId((uint32)l_iDeviceNumber));
			}
		}
	}

	// Code from CConfigurationGlade::postConfigure() in order to have the channel names before making the pairs !
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

	//releasing ressources as we dont need it anymore
	gtk_widget_hide(m_pDialogFastModeSettings);

	g_object_unref(m_pGladeConfigureFastModeSettingsInterface);
	m_pGladeConfigureFastModeSettingsInterface=NULL;

	// making the pairs names
	if(m_pHeaderBrainProductsVAmp->getDataMode() == dm20kHz4Channels)
	{
		uint32 l_uint32PairCount = 0;
		vector<uint32> l_vPairIndex; // if the user has n<4 pairs but not in the n first settings, we need the indexes
		for (uint32 i=0; i < 4; i++)
		{
			if(		m_pHeaderBrainProductsVAmp->getFastModeSettings().Mode20kHz4Channels.ChannelsPos[i] != -1
				||	m_pHeaderBrainProductsVAmp->getFastModeSettings().Mode20kHz4Channels.ChannelsNeg[i] != -1)
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

	if(! CConfigurationGlade::postConfigure()) // normal header is filled, ressources are realesed
	{
		return false;
	}

	return true;
}

//____________________________________________________________________________________//

void CConfigurationBrainProductsVAmp::buttonFastModeSettingsCB(void)
{
	::GtkDialog* l_pDialog=GTK_DIALOG(glade_xml_get_widget(m_pGladeConfigureFastModeSettingsInterface, "dialog_fast_mode_settings"));
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
				gtk_combo_box_set_active_text(
					GTK_COMBO_BOX(m_pAcquisitionMode), "Fast");
				break;
			}
			case GTK_RESPONSE_CANCEL:
			{
				//Data mode
				gtk_combo_box_set_active_text(
					GTK_COMBO_BOX(m_pAcquisitionMode), (m_pHeaderBrainProductsVAmp->getDataMode() == dmNormal ? "Normal" : "Fast"));

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
	}while(l_iDialogResponse!=GTK_RESPONSE_APPLY && l_iDialogResponse!=GTK_RESPONSE_CANCEL);

	gtk_widget_hide(GTK_WIDGET(l_pDialog));
}

#endif // TARGET_HAS_ThirdPartyUSBFirstAmpAPI
