#include "ovasCAcquisitionServer.h"

#include "mindmedia-nexus32b/ovasCDriverMindMediaNeXus32B.h"
#include "brainamp-standard/ovasCDriverBrainampStandard.h"
#include "micromed-intraeeg/ovasCDriverMicromedIntraEEG.h"
#include "ctfvsm-meg/ovasCDriverCtfVsmMeg.h"
#include "openeeg-modulareeg/ovasCDriverOpenEEGModularEEG.h"
#include "generic-oscilator/ovasCDriverGenericOscilator.h"
#include "gtec-gusbamp/ovasCDriverGTecGUSBamp.h"
#include "brainamp-vamp/ovasCDriverBrainProductsVAmp.h"
// #include "neuroscan-synamps2/ovasCDriverNeuroscanSynamps2.h"
#include <openvibe-toolkit/ovtk_all.h>

#include <system/Memory.h>
#include <system/Time.h>

#include <openvibe-toolkit/ovtk_all.h>

#include <fstream>
#include <sstream>

#include <string>
#include <algorithm>
#include <functional>
#include <cctype>
#include <cstring>

#include <cassert>

#define boolean OpenViBE::boolean

namespace
{
	// because std::tolower has multiple signatures,
	// it can not be easily used in std::transform
	// this workaround is taken from http://www.gcek.net/ref/books/sw/cpp/ticppv2/
	template <class charT>
	charT to_lower(charT c)
	{
		return std::tolower(c);
	}
};

#define OVAS_GUI_File            "../share/openvibe-applications/acquisition-server/interface.glade"

// #define _DEBUG_Callbacks_
using namespace OpenViBE;
using namespace OpenViBE::Kernel;

using namespace OpenViBEAcquisitionServer;
using namespace std;

namespace OpenViBEAcquisitionServer
{
	class CDriverContext : public OpenViBEAcquisitionServer::IDriverContext
	{
	public:

		CDriverContext(const OpenViBE::Kernel::IKernelContext& rKernelContext, const OpenViBEAcquisitionServer::CAcquisitionServer& rAcquisitionServer)
			:m_rKernelContext(rKernelContext)
			,m_rAcquisitionServer(rAcquisitionServer)
		{
		}

		virtual ILogManager& getLogManager(void) const
		{
			return m_rKernelContext.getLogManager();
		}

		virtual boolean isConnected(void) const
		{
			return m_rAcquisitionServer.isConnected();
		}

		virtual boolean isStarted(void) const
		{
			return m_rAcquisitionServer.isStarted();
		}

		virtual boolean updateImpedance(const uint32 ui32ChannelIndex, const float64 f64Impedance)
		{
			if(!this->isConnected()) return false;
			if(this->isStarted()) return false;

			if(f64Impedance>=0)
			{
				float64 l_dFraction=(f64Impedance*.001/20);
				if(l_dFraction>1) l_dFraction=1;

				char l_sMessage[1024];
				char l_sLabel[1024];
				char l_sImpedance[1024];
				char l_sStatus[1024];

				if(::strcmp(m_pHeader->getChannelName(ui32ChannelIndex), ""))
				{
					::strcpy(l_sLabel, m_pHeader->getChannelName(ui32ChannelIndex));
				}
				else
				{
					::sprintf(l_sLabel, "Channel %i", ui32ChannelIndex+1);
				}

				if(l_dFraction==1)
				{
					::sprintf(l_sImpedance, "Too high !");
				}
				else
				{
					::sprintf(l_sImpedance, "%.2f kOhm", f64Impedance*.001);
				}

				::sprintf(l_sStatus, "%s", l_dFraction<.25?"Good !":"Bad...");
				::sprintf(l_sMessage, "%s\n%s\n\n%s", l_sLabel, l_sImpedance, l_sStatus);

				::gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(m_vLevelMesure[ui32ChannelIndex]), l_dFraction);
				::gtk_progress_bar_set_text(GTK_PROGRESS_BAR(m_vLevelMesure[ui32ChannelIndex]), l_sMessage);
			}
			else
			{
				::gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(m_vLevelMesure[ui32ChannelIndex]), 0);
				if(f64Impedance==-1)
				{
					::gtk_progress_bar_set_text(GTK_PROGRESS_BAR(m_vLevelMesure[ui32ChannelIndex]), "Measuring...");
				}
				else if (f64Impedance==-2)
				{
					::gtk_progress_bar_set_text(GTK_PROGRESS_BAR(m_vLevelMesure[ui32ChannelIndex]), "n/a");
				}
				else
				{
					::gtk_progress_bar_set_text(GTK_PROGRESS_BAR(m_vLevelMesure[ui32ChannelIndex]), "Unknown");
				}
			}

			::gtk_widget_show_all(m_pImpedanceWindow);

