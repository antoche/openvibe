#include "ovsgCDriverSkeletonGenerator.h"

#include <iostream>
#include <sstream>

#include <glib/gstdio.h>
#include <cstdio>

#include <boost/regex.hpp>

using namespace std;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBESkeletonGenerator;

//-----------------------------------------------------------------------
static void button_check_cb(::GtkButton* pButton, void* pUserData)
{
	static_cast<CDriverSkeletonGenerator*>(pUserData)->buttonCheckCB();
}
static void button_tooltip_cb(::GtkButton* pButton, void* pUserData)
{
	static_cast<CDriverSkeletonGenerator*>(pUserData)->buttonTooltipCB(pButton);
}
static void button_ok_cb(::GtkButton* pButton, void* pUserData)
{
	static_cast<CDriverSkeletonGenerator*>(pUserData)->buttonOkCB();
}
static void button_exit_cb(::GtkButton* pButton, void* pUserData)
{
	static_cast<CDriverSkeletonGenerator*>(pUserData)->buttonExitCB();
	::gtk_exit(0);
}

//-----------------------------------------------------------------------
void CDriverSkeletonGenerator::buttonExitCB()
{
	getCommonParameters();
	getCurrentParameters();
	cleanConfigurationFile(m_sConfigurationFile);
	saveCommonParameters(m_sConfigurationFile);
	save(m_sConfigurationFile);

	m_rKernelContext.getLogManager() << LogLevel_Info << "All entries saved in ["<< m_sConfigurationFile<<"]. Exiting.\n";
}

