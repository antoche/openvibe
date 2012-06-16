#if defined TARGET_HAS_ThirdPartyThinkGearAPI

#include "ovasCConfigurationNeuroskyMindset.h"

#include <system/Time.h>

#include <sstream>

#include <thinkgear.h>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBEAcquisitionServer;
using namespace std;

//------------------------------------------------------------------------------------------
// NOTE : The signal checker is implemented but the device does not seem to handle 
//        a lot of connection/reconnection and this functionnality may cause 
//        the bluetooth connection to crash when configuring, checking, 
//        then reconnecting, and reading data. The signal check button in the 
//        configuration window is not visible, but the checking is always performed online.
//------------------------------------------------------------------------------------------
/*
static void button_check_signal_quality_cb(::GtkButton* pButton, void* pUserData)
{
	static_cast<CConfigurationNeuroskyMindset*>(pUserData)->buttonCheckSignalQualityCB();
}
void CConfigurationNeuroskyMindset::buttonCheckSignalQualityCB()
{
	m_bCheckSignalQuality = true;

	int l_iComPort;
	const char* l_sUSBIndex=::gtk_combo_box_get_active_text(GTK_COMBO_BOX(gtk_builder_get_object(m_pBuilderConfigureInterface, "combobox_com_port")));
	if(l_sUSBIndex)
	{
		if(!::sscanf(l_sUSBIndex, "COM%i", &l_iComPort)==1)
		{
			m_rDriverContext.getLogManager() << LogLevel_Error << "No serial port selected.\n";	
			return;
		}
	}

	// Get a new connection ID handle to ThinkGear API //	
	int l_iConnectionId = TG_GetNewConnectionId();
	if( l_iConnectionId >= 0 )
	{
		m_rDriverContext.getLogManager() << LogLevel_Info << "New connection to ThinkGear driver (ID "<<l_iConnectionId<<").\n";

		// Attempt to connect the connection ID handle to serial port //
		stringstream l_ssComPortName;
		l_ssComPortName << "\\\\.\\COM" << l_iComPort;
		m_rDriverContext.getLogManager() << LogLevel_Info << "Communication through selected serial port COM"<<l_iComPort<<"...";
		int l_iErrCode = TG_Connect(l_iConnectionId,l_ssComPortName.str().c_str(),TG_BAUD_9600,TG_STREAM_PACKETS );
		if( l_iErrCode >= 0 ) 
		{
			printf(" established !.\n");
					
			m_ui32CurrentConnectionId = l_iConnectionId;
			::GtkDialog* l_pDialog=GTK_DIALOG(gtk_builder_get_object(m_pBuilderConfigureInterface, "dialog_check_signal_quality"));
			int32 l_iDialogResponse;
			
			do
			{
				l_iDialogResponse=gtk_dialog_run(l_pDialog);
			}while(l_iDialogResponse!=GTK_RESPONSE_APPLY);

			gtk_widget_hide(GTK_WIDGET(l_pDialog));

			TG_Disconnect(l_iConnectionId);
			m_rDriverContext.getLogManager() << LogLevel_Info << "Disconnected from serial port COM"<<l_iComPort<<".\n";
		}
		else
		{
			printf(" FAILED !.\n");
			m_rDriverContext.getLogManager() << LogLevel_Error << "The driver was unable to connect to serial port COM"<<l_iComPort<<" (error code "<<l_iErrCode<<").\n";	
		}
		TG_FreeConnection(l_iConnectionId);
		m_rDriverContext.getLogManager() << LogLevel_Info << "Connection with ID "<<l_iConnectionId<<" freed.\n";
	}
	else
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "The driver was unable to connect to the ThinkGear Communication Driver. (error code "<<l_iConnectionId<<").\n";;	
		gtk_widget_hide(GTK_WIDGET(gtk_builder_get_object(m_pBuilderConfigureInterface, "window_check_signal_quality")));
	}
}

//-----------------------------------------------------------------------------
static void button_refresh_cb(::GtkButton* pButton, void* pUserData)
{
	static_cast<CConfigurationNeuroskyMindset*>(pUserData)->buttonRefreshCB();
}
void CConfigurationNeuroskyMindset::buttonRefreshCB()
{
	bool l_bValueUpdated = false;
	//-------------- UPDATE VALUE -----------------//
	uint32 l_ui32StartTime = System::Time::getTime();
	//1 second timeout
	while(!l_bValueUpdated && System::Time::getTime() < l_ui32StartTime + 1000.0)
	{
		int l_iErrCode = TG_ReadPackets( m_ui32CurrentConnectionId, -1 );
		if(l_iErrCode > 0) 
		{
			//m_rDriverContext.getLogManager() << LogLevel_Info << "Packet read....\n";

			//checking the signal quality
			//if it has been updated...
			if( TG_GetValueStatus(m_ui32CurrentConnectionId, TG_DATA_POOR_SIGNAL ) != 0 )
			{
				float32 signal_quality = (float32) TG_GetValue(m_ui32CurrentConnectionId, TG_DATA_POOR_SIGNAL); //0-200
				m_rDriverContext.getLogManager() << LogLevel_Info << "Poor signal value received: "<<signal_quality<<".\n";

				gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(gtk_builder_get_object(m_pBuilderConfigureInterface, "progressbar_signal_quality")),1-(signal_quality/200.0));
				if(signal_quality ==200)
				{
					gtk_label_set_label(GTK_LABEL(gtk_builder_get_object(m_pBuilderConfigureInterface, "label_status")),"No contact between the forehead and the electrode.");
				}
				else if(signal_quality > 50)
				{
					gtk_label_set_label(GTK_LABEL(gtk_builder_get_object(m_pBuilderConfigureInterface, "label_status")),"Poor signal detected, please check the electrodes (forehead and ear).");
				}
				else
				{
					gtk_label_set_label(GTK_LABEL(gtk_builder_get_object(m_pBuilderConfigureInterface, "label_status")),"The signal is good !");
				}
				//System::Time::sleep(500);
				l_bValueUpdated = true;
			} 
		}
	}

	if(!l_bValueUpdated)
	{
		m_rDriverContext.getLogManager() << LogLevel_Warning << "Can't read new value - Timeout.\n";
	}
}
*/
//_________________________________________________