			return true;
		}

		virtual void onInitialize(const IHeader& rHeader)
		{
			m_pHeader=&rHeader;
			::GtkWidget* l_pTable=gtk_table_new(1, rHeader.getChannelCount(), true);

			for(uint32 i=0; i<rHeader.getChannelCount(); i++)
			{
				::GtkWidget* l_pProgressBar=::gtk_progress_bar_new();
				::gtk_progress_bar_set_orientation(GTK_PROGRESS_BAR(l_pProgressBar), GTK_PROGRESS_BOTTOM_TO_TOP);
				::gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(l_pProgressBar), 0);
				::gtk_progress_bar_set_text(GTK_PROGRESS_BAR(l_pProgressBar), "n/a");
				::gtk_table_attach_defaults(GTK_TABLE(l_pTable), l_pProgressBar, i, i+1, 0, 1);
				m_vLevelMesure.push_back(l_pProgressBar);
			}

			m_pImpedanceWindow=gtk_window_new(GTK_WINDOW_TOPLEVEL);
			::gtk_window_set_title(GTK_WINDOW(m_pImpedanceWindow), "Impedance check");
			::gtk_container_add(GTK_CONTAINER(m_pImpedanceWindow), l_pTable);
		}

		virtual void onStart(const IHeader& rHeader)
		{
			m_pHeader=&rHeader;
			::gtk_widget_hide(m_pImpedanceWindow);
		}

		virtual void onStop(const IHeader& rHeader)
		{
			m_pHeader=&rHeader;
		}

		virtual void onUninitialize(const IHeader& rHeader)
		{
			m_pHeader=&rHeader;
			::gtk_widget_destroy(m_pImpedanceWindow);
			m_vLevelMesure.clear();
		}

	protected:

		const IKernelContext& m_rKernelContext;
		const CAcquisitionServer& m_rAcquisitionServer;
		const IHeader* m_pHeader;

		::GtkWidget* m_pImpedanceWindow;
		std::vector < ::GtkWidget* > m_vLevelMesure;
	};
}

/*
	// Monitors socket on reading to detect disconnection
	if(m_rConnection.isReadyToReceive())
	{
		uint8 l_pNullPacket[10224];
		m_rConnection.receiveBuffer(l_pNullPacket, sizeof(l_pNullPacket));
	}

	assert(ui64BufferSize <= 0xffffffff);
	m_rConnection.sendBuffer(pBuffer, (uint32)ui64BufferSize);
*/

//___________________________________________________________________//
//                                                                   //

static int idle_cb(void* pUserData)
{
	static_cast<CAcquisitionServer*>(pUserData)->idleCB();
	return true;
}

static void button_configure_pressed_cb(::GtkButton* pButton, void* pUserData)
{
	static_cast<CAcquisitionServer*>(pUserData)->buttonConfigurePressedCB(pButton);
}

static void button_connect_toggled_cb(::GtkToggleButton* pButton, void* pUserData)
{
	static_cast<CAcquisitionServer*>(pUserData)->buttonConnectToggledCB(pButton);
}

static void button_start_pressed_cb(::GtkButton* pButton, void* pUserData)
{
	static_cast<CAcquisitionServer*>(pUserData)->buttonStartPressedCB(pButton);
}

static void button_stop_pressed_cb(::GtkButton* pButton, void* pUserData)
{
	static_cast<CAcquisitionServer*>(pUserData)->buttonStopPressedCB(pButton);
}

static void combobox_driver_changed_cb(::GtkComboBox* pComboBox, void* pUserData)
{
	static_cast<CAcquisitionServer*>(pUserData)->comboBoxDriverChanged(pComboBox);
}

//___________________________________________________________________//
//                                                                   //