void CDriverSkeletonGenerator::buttonCheckCB()
{
	//Author and Company
	getCommonParameters();
	getCurrentParameters();

	m_rKernelContext.getLogManager() << LogLevel_Info << "Checking values... \n";

	boolean l_bSuccess = true;

	stringstream l_ssTextBuffer;
	l_ssTextBuffer << "----- STATUS -----\n";

	//-------------------------------------------------------------------------------------------------------------------------------------------//
	//::GtkWidget * l_pEntryDriverName = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "entry_driver_name"));
	//m_sDriverName = gtk_entry_get_text(GTK_ENTRY(l_pEntryDriverName));
	const boost::regex l_RegExpDriverName("([a-z]|[A-Z]|[0-9])+([a-z]|[A-Z]|[0-9]|[ \t\r\n]|[\\.-_\\(\\)])*",boost::regex::perl);
	if(boost::regex_match(string(m_sDriverName),l_RegExpDriverName) == false)
	{
		m_rKernelContext.getLogManager() << LogLevel_Warning << "-- Driver Name: INVALID\n";
		l_bSuccess = false;
		l_ssTextBuffer << "[FAILED] Invalid driver name. Please use only characters (lower or uppercase) and numbers (blanck allowed).\n";
	}
	else
	{
		m_rKernelContext.getLogManager() << LogLevel_Info << "-- Driver Name: VALID (" << (const char *)m_sDriverName << ")\n";
		l_ssTextBuffer << "[   OK   ] Valid driver name.\n";
	}
	//-------------------------------------------------------------------------------------------------------------------------------------------//
	//::GtkWidget * l_pEntryClassName = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "entry_class_name"));
	//m_sClassName = gtk_entry_get_text(GTK_ENTRY(l_pEntryClassName));
	const boost::regex l_RegExpClassName("([a-z]|[A-Z]|[0-9])+",boost::regex::perl);
	if(boost::regex_match(string(m_sClassName),l_RegExpClassName) == false)
	{
		m_rKernelContext.getLogManager() << LogLevel_Warning << "-- Class Name: INVALID\n";
		l_bSuccess = false;
		l_ssTextBuffer << "[FAILED] Invalid class name. Please use only characters (lower or uppercase) and numbers  (no blanck allowed).\n";
	}
	else
	{
		m_rKernelContext.getLogManager() << LogLevel_Info << "-- Class Name: VALID (" << (const char *)m_sClassName << ")\n";
		l_ssTextBuffer << "[   OK   ] Valid class name.\n";
	}
	//-------------------------------------------------------------------------------------------------------------------------------------------//
	::GtkWidget * l_pSpinbuttonMinChannel = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "spinbutton_min_channel"));
	//stringstream ss1,ss2;
	//ss1 << (uint32) gtk_spin_button_get_value(GTK_SPIN_BUTTON(l_pSpinbuttonMinChannel));
	//m_sMinChannel = CString(ss1.str().c_str());
	::GtkWidget * l_pSpinbuttonMaxChannel = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "spinbutton_max_channel"));
	//ss2 << (uint32) gtk_spin_button_get_value(GTK_SPIN_BUTTON(l_pSpinbuttonMaxChannel));
	//m_sMaxChannel = CString(ss2.str().c_str());
	if(gtk_spin_button_get_value(GTK_SPIN_BUTTON(l_pSpinbuttonMinChannel)) > gtk_spin_button_get_value(GTK_SPIN_BUTTON(l_pSpinbuttonMaxChannel)))
	{
		m_rKernelContext.getLogManager() << LogLevel_Warning << "-- Channels: INVALID\n";
		l_bSuccess = false;
		l_ssTextBuffer << "[FAILED] Invalid channel count. Be sure that Min <= Max.\n";
	}
	else
	{
		m_rKernelContext.getLogManager() << LogLevel_Info << "-- Channels: VALID (" << (const char *)m_sMinChannel << "/" << (const char *)m_sMaxChannel << ")\n";
		l_ssTextBuffer << "[   OK   ] Valid channel count.\n";
	}
	//-------------------------------------------------------------------------------------------------------------------------------------------//
	/*::GtkWidget * l_pEntrySF = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "entry_sampling_frequencies"));
	CString l_sSamplingFrequencies = gtk_entry_get_text(GTK_ENTRY(l_pEntrySF));
	*/const boost::regex l_RegExpSamplingFrequencies("(([1-9][0-9]*);)*([1-9][0-9]*)",boost::regex::perl);
	if(boost::regex_match(string(m_sSamplingFrequencies),l_RegExpSamplingFrequencies) == false)
	{
		m_rKernelContext.getLogManager() << LogLevel_Warning << "-- Sampling frequencies: INVALID\n";
		l_bSuccess = false;
		l_ssTextBuffer << "[FAILED] Invalid sampling frequencies. Please use only whole numbers separated with ';' (no blanck allowed).\n";
	}
	else
	{
		// Maximum 16 frequencies
		int l_pSF[16];
		uint32 l_ui32SamplingFrequencyCount =  sscanf((const char *)m_sSamplingFrequencies,
			"%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d",
			&l_pSF[0],&l_pSF[1],&l_pSF[2],&l_pSF[3],&l_pSF[4],&l_pSF[5],&l_pSF[6],&l_pSF[7],
			&l_pSF[8],&l_pSF[9],&l_pSF[10],&l_pSF[11],&l_pSF[12],&l_pSF[13],&l_pSF[14],&l_pSF[15]);

		m_rKernelContext.getLogManager() << LogLevel_Info << "-- Sampling frequencies: VALID\n";
		m_vSamplingFrequencies.clear();
		for(unsigned int i = 0;i<l_ui32SamplingFrequencyCount;i++)
		{
			stringstream ss;
			ss << l_pSF[i];
			m_vSamplingFrequencies.push_back( CString(ss.str().c_str()) );
			printf("- %s Hz\n", ss.str().c_str());
		}

		l_ssTextBuffer << "[   OK   ] " << l_ui32SamplingFrequencyCount << " valid sampling frequencie(s).\n";
	}
	//-------------------------------------------------------------------------------------------------------------------------------------------//
	/*::GtkWidget * l_pFileChooser = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "filechooserbutton_target_directory"));
	char * l_pTargetDirectory = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(l_pFileChooser));
	m_sTargetDirectory = CString(l_pTargetDirectory);
	g_free(l_pTargetDirectory);*/

#ifdef OV_OS_Windows
	string space("%20");
	if(((string)m_sTargetDirectory).rfind(space) != string::npos)
	{
		l_ssTextBuffer << "[FAILED] Invalid destination folder :" << (const char *)m_sTargetDirectory << ".\n";
		m_rKernelContext.getLogManager() << LogLevel_Error << "Invalid destination folder :" << (const char *)m_sTargetDirectory << ".\n";
		l_bSuccess = false;
	}
	else