CConfigurationNeuroskyMindset::CConfigurationNeuroskyMindset(IDriverContext& rDriverContext, const char* sGtkBuilderFileName,uint32& rComPort,boolean& rESenseChannels,boolean& rBandPowerChannels,boolean& rBlinkStimulations,boolean& rBlinkStrenghtChannel)
	:CConfigurationBuilder(sGtkBuilderFileName)
	,m_rDriverContext(rDriverContext)
	,m_rComPort(rComPort)
	,m_bCheckSignalQuality(false)
	,m_ui32CurrentConnectionId((uint32)-1)
	,m_rESenseChannels(rESenseChannels)
	,m_rBandPowerChannels(rBandPowerChannels)
	,m_rBlinkStimulations(rBlinkStimulations)
	,m_rBlinkStrenghtChannel(rBlinkStrenghtChannel)
{
}

boolean CConfigurationNeuroskyMindset::preConfigure(void)
{
	if(! CConfigurationBuilder::preConfigure())
	{
		return false;
	}

	
	//::GtkWidget * l_pWindowCheckSignalQuality=GTK_WIDGET(gtk_builder_get_object(m_pBuilderConfigureInterface, "dialog_check_signal_quality"));

	/*
	g_signal_connect(gtk_builder_get_object(m_pBuilderConfigureInterface, "button_check_signal_quality"),"pressed",G_CALLBACK(button_check_signal_quality_cb), this);

	g_signal_connect(gtk_builder_get_object(m_pBuilderConfigureInterface, "button_refresh"),"pressed",G_CALLBACK(button_refresh_cb), this);

	//hide on close
	g_signal_connect (G_OBJECT(l_pWindowCheckSignalQuality),
		"delete_event",
		G_CALLBACK(::gtk_widget_hide), NULL);
	*/

	char l_sBuffer[1024];
	int l_iCount=0;
	boolean l_bSelected=false;

	::GtkComboBox* l_pComboBox=GTK_COMBO_BOX(gtk_builder_get_object(m_pBuilderConfigureInterface, "combobox_com_port"));
	
	/* Get a new connection ID handle to ThinkGear API */
	int l_iConnectionId = TG_GetNewConnectionId();
	if( l_iConnectionId >= 0 )
	{
		m_rDriverContext.getLogManager() << LogLevel_Info << "ThinkGear Connection ID is: "<< l_iConnectionId <<".\n";
		// try the com ports
		m_rDriverContext.getLogManager() << LogLevel_Info << "Scanning COM ports 1 to 16...\n";
		for(uint32 i=1; i<16; i++)
		{
			/* Attempt to connect the connection ID handle to serial port */
			stringstream l_ssComPortName;
			l_ssComPortName << "\\\\.\\COM" << i;
			int l_iErrCode = TG_Connect(l_iConnectionId,l_ssComPortName.str().c_str(),TG_BAUD_9600,TG_STREAM_PACKETS );
			if( l_iErrCode >= 0 ) 
			{
				m_rDriverContext.getLogManager() << LogLevel_Info << "Connection available on port COM"<< i <<" -- STATUS: ";
		
				//we try to read one packet, to check the connection.
				l_iErrCode = TG_ReadPackets( l_iConnectionId, 1 );
				if(l_iErrCode >= 0)
				{	
					printf("OK\n");
					sprintf(l_sBuffer, "COM%i", i);
					gtk_combo_box_append_text(l_pComboBox, l_sBuffer);
					if(m_rComPort==i)
					{
						::gtk_combo_box_set_active(l_pComboBox, l_iCount);
						l_bSelected=true;
					}
					l_iCount++;
				}
				else
				{
					if(l_iErrCode == -1) printf("FAIL (Invalid connection ID)\n");
					if(l_iErrCode == -2) printf("FAIL (0 bytes on the stream)\n");
					if(l_iErrCode == -3) printf("FAIL (I/O error occured)\n");
				}
				
				TG_Disconnect(l_iConnectionId);	
			}
		}
		// free the connection to ThinkGear API
		TG_FreeConnection(l_iConnectionId);
	}
	else
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "The driver was unable to connect to the ThinkGear Communication Driver.\n";	
		return false;
	}

	if(l_iCount == 0)
	{
		m_rDriverContext.getLogManager() << LogLevel_Error << "The driver was unable to find any valid device on serial port COM1 to COM16.\n";
		return false;
	}

	if(!l_bSelected && l_iCount!=0)
	{
		::gtk_combo_box_set_active(l_pComboBox, 0);
	}

	::GtkToggleButton* l_pToggleESense        = GTK_TOGGLE_BUTTON(gtk_builder_get_object(m_pBuilderConfigureInterface, "check_esense"));
	::GtkToggleButton* l_pTogglePower         = GTK_TOGGLE_BUTTON(gtk_builder_get_object(m_pBuilderConfigureInterface, "check_power"));
	::GtkToggleButton* l_pToggleBlink         = GTK_TOGGLE_BUTTON(gtk_builder_get_object(m_pBuilderConfigureInterface, "check_blink"));
	::GtkToggleButton* l_pToggleBlinkStrenght = GTK_TOGGLE_BUTTON(gtk_builder_get_object(m_pBuilderConfigureInterface, "check_blink_strenght"));

	gtk_toggle_button_set_active(l_pToggleESense,m_rESenseChannels);
	gtk_toggle_button_set_active(l_pTogglePower,m_rBandPowerChannels);
	gtk_toggle_button_set_active(l_pToggleBlink,m_rBlinkStimulations);
	gtk_toggle_button_set_active(l_pToggleBlinkStrenght,m_rBlinkStrenghtChannel);
	

	return true;
}