CAcquisitionServer::CAcquisitionServer(const OpenViBE::Kernel::IKernelContext& rKernelContext)
	:m_rKernelContext(rKernelContext)
	,m_pDriverContext(NULL)
	,m_pAcquisitionStreamEncoder(NULL)
	,m_pExperimentInformationStreamEncoder(NULL)
	,m_pSignalStreamEncoder(NULL)
	,m_pStimulationStreamEncoder(NULL)
	,m_pChannelLocalisationStreamEncoder(NULL)
	,m_pGladeInterface(NULL)
	,m_pConnectionServer(NULL)
	,m_bInitialized(false)
	,m_bStarted(false)
	,m_bGotData(false)
	,m_pDriver(NULL)
{
	m_pDriverContext=new CDriverContext(rKernelContext, *this);
	m_pAcquisitionStreamEncoder=&m_rKernelContext.getAlgorithmManager().getAlgorithm(m_rKernelContext.getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_AcquisitionStreamEncoder));
	m_pExperimentInformationStreamEncoder=&m_rKernelContext.getAlgorithmManager().getAlgorithm(m_rKernelContext.getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_ExperimentInformationStreamEncoder));
	m_pSignalStreamEncoder=&m_rKernelContext.getAlgorithmManager().getAlgorithm(m_rKernelContext.getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamEncoder));
	m_pStimulationStreamEncoder=&m_rKernelContext.getAlgorithmManager().getAlgorithm(m_rKernelContext.getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamEncoder));
	// m_pChannelLocalisationStreamEncoder=&m_rKernelContext.getAlgorithmManager().getAlgorithm(m_rKernelContext.getAlgorithmManager().createAlgorithm(/*TODO*/));

	m_pAcquisitionStreamEncoder->initialize();
	m_pExperimentInformationStreamEncoder->initialize();
	m_pSignalStreamEncoder->initialize();
	m_pStimulationStreamEncoder->initialize();
	// m_pChannelLocalisationStreamEncoder->initialize();

	op_pAcquisitionMemoryBuffer.initialize(m_pAcquisitionStreamEncoder->getOutputParameter(OVP_GD_Algorithm_AcquisitionStreamEncoder_OutputParameterId_EncodedMemoryBuffer));
	op_pExperimentInformationMemoryBuffer.initialize(m_pExperimentInformationStreamEncoder->getOutputParameter(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_OutputParameterId_EncodedMemoryBuffer));
	op_pSignalMemoryBuffer.initialize(m_pSignalStreamEncoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamEncoder_OutputParameterId_EncodedMemoryBuffer));
	op_pStimulationMemoryBuffer.initialize(m_pStimulationStreamEncoder->getOutputParameter(OVP_GD_Algorithm_StimulationStreamEncoder_OutputParameterId_EncodedMemoryBuffer));
	// op_pChannelLocalisationMemoryBuffer.initialize(m_pChannelLocalisationStreamEncoder->getOutputParameter(OVP_GD_Algorithm_ChannelLocalisationStreamEncoder_OutputParameterId_EncodedMemoryBuffer));

	TParameterHandler < IMemoryBuffer* > ip_pAcquisitionExperimentInformationMemoryBuffer(m_pAcquisitionStreamEncoder->getInputParameter(OVP_GD_Algorithm_AcquisitionStreamEncoder_InputParameterId_ExperimentInformationStream));
	TParameterHandler < IMemoryBuffer* > ip_pAcquisitionSignalMemoryBuffer(m_pAcquisitionStreamEncoder->getInputParameter(OVP_GD_Algorithm_AcquisitionStreamEncoder_InputParameterId_SignalStream));
	TParameterHandler < IMemoryBuffer* > ip_pAcquisitionStimulationMemoryBuffer(m_pAcquisitionStreamEncoder->getInputParameter(OVP_GD_Algorithm_AcquisitionStreamEncoder_InputParameterId_StimulationStream));
	// TParameterHandler < IMemoryBuffer* > ip_pAcquisitionChannelLocalisationMemoryBuffer(m_pAcquisitionStreamEncoder->getInputParameter(OVP_GD_Algorithm_AcquisitionStreamEncoder_InputParameterId_ChannelLocalisationStream));

	ip_pAcquisitionExperimentInformationMemoryBuffer.setReferenceTarget(op_pExperimentInformationMemoryBuffer);
	ip_pAcquisitionSignalMemoryBuffer.setReferenceTarget(op_pSignalMemoryBuffer);
	ip_pAcquisitionStimulationMemoryBuffer.setReferenceTarget(op_pStimulationMemoryBuffer);
	// ip_pAcquisitionChannelLocalisationMemoryBuffer.setReferenceTarget(op_pStimulationMemoryBuffer);

#if defined OVAS_OS_Windows
	m_vDriver.push_back(new CDriverMindMediaNeXus32B(*m_pDriverContext));
#endif
	m_vDriver.push_back(new CDriverOpenEEGModularEEG(*m_pDriverContext));
#if defined TARGET_HAS_ThirdPartyGUSBampCAPI
		m_vDriver.push_back(new CDriverGTecGUSBamp(*m_pDriverContext));
#endif
#if defined TARGET_HAS_ThirdPartyUSBFirstAmpAPI
		m_vDriver.push_back(new CDriverBrainProductsVAmp(*m_pDriverContext));
#endif
	if(m_rKernelContext.getConfigurationManager().expandAsBoolean("${AcquisitionServer_ShowUnstable}", false))
	{
		m_vDriver.push_back(new CDriverBrainampStandard(*m_pDriverContext));
		m_vDriver.push_back(new CDriverMicromedIntraEEG(*m_pDriverContext));
		m_vDriver.push_back(new CDriverCtfVsmMeg(*m_pDriverContext));
		// m_vDriver.push_back(new CDriverNeuroscanSynamps2(*m_pDriverContext));
	}
	m_vDriver.push_back(new CDriverGenericOscillator(*m_pDriverContext));
}