#endif
	{
		m_rKernelContext.getLogManager() << LogLevel_Info << "-- Target directory: " << (const char *)m_sTargetDirectory << "\n";
		l_ssTextBuffer << "[   OK   ] Valid target directory: " << (const char *)m_sTargetDirectory << "\n";
	}
	//-------------------------------------------------------------------------------------------------------------------------------------------//
	::GtkWidget * l_pTooltipTextview = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-driver-tooltips-textview"));
	::GtkTextBuffer * l_pTextBuffer  = gtk_text_view_get_buffer(GTK_TEXT_VIEW(l_pTooltipTextview));
	if(l_bSuccess)
	{
		l_ssTextBuffer << "----- SUCCESS -----\nPress OK to generate the files. If you want to modify your choice(s), please press the \"Check\" button again.";
		::GtkWidget * l_pButtonOk = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-driver-ok-button"));
		gtk_widget_set_sensitive(l_pButtonOk,true);
	}
	else
	{
		l_ssTextBuffer << "----- PROCESS FAILED -----\nModify your choices and press the \"Check\" button again.";
		::GtkWidget * l_pButtonOk = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-driver-ok-button"));
		gtk_widget_set_sensitive(l_pButtonOk,false);
	}

	gtk_text_buffer_set_text (l_pTextBuffer,
		l_ssTextBuffer.str().c_str()
		, -1);
}