boolean CConfigurationNeuroskyMindset::postConfigure(void)
{
	if(m_bApplyConfiguration)
	{
		::GtkComboBox* l_pComboBox=GTK_COMBO_BOX(gtk_builder_get_object(m_pBuilderConfigureInterface, "combobox_com_port"));

		int l_iComPort=0;
		const char* l_sUSBIndex=::gtk_combo_box_get_active_text(l_pComboBox);
		if(l_sUSBIndex)
		{
			if(::sscanf(l_sUSBIndex, "COM%i", &l_iComPort)==1)
			{
				m_rComPort=(uint32)l_iComPort;
			}
		}

		::GtkToggleButton* l_pToggleESense        = GTK_TOGGLE_BUTTON(gtk_builder_get_object(m_pBuilderConfigureInterface, "check_esense"));
		::GtkToggleButton* l_pTogglePower         = GTK_TOGGLE_BUTTON(gtk_builder_get_object(m_pBuilderConfigureInterface, "check_power"));
		::GtkToggleButton* l_pToggleBlink         = GTK_TOGGLE_BUTTON(gtk_builder_get_object(m_pBuilderConfigureInterface, "check_blink"));
		::GtkToggleButton* l_pToggleBlinkStrenght = GTK_TOGGLE_BUTTON(gtk_builder_get_object(m_pBuilderConfigureInterface, "check_blink_strenght"));

		m_rESenseChannels = ::gtk_toggle_button_get_active(l_pToggleESense);
		m_rBandPowerChannels = ::gtk_toggle_button_get_active(l_pTogglePower);
		m_rBlinkStimulations = ::gtk_toggle_button_get_active(l_pToggleBlink);
		m_rBlinkStrenghtChannel = ::gtk_toggle_button_get_active(l_pToggleBlinkStrenght);
	}

	if(! CConfigurationBuilder::postConfigure()) // normal header is filled, ressources are realesed
	{
		return false;
	}

	return true;
}

#endif // TARGET_HAS_ThirdPartyThinkGearAPI