CAcquisitionServer::~CAcquisitionServer(void)
{
	if(m_pConnectionServer)
	{
		m_pConnectionServer->release();
		m_pConnectionServer=NULL;
	}

	list < pair < Socket::IConnection*, uint64 > >::iterator itConnection=m_vConnection.begin();
	while(itConnection!=m_vConnection.end())
	{
		itConnection->first->release();
		itConnection=m_vConnection.erase(itConnection);
	}

	vector<IDriver*>::iterator itDriver;
	for(itDriver=m_vDriver.begin(); itDriver!=m_vDriver.end(); itDriver++)
	{
		delete (*itDriver);
	}
	m_vDriver.clear();

	// op_pChannelLocalisationMemoryBuffer.uninitialize();
	op_pStimulationMemoryBuffer.uninitialize();
	op_pSignalMemoryBuffer.uninitialize();
	op_pExperimentInformationMemoryBuffer.uninitialize();
	op_pAcquisitionMemoryBuffer.uninitialize();

	// m_pChannelLocalisationStreamEncoder->uninitialize();
	m_pStimulationStreamEncoder->uninitialize();
	m_pSignalStreamEncoder->uninitialize();
	m_pExperimentInformationStreamEncoder->uninitialize();
	m_pAcquisitionStreamEncoder->uninitialize();

	// m_rKernelContext.getAlgorithmManager().releaseAlgorithm(*m_pChannelLocalisationStreamEncoder);
	m_rKernelContext.getAlgorithmManager().releaseAlgorithm(*m_pStimulationStreamEncoder);
	m_rKernelContext.getAlgorithmManager().releaseAlgorithm(*m_pSignalStreamEncoder);
	m_rKernelContext.getAlgorithmManager().releaseAlgorithm(*m_pExperimentInformationStreamEncoder);
	m_rKernelContext.getAlgorithmManager().releaseAlgorithm(*m_pAcquisitionStreamEncoder);

	delete m_pDriverContext;
}

//___________________________________________________________________//
//                                                                   //

boolean CAcquisitionServer::initialize(void)
{
	m_pGladeInterface=glade_xml_new(OVAS_GUI_File, NULL, NULL);

	// Connects custom GTK signals

	g_signal_connect(glade_xml_get_widget(m_pGladeInterface, "button_configure"),     "pressed", G_CALLBACK(button_configure_pressed_cb), this);
	g_signal_connect(glade_xml_get_widget(m_pGladeInterface, "togglebutton_connect"), "toggled", G_CALLBACK(button_connect_toggled_cb),   this);
	g_signal_connect(glade_xml_get_widget(m_pGladeInterface, "button_play"),          "pressed", G_CALLBACK(button_start_pressed_cb),     this);
	g_signal_connect(glade_xml_get_widget(m_pGladeInterface, "button_stop"),          "pressed", G_CALLBACK(button_stop_pressed_cb),      this);
	g_signal_connect(glade_xml_get_widget(m_pGladeInterface, "combobox_driver"),      "changed", G_CALLBACK(combobox_driver_changed_cb),  this);
	glade_xml_signal_autoconnect(m_pGladeInterface);

	::GtkComboBox* l_pComboBoxDriver=GTK_COMBO_BOX(glade_xml_get_widget(m_pGladeInterface, "combobox_driver"));

	enum
	{
		Resource_StringMarkup,
	};

	// Prepares drivers combo box

	gtk_combo_box_set_model(l_pComboBoxDriver, NULL);

	::GtkCellRenderer* l_pCellRendererName=gtk_cell_renderer_text_new();

	gtk_cell_layout_clear(GTK_CELL_LAYOUT(l_pComboBoxDriver));
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(l_pComboBoxDriver), l_pCellRendererName, TRUE);
	gtk_cell_layout_add_attribute(GTK_CELL_LAYOUT(l_pComboBoxDriver), l_pCellRendererName, "markup", Resource_StringMarkup);

	::GtkTreeStore* l_pDriverTreeStore=gtk_tree_store_new(1, G_TYPE_STRING);
	gtk_combo_box_set_model(l_pComboBoxDriver, GTK_TREE_MODEL(l_pDriverTreeStore));

	vector<IDriver*>::size_type i;
	string l_sDefaultDriverName=m_rKernelContext.getConfigurationManager().expand("${AcquisitionServer_DefaultDriver}").toASCIIString();
	transform(l_sDefaultDriverName.begin(), l_sDefaultDriverName.end(), l_sDefaultDriverName.begin(), ::to_lower<string::value_type>);
	for(i=0; i<m_vDriver.size(); i++)
	{
		::GtkTreeIter l_oIter;
		gtk_tree_store_append(l_pDriverTreeStore, &l_oIter, NULL);

		string l_sDriverName=m_vDriver[i]->getName();
		if(m_vDriver[i]->isFlagSet(DriverFlag_IsUnstable))
		{
			gtk_tree_store_set(l_pDriverTreeStore, &l_oIter,
				Resource_StringMarkup, (string("<span foreground=\"#6f6f6f\">")+l_sDriverName+string("</span> <span size=\"smaller\" style=\"italic\">(<span foreground=\"#202060\">unstable</span>)</span>")).c_str(),
				-1);
		}
		else
		{
			gtk_tree_store_set(l_pDriverTreeStore, &l_oIter,
				Resource_StringMarkup, (string("")+l_sDriverName+string("")).c_str(),
				-1);
		}

		transform(l_sDriverName.begin(), l_sDriverName.end(), l_sDriverName.begin(), ::to_lower<string::value_type>);
		if(l_sDefaultDriverName==l_sDriverName)
		{
			gtk_combo_box_set_active(l_pComboBoxDriver, i);
		}
	}
	if(gtk_combo_box_get_active(l_pComboBoxDriver)==-1)
	{
		gtk_combo_box_set_active(l_pComboBoxDriver, 0);
	}

	// Prepares sample count per buffer combo box

	string l_sDefaultSampleCountPerBuffer=m_rKernelContext.getConfigurationManager().expand("${AcquisitionServer_DefaultSampleCountPerBuffer}").toASCIIString();
	::GtkComboBox* l_pComboBoxSampleCountPerBuffer=GTK_COMBO_BOX(glade_xml_get_widget(m_pGladeInterface, "combobox_sample_count_per_sent_block"));
	for(int i=0; ; i++)
	{
		gtk_combo_box_set_active(l_pComboBoxSampleCountPerBuffer, i);
		if(gtk_combo_box_get_active(l_pComboBoxSampleCountPerBuffer)==-1)
		{
			gtk_combo_box_set_active(l_pComboBoxSampleCountPerBuffer, 0);
			break;
		}
		if(l_sDefaultSampleCountPerBuffer==gtk_combo_box_get_active_text(l_pComboBoxSampleCountPerBuffer))
		{
			break;
		}
	}

	// Prepares default connection port

	::GtkSpinButton* l_pSpinButtonConnectionPort=GTK_SPIN_BUTTON(glade_xml_get_widget(m_pGladeInterface, "spinbutton_connection_port"));
	uint64 l_ui64DefaultConnectionPort=m_rKernelContext.getConfigurationManager().expandAsUInteger("${AcquisitionServer_DefaultConnectionPort}", 1024);
	gtk_spin_button_set_value(l_pSpinButtonConnectionPort, (gdouble)l_ui64DefaultConnectionPort);

	// Shows main window

	gtk_widget_show(glade_xml_get_widget(m_pGladeInterface, "openvibe-acquisition-server"));

	return true;
}