void CDriverSkeletonGenerator::buttonOkCB()
{

	m_rKernelContext.getLogManager() << LogLevel_Info << "Generating files... \n";
	CString l_sLogMessages = "Generating files...\n";
	
	boolean l_bSuccess = true;

	CString l_sDate = getDate();

	// we construct the map of substitutions
	map<CString,CString> l_mSubstitutions;
	l_mSubstitutions[CString("@@AuthorName@@")] = m_sAuthor;
	l_mSubstitutions[CString("@@CompanyName@@")] = m_sCompany;
	l_mSubstitutions[CString("@@Date@@")] = l_sDate;
	l_mSubstitutions[CString("@@ClassName@@")] = m_sClassName;
	l_mSubstitutions[CString("@@DriverName@@")] = m_sDriverName;
	l_mSubstitutions[CString("@@MinChannel@@")] = m_sMinChannel;
	l_mSubstitutions[CString("@@MaxChannel@@")] = m_sMaxChannel;
	l_mSubstitutions[CString("@@SamplingFrequency@@")] = m_vSamplingFrequencies[0];
	
	::GtkWidget * l_pTooltipTextview = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-driver-tooltips-textview"));
	::GtkTextBuffer * l_pTextBuffer  = gtk_text_view_get_buffer(GTK_TEXT_VIEW(l_pTooltipTextview));

	
	//-------------------------------------------------------------------------------------------------------------------------------------------//
	// driver.h
	CString l_sDest = m_sTargetDirectory + "/ovasCDriver" + m_sClassName + ".h";
	CString l_sTemplate("../share/openvibe-applications/skeleton-generator/driver.h-skeleton");
	
	if(!this->generate(l_sTemplate,l_sDest,l_mSubstitutions,l_sLogMessages))
	{
		gtk_text_buffer_set_text (l_pTextBuffer,
			l_sLogMessages,
			-1);
		l_bSuccess = false;
	}

	//-------------------------------------------------------------------------------------------------------------------------------------------//
	// driver.cpp
	l_sDest = m_sTargetDirectory + "/ovasCDriver" + m_sClassName + ".cpp";
	l_sTemplate = CString("../share/openvibe-applications/skeleton-generator/driver.cpp-skeleton");
	
	if(!this->generate(l_sTemplate,l_sDest,l_mSubstitutions,l_sLogMessages))
	{
		gtk_text_buffer_set_text (l_pTextBuffer,
			l_sLogMessages,
			-1);
		l_bSuccess = false;
	}
	//-------------------------------------------------------------------------------------------------------------------------------------------//
	// config.h
	l_sDest = m_sTargetDirectory + "/ovasCConfiguration" + m_sClassName + ".h";
	l_sTemplate = CString("../share/openvibe-applications/skeleton-generator/configuration.h-skeleton");
	
	if(!this->generate(l_sTemplate,l_sDest,l_mSubstitutions,l_sLogMessages))
	{
		gtk_text_buffer_set_text (l_pTextBuffer,
			l_sLogMessages,
			-1);
		l_bSuccess = false;
	}
	//-------------------------------------------------------------------------------------------------------------------------------------------//
	// config.cpp
	l_sDest = m_sTargetDirectory + "/ovasCConfiguration" + m_sClassName + ".cpp";
	l_sTemplate = CString("../share/openvibe-applications/skeleton-generator/configuration.cpp-skeleton");
	
	if(!this->generate(l_sTemplate,l_sDest,l_mSubstitutions,l_sLogMessages))
	{
		gtk_text_buffer_set_text (l_pTextBuffer,
			l_sLogMessages,
			-1);
		l_bSuccess = false;
	}
	//-------------------------------------------------------------------------------------------------------------------------------------------//
	// interface.ui
	l_sDest = m_sTargetDirectory + "/interface-" + m_sClassName + ".ui";
	l_sTemplate = CString("../share/openvibe-applications/skeleton-generator/interface.ui-skeleton");
	
	if(!this->generate(l_sTemplate,l_sDest,l_mSubstitutions,l_sLogMessages))
	{
		gtk_text_buffer_set_text (l_pTextBuffer,
			l_sLogMessages,
			-1);
		l_bSuccess = false;
	}
	// the following substitution is done in a .ui file, and not in a cpp file. 
	// The SED primitive immplemented do not cover that case, and some typo problem happen with the character "
	CString l_sCommandSed = "s/@@SamplingFrequencyList@@/";
	for(vector<CString>::iterator it = m_vSamplingFrequencies.begin(); it != m_vSamplingFrequencies.end(); )
	{
		l_sCommandSed = l_sCommandSed + (*it++);
		if(it!=m_vSamplingFrequencies.end()) l_sCommandSed = l_sCommandSed + "<\\/col><\\/row><row><col id=\\\"0\\\" translatable=\\\"yes\\\">";
	}
	l_sCommandSed = l_sCommandSed +  "/g";
	l_bSuccess &= executeSedCommand(l_sDest, l_sCommandSed);

	//-------------------------------------------------------------------------------------------------------------------------------------------//
	// readme-driver.txt
	l_sDest = m_sTargetDirectory + "/README.txt";
	l_sTemplate = CString("../share/openvibe-applications/skeleton-generator/readme-driver.txt-skeleton");
	
	if(!this->generate(l_sTemplate,l_sDest,l_mSubstitutions,l_sLogMessages))
	{
		gtk_text_buffer_set_text (l_pTextBuffer,
			l_sLogMessages,
			-1);
		l_bSuccess = false;
	}
	//-------------------------------------------------------------------------------------------------------------------------------------------//

	if(l_bSuccess)
	{
		l_bSuccess&=cleanConfigurationFile(m_sConfigurationFile);
		l_bSuccess&=saveCommonParameters(m_sConfigurationFile);
		l_bSuccess&=save(m_sConfigurationFile);
	}

	if(!l_bSuccess)
	{
		l_sLogMessages = l_sLogMessages + "Generation process did not completly succeed. Some files may have not been produced.\n";
		m_rKernelContext.getLogManager() << LogLevel_Warning << "Generation process did not completly succeed. Some files may have not been produced.\n";
	}
	else
	{
		l_sLogMessages = l_sLogMessages + "Generation process successful. All information saved in [" + m_sConfigurationFile + "]\n";
		l_sLogMessages = l_sLogMessages + "Please read the file [README.txt] !\n";
		m_rKernelContext.getLogManager() << LogLevel_Info << "Generation process successful. All information saved in [" << m_sConfigurationFile << "]\n";
	}

	// Launch the browser to display the produced files
	CString l_sBrowser = m_rKernelContext.getConfigurationManager().expand("${Designer_WebBrowserCommand_${OperatingSystem}}");
	CString l_sBrowserCmd = l_sBrowser + " \"" +  m_sTargetDirectory+"\"";

#ifdef OV_OS_Windows
	l_sBrowserCmd =  l_sBrowser + " file:///"+  m_sTargetDirectory; //otherwise the browser does not find the directory (problem with / and \ char)
#endif

	if(system((const char *)l_sBrowserCmd))
	{
	}

	gtk_text_buffer_set_text(
		l_pTextBuffer,
		l_sLogMessages,
		-1);
}