//___________________________________________________________________//
//                                                                   //

void CAcquisitionServer::idleCB(void)
{
	// m_rKernelContext.getLogManager() << LogLevel_Debug << "idleCB\n";

	// if(m_bStarted)
	{
		if(!m_pDriver->loop())
		{
			m_rKernelContext.getLogManager() << LogLevel_ImportantWarning << "Something bad happened in the loop callback, stoping the acquisition\n";
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(m_pGladeInterface, "togglebutton_connect")), false);
			return;
		}
	}

	boolean l_bLabelNeedsUpdate=false;

	// Searches for new connection(s)
	if(m_pConnectionServer)
	{
		if(m_pConnectionServer->isReadyToReceive())
		{
			// Accespts new client
			Socket::IConnection* l_pConnection=m_pConnectionServer->accept();
			if(l_pConnection!=NULL)
			{
				m_rKernelContext.getLogManager() << LogLevel_Trace << "Received new connection\n";

				m_vConnection.push_back(pair < Socket::IConnection*, uint64 >(l_pConnection, 0));

				l_bLabelNeedsUpdate=true;

				m_rKernelContext.getLogManager() << LogLevel_Debug << "Creating header\n";

				// op_pChannelLocalisationMemoryBuffer->setSize(0, true);
				op_pStimulationMemoryBuffer->setSize(0, true);
				op_pSignalMemoryBuffer->setSize(0, true);
				op_pExperimentInformationMemoryBuffer->setSize(0, true);
				op_pAcquisitionMemoryBuffer->setSize(0, true);

				// m_pChannelLocalisationStreamEncoder->process(OVP_GD_Algorithm_ChannelLocalisationStreamEncoder_InputTriggerId_EncodeHeader);
				m_pStimulationStreamEncoder->process(OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeHeader);
				m_pSignalStreamEncoder->process(OVP_GD_Algorithm_SignalStreamEncoder_InputTriggerId_EncodeHeader);
				m_pExperimentInformationStreamEncoder->process(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputTriggerId_EncodeHeader);
				m_pAcquisitionStreamEncoder->process(OVP_GD_Algorithm_AcquisitionStreamEncoder_InputTriggerId_EncodeHeader);

				uint64 l_ui64MemoryBufferSize=op_pAcquisitionMemoryBuffer->getSize();
				l_pConnection->sendBufferBlocking(&l_ui64MemoryBufferSize, sizeof(l_ui64MemoryBufferSize));
				l_pConnection->sendBufferBlocking(op_pAcquisitionMemoryBuffer->getDirectPointer(), (uint32)op_pAcquisitionMemoryBuffer->getSize());
			}
		}
	}

	// Sends data to connected client(s)
	// and clean disconnected client(s)
	list < pair < Socket::IConnection*, uint64 > >::iterator itConnection=m_vConnection.begin();
	while(itConnection!=m_vConnection.end())
	{
		Socket::IConnection* l_pConnection=itConnection->first;

		if(!l_pConnection->isConnected())
		{
			l_pConnection->release();
			itConnection=m_vConnection.erase(itConnection);
			l_bLabelNeedsUpdate=true;
		}
		else
		{
			// Sends buffer
			if(m_bStarted && m_bGotData)
			{
				m_rKernelContext.getLogManager() << LogLevel_Debug << "Creating buffer\n";

				// op_pChannelLocalisationMemoryBuffer->setSize(0, true);
				op_pStimulationMemoryBuffer->setSize(0, true);
				op_pSignalMemoryBuffer->setSize(0, true);
				op_pExperimentInformationMemoryBuffer->setSize(0, true);
				op_pAcquisitionMemoryBuffer->setSize(0, true);

				uint64 l_ui64TimeOffset=((itConnection->second<<32)/m_pDriver->getHeader()->getSamplingFrequency());
				TParameterHandler < IStimulationSet* > ip_pStimulationSet(m_pStimulationStreamEncoder->getInputParameter(OVP_GD_Algorithm_StimulationStreamEncoder_InputParameterId_StimulationSet));
				OpenViBEToolkit::Tools::StimulationSet::copy(*ip_pStimulationSet, m_oStimulationSet, l_ui64TimeOffset);

				// m_pChannelLocalisationStreamEncoder->process(OVP_GD_Algorithm_ChannelLocalisationStreamEncoder_InputTriggerId_EncodeBuffer);
				m_pStimulationStreamEncoder->process(OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeBuffer);
				m_pSignalStreamEncoder->process(OVP_GD_Algorithm_SignalStreamEncoder_InputTriggerId_EncodeBuffer);
				m_pExperimentInformationStreamEncoder->process(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputTriggerId_EncodeBuffer);
				m_pAcquisitionStreamEncoder->process(OVP_GD_Algorithm_AcquisitionStreamEncoder_InputTriggerId_EncodeBuffer);

				uint64 l_ui64MemoryBufferSize=op_pAcquisitionMemoryBuffer->getSize();
				l_pConnection->sendBufferBlocking(&l_ui64MemoryBufferSize, sizeof(l_ui64MemoryBufferSize));
				l_pConnection->sendBufferBlocking(op_pAcquisitionMemoryBuffer->getDirectPointer(), (uint32)op_pAcquisitionMemoryBuffer->getSize());

				itConnection->second+=m_ui32SampleCountPerSentBlock;
			}
			itConnection++;
		}
	}

	// Updates 'host count' label when needed
	if(l_bLabelNeedsUpdate)
	{
		char l_sLabel[1024];
		sprintf(l_sLabel, "%u host%s connected...", (unsigned int)m_vConnection.size(), m_vConnection.size()?"s":"");
		gtk_label_set_label(GTK_LABEL(glade_xml_get_widget(m_pGladeInterface, "label_connected_host_count")), l_sLabel);
	}

	if(m_bGotData)
	{
		m_bGotData=false;
	}
	else
	{
		const IHeader& l_rHeader=*m_pDriver->getHeader();
		System::Time::sleep((m_ui32SampleCountPerSentBlock*1000)/(16*l_rHeader.getSamplingFrequency()));
	}
}