void CDriverSkeletonGenerator::buttonTooltipCB(::GtkButton* pButton)
{
	EWidgetName l_eWidgetName=m_vWidgetName[pButton];

	::GtkWidget * l_pTooltipTextview = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-driver-tooltips-textview"));
	::GtkTextBuffer * l_pTextBuffer  = gtk_text_view_get_buffer(GTK_TEXT_VIEW(l_pTooltipTextview));

	if(l_eWidgetName == WidgetName_DriverName)
	{
		gtk_text_buffer_set_text (l_pTextBuffer,
			"Driver Name: \nThis name will be the one displayed in the Acquisition Server selection combobox.\nUsually, the driver is named according to the EEG device (with precisions such as a version number).\n------\nExample: OpenEEG Modular EEG (P2)\n\n\n"
			, -1);
	}
	else if(l_eWidgetName == WidgetName_ClassName)
	{
		gtk_text_buffer_set_text (l_pTextBuffer,
			"Class Name: \nThis name will be used to generate all source and GUI files.\nYou should choose a class name close to the device name (no blank allowed !).\n------\nExample: OpenEEGModularEEG will generate\n - ovasCDriverOpenEEGModularEEG.h/.cpp, the driver skeleton \n - ovasCConfigurationOpenEEGModularEEG.h/.cpp, the configuration class skeleton\n - interface-OpenEEG-ModularEEG.ui, the GUI description file"
			, -1);
	}
	else if(l_eWidgetName == WidgetName_ChannelCount)
	{
		gtk_text_buffer_set_text (l_pTextBuffer,
			"Channel count: \nEnter in the two fields the minimum and maximum number of channels the device is capable of.\nOf course you can still change it later in the source code.\n------\nExample: Min(1) Max(16)\n\n\n"
			, -1);
	}
	else if(l_eWidgetName == WidgetName_SamplingFrequencies)
	{
		gtk_text_buffer_set_text (l_pTextBuffer,
			"Sampling frequencies: \nEnter in the text field the sampling frequencies your device is capable of.\nYou can specify a list of defined frequencies (value separator ';').\n------\nExample:\n\"128;256;512\" for three defined frequencies.\n\n"
			, -1);
	}
	else if(l_eWidgetName == WidgetName_TargetDirectory)
	{
		gtk_text_buffer_set_text (l_pTextBuffer,
			"Target directory: \nEnter the destination directory in which all files will be generated. \nAny existing files will be overwritten.\n------\nExample: ~/skeleton-generator/foobar-driver/\n\n\n"
			, -1);
	}
	else
	{
	}
}

CDriverSkeletonGenerator::CDriverSkeletonGenerator(IKernelContext & rKernelContext, ::GtkBuilder * pBuilderInterface)
	:CSkeletonGenerator(rKernelContext, pBuilderInterface)
{
}
CDriverSkeletonGenerator::~CDriverSkeletonGenerator(void)
{
}

OpenViBE::boolean CDriverSkeletonGenerator::initialize( void )
{
	::GtkWidget * l_pWindowDriver = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-driver-window"));

	// Buttons and signals
	::GtkWidget * l_pButtonCheck  = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-driver-check-button"));
	::GtkWidget * l_pButtonOk     = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-driver-ok-button"));

	g_signal_connect(l_pButtonCheck,"pressed",G_CALLBACK(button_check_cb), this);
	g_signal_connect(l_pButtonOk,"pressed",G_CALLBACK(button_ok_cb), this);

	////target directory
	//::GtkWidget * l_pFileChooser = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "filechooserbutton_target_directory"));
	//CString l_sTargetDirectory = m_rKernelContext.getConfigurationManager().expand("${SkeletonGenerator_TargetDirectory}");
	//if(!gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(l_pFileChooser),(const char *)l_sTargetDirectory))
	//{
	//	gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(l_pFileChooser),"..");
	//}
	

	// Tooltips buttons and signal
	::GtkButton * l_pTooltipButton_driverName          = GTK_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "sg-driver-driver-name-tooltip-button"));
	::GtkButton * l_pTooltipButton_className           = GTK_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "sg-driver-class-name-tooltip-button"));
	::GtkButton * l_pTooltipButton_channelCount        = GTK_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "sg-driver-channel-count-tooltip-button"));
	::GtkButton * l_pTooltipButton_samplingFrequencies = GTK_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "sg-driver-sampling-frequencies-tooltip-button"));
	::GtkButton * l_pTooltipButton_targetDirectory     = GTK_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "sg-driver-target-directory-tooltip-button"));

	m_vWidgetName[l_pTooltipButton_driverName]=WidgetName_DriverName;
	m_vWidgetName[l_pTooltipButton_className]=WidgetName_ClassName;
	m_vWidgetName[l_pTooltipButton_channelCount]=WidgetName_ChannelCount;
	m_vWidgetName[l_pTooltipButton_samplingFrequencies]=WidgetName_SamplingFrequencies;
	m_vWidgetName[l_pTooltipButton_targetDirectory]=WidgetName_TargetDirectory;

	g_signal_connect(l_pTooltipButton_driverName,         "pressed",G_CALLBACK(button_tooltip_cb), this);
	g_signal_connect(l_pTooltipButton_className,          "pressed",G_CALLBACK(button_tooltip_cb), this);
	g_signal_connect(l_pTooltipButton_channelCount,       "pressed",G_CALLBACK(button_tooltip_cb), this);
	g_signal_connect(l_pTooltipButton_samplingFrequencies,"pressed",G_CALLBACK(button_tooltip_cb), this);
	g_signal_connect(l_pTooltipButton_targetDirectory,    "pressed",G_CALLBACK(button_tooltip_cb), this);

	//Close with X and "cancel" button
	g_signal_connect (G_OBJECT(l_pWindowDriver),"delete_event",G_CALLBACK(::gtk_exit),0);
	::GtkWidget * l_pButtonCancel = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-driver-cancel-button"));
	g_signal_connect(l_pButtonCancel,"pressed", G_CALLBACK(button_exit_cb), this);

	//load everything from file
	load(m_sConfigurationFile);

	gtk_widget_show_all(l_pWindowDriver);

	return true;
}