//___________________________________________________________________//
//                                                                   //

void CAcquisitionServer::buttonConnectToggledCB(::GtkToggleButton* pButton)
{
	m_rKernelContext.getLogManager() << LogLevel_Debug << "buttonConnectToggledCB\n";

	if(gtk_toggle_button_get_active(pButton))
	{
		m_ui32SampleCountPerSentBlock=atoi(gtk_combo_box_get_active_text(GTK_COMBO_BOX(glade_xml_get_widget(m_pGladeInterface, "combobox_sample_count_per_sent_block"))));
		m_ui64SampleCount=0;
		m_ui32IdleCallbackId=gtk_idle_add(idle_cb, this);

		// Initializes driver
		if(!m_pDriver->initialize(m_ui32SampleCountPerSentBlock, *this))
		{
			gtk_toggle_button_set_active(pButton, false);
			gtk_label_set_label(GTK_LABEL(glade_xml_get_widget(m_pGladeInterface, "label_status")), "Initialization failed !");
			return;
		}
		m_pDriverContext->onInitialize(*m_pDriver->getHeader());

		uint32 l_ui32ConnectionPort=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(glade_xml_get_widget(m_pGladeInterface, "spinbutton_connection_port")));
		m_pConnectionServer=Socket::createConnectionServer();
		if(m_pConnectionServer->listen(l_ui32ConnectionPort))
		{
			gtk_button_set_label(GTK_BUTTON(pButton), "gtk-disconnect");

			gtk_widget_set_sensitive(glade_xml_get_widget(m_pGladeInterface, "button_configure"), false);
			gtk_widget_set_sensitive(glade_xml_get_widget(m_pGladeInterface, "button_play"), true);
			gtk_widget_set_sensitive(glade_xml_get_widget(m_pGladeInterface, "button_stop"), false);

			gtk_widget_set_sensitive(glade_xml_get_widget(m_pGladeInterface, "spinbutton_connection_port"), false);
			gtk_widget_set_sensitive(glade_xml_get_widget(m_pGladeInterface, "combobox_sample_count_per_sent_block"), false);
			gtk_widget_set_sensitive(glade_xml_get_widget(m_pGladeInterface, "combobox_driver"), false);

			gtk_label_set_label(GTK_LABEL(glade_xml_get_widget(m_pGladeInterface, "label_status")), "Connected ! Ready...");
			gtk_label_set_label(GTK_LABEL(glade_xml_get_widget(m_pGladeInterface, "label_connected_host_count")), "0 host connected...");

			m_bGotData=false;
			m_bInitialized=true;

			TParameterHandler < uint64 > ip_ui64BuferDuration(m_pAcquisitionStreamEncoder->getInputParameter(OVP_GD_Algorithm_AcquisitionStreamEncoder_InputParameterId_BufferDuration));

			TParameterHandler < uint64 > ip_ui64ExperimentIdentifier(m_pExperimentInformationStreamEncoder->getInputParameter(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_ExperimentIdentifier));
			TParameterHandler < CString* > ip_pExperimentDate(m_pExperimentInformationStreamEncoder->getInputParameter(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_ExperimentDate));
			TParameterHandler < uint64 > ip_ui64SubjectIdentifier(m_pExperimentInformationStreamEncoder->getInputParameter(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_SubjectIdentifier));
			TParameterHandler < CString* > ip_pSubjectName(m_pExperimentInformationStreamEncoder->getInputParameter(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_SubjectName));
			TParameterHandler < uint64 > ip_ui64SubjectAge(m_pExperimentInformationStreamEncoder->getInputParameter(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_SubjectAge));
			TParameterHandler < uint64 > ip_ui64SubjectGender(m_pExperimentInformationStreamEncoder->getInputParameter(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_SubjectGender));
			TParameterHandler < uint64 > ip_ui64LaboratoryIdentifier(m_pExperimentInformationStreamEncoder->getInputParameter(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_LaboratoryIdentifier));
			TParameterHandler < CString* > ip_pLaboratoryName(m_pExperimentInformationStreamEncoder->getInputParameter(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_LaboratoryName));
			TParameterHandler < uint64 > ip_ui64TechnicianIdentifier(m_pExperimentInformationStreamEncoder->getInputParameter(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_TechnicianIdentifier));
			TParameterHandler < CString* > ip_pTechnicianName(m_pExperimentInformationStreamEncoder->getInputParameter(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_TechnicianName));

			TParameterHandler < IMatrix* > ip_pMatrix(m_pSignalStreamEncoder->getInputParameter(OVP_GD_Algorithm_SignalStreamEncoder_InputParameterId_Matrix));
			TParameterHandler < uint64 > ip_ui64SamplingRate(m_pSignalStreamEncoder->getInputParameter(OVP_GD_Algorithm_SignalStreamEncoder_InputParameterId_SamplingRate));

			TParameterHandler < IStimulationSet* > ip_pStimulationSet(m_pStimulationStreamEncoder->getInputParameter(OVP_GD_Algorithm_StimulationStreamEncoder_InputParameterId_StimulationSet));

			const IHeader& l_rHeader=*m_pDriver->getHeader();

			ip_ui64BuferDuration=(((uint64)m_ui32SampleCountPerSentBlock)<<32)/l_rHeader.getSamplingFrequency();

			ip_ui64ExperimentIdentifier=l_rHeader.getExperimentIdentifier();
			ip_ui64SubjectIdentifier=l_rHeader.getSubjectAge();
			ip_ui64SubjectGender=l_rHeader.getSubjectGender();

			ip_ui64SamplingRate=l_rHeader.getSamplingFrequency();
			ip_pMatrix->setDimensionCount(2);
			ip_pMatrix->setDimensionSize(0, l_rHeader.getChannelCount());
			ip_pMatrix->setDimensionSize(1, m_ui32SampleCountPerSentBlock);
			for(uint32 i=0; i<l_rHeader.getChannelCount(); i++)
			{
				string l_sChannelName=l_rHeader.getChannelName(i);
				if(l_sChannelName!="")
				{
					ip_pMatrix->setDimensionLabel(0, i, l_sChannelName.c_str());
				}
				else
				{
					std::stringstream ss;
					ss << "Channel " << i+1;
					ip_pMatrix->setDimensionLabel(0, i, ss.str().c_str());
				}
			}

			// TODO Gain is ignored
		}
		else
		{
			gtk_toggle_button_set_active(pButton, false);
			gtk_label_set_label(GTK_LABEL(glade_xml_get_widget(m_pGladeInterface, "label_status")), "Connection failed !");
		}
	}
	else
	{
		if(m_bStarted)
		{
			gtk_button_pressed(GTK_BUTTON(glade_xml_get_widget(m_pGladeInterface, "button_stop")));
		}

		if(m_bInitialized)
		{
			m_pDriver->uninitialize();
			m_pDriverContext->onUninitialize(*m_pDriver->getHeader());
		}

		gtk_idle_remove(m_ui32IdleCallbackId);

		list < pair < Socket::IConnection*, uint64 > >::iterator itConnection=m_vConnection.begin();
		while(itConnection!=m_vConnection.end())
		{
			itConnection->first->release();
			itConnection=m_vConnection.erase(itConnection);
		}

		if(m_pConnectionServer)
		{
			m_pConnectionServer->release();
			m_pConnectionServer=NULL;
		}

		gtk_button_set_label(GTK_BUTTON(pButton), "gtk-connect");

		gtk_widget_set_sensitive(glade_xml_get_widget(m_pGladeInterface, "button_configure"), m_pDriver->isConfigurable());
		gtk_widget_set_sensitive(glade_xml_get_widget(m_pGladeInterface, "button_play"), false);
		gtk_widget_set_sensitive(glade_xml_get_widget(m_pGladeInterface, "button_stop"), false);

		gtk_widget_set_sensitive(glade_xml_get_widget(m_pGladeInterface, "spinbutton_connection_port"), true);
		gtk_widget_set_sensitive(glade_xml_get_widget(m_pGladeInterface, "combobox_sample_count_per_sent_block"), true);
		gtk_widget_set_sensitive(glade_xml_get_widget(m_pGladeInterface, "combobox_driver"), true);

		gtk_label_set_label(GTK_LABEL(glade_xml_get_widget(m_pGladeInterface, "label_status")), "");
		gtk_label_set_label(GTK_LABEL(glade_xml_get_widget(m_pGladeInterface, "label_connected_host_count")), "");

		m_bInitialized=false;
	}
}

void CAcquisitionServer::buttonStartPressedCB(::GtkButton* pButton)
{
	m_rKernelContext.getLogManager() << LogLevel_Debug << "buttonStartPressedCB\n";

	// Starts driver
	if(!m_pDriver->start())
	{
		return;
	}
	m_pDriverContext->onStart(*m_pDriver->getHeader());

	gtk_widget_set_sensitive(glade_xml_get_widget(m_pGladeInterface, "button_play"), false);
	gtk_widget_set_sensitive(glade_xml_get_widget(m_pGladeInterface, "button_stop"), true);

	gtk_label_set_label(GTK_LABEL(glade_xml_get_widget(m_pGladeInterface, "label_status")), "Sending...");

	m_bStarted=true;
}

void CAcquisitionServer::buttonStopPressedCB(::GtkButton* pButton)
{
	m_rKernelContext.getLogManager() << LogLevel_Debug << "buttonStopPressedCB\n";

	gtk_widget_set_sensitive(glade_xml_get_widget(m_pGladeInterface, "button_play"), true);
	gtk_widget_set_sensitive(glade_xml_get_widget(m_pGladeInterface, "button_stop"), false);

	gtk_label_set_label(GTK_LABEL(glade_xml_get_widget(m_pGladeInterface, "label_status")), "Connected ! Ready...");

	// Stops driver
	m_pDriver->stop();
	m_pDriverContext->onStop(*m_pDriver->getHeader());

	m_bStarted=false;
}