boolean CDriverSkeletonGenerator::save(OpenViBE::CString sFileName)
{
	FILE* l_pFile=::fopen(sFileName.toASCIIString(), "ab");
	if(!l_pFile)
	{
		m_rKernelContext.getLogManager() << LogLevel_Warning << "Saving the driver entries in [" << m_sConfigurationFile << "] failed !\n";
		return false;
	}
	::fprintf(l_pFile, "# ----------------------DRIVER-------------------------\n");
	string::iterator it;
	string l_sTempTargetDirectory(m_sTargetDirectory.toASCIIString());
	for(it=l_sTempTargetDirectory.begin(); it<l_sTempTargetDirectory.end(); it++)
	{
		if((*it)=='\\')
		{
			l_sTempTargetDirectory.replace(it, it+1, 1, '/');
		}
	}

	::fprintf(l_pFile, "SkeletonGenerator_Driver_DriverName = %s\n", m_sDriverName.toASCIIString());
	::fprintf(l_pFile, "SkeletonGenerator_Driver_ClassName = %s\n", m_sClassName.toASCIIString());
	::fprintf(l_pFile, "SkeletonGenerator_Driver_MinChannel = %s\n", m_sMinChannel.toASCIIString());
	::fprintf(l_pFile, "SkeletonGenerator_Driver_MaxChannel = %s\n", m_sMaxChannel.toASCIIString());
	::fprintf(l_pFile, "SkeletonGenerator_Driver_SamplingFrequencies = %s\n", m_sSamplingFrequencies.toASCIIString());
	::fprintf(l_pFile, "SkeletonGenerator_Driver_TargetDirectory = %s\n", l_sTempTargetDirectory.c_str());
	::fprintf(l_pFile, "# -----------------------------------------------------\n");
	::fclose(l_pFile);
	m_rKernelContext.getLogManager() << LogLevel_Info << "Driver entries saved in [" << m_sConfigurationFile << "]\n";

	m_bConfigurationFileLoaded = false;

	return true;
}

boolean CDriverSkeletonGenerator::load(OpenViBE::CString sFileName)
{
	if(!m_bConfigurationFileLoaded && !m_rKernelContext.getConfigurationManager().addConfigurationFromFile(sFileName))
	{
		m_rKernelContext.getLogManager() << LogLevel_Warning << "Driver: Configuration file [" << sFileName << "] could not be loaded. It will be automatically generated after first use.\n";
		return false;
	}

	::GtkWidget * l_pEntryDriverName = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "entry_driver_name"));
	gtk_entry_set_text(GTK_ENTRY(l_pEntryDriverName),m_rKernelContext.getConfigurationManager().expand("${SkeletonGenerator_Driver_DriverName}"));

	::GtkWidget * l_pEntryClassName = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "entry_class_name"));
	gtk_entry_set_text(GTK_ENTRY(l_pEntryClassName),m_rKernelContext.getConfigurationManager().expand("${SkeletonGenerator_Driver_ClassName}"));

	::GtkWidget * l_pSpinbuttonMinChannel = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "spinbutton_min_channel"));
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(l_pSpinbuttonMinChannel),(gdouble)m_rKernelContext.getConfigurationManager().expandAsInteger("${SkeletonGenerator_Driver_MinChannel}"));

	::GtkWidget * l_pSpinbuttonMaxChannel = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "spinbutton_max_channel"));
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(l_pSpinbuttonMaxChannel),(gdouble)m_rKernelContext.getConfigurationManager().expandAsInteger("${SkeletonGenerator_Driver_MaxChannel}"));

	::GtkWidget * l_pEntrySF = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "entry_sampling_frequencies"));
	gtk_entry_set_text(GTK_ENTRY(l_pEntrySF),m_rKernelContext.getConfigurationManager().expand("${SkeletonGenerator_Driver_SamplingFrequencies}"));

	::GtkWidget * l_pFileChooser = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "filechooserbutton_target_directory"));
	CString l_sTargetDirectory;

	// if the user specified a target directory, it has full priority
	l_sTargetDirectory = m_rKernelContext.getConfigurationManager().expand("${SkeletonGenerator_TargetDirectory}");
	boolean l_bNeedFilePrefix = false;
	if((string)l_sTargetDirectory != string(""))
	{
		m_rKernelContext.getLogManager() << LogLevel_Debug << "Target dir user  [" << l_sTargetDirectory << "]\n";
		l_bNeedFilePrefix = true;
	}
	else
	{
		//previous entry
		l_sTargetDirectory = m_rKernelContext.getConfigurationManager().expand("${SkeletonGenerator_Driver_TargetDirectory}");
		if((string)l_sTargetDirectory != string(""))
		{
			m_rKernelContext.getLogManager() << LogLevel_Debug << "Target previous  [" << l_sTargetDirectory << "]\n";
			l_bNeedFilePrefix = true;
		}
		else
		{
			//default path = dist
			m_rKernelContext.getLogManager() << LogLevel_Debug << "Target default  [dist]\n";
#ifdef OV_OS_Linux
			l_sTargetDirectory = CString(gtk_file_chooser_get_current_folder_uri(GTK_FILE_CHOOSER(l_pFileChooser)));
			l_sTargetDirectory = l_sTargetDirectory + "/..";
#elif defined OV_OS_Windows
			l_sTargetDirectory = "..";
#endif
		}
	}
#ifdef OV_OS_Linux
	if(l_bNeedFilePrefix) l_sTargetDirectory = "file://"+l_sTargetDirectory;
	gtk_file_chooser_set_current_folder_uri(GTK_FILE_CHOOSER(l_pFileChooser),(const char *)l_sTargetDirectory);
#elif defined OV_OS_Windows
	gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(l_pFileChooser),(const char *)l_sTargetDirectory);
#endif

	m_rKernelContext.getLogManager() << LogLevel_Info << "Driver entries from [" << sFileName << "] loaded.\n";
	return true;
}

void CDriverSkeletonGenerator::getCurrentParameters(void)
{
	::GtkWidget * l_pEntryDriverName = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "entry_driver_name"));
	m_sDriverName = gtk_entry_get_text(GTK_ENTRY(l_pEntryDriverName));
	::GtkWidget * l_pEntryClassName = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "entry_class_name"));
	m_sClassName = gtk_entry_get_text(GTK_ENTRY(l_pEntryClassName));
	::GtkWidget * l_pSpinbuttonMinChannel = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "spinbutton_min_channel"));
	stringstream ss1,ss2;
	ss1 << (uint32) gtk_spin_button_get_value(GTK_SPIN_BUTTON(l_pSpinbuttonMinChannel));
	m_sMinChannel = CString(ss1.str().c_str());
	::GtkWidget * l_pSpinbuttonMaxChannel = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "spinbutton_max_channel"));
	ss2 << (uint32) gtk_spin_button_get_value(GTK_SPIN_BUTTON(l_pSpinbuttonMaxChannel));
	m_sMaxChannel = CString(ss2.str().c_str());
	::GtkWidget * l_pEntrySF = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "entry_sampling_frequencies"));
	m_sSamplingFrequencies = gtk_entry_get_text(GTK_ENTRY(l_pEntrySF));
	::GtkWidget * l_pFileChooser = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "filechooserbutton_target_directory"));
	char * l_pTargetDirectory = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(l_pFileChooser));
	m_sTargetDirectory = CString(l_pTargetDirectory);
	g_free(l_pTargetDirectory);
}