void CAcquisitionServer::buttonConfigurePressedCB(::GtkButton* pButton)
{
	m_rKernelContext.getLogManager() << LogLevel_Debug << "buttonConfigurePressedCB\n";

	if(m_pDriver->isConfigurable())
	{
		m_pDriver->configure();
	}
}

void CAcquisitionServer::comboBoxDriverChanged(::GtkComboBox* pComboBox)
{
	m_pDriver=m_vDriver[gtk_combo_box_get_active(pComboBox)];
	gtk_widget_set_sensitive(glade_xml_get_widget(m_pGladeInterface, "button_configure"), m_pDriver->isConfigurable());
}

//___________________________________________________________________//
//                                                                   //

void CAcquisitionServer::setSamples(const float32* pSample)
{
	if(m_bStarted)
	{
		if(m_bGotData)
		{
			// some data will be dropped !
			m_rKernelContext.getLogManager() << LogLevel_ImportantWarning << "dropped data\n";
		}
		else
		{
			TParameterHandler < IMatrix* > ip_pMatrix(m_pSignalStreamEncoder->getInputParameter(OVP_GD_Algorithm_SignalStreamEncoder_InputParameterId_Matrix));
			for(uint32 i=0; i<ip_pMatrix->getBufferElementCount(); i++)
			{
				ip_pMatrix->getBuffer()[i]=pSample[i];
			}
			m_ui64SampleCount+=m_ui32SampleCountPerSentBlock;
		}
		m_bGotData=true;
	}
	else
	{
		m_rKernelContext.getLogManager() << LogLevel_Warning << "not started\n";
	}
}

void CAcquisitionServer::setStimulationSet(const IStimulationSet& rStimulationSet)
{
	if(m_bStarted)
	{
		OpenViBEToolkit::Tools::StimulationSet::copy(m_oStimulationSet, rStimulationSet);
	}
	else
	{
		m_rKernelContext.getLogManager() << LogLevel_Warning << "not started\n";
	}
}
