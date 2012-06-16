#include "ovsgCBoxAlgorithmSkeletonGenerator.h"


#include <iostream>
#include <sstream>

#include <glib/gstdio.h>
#include <cstdio>

#include <boost/regex.hpp>

#include <ctime>
#include <cmath>

#include <list>

using namespace std;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;
using namespace OpenViBESkeletonGenerator;

boolean CDummyAlgoProto::addInputParameter(const CIdentifier& rInputParameterIdentifier,const CString& sInputName,const EParameterType eParameterType,const CIdentifier& rSubTypeIdentifier)
{
	m_vInputs[sInputName] = eParameterType;
	return true;
}
boolean CDummyAlgoProto::addOutputParameter(const CIdentifier& rOutputParameterIdentifier,const CString& sOutputName,const EParameterType eParameterType,const CIdentifier& rSubTypeIdentifier)
{
	m_vOutputs[sOutputName] = eParameterType;
	return true;
}

boolean CDummyAlgoProto::addInputTrigger(const CIdentifier& rInputTriggerIdentifier,const CString& rInputTriggerName)
{
	m_vInputTriggers.push_back(rInputTriggerName);
	return true;
}

boolean CDummyAlgoProto::addOutputTrigger(const CIdentifier& rOutputTriggerIdentifier,const CString& rOutputTriggerName)
{
	m_vOutputTriggers.push_back(rOutputTriggerName);
	return true;
}

//-----------------------------------------------------------------------
static void button_check_cb(::GtkButton* pButton, void* pUserData)
{
	static_cast<CBoxAlgorithmSkeletonGenerator*>(pUserData)->buttonCheckCB();
}
static void button_tooltip_cb(::GtkButton* pButton, void* pUserData)
{
	static_cast<CBoxAlgorithmSkeletonGenerator*>(pUserData)->buttonTooltipCB(pButton);
}
static void button_ok_cb(::GtkButton* pButton, void* pUserData)
{
	static_cast<CBoxAlgorithmSkeletonGenerator*>(pUserData)->buttonOkCB();
}

static void button_add_input_cb(::GtkButton* pButton, void* pUserData)
{
	static_cast<CBoxAlgorithmSkeletonGenerator*>(pUserData)->buttonAddInputCB();
}
static void button_remove_input_cb(::GtkButton* pButton, void* pUserData)
{
	static_cast<CBoxAlgorithmSkeletonGenerator*>(pUserData)->buttonRemoveInputCB();
}

static void button_add_output_cb(::GtkButton* pButton, void* pUserData)
{
	static_cast<CBoxAlgorithmSkeletonGenerator*>(pUserData)->buttonAddOutputCB();
}
static void button_remove_output_cb(::GtkButton* pButton, void* pUserData)
{
	static_cast<CBoxAlgorithmSkeletonGenerator*>(pUserData)->buttonRemoveOutputCB();
}

static void button_add_setting_cb(::GtkButton* pButton, void* pUserData)
{
	static_cast<CBoxAlgorithmSkeletonGenerator*>(pUserData)->buttonAddSettingCB();
}
static void button_remove_setting_cb(::GtkButton* pButton, void* pUserData)
{
	static_cast<CBoxAlgorithmSkeletonGenerator*>(pUserData)->buttonRemoveSettingCB();
}

static void button_add_algorithm_cb(::GtkButton* pButton, void* pUserData)
{
	static_cast<CBoxAlgorithmSkeletonGenerator*>(pUserData)->buttonAddAlgorithmCB();
}
static void button_remove_algorithm_cb(::GtkButton* pButton, void* pUserData)
{
	static_cast<CBoxAlgorithmSkeletonGenerator*>(pUserData)->buttonRemoveAlgorithmCB();
}
static void algorithm_selected_cb(::GtkComboBox* pCombobox, void* pUserData)
{
	static_cast<CBoxAlgorithmSkeletonGenerator*>(pUserData)->algorithmSelectedCB(gtk_combo_box_get_active(pCombobox));
}

static void button_exit_cb(::GtkButton* pButton, void* pUserData)
{
	static_cast<CBoxAlgorithmSkeletonGenerator*>(pUserData)->buttonExitCB();
	::gtk_exit(0);
}

extern "C" G_MODULE_EXPORT void entry_modified_cb(::GtkWidget * pObject, void* pUserData)
{
	static_cast<CBoxAlgorithmSkeletonGenerator*>(pUserData)->forceRecheckCB();
}
extern "C" G_MODULE_EXPORT void listener_checkbutton_toggled_cb(::GtkWidget * pObject, void* pUserData)
{
	static_cast<CBoxAlgorithmSkeletonGenerator*>(pUserData)->toggleListenerCheckbuttonsStateCB(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pObject)));
}
extern "C" G_MODULE_EXPORT void processing_method_clock_toggled(::GtkWidget * pObject, void* pUserData)
{
	static_cast<CBoxAlgorithmSkeletonGenerator*>(pUserData)->toggleClockFrequencyStateCB(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pObject)));
}
//-----------------------------------------------------------------------
void CBoxAlgorithmSkeletonGenerator::buttonExitCB(void)
{
	getCommonParameters();
	getCurrentParameters();
	cleanConfigurationFile(m_sConfigurationFile);
	saveCommonParameters(m_sConfigurationFile);
	save(m_sConfigurationFile);

	m_rKernelContext.getLogManager() << LogLevel_Info << "All entries saved in ["<< m_sConfigurationFile<<"]. Exiting.\n";
}

void CBoxAlgorithmSkeletonGenerator::forceRecheckCB(void)
{
	::GtkWidget * l_pButtonOk = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-ok-button"));
	if(l_pButtonOk != NULL) gtk_widget_set_sensitive(l_pButtonOk,false);
}

void CBoxAlgorithmSkeletonGenerator::toggleListenerCheckbuttonsStateCB(boolean bNewState)
{
	::GtkWidget * l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-input-added-checkbutton"));
	gtk_widget_set_sensitive(l_pListenerWidget,bNewState);
	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-input-removed-checkbutton"));
	gtk_widget_set_sensitive(l_pListenerWidget,bNewState);
	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-input-type-checkbutton"));
	gtk_widget_set_sensitive(l_pListenerWidget,bNewState);
	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-input-name-checkbutton"));
	gtk_widget_set_sensitive(l_pListenerWidget,bNewState);
	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-input-connected-checkbutton"));
	gtk_widget_set_sensitive(l_pListenerWidget,bNewState);
	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-input-disconnected-checkbutton"));
	gtk_widget_set_sensitive(l_pListenerWidget,bNewState);
	
	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-output-added-checkbutton"));
	gtk_widget_set_sensitive(l_pListenerWidget,bNewState);
	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-output-removed-checkbutton"));
	gtk_widget_set_sensitive(l_pListenerWidget,bNewState);
	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-output-type-checkbutton"));
	gtk_widget_set_sensitive(l_pListenerWidget,bNewState);
	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-output-name-checkbutton"));
	gtk_widget_set_sensitive(l_pListenerWidget,bNewState);
	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-output-connected-checkbutton"));
	gtk_widget_set_sensitive(l_pListenerWidget,bNewState);
	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-output-disconnected-checkbutton"));
	gtk_widget_set_sensitive(l_pListenerWidget,bNewState);

	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-setting-added-checkbutton"));
	gtk_widget_set_sensitive(l_pListenerWidget,bNewState);
	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-setting-removed-checkbutton"));
	gtk_widget_set_sensitive(l_pListenerWidget,bNewState);
	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-setting-type-checkbutton"));
	gtk_widget_set_sensitive(l_pListenerWidget,bNewState);
	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-setting-name-checkbutton"));
	gtk_widget_set_sensitive(l_pListenerWidget,bNewState);
	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-setting-default-checkbutton"));
	gtk_widget_set_sensitive(l_pListenerWidget,bNewState);
	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-setting-value-checkbutton"));
	gtk_widget_set_sensitive(l_pListenerWidget,bNewState);
	
}

void CBoxAlgorithmSkeletonGenerator::toggleClockFrequencyStateCB(boolean bNewState)
{
	::GtkWidget * l_pWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-process-frequency-spinbutton"));
	gtk_widget_set_sensitive(l_pWidget,bNewState);
}
void CBoxAlgorithmSkeletonGenerator::buttonCheckCB(void)
{
	m_rKernelContext.getLogManager() << LogLevel_Info << "Extracting values... \n";
	//Author and Company
	getCommonParameters();
	//Box generator entries
	getCurrentParameters();

	m_rKernelContext.getLogManager() << LogLevel_Info << "Checking values... \n";

	boolean l_bSuccess = true;

	stringstream l_ssTextBuffer;
	l_ssTextBuffer << "----- STATUS -----\n";

	//-------------------------------------------------------------------------------------------------------------------------------------------//
	// Box Description
	//-------------------------------------------------------------------------------------------------------------------------------------------//
	if(string((const char *)m_sName) == "")
	{
		m_rKernelContext.getLogManager() << LogLevel_Warning << "-- box name: INVALID (" << (const char *)m_sName << ")\n";
		l_ssTextBuffer << "[FAILED] No name found. Please provide a name for the box (all characters allowed).\n";
	}
	else
	{
		//m_sName = ensureSedCompliancy(m_sName);
		m_rKernelContext.getLogManager() << LogLevel_Info << "-- box name: VALID (" << (const char *)m_sName << ")\n";
		l_ssTextBuffer << "[   OK   ] Valid box name.\n";
	}

	const boost::regex l_RegExpClassName("([a-z]|[A-Z])+([a-z]|[A-Z]|[0-9]|[_])*",boost::regex::perl);
	if(boost::regex_match(string(m_sClassName),l_RegExpClassName) == false)
	{
		m_rKernelContext.getLogManager() << LogLevel_Warning << "-- class name: INVALID (" << (const char *)m_sClassName << ")\n";
		l_ssTextBuffer << "[FAILED] Class name invalid. Please provide a class name using lower/upper case letters, numbers or underscores.\n";
	}
	else
	{
		m_rKernelContext.getLogManager() << LogLevel_Info << "-- class name: VALID (" << (const char *)m_sClassName << ")\n";
		l_ssTextBuffer << "[   OK   ] Valid class name.\n";
	}

	const boost::regex l_RegExpCategory("([a-z]|[A-Z])+([a-z]|[A-Z]|[ ]|[/])*",boost::regex::perl);
	if(boost::regex_match(string(m_sCategory),l_RegExpCategory) == false)
	{
		m_rKernelContext.getLogManager() << LogLevel_Warning << "-- category: INVALID (" << (const char *)m_sCategory << ")\n";
		l_ssTextBuffer << "[FAILED] Category invalid. Please provide a category using only letters and spaces (for sub-category, use '/' separator).\n";
	}
	else
	{
		m_rKernelContext.getLogManager() << LogLevel_Info << "-- category: VALID (" << (const char *)m_sCategory << ")\n";
		l_ssTextBuffer << "[   OK   ] Valid category.\n";
	}

	const boost::regex l_RegExpboxVersion("([0-9])+([a-z]|[A-Z]|[0-9]|[\\.])*",boost::regex::perl);
	if(boost::regex_match(string(m_sVersion),l_RegExpboxVersion) == false)
	{
		m_rKernelContext.getLogManager() << LogLevel_Warning << "-- box version: INVALID (" << (const char *)m_sVersion << ")\n";
		l_bSuccess = false;
		l_ssTextBuffer << "[FAILED] Invalid box version (" << (const char *)m_sVersion << "). Please use a number followed by either numbers, letters or '.'\n";
	}
	else
	{
		m_rKernelContext.getLogManager() << LogLevel_Info << "-- box version: VALID (" << (const char *)m_sVersion << ")\n";
		l_ssTextBuffer << "[   OK   ] Valid box version.\n";
	}

	{
		//m_sShortDescription = ensureSedCompliancy(m_sShortDescription);
		m_rKernelContext.getLogManager() << LogLevel_Info << "-- short description: VALID (" << (const char *)m_sShortDescription << ")\n";
		l_ssTextBuffer << "[   OK   ] Valid short description.\n";
	}

	if(((string)m_sDetailedDescription).length()<500)
	{
		//m_sDetailedDescription = ensureSedCompliancy(m_sDetailedDescription);
		m_rKernelContext.getLogManager() << LogLevel_Info << "-- detailed description: VALID (" << (const char *)m_sDetailedDescription << ")\n";
		l_ssTextBuffer << "[   OK   ] Valid detailed description.\n";
	}

	//-------------------------------------------------------------------------------------------------------------------------------------------//
	// Box INPUTS OUTPUTS and SETTINGS
	//-------------------------------------------------------------------------------------------------------------------------------------------//

	//checking the inputs...
	if(m_vInputs.size() != 0)
	{
		l_ssTextBuffer << "Checking inputs... \n";
		m_rKernelContext.getLogManager() << LogLevel_Info << "-- checking inputs...\n";
	}
	else
	{
		l_ssTextBuffer << "[----//----] No input specified.\n";
		m_rKernelContext.getLogManager() << LogLevel_Info << "No input specified.\n";
	}
	for(uint32 i = 0; i < m_vInputs.size(); i++)
	{
		if(string((const char *)(m_vInputs[i]._name)) == "" || string((const char *)(m_vInputs[i]._type)) == "")
		{
			m_rKernelContext.getLogManager() << LogLevel_Warning << "  -- Input "<<i<<": [" << (const char *)m_vInputs[i]._name<<"],["<< (const char *)m_vInputs[i]._type << "] INVALID.\n";
			l_ssTextBuffer << ">>[FAILED] Invalid input "<<i<<". Please provide a name and a type for each input.\n";
			l_bSuccess = false;
		}
		else
		{
			m_vInputs[i]._name = ensureSedCompliancy(m_vInputs[i]._name);
			m_rKernelContext.getLogManager() << LogLevel_Info << "  -- Input "<<i<<": [" << (const char *)m_vInputs[i]._name<<"],["<< (const char *)m_vInputs[i]._type << "] VALID.\n";
			l_ssTextBuffer << ">>[   OK   ] Valid input "<<i<<" [" << (const char *)m_vInputs[i]._name<<"]\n";
		}
	}

	//checking the outputs...
	if(m_vOutputs.size() != 0)
	{
		l_ssTextBuffer << "Checking outputs... \n";
		m_rKernelContext.getLogManager() << LogLevel_Info << "-- checking outputs...\n";
	}
	else
	{
		l_ssTextBuffer << "[----//----] No output specified.\n";
		m_rKernelContext.getLogManager() << LogLevel_Info << "No output specified.\n";
	}
	for(uint32 i = 0; i < m_vOutputs.size(); i++)
	{
		if(string((const char *)(m_vOutputs[i]._name)) == "" || string((const char *)(m_vOutputs[i]._type)) == "")
		{
			m_rKernelContext.getLogManager() << LogLevel_Warning << "  -- Output "<<i<<": [" << (const char *)m_vOutputs[i]._name<<"],["<< (const char *)m_vOutputs[i]._type << "] INVALID.\n";
			l_ssTextBuffer << ">>[FAILED] Invalid output "<<i<<". Please provide a name and a type for each output.\n";
			l_bSuccess = false;
		}
		else
		{
			m_vOutputs[i]._name = ensureSedCompliancy(m_vOutputs[i]._name);
			m_rKernelContext.getLogManager() << LogLevel_Info << "  -- Output "<<i<<": [" << (const char *)m_vOutputs[i]._name<<"],["<< (const char *)m_vOutputs[i]._type << "] VALID.\n";
			l_ssTextBuffer << ">>[   OK   ] Valid output "<<i<<" [" << (const char *)m_vOutputs[i]._name<<"]\n";
		}
	}

	//checking the settings...
	if(m_vSettings.size() != 0)
	{
		l_ssTextBuffer << "Checking settings... \n";
		m_rKernelContext.getLogManager() << LogLevel_Info << "-- checking settings...\n";
	}
	else
	{
		l_ssTextBuffer << "[----//----] No setting specified.\n";
		m_rKernelContext.getLogManager() << LogLevel_Info << "No setting specified.\n";
	}
	for(uint32 i = 0; i < m_vSettings.size(); i++)
	{
		if(string((const char *)(m_vSettings[i]._name)) == "" || string((const char *)(m_vSettings[i]._type)) == "")
		{
			m_rKernelContext.getLogManager() << LogLevel_Warning << "  -- Setting "<<i<<": [" << (const char *)m_vSettings[i]._name<<"],["<< (const char *)m_vSettings[i]._type << "] INVALID.\n";
			l_ssTextBuffer << ">>[FAILED] Invalid setting "<<i<<". Please provide a name, a type and a default value for each setting.\n";
			l_bSuccess = false;
		}
		else
		{
			m_vSettings[i]._name = ensureSedCompliancy(m_vSettings[i]._name);
			m_rKernelContext.getLogManager() << LogLevel_Info << "  -- Setting "<<i<<": [" << (const char *)m_vSettings[i]._name<<"],["<< (const char *)m_vSettings[i]._type <<"],["<< (const char *)m_vSettings[i]._defaultValue << "] VALID.\n";
			l_ssTextBuffer << ">>[   OK   ] Valid setting "<<i<<" [" << (const char *)m_vSettings[i]._name<<"]\n";
		}
	}

	//checking the algorithms...
	if(m_vAlgorithms.size() != 0)
	{
		l_ssTextBuffer << "Checking algorithm... \n";
		m_rKernelContext.getLogManager() << LogLevel_Info << "-- checking algorithm...\n";
	}
	else
	{
		l_ssTextBuffer << "[----//----] No algorithm specified.\n";
		m_rKernelContext.getLogManager() << LogLevel_Info << "No algorithm specified.\n";
	}
	for(uint32 i = 0; i < m_vAlgorithms.size(); i++)
	{
		m_rKernelContext.getLogManager() << LogLevel_Info << "  -- Algorithm "<<i<<": [" << (const char *)m_vAlgorithms[i]<<"] VALID.\n";
		l_ssTextBuffer << ">>[   OK   ] Valid algorithm "<<i<<" [" << (const char *)m_vAlgorithms[i]<<"]\n";
	}

	//-------------------------------------------------------------------------------------------------------------------------------------------//
	::GtkWidget * l_pTooltipTextview = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-tooltips-textview"));
	::GtkTextBuffer * l_pTextBuffer  = gtk_text_view_get_buffer(GTK_TEXT_VIEW(l_pTooltipTextview));
	if(l_bSuccess)
	{
		l_ssTextBuffer << "----- SUCCESS -----\nPress 'Generate!' to generate the files. If you want to modify your choice(s), please press the \"Check\" button again.";
		::GtkWidget * l_pButtonOk = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-ok-button"));
		gtk_widget_set_sensitive(l_pButtonOk,true);

	}
	else
	{
		l_ssTextBuffer << "----- PROCESS FAILED -----\nModify your choices and press the \"Check\" button again.";
		::GtkWidget * l_pButtonOk = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-ok-button"));
		gtk_widget_set_sensitive(l_pButtonOk,false);
	}

	gtk_text_buffer_set_text (l_pTextBuffer,l_ssTextBuffer.str().c_str(), -1);

}

void CBoxAlgorithmSkeletonGenerator::buttonOkCB(void)
{
	m_rKernelContext.getLogManager() << LogLevel_Info << "Generating files... \n";
	CString l_sLogMessages = "Generating files...\n";
	::GtkWidget * l_pTooltipTextview = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-tooltips-textview"));
	::GtkTextBuffer * l_pTextBuffer  = gtk_text_view_get_buffer(GTK_TEXT_VIEW(l_pTooltipTextview));
	
	boolean l_bSuccess = true;

	// we ask for a target directory
	::GtkWidget* l_pWidgetDialogOpen=gtk_file_chooser_dialog_new(
			"Select the destination folder",
			NULL,
			GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
			NULL);
	
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
		l_sTargetDirectory = m_rKernelContext.getConfigurationManager().expand("${SkeletonGenerator_Box_TargetDirectory}");
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
			l_sTargetDirectory = CString(gtk_file_chooser_get_current_folder_uri(GTK_FILE_CHOOSER(l_pWidgetDialogOpen)));
			l_sTargetDirectory = l_sTargetDirectory + "/..";
#elif defined OV_OS_Windows
			l_sTargetDirectory = "..";
#endif
		}
	}
#ifdef OV_OS_Linux
	if(l_bNeedFilePrefix) l_sTargetDirectory = "file://"+l_sTargetDirectory;
	gtk_file_chooser_set_current_folder_uri(GTK_FILE_CHOOSER(l_pWidgetDialogOpen),(const char *)l_sTargetDirectory);
#elif defined OV_OS_Windows
	gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(l_pWidgetDialogOpen),(const char *)l_sTargetDirectory);
#endif


	if(gtk_dialog_run(GTK_DIALOG(l_pWidgetDialogOpen))==GTK_RESPONSE_ACCEPT)
	{
		//char* l_sFileName=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(l_pWidgetDialogOpen));
		char * l_pTargetDirectory = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(l_pWidgetDialogOpen));
		m_sTargetDirectory = CString(l_pTargetDirectory);
	}
	else
	{
		m_rKernelContext.getLogManager() << LogLevel_Info << "User cancel. Aborting generation.\n";
		l_sLogMessages = l_sLogMessages + "User cancel. Aborting generation.\n";
		gtk_text_buffer_set_text(l_pTextBuffer,l_sLogMessages,-1);
		gtk_widget_destroy(l_pWidgetDialogOpen);
		return;
	}
	gtk_widget_destroy(l_pWidgetDialogOpen);

	CString l_sDate = getDate();

	// construction of the namespace name from category
	string l_sNamespace(m_sCategory);
	for(uint32 s=0; s<l_sNamespace.length(); s++)
	{
		if(s == 0 && l_sNamespace[s] >= 'a' && l_sNamespace[s]<= 'z')
		{
			l_sNamespace.replace(s,1,1,(char)(l_sNamespace[s]+'A'-'a'));
		}
		boolean l_bErase = false;
		while(s<l_sNamespace.length() && (l_sNamespace[s]==' ' || l_sNamespace[s]=='/'))
		{
			l_sNamespace.erase(s,1);
			l_bErase = true;
		}
		if(l_bErase && s<l_sNamespace.length() && l_sNamespace[s] >= 'a' && l_sNamespace[s]<= 'z')
		{	
			l_sNamespace.replace(s,1,1,(char)(l_sNamespace[s]+'A'-'a'));
		}
	}

	// generating some random identifiers
	CString l_sClassIdentifier = getRandomIdentifierString();
	CString l_sDescriptorIdentifier = getRandomIdentifierString();

	// replace tags in the allgorithm description
	if(m_bUseCodecToolkit)
	{
		for(uint32 i = 0; i < m_vAlgorithms.size(); i++)
		{

			string l_sAlgo = string((const char *)m_mAlgorithmHeaderDeclaration[m_vAlgorithms[i]]);
			size_t it = l_sAlgo.find("@@ClassName@@");
			if(it != string::npos)
			{
				string l_sClass(m_sClassName);
				l_sClass = "CBoxAlgorithm" + l_sClass;
				l_sAlgo.replace(it,13, l_sClass);
				m_mAlgorithmHeaderDeclaration[m_vAlgorithms[i]] = CString(l_sAlgo.c_str());
			}
		}
	}

	// we construct the map of substitutions
	map<CString,CString> l_mSubstitutions;
	l_mSubstitutions[CString("@@Author@@")] = m_sAuthor;
	l_mSubstitutions[CString("@@Date@@")] = l_sDate;
	l_mSubstitutions[CString("@@Company@@")] = m_sCompany;
	l_mSubstitutions[CString("@@Date@@")] = l_sDate;
	l_mSubstitutions[CString("@@BoxName@@")] = m_sName;
	l_mSubstitutions[CString("@@ClassName@@")] = m_sClassName;
	l_mSubstitutions[CString("@@RandomIdentifierClass@@")] = l_sClassIdentifier;
	l_mSubstitutions[CString("@@RandomIdentifierDescriptor@@")] = l_sDescriptorIdentifier;
	l_mSubstitutions[CString("@@ShortDescription@@")] = m_sShortDescription;
	l_mSubstitutions[CString("@@DetailedDescription@@")] = m_sDetailedDescription;
	l_mSubstitutions[CString("@@Category@@")] = m_sCategory;
	l_mSubstitutions[CString("@@Namespace@@")] = CString(l_sNamespace.c_str());
	l_mSubstitutions[CString("@@Version@@")] = m_sVersion;
	l_mSubstitutions[CString("@@StockItemName@@")] = m_sGtkStockItemName;
	l_mSubstitutions[CString("@@InputFlagCanAdd@@")] = (m_bCanAddInputs ? "rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddInput);" : "//rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddInput);");
	l_mSubstitutions[CString("@@InputFlagCanModify@@")] = (m_bCanModifyInputs ? "rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyInput);" : "//rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyInput);");
	l_mSubstitutions[CString("@@OutputFlagCanAdd@@")] = (m_bCanAddOutputs ? "rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddOutput);" : "//rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddOutput);");
	l_mSubstitutions[CString("@@OutputFlagCanModify@@")] = (m_bCanModifyOutputs ? "rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyOutput);" : "//rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyOutput);");
	l_mSubstitutions[CString("@@SettingFlagCanAdd@@")] = (m_bCanAddSettings ? "rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddSetting);" : "//rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddSetting);");
	l_mSubstitutions[CString("@@SettingFlagCanModify@@")] = (m_bCanModifySettings ? "rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifySetting);" : "//rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifySetting);");
	l_mSubstitutions[CString("@@BoxListenerCommentIn@@")] = (m_bUseBoxListener ? "" : "/*");
	l_mSubstitutions[CString("@@BoxListenerCommentOut@@")] = (m_bUseBoxListener ? "" : "*/");
	l_mSubstitutions[CString("@@BoxListenerOnInputConnectedComment@@")] = (m_bBoxListenerOnInputConnected ? "" : "//");
	l_mSubstitutions[CString("@@BoxListenerOnInputDisconnectedComment@@")] = (m_bBoxListenerOnInputDisconnected ? "" : "//");
	l_mSubstitutions[CString("@@BoxListenerOnInputAddedComment@@")] = (m_bBoxListenerOnInputAdded ? "" : "//");
	l_mSubstitutions[CString("@@BoxListenerOnInputRemovedComment@@")] = (m_bBoxListenerOnInputRemoved ? "" : "//");
	l_mSubstitutions[CString("@@BoxListenerOnInputTypeChangedComment@@")] = (m_bBoxListenerOnInputTypeChanged ? "" : "//");
	l_mSubstitutions[CString("@@BoxListenerOnInputNameChangedComment@@")] = (m_bBoxListenerOnInputNameChanged ? "" : "//");
	l_mSubstitutions[CString("@@BoxListenerOnOutputConnectedComment@@")] = (m_bBoxListenerOnOutputConnected ? "" : "//");
	l_mSubstitutions[CString("@@BoxListenerOnOutputDisconnectedComment@@")] = (m_bBoxListenerOnOutputDisconnected ? "" : "//");
	l_mSubstitutions[CString("@@BoxListenerOnOutputAddedComment@@")] = (m_bBoxListenerOnOutputAdded ? "" : "//");
	l_mSubstitutions[CString("@@BoxListenerOnOutputRemovedComment@@")] = (m_bBoxListenerOnOutputRemoved ? "" : "//");
	l_mSubstitutions[CString("@@BoxListenerOnOutputTypeChangedComment@@")] = (m_bBoxListenerOnOutputTypeChanged ? "" : "//");
	l_mSubstitutions[CString("@@BoxListenerOnOutputNameChangedComment@@")] = (m_bBoxListenerOnOutputNameChanged ? "" : "//");
	l_mSubstitutions[CString("@@BoxListenerOnSettingAddedComment@@")] = (m_bBoxListenerOnSettingAdded ? "" : "//");
	l_mSubstitutions[CString("@@BoxListenerOnSettingRemovedComment@@")] = (m_bBoxListenerOnSettingRemoved ? "" : "//");
	l_mSubstitutions[CString("@@BoxListenerOnSettingTypeChangedComment@@")] = (m_bBoxListenerOnSettingTypeChanged ? "" : "//");
	l_mSubstitutions[CString("@@BoxListenerOnSettingNameChangedComment@@")] = (m_bBoxListenerOnSettingNameChanged ? "" : "//");
	l_mSubstitutions[CString("@@BoxListenerOnSettingDefaultValueChangedComment@@")] = (m_bBoxListenerOnSettingDefaultValueChanged ? "" : "//");
	l_mSubstitutions[CString("@@BoxListenerOnSettingValueChangedComment@@")] = (m_bBoxListenerOnSettingValueChanged ? "" : "//");
	l_mSubstitutions[CString("@@ProcessClockComment@@")] = (m_bProcessClock ? "" : "//");
	l_mSubstitutions[CString("@@ProcessInputComment@@")] = (m_bProcessInput ? "" : "//");
	l_mSubstitutions[CString("@@ProcessClockCommentIn@@")] = (m_bProcessClock ? "" : "/*");
	l_mSubstitutions[CString("@@ProcessInputCommentIn@@")] = (m_bProcessInput ? "" : "/*");
	stringstream ss; ss << m_ui32ClockFrequency << "LL<<32";
	l_mSubstitutions[CString("@@ClockFrequency@@")] = ss.str().c_str();
	
	//-------------------------------------------------------------------------------------------------------------------------------------------//
	// box.h
	CString l_sDest = m_sTargetDirectory + "/ovpCBoxAlgorithm" + m_sClassName + ".h";
	CString l_sTemplate("../share/openvibe-applications/skeleton-generator/box.h-skeleton");
	
	if(!this->generate(l_sTemplate,l_sDest,l_mSubstitutions,l_sLogMessages))
	{
		gtk_text_buffer_set_text (l_pTextBuffer,
			l_sLogMessages,
			-1);
		l_bSuccess = false;
	}
	//--------------------------------------------------------------------------------------
	//Inputs
	//--------------------------------------------------------------------------------------
	CString l_sCommandSed = "s/@@Inputs@@/";
	if(m_vInputs.empty()) l_sCommandSed = l_sCommandSed + "\\/\\/No input specified.To add inputs use :\\n\\/\\/rBoxAlgorithmPrototype.addInput(\\\"Input Name\\\",OV_TypeId_XXXX);\\n";
	for(vector<IOSStruct>::iterator it = m_vInputs.begin(); it != m_vInputs.end(); it++)
	{
		if(it != m_vInputs.begin()) 
			l_sCommandSed = l_sCommandSed + "\\t\\t\\t\\t";
		//add the CIdentifier corresponding to type
		//l_sCommandSed = l_sCommandSed + "rBoxAlgorithmPrototype.addInput(\\\""+(*it)._name+"\\\", OpenViBE::Cidentifier"+(*it)._typeId+");\\n";
		//reconstruct the type_id
		string l_sTypeName((const char *)(*it)._type);
		for(uint32 s=0; s<l_sTypeName.length(); s++)
		{
			if(l_sTypeName[s]==' ')
			{
				l_sTypeName.erase(s,1);
				if(l_sTypeName[s] >= 'a' && l_sTypeName[s]<= 'z') l_sTypeName.replace(s,1,1,(char)(l_sTypeName[s]+'A'-'a'));
			}
		}
		l_sCommandSed = l_sCommandSed + "rBoxAlgorithmPrototype.addInput(\\\""+(*it)._name+"\\\",OV_TypeId_"+CString(l_sTypeName.c_str())+");\\n";
	}
	l_sCommandSed = l_sCommandSed +  "/g";
	l_bSuccess &= executeSedCommand(l_sDest, l_sCommandSed);

	//--------------------------------------------------------------------------------------
	//Outputs
	//--------------------------------------------------------------------------------------
	l_sCommandSed = " s/@@Outputs@@/";
	if(m_vOutputs.empty()) l_sCommandSed = l_sCommandSed + "\\/\\/No output specified.To add outputs use :\\n\\/\\/rBoxAlgorithmPrototype.addOutput(\\\"Output Name\\\",OV_TypeId_XXXX);\\n";
	for(vector<IOSStruct>::iterator it = m_vOutputs.begin(); it != m_vOutputs.end(); it++)
	{
		if(it != m_vOutputs.begin()) 
			l_sCommandSed = l_sCommandSed + "\\t\\t\\t\\t";
		//add the CIdentifier corresponding to type
		//l_sCommandSed = l_sCommandSed + "rBoxAlgorithmPrototype.addOutput(\\\""+(*it)._name+"\\\", OpenViBE::Cidentifier"+(*it)._typeId+");\\n";
		//reconstruct the type_id
		string l_sTypeName((const char *)(*it)._type);
		for(uint32 s=0; s<l_sTypeName.length(); s++)
		{
			if(l_sTypeName[s]==' ')
			{
				l_sTypeName.erase(s,1);
				if(l_sTypeName[s] >= 'a' && l_sTypeName[s]<= 'z') l_sTypeName.replace(s,1,1,(char)(l_sTypeName[s]+'A'-'a'));
			}
		}
		l_sCommandSed = l_sCommandSed + "rBoxAlgorithmPrototype.addOutput(\\\""+(*it)._name+"\\\",OV_TypeId_"+CString(l_sTypeName.c_str())+");\\n";
	}
	l_sCommandSed = l_sCommandSed +  "/g";
	l_bSuccess &= executeSedCommand(l_sDest, l_sCommandSed);

	//--------------------------------------------------------------------------------------
	//Settings
	//--------------------------------------------------------------------------------------
	l_sCommandSed = "s/@@Settings@@/";
	if(m_vSettings.empty()) l_sCommandSed = l_sCommandSed + "\\/\\/No setting specified.To add settings use :\\n\\/\\/rBoxAlgorithmPrototype.addSetting(\\\"Setting Name\\\",OV_TypeId_XXXX,\\\"default value\\\");\\n";
	for(vector<IOSStruct>::iterator it = m_vSettings.begin(); it != m_vSettings.end(); it++)
	{
		if(it != m_vSettings.begin()) 
			l_sCommandSed = l_sCommandSed + "\\t\\t\\t\\t";
		//add the CIdentifier corresponding to type
		//l_sCommandSed = l_sCommandSed + "rBoxAlgorithmPrototype.addSetting(\\\""+(*it)._name+"\\\", OpenViBE::Cidentifier"+(*it)._typeId+",\\\""+(*it)._defaultValue+"\\\");\\n";
		//reconstruct the type_id by erasing the spaces and upcasing the following letter
		string l_sTypeName((const char *)(*it)._type);
		for(uint32 s=0; s<l_sTypeName.length(); s++)
		{
			if(l_sTypeName[s]==' ')
			{
				l_sTypeName.erase(s,1);
				if(l_sTypeName[s] >= 'a' && l_sTypeName[s]<= 'z') l_sTypeName.replace(s,1,1,(char)(l_sTypeName[s]+'A'-'a'));
			}
		}
		l_sCommandSed = l_sCommandSed + "rBoxAlgorithmPrototype.addSetting(\\\""+(*it)._name+"\\\",OV_TypeId_"+CString(l_sTypeName.c_str())+",\\\""+(*it)._defaultValue+"\\\");\\n";
	}
	l_sCommandSed = l_sCommandSed +  "/g";
	l_bSuccess &= executeSedCommand(l_sDest, l_sCommandSed);

	//--------------------------------------------------------------------------------------
	//Codecs algorithms
	//--------------------------------------------------------------------------------------
	l_sCommandSed = "s/@@Algorithms@@/";
	if(m_vAlgorithms.size() == 0)
	{
		l_sCommandSed = l_sCommandSed + "\\/\\/ No codec algorithms were specified in the skeleton-generator.\\n";
	}
	else
	{
		l_sCommandSed = l_sCommandSed + "\\/\\/ Codec algorithms specified in the skeleton-generator:\\n";
	}
	for(uint32 a=0; a<m_vAlgorithms.size(); a++)
	{
		/*if(a != 0) 
			l_sCommandSed = l_sCommandSed + "\\t\\t\\t";
*/
		string l_sBlock = string((const char *)m_mAlgorithmHeaderDeclaration[m_vAlgorithms[a]]);
		stringstream ss; ss << "Algo" << a << "_";
		string l_sUniqueMarker = ss.str();
		for(uint32 s=0; s<l_sBlock.length(); s++)
		{
			if(l_sBlock[s]=='@')
			{
				l_sBlock.erase(s,1);
				l_sBlock.insert(s,l_sUniqueMarker);
			}
		}
		l_sCommandSed = l_sCommandSed + CString(l_sBlock.c_str());
	}
		
	l_sCommandSed = l_sCommandSed + "/g";
	l_bSuccess &= executeSedCommand(l_sDest, l_sCommandSed);

	//-------------------------------------------------------------------------------------------------------------------------------------------//
	// box.cpp
	l_sDest = m_sTargetDirectory + "/ovpCBoxAlgorithm" + m_sClassName + ".cpp";
	if(m_bUseCodecToolkit)
	{
		l_sTemplate= "../share/openvibe-applications/skeleton-generator/box.cpp-codec-toolkit-skeleton";
	}
	else
	{
		l_sTemplate= "../share/openvibe-applications/skeleton-generator/box.cpp-skeleton";
	}
	if(!this->generate(l_sTemplate,l_sDest,l_mSubstitutions,l_sLogMessages))
	{
		gtk_text_buffer_set_text (l_pTextBuffer,
			l_sLogMessages,
			-1);
		l_bSuccess = false;
	}

	// Codec Algorithm stuff. too complicated for the simple SED primitives.
	l_sCommandSed = "s/@@AlgorithmInitialisation@@/";
	for(uint32 a=0; a<m_vAlgorithms.size(); a++)
	{
		string l_sBlock = string((const char *)m_mAlgorithmInitialisation[m_vAlgorithms[a]]);
		stringstream ss; ss << "Algo" << a << "_";
		string l_sUniqueMarker = ss.str();
		for(uint32 s=0; s<l_sBlock.length(); s++)
		{
			if(l_sBlock[s]=='@')
			{
				l_sBlock.erase(s,1);
				l_sBlock.insert(s,l_sUniqueMarker);
			}
		}
		l_sCommandSed = l_sCommandSed + CString(l_sBlock.c_str());
	}
	l_sCommandSed = l_sCommandSed + "/g";
	l_bSuccess &= executeSedCommand(l_sDest, l_sCommandSed);
		
	l_sCommandSed = "s/@@AlgorithmInitialisationReferenceTargets@@/";
	if(m_bUseCodecToolkit)
	{
		l_sCommandSed = l_sCommandSed + "\\t\\/\\/ If you need to, you can manually set the reference targets to link the codecs input and output. To do so, you can use :\\n";
		l_sCommandSed = l_sCommandSed + "\\t\\/\\/m_oEncoder.getInputX().setReferenceTarget(m_oDecoder.getOutputX())\\n";
		l_sCommandSed = l_sCommandSed + "\\t\\/\\/ Where 'X' depends on the codec type. Please refer to the Codec Toolkit Reference Page\\n";
		l_sCommandSed = l_sCommandSed + "\\t\\/\\/ (http:\\/\\/openvibe.inria.fr\\/documentation\\/unstable\\/Doc_Tutorial_Developer_SignalProcessing_CodecToolkit_Ref.html) for a complete list.\\n";
	}
	else
	{
		for(uint32 a=0; a<m_vAlgorithms.size(); a++)
		{
			string l_sBlock = string((const char *)m_mAlgorithmInitialisation_ReferenceTargets[m_vAlgorithms[a]]);
			stringstream ss; ss << "Algo" << a << "_";
			string l_sUniqueMarker = ss.str();
			for(uint32 s=0; s<l_sBlock.length(); s++)
			{
				if(l_sBlock[s]=='@')
				{
					l_sBlock.erase(s,1);
					l_sBlock.insert(s,l_sUniqueMarker);
				}
			}
			l_sCommandSed = l_sCommandSed + CString(l_sBlock.c_str());
		}
	}
	l_sCommandSed = l_sCommandSed + "/g";
	l_bSuccess &= executeSedCommand(l_sDest, l_sCommandSed);
		
		
	l_sCommandSed = "s/@@AlgorithmUninitialisation@@/";
	for(uint32 a=0; a<m_vAlgorithms.size(); a++)
	{
		string l_sBlock = string((const char *)m_mAlgorithmUninitialisation[m_vAlgorithms[a]]);
		stringstream ss; ss << "Algo" << a << "_";
		string l_sUniqueMarker = ss.str();
		for(uint32 s=0; s<l_sBlock.length(); s++)
		{
			if(l_sBlock[s]=='@')
			{
				l_sBlock.erase(s,1);
				l_sBlock.insert(s,l_sUniqueMarker);
			}
		}
		l_sCommandSed = l_sCommandSed + CString(l_sBlock.c_str());
	}
	l_sCommandSed = l_sCommandSed + "/g";
	l_bSuccess &= executeSedCommand(l_sDest, l_sCommandSed);

	//-------------------------------------------------------------------------------------------------------------------------------------------//
	// readme-box.cpp
	l_sDest = m_sTargetDirectory + "/README.txt";
	l_sTemplate = "../share/openvibe-applications/skeleton-generator/readme-box.txt-skeleton";
	
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
		//re-load all entries, the internal variables may have been modified to be sed compliant.
		getCommonParameters();
		getCurrentParameters();
		//save the entries as the user typed them
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
		l_sLogMessages = l_sLogMessages + "Generation process successful. All entries saved in [" + m_sConfigurationFile + "]\n";
		l_sLogMessages = l_sLogMessages + "Please read file [README.txt] !\n";
		m_rKernelContext.getLogManager() << LogLevel_Info << "Generation process successful. All entries saved in [" << m_sConfigurationFile << "]\n";
		
		// opening browser to see the produced files
		CString l_sBrowser = m_rKernelContext.getConfigurationManager().expand("${Designer_WebBrowserCommand_${OperatingSystem}}");
		CString l_sBrowserCmd = l_sBrowser + " \"" +  m_sTargetDirectory+"\"";

#ifdef OV_OS_Windows
		l_sBrowserCmd =  l_sBrowser + " file:///"+  m_sTargetDirectory; //otherwise the browser does not find the directory (problem with / and \ char)
#endif
		if(system((const char *)l_sBrowserCmd))
		{
		}
	}

	gtk_text_buffer_set_text(l_pTextBuffer,l_sLogMessages,-1);
}

void CBoxAlgorithmSkeletonGenerator::buttonTooltipCB(::GtkButton* pButton)
{
	CString l_sTooltip = m_vTooltips[pButton];

	::GtkWidget * l_pTooltipTextview = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-tooltips-textview"));
	::GtkTextBuffer * l_pTextBuffer  = gtk_text_view_get_buffer(GTK_TEXT_VIEW(l_pTooltipTextview));
	gtk_text_buffer_set_text (l_pTextBuffer, (const char *) l_sTooltip, -1);
}

void CBoxAlgorithmSkeletonGenerator::buttonAddInputCB(void)
{
	::GtkWidget * l_pDialog = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-IO-add-dialog"));
	::GtkWidget * l_pNameEntry = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-IO-add-name-entry"));
	::GtkWidget * l_pTypeCombobox = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-IO-add-type-combobox"));
		
	gtk_entry_set_text(GTK_ENTRY(l_pNameEntry),"");
		
	gint resp = gtk_dialog_run(GTK_DIALOG(l_pDialog));

	if(resp== GTK_RESPONSE_APPLY)
	{
		const gchar * l_sName = gtk_entry_get_text(GTK_ENTRY(l_pNameEntry));
		//we get the two types (user/ov)
		GtkTreeIter l_iterType;
		GtkTreeModel * l_treeModelType = gtk_combo_box_get_model(GTK_COMBO_BOX(l_pTypeCombobox));
		gtk_combo_box_get_active_iter(GTK_COMBO_BOX(l_pTypeCombobox),&l_iterType);
		gchar* l_dataTypeUser;
		gchar* l_dataTypeOv;
		gtk_tree_model_get(l_treeModelType,&l_iterType,0,&l_dataTypeUser,1,&l_dataTypeOv,-1);
		//const gchar * l_sType = gtk_combo_box_get_active_text(GTK_COMBO_BOX(l_pTypeCombobox));

		::GtkWidget * l_pInputTreeView = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-inputs-treeview"));
		::GtkTreeModel * l_pInputListStore = gtk_tree_view_get_model(GTK_TREE_VIEW(l_pInputTreeView));
		GtkTreeIter l_iter;
		gtk_list_store_append(GTK_LIST_STORE(l_pInputListStore),&l_iter);
		gtk_list_store_set (GTK_LIST_STORE(l_pInputListStore), &l_iter, 0, l_sName,1,l_dataTypeUser,2,l_dataTypeOv,-1);
		gtk_widget_hide(l_pDialog);

		g_free(l_dataTypeUser);g_free(l_dataTypeOv);

		::GtkWidget * l_pButtonOk = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-ok-button"));
		gtk_widget_set_sensitive(l_pButtonOk,false);
	}
	else
	{
		gtk_widget_hide(l_pDialog);
	}

}
void CBoxAlgorithmSkeletonGenerator::buttonRemoveInputCB(void)
{
	::GtkWidget * l_pInputTreeView = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-inputs-treeview"));
	::GtkTreeModel * l_pInputListStore = gtk_tree_view_get_model(GTK_TREE_VIEW(l_pInputTreeView));
	GtkTreeIter l_iter;
	GtkTreeSelection *l_select;
	l_select = gtk_tree_view_get_selection(GTK_TREE_VIEW(l_pInputTreeView));
	if(gtk_tree_selection_get_selected (l_select, &l_pInputListStore, &l_iter))
	{
		gtk_list_store_remove(GTK_LIST_STORE(l_pInputListStore),&l_iter);
		::GtkWidget * l_pButtonOk = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-ok-button"));
		gtk_widget_set_sensitive(l_pButtonOk,false);
	}

}

void CBoxAlgorithmSkeletonGenerator::buttonAddOutputCB(void)
{
	::GtkWidget * l_pDialog = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-IO-add-dialog"));
	::GtkWidget * l_pNameEntry = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-IO-add-name-entry"));
	::GtkWidget * l_pTypeCombobox = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-IO-add-type-combobox"));
		
	gtk_entry_set_text(GTK_ENTRY(l_pNameEntry),"");
		
	gint resp = gtk_dialog_run(GTK_DIALOG(l_pDialog));

	if(resp== GTK_RESPONSE_APPLY)
	{
		const gchar * l_sName = gtk_entry_get_text(GTK_ENTRY(l_pNameEntry));
		//we get the two types (user/ov)
		GtkTreeIter l_iterType;
		GtkTreeModel * l_treeModelType = gtk_combo_box_get_model(GTK_COMBO_BOX(l_pTypeCombobox));
		gtk_combo_box_get_active_iter(GTK_COMBO_BOX(l_pTypeCombobox),&l_iterType);
		gchar* l_dataTypeUser;
		gchar* l_dataTypeOv;
		gtk_tree_model_get(l_treeModelType,&l_iterType,0,&l_dataTypeUser,1,&l_dataTypeOv,-1);
		//const gchar * l_sType = gtk_combo_box_get_active_text(GTK_COMBO_BOX(l_pTypeCombobox));

		::GtkWidget * l_pOutputTreeView = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-outputs-treeview"));
		::GtkTreeModel * l_pOutputListStore = gtk_tree_view_get_model(GTK_TREE_VIEW(l_pOutputTreeView));
		GtkTreeIter l_iter;
	
		gtk_list_store_append(GTK_LIST_STORE(l_pOutputListStore),&l_iter);
		gtk_list_store_set (GTK_LIST_STORE(l_pOutputListStore), &l_iter, 0, l_sName,1,l_dataTypeUser,2,l_dataTypeOv,-1);
		gtk_widget_hide(l_pDialog);

		g_free(l_dataTypeUser);g_free(l_dataTypeOv);

		::GtkWidget * l_pButtonOk = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-ok-button"));
		gtk_widget_set_sensitive(l_pButtonOk,false);
	}
	else
	{
		gtk_widget_hide(l_pDialog);
	}
}
void CBoxAlgorithmSkeletonGenerator::buttonRemoveOutputCB(void)
{
	::GtkWidget * l_pOutputTreeView = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-outputs-treeview"));
	::GtkTreeModel * l_pOutputListStore = gtk_tree_view_get_model(GTK_TREE_VIEW(l_pOutputTreeView));
	GtkTreeIter l_iter;
	GtkTreeSelection *l_select;
	l_select = gtk_tree_view_get_selection(GTK_TREE_VIEW(l_pOutputTreeView));
	if(gtk_tree_selection_get_selected (l_select, &l_pOutputListStore, &l_iter))
	{
		gtk_list_store_remove(GTK_LIST_STORE(l_pOutputListStore),&l_iter);
		::GtkWidget * l_pButtonOk = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-ok-button"));
		gtk_widget_set_sensitive(l_pButtonOk,false);
	}
}

void CBoxAlgorithmSkeletonGenerator::buttonAddSettingCB(void)
{
	::GtkWidget * l_pDialog = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-settings-add-dialog"));
	::GtkWidget * l_pNameEntry = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-settings-add-name-entry"));
	::GtkWidget * l_pTypeCombobox = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-settings-add-type-combobox"));
	::GtkWidget * l_pValueEntry = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-settings-add-default-value-entry"));
	
	gtk_entry_set_text(GTK_ENTRY(l_pNameEntry),"");
	gtk_entry_set_text(GTK_ENTRY(l_pValueEntry),"");
	
	gint resp = gtk_dialog_run(GTK_DIALOG(l_pDialog));
		
	if(resp== GTK_RESPONSE_APPLY)
	{
		const gchar * l_sName = gtk_entry_get_text(GTK_ENTRY(l_pNameEntry));
		const gchar * l_sValue = gtk_entry_get_text(GTK_ENTRY(l_pValueEntry));
		//we get the two types (user/ov)
		GtkTreeIter l_iterType;
		GtkTreeModel * l_treeModelType = gtk_combo_box_get_model(GTK_COMBO_BOX(l_pTypeCombobox));
		gtk_combo_box_get_active_iter(GTK_COMBO_BOX(l_pTypeCombobox),&l_iterType);
		gchar* l_dataTypeUser;
		gchar* l_dataTypeOv;
		gtk_tree_model_get(l_treeModelType,&l_iterType,0,&l_dataTypeUser,1,&l_dataTypeOv,-1);
		//const gchar * l_sType = gtk_combo_box_get_active_text(GTK_COMBO_BOX(l_pTypeCombobox));

		::GtkWidget * l_pOutputTreeView = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-settings-treeview"));
		::GtkTreeModel * l_pOutputListStore = gtk_tree_view_get_model(GTK_TREE_VIEW(l_pOutputTreeView));
		GtkTreeIter l_iter;
	
		gtk_list_store_append(GTK_LIST_STORE(l_pOutputListStore),&l_iter);
		gtk_list_store_set (GTK_LIST_STORE(l_pOutputListStore), &l_iter, 0, l_sName,1,l_dataTypeUser,2,l_sValue,3,l_dataTypeOv,-1);
		gtk_widget_hide(l_pDialog);

		g_free(l_dataTypeUser);g_free(l_dataTypeOv);
		
		::GtkWidget * l_pButtonOk = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-ok-button"));
		gtk_widget_set_sensitive(l_pButtonOk,false);
	}
	else
	{
		gtk_widget_hide(l_pDialog);
	}
}
void CBoxAlgorithmSkeletonGenerator::buttonRemoveSettingCB(void)
{
	::GtkWidget * l_pSettingTreeView = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-settings-treeview"));
	::GtkTreeModel * l_pSettingListStore = gtk_tree_view_get_model(GTK_TREE_VIEW(l_pSettingTreeView));
	GtkTreeIter l_iter;
	GtkTreeSelection *l_select;
	l_select = gtk_tree_view_get_selection(GTK_TREE_VIEW(l_pSettingTreeView));
	if(gtk_tree_selection_get_selected (l_select, &l_pSettingListStore, &l_iter))
	{
		gtk_list_store_remove(GTK_LIST_STORE(l_pSettingListStore),&l_iter);
		::GtkWidget * l_pButtonOk = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-ok-button"));
		gtk_widget_set_sensitive(l_pButtonOk,false);
	}
}

void CBoxAlgorithmSkeletonGenerator::buttonAddAlgorithmCB(void)
{
	::GtkWidget * l_pDialog = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-algorithms-add-dialog"));
	::GtkWidget * l_pAlgoCombobox = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-algorithms-add-combobox"));
	
	::GtkWidget * l_pAlgoInputsTreeView = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-algorithms-add-inputs-treeview"));
	::GtkTreeModel * l_pAlgoInputsListStore = gtk_tree_view_get_model(GTK_TREE_VIEW(l_pAlgoInputsTreeView));
	::GtkWidget * l_pAlgoOutputsTreeView = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-algorithms-add-outputs-treeview"));
	::GtkTreeModel * l_pAlgoOutputsListStore = gtk_tree_view_get_model(GTK_TREE_VIEW(l_pAlgoOutputsTreeView));
	::GtkWidget * l_pAlgoInputTriggersTreeView = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-algorithms-add-input-triggers-treeview"));
	::GtkTreeModel * l_pAlgoInputTriggersListStore = gtk_tree_view_get_model(GTK_TREE_VIEW(l_pAlgoInputTriggersTreeView));
	::GtkWidget * l_pAlgoOutputTriggersTreeView = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-algorithms-add-output-triggers-treeview"));
	::GtkTreeModel * l_pAlgoOutputTriggersListStore = gtk_tree_view_get_model(GTK_TREE_VIEW(l_pAlgoOutputTriggersTreeView));
	
	::GtkWidget * l_pAlgoCategoryEntry = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-algorithms-add-category-entry"));
	gtk_entry_set_text(GTK_ENTRY(l_pAlgoCategoryEntry),"");
	::GtkWidget * l_pAlgoShortEntry = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-algorithms-add-short-description-entry"));
	gtk_entry_set_text(GTK_ENTRY(l_pAlgoShortEntry),"");
	::GtkWidget * l_pAlgoDetailedTextview = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-algorithms-add-detailed-description-textview"));
	::GtkTextBuffer * l_pTextBuffer  = gtk_text_view_get_buffer(GTK_TEXT_VIEW(l_pAlgoDetailedTextview));
	gtk_text_buffer_set_text(l_pTextBuffer,"",-1);


	gtk_list_store_clear(GTK_LIST_STORE(l_pAlgoInputsListStore));
	gtk_list_store_clear(GTK_LIST_STORE(l_pAlgoOutputsListStore));
	gtk_list_store_clear(GTK_LIST_STORE(l_pAlgoInputTriggersListStore));
	gtk_list_store_clear(GTK_LIST_STORE(l_pAlgoOutputTriggersListStore));
	gtk_combo_box_set_active(GTK_COMBO_BOX(l_pAlgoCombobox),-1);

	gint resp = gtk_dialog_run(GTK_DIALOG(l_pDialog));

	if(resp== GTK_RESPONSE_APPLY)
	{
		const gchar * l_sAlgo = gtk_combo_box_get_active_text(GTK_COMBO_BOX(l_pAlgoCombobox));

		::GtkWidget * l_pAlgoTreeView = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-algorithms-treeview"));
		::GtkTreeModel * l_pAlgoListStore = gtk_tree_view_get_model(GTK_TREE_VIEW(l_pAlgoTreeView));
		GtkTreeIter l_iter;
		gtk_list_store_append(GTK_LIST_STORE(l_pAlgoListStore),&l_iter);
		gtk_list_store_set (GTK_LIST_STORE(l_pAlgoListStore), &l_iter, 0, l_sAlgo,-1);
		gtk_widget_hide(l_pDialog);
		::GtkWidget * l_pButtonOk = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-ok-button"));
		gtk_widget_set_sensitive(l_pButtonOk,false);
	}
	else
	{
		gtk_widget_hide(l_pDialog);
	}

}
void CBoxAlgorithmSkeletonGenerator::buttonRemoveAlgorithmCB(void)
{
	::GtkWidget * l_pAlgoTreeView = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-algorithms-treeview"));
	::GtkTreeModel * l_pAlgoListStore = gtk_tree_view_get_model(GTK_TREE_VIEW(l_pAlgoTreeView));
	GtkTreeIter l_iter;
	GtkTreeSelection *l_select;
	l_select = gtk_tree_view_get_selection(GTK_TREE_VIEW(l_pAlgoTreeView));
	if(gtk_tree_selection_get_selected (l_select, &l_pAlgoListStore, &l_iter))
	{
		gtk_list_store_remove(GTK_LIST_STORE(l_pAlgoListStore),&l_iter);
		::GtkWidget * l_pButtonOk = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-ok-button"));
		gtk_widget_set_sensitive(l_pButtonOk,false);
	}
}
void CBoxAlgorithmSkeletonGenerator::algorithmSelectedCB(int32 i32IndexSelected)
{
	::GtkWidget * l_pAlgoInputsTreeView = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-algorithms-add-inputs-treeview"));
	::GtkTreeModel * l_pAlgoInputsListStore = gtk_tree_view_get_model(GTK_TREE_VIEW(l_pAlgoInputsTreeView));
	::GtkWidget * l_pAlgoOutputsTreeView = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-algorithms-add-outputs-treeview"));
	::GtkTreeModel * l_pAlgoOutputsListStore = gtk_tree_view_get_model(GTK_TREE_VIEW(l_pAlgoOutputsTreeView));
	::GtkWidget * l_pAlgoInputTriggersTreeView = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-algorithms-add-input-triggers-treeview"));
	::GtkTreeModel * l_pAlgoInputTriggersListStore = gtk_tree_view_get_model(GTK_TREE_VIEW(l_pAlgoInputTriggersTreeView));
	::GtkWidget * l_pAlgoOutputTriggersTreeView = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-algorithms-add-output-triggers-treeview"));
	::GtkTreeModel * l_pAlgoOutputTriggersListStore = gtk_tree_view_get_model(GTK_TREE_VIEW(l_pAlgoOutputTriggersTreeView));
	
	gtk_list_store_clear(GTK_LIST_STORE(l_pAlgoInputsListStore));
	gtk_list_store_clear(GTK_LIST_STORE(l_pAlgoOutputsListStore));
	gtk_list_store_clear(GTK_LIST_STORE(l_pAlgoInputTriggersListStore));
	gtk_list_store_clear(GTK_LIST_STORE(l_pAlgoOutputTriggersListStore));

	::GtkWidget * l_pAlgoCategoryEntry = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-algorithms-add-category-entry"));
	gtk_entry_set_text(GTK_ENTRY(l_pAlgoCategoryEntry),"");
	::GtkWidget * l_pAlgoShortEntry = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-algorithms-add-short-description-entry"));
	gtk_entry_set_text(GTK_ENTRY(l_pAlgoShortEntry),"");
	::GtkWidget * l_pAlgoDetailedTextview = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-algorithms-add-detailed-description-textview"));
	::GtkTextBuffer * l_pTextBuffer  = gtk_text_view_get_buffer(GTK_TEXT_VIEW(l_pAlgoDetailedTextview));
	gtk_text_buffer_set_text(l_pTextBuffer,"",-1);

	if(i32IndexSelected != -1)
	{
		::GtkWidget * l_pAlgoCombobox = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-algorithms-add-combobox"));
		::GtkTreeModel * l_pAlgoListStore = gtk_combo_box_get_model(GTK_COMBO_BOX(l_pAlgoCombobox));
		::GtkTreeIter l_iter;
		gchar *l_sIdentifier;
		gtk_tree_model_iter_nth_child(l_pAlgoListStore,&l_iter,NULL,i32IndexSelected);
		gtk_tree_model_get(l_pAlgoListStore,&l_iter,1,&l_sIdentifier,-1);
		CIdentifier l_identifier;
		l_identifier.fromString(CString(l_sIdentifier));
		
		//we need to create a dummy instance of the algorithm proto to know its input/output/triggers
		const IPluginObjectDesc * l_pDesc = m_rKernelContext.getPluginManager().getPluginObjectDesc(l_identifier);
		CDummyAlgoProto l_oDummyProto;
		((IAlgorithmDesc *)l_pDesc)->getAlgorithmPrototype(l_oDummyProto);

		//inputs of the algorithm
		::GtkWidget * l_pAlgoInputsTreeView = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-algorithms-add-inputs-treeview"));
		::GtkTreeModel * l_pAlgoInputsListStore = gtk_tree_view_get_model(GTK_TREE_VIEW(l_pAlgoInputsTreeView));
		for(map<CString,EParameterType>::iterator i = l_oDummyProto.m_vInputs.begin(); i!=l_oDummyProto.m_vInputs.end(); i++)
		{
			GtkTreeIter l_iter;
			gtk_list_store_append(GTK_LIST_STORE(l_pAlgoInputsListStore),&l_iter);
			gtk_list_store_set(GTK_LIST_STORE(l_pAlgoInputsListStore), &l_iter, 0, (const char *)(*i).first,-1);
		}

		//outputs of the algorithm
		::GtkWidget * l_pAlgoOutputsTreeView = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-algorithms-add-outputs-treeview"));
		::GtkTreeModel * l_pAlgoOutputsListStore = gtk_tree_view_get_model(GTK_TREE_VIEW(l_pAlgoOutputsTreeView));
		for(map<CString,EParameterType>::iterator i = l_oDummyProto.m_vOutputs.begin(); i!=l_oDummyProto.m_vOutputs.end(); i++)
		{
			GtkTreeIter l_iter;
			gtk_list_store_append(GTK_LIST_STORE(l_pAlgoOutputsListStore),&l_iter);
			gtk_list_store_set(GTK_LIST_STORE(l_pAlgoOutputsListStore), &l_iter, 0, (const char *)(*i).first,-1);
		}

		//Input triggers of the algorithm
		::GtkWidget * l_pAlgoInputTriggersTreeView = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-algorithms-add-input-triggers-treeview"));
		::GtkTreeModel * l_pAlgoInputTriggersListStore = gtk_tree_view_get_model(GTK_TREE_VIEW(l_pAlgoInputTriggersTreeView));
		for(uint32 i = 0; i<l_oDummyProto.m_vInputTriggers.size(); i++)
		{
			GtkTreeIter l_iter;
			gtk_list_store_append(GTK_LIST_STORE(l_pAlgoInputTriggersListStore),&l_iter);
			gtk_list_store_set(GTK_LIST_STORE(l_pAlgoInputTriggersListStore), &l_iter, 0, (const char *)l_oDummyProto.m_vInputTriggers[i],-1);
		}
		//Output triggers of the algorithm
		::GtkWidget * l_pAlgoOutputTriggersTreeView = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-algorithms-add-output-triggers-treeview"));
		::GtkTreeModel * l_pAlgoOutputTriggersListStore = gtk_tree_view_get_model(GTK_TREE_VIEW(l_pAlgoOutputTriggersTreeView));
		for(uint32 i = 0; i<l_oDummyProto.m_vOutputTriggers.size(); i++)
		{
			GtkTreeIter l_iter;
			gtk_list_store_append(GTK_LIST_STORE(l_pAlgoOutputTriggersListStore),&l_iter);
			gtk_list_store_set(GTK_LIST_STORE(l_pAlgoOutputTriggersListStore), &l_iter, 0, (const char *)l_oDummyProto.m_vOutputTriggers[i],-1);
		}

		::GtkWidget * l_pAlgoCategoryEntry = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-algorithms-add-category-entry"));
		gtk_entry_set_text(GTK_ENTRY(l_pAlgoCategoryEntry),(const char*)l_pDesc->getCategory());
		::GtkWidget * l_pAlgoShortEntry = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-algorithms-add-short-description-entry"));
		gtk_entry_set_text(GTK_ENTRY(l_pAlgoShortEntry),(const char*)l_pDesc->getShortDescription());
		::GtkWidget * l_pAlgoDetailedTextview = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-algorithms-add-detailed-description-textview"));
		::GtkTextBuffer * l_pTextBuffer  = gtk_text_view_get_buffer(GTK_TEXT_VIEW(l_pAlgoDetailedTextview));
		gtk_text_buffer_set_text(l_pTextBuffer,(const char*)l_pDesc->getDetailedDescription(),-1);

	}
}
//--------------------------------------------------------------------------
CBoxAlgorithmSkeletonGenerator::CBoxAlgorithmSkeletonGenerator(IKernelContext & rKernelContext, ::GtkBuilder * pBuilderInterface)
	:CSkeletonGenerator(rKernelContext, pBuilderInterface)
{
}
CBoxAlgorithmSkeletonGenerator::~CBoxAlgorithmSkeletonGenerator(void)
{
}

OpenViBE::boolean CBoxAlgorithmSkeletonGenerator::initialize( void )
{
	::GtkWidget * l_pBox = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-window"));
	
	// Main Buttons and signals
	::GtkWidget * l_pButtonCheck  = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-check-button"));
	::GtkWidget * l_pButtonOk     = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-ok-button"));
	gtk_widget_set_sensitive(l_pButtonOk,false);

	g_signal_connect(l_pButtonCheck, "pressed",G_CALLBACK(button_check_cb), this);
	g_signal_connect(l_pButtonOk,    "pressed",G_CALLBACK(button_ok_cb), this);

	//connect all the signals in the .ui file (entry_modified_cb)
	gtk_builder_connect_signals(m_pBuilderInterface, this);

	// Tooltips buttons and signal
	::GtkButton * l_pTooltipButton_nameVersion         = GTK_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "sg-box-name-version-tooltip-button"));
	::GtkButton * l_pTooltipButton_category            = GTK_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "sg-box-category-tooltip-button"));
	::GtkButton * l_pTooltipButton_description         = GTK_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "sg-box-description-tooltip-button"));
	::GtkButton * l_pTooltipButton_icon                = GTK_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "sg-box-icon-tooltip-button"));
	::GtkButton * l_pTooltipButton_inputs              = GTK_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "sg-box-inputs-list-tooltip-button"));
	::GtkButton * l_pTooltipButton_inputs_modify       = GTK_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "sg-box-inputs-modify-tooltip-button"));
	::GtkButton * l_pTooltipButton_inputs_addRemove    = GTK_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "sg-box-inputs-add-tooltip-button"));
	::GtkButton * l_pTooltipButton_outputs             = GTK_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "sg-box-outputs-list-tooltip-button"));
	::GtkButton * l_pTooltipButton_outputs_modify      = GTK_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "sg-box-outputs-modify-tooltip-button"));
	::GtkButton * l_pTooltipButton_outputs_addRemove   = GTK_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "sg-box-outputs-add-tooltip-button"));
	::GtkButton * l_pTooltipButton_settings            = GTK_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "sg-box-settings-list-tooltip-button"));
	::GtkButton * l_pTooltipButton_settings_modify     = GTK_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "sg-box-settings-modify-tooltip-button"));
	::GtkButton * l_pTooltipButton_settings_addRemove  = GTK_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "sg-box-settings-add-tooltip-button"));
	::GtkButton * l_pTooltipButton_algorithms          = GTK_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "sg-box-algorithms-tooltip-button"));
	::GtkButton * l_pTooltipButton_className           = GTK_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "sg-box-class-name-tooltip-button"));
	::GtkButton * l_pTooltipButton_UseCodecToolkit     = GTK_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "sg-box-algorithms-toolkit-tooltip-button"));
	::GtkButton * l_pTooltipButton_BoxListener         = GTK_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-tooltip-button"));

	m_vTooltips[l_pTooltipButton_nameVersion]        = CString("Box Name: \nThis name will be the one displayed in the Designer.\nUsually, the box name reflects its main purpose.\nIf your box relies on GPL licensed libraries such as IT++, please precise it in the name.\nPlease also enter a version number for your box.\nAuthorized characters: letters (lower and upper case), numbers, special characters '()[]._-'\n------\nExample: Clock Stimulator (tic tac), version 1.2");
	m_vTooltips[l_pTooltipButton_category]           = CString("Category: \nThe category decides where the box will be strored in designer's box panel.\nYou can refer to an existing category, already used in the designer, or choose a new one.\nIf you need to specifiy a subcategory, use the character '\\'.\nAuthorized characters: letters (lower and upper case) and spaces.\n------\nExample: Samples\\Seleton Generator\n");
	m_vTooltips[l_pTooltipButton_description]        = CString("Description: \nThe short description will be displayed next to the box in the designer box panel.\nThe detailed description is showed on the 'About Box...' panel.\nAll characters are authorized.\n------\nExample:\nShort Description : Periodic stimulation generator\nDetailed description : This box triggers stimulation at fixed frequency.");
	m_vTooltips[l_pTooltipButton_icon]               = CString("Box Icon: \nThe icon used in the designer box panel for this box.\nThis is an optionnal field.\n------\nExample: 'gtk-help' will be the corresponding gtk stock item (depending on the gtk theme used)\n\n\n");
	m_vTooltips[l_pTooltipButton_inputs]             = CString("Inputs: \nUse the 'Add' and 'Remove' buttons to set all the inputs your box will have.\nWhen pressing 'Add' a dialog window will appear to know the name and type of the new input.\n------\nExample:\n'Incoming Signal' of type 'Signal'\n\n");
	m_vTooltips[l_pTooltipButton_inputs_modify]      = CString("Modify: \nCheck this option if the input(s) of your box can be modified (type and name) in the Designer by right-clicking the box.\nIn the implementation, this option decides whether or not the box will have the flag 'BoxFlag_CanModifyInput'.\n\n\n\n\n");
	m_vTooltips[l_pTooltipButton_inputs_addRemove]   = CString("Add/Remove: \nCheck this option if the user must be able to add (or remove) inputs, by right-clicking the box.\nIn the implementation, this option decides whether or not the box will have the flag 'BoxFlag_CanAddInput'.\n\n\n\n");
	m_vTooltips[l_pTooltipButton_outputs]            = CString("Outputs: \nUse the 'Add' and 'Remove' buttons to set all the outputs your box will have.\nWhen pressing 'Add' a dialog window will appear to know the name and type of the new output.\n------\nExample:\n'Filtered Signal' of type 'Signal'\n\n");
	m_vTooltips[l_pTooltipButton_outputs_modify]     = CString("Modify: \nCheck this option if the output(s) of your box can be modified (type and name) in the Designer by right-clicking the box.\nIn the implementation, this option decides whether or not the box will have the flag 'BoxFlag_CanModifyOutput'.\n\n\n\n\n");
	m_vTooltips[l_pTooltipButton_outputs_addRemove]  = CString("Add/Remove: \nCheck this option if the user must be able to add (or remove) outputs, by right-clicking the box.\nIn the implementation, this option decides whether or not the box will have the flag 'BoxFlag_CanAddOutput'.\n\n\n\n");
	m_vTooltips[l_pTooltipButton_settings]           = CString("Settings: \nUse the 'Add' and 'Remove' buttons to set all the settings your box will have.\nWhen pressing 'Add' a dialog window will appear to know the name,type and default value of the new output.\n------\nExample:\n'Filter order' of type 'int' with default value '4'\n\n");
	m_vTooltips[l_pTooltipButton_settings_modify]    = CString("Modify: \nCheck this option if the setting(s) of your box can be modified (type and name) in the Designer by right-clicking the box.\nIn the implementation, this option decides whether or not the box will have the flag 'BoxFlag_CanModifySetting'.\n\n\n\n\n");
	m_vTooltips[l_pTooltipButton_settings_addRemove] = CString("Add/Remove: \nCheck this option if the user must be able to add (or remove) settings, by right-clicking the box.\nIn the implementation, this option decides whether or not the box will have the flag 'BoxFlag_CanAddSetting'.\n\n\n\n");
	m_vTooltips[l_pTooltipButton_algorithms]         = CString("Codec Algorithms: \nChoose the decoder(s) and encoder(s) used by the box. \nYou can choose between all the different stream codecs currently in OpenViBE.\nWhen choosing a codec, the dialog window will display the algorithm inputs and outputs that can be retrieve through getter methods. \n------\nExample: Signal Decoder, that outputs a Streamed Matrix and a Sampling Frequency value from a Memory Buffer.\n\n");
	m_vTooltips[l_pTooltipButton_className]          = CString("Class Name: \nThis name will be used in the code to build the class name.\nUsually, the class name is close to the box name, just without any blank.\nIf your box relies on GPL licensed libraries such as IT++, please precise it in the class name with the postfix 'GPL'.\nAuthorized characters: letters (lower and upper case), numbers, NO special characters, NO blank.\n------\nExample: ClockStimulator\n");
	m_vTooltips[l_pTooltipButton_UseCodecToolkit]    = CString("Codec Toolkit: \nTells the generator to use or not the Codec Toolkit in the box implementation. \nThe Codec Toolkit makes the decoding and encoding process much more simpler, and is highly advised.\n\n\n\n\n");
	m_vTooltips[l_pTooltipButton_BoxListener]        = CString("Box Listener: \nImplement or not a box listener class in the header.\nA box listener has various callbacks that you can overwrite, related to any modification of the box structure.\n------\nExample:\nThe Identity box uses a box listener with 2 callbacks: 'onInputAdded' and 'onOutputAdded'.\nWhenever an input (output) is added, the listener automatically add an output (input) of the same type.\n");
	
	g_signal_connect(l_pTooltipButton_nameVersion,        "pressed",G_CALLBACK(button_tooltip_cb), this);
	g_signal_connect(l_pTooltipButton_category,           "pressed",G_CALLBACK(button_tooltip_cb), this);
	g_signal_connect(l_pTooltipButton_description,        "pressed",G_CALLBACK(button_tooltip_cb), this);
	g_signal_connect(l_pTooltipButton_icon,               "pressed",G_CALLBACK(button_tooltip_cb), this);
	g_signal_connect(l_pTooltipButton_inputs,             "pressed",G_CALLBACK(button_tooltip_cb), this);
	g_signal_connect(l_pTooltipButton_inputs_modify,      "pressed",G_CALLBACK(button_tooltip_cb), this);
	g_signal_connect(l_pTooltipButton_inputs_addRemove,   "pressed",G_CALLBACK(button_tooltip_cb), this);
	g_signal_connect(l_pTooltipButton_outputs,            "pressed",G_CALLBACK(button_tooltip_cb), this);
	g_signal_connect(l_pTooltipButton_outputs_modify,     "pressed",G_CALLBACK(button_tooltip_cb), this);
	g_signal_connect(l_pTooltipButton_outputs_addRemove,  "pressed",G_CALLBACK(button_tooltip_cb), this);
	g_signal_connect(l_pTooltipButton_settings,           "pressed",G_CALLBACK(button_tooltip_cb), this);
	g_signal_connect(l_pTooltipButton_settings_modify,    "pressed",G_CALLBACK(button_tooltip_cb), this);
	g_signal_connect(l_pTooltipButton_settings_addRemove, "pressed",G_CALLBACK(button_tooltip_cb), this);
	g_signal_connect(l_pTooltipButton_algorithms,         "pressed",G_CALLBACK(button_tooltip_cb), this);
	g_signal_connect(l_pTooltipButton_className,          "pressed",G_CALLBACK(button_tooltip_cb), this);
	g_signal_connect(l_pTooltipButton_UseCodecToolkit,    "pressed",G_CALLBACK(button_tooltip_cb), this);
	g_signal_connect(l_pTooltipButton_BoxListener,        "pressed",G_CALLBACK(button_tooltip_cb), this);
	
	//'Inputs' buttons
	::GtkButton * l_pInputsButton_add            = GTK_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "sg-box-inputs-add-button"));
	::GtkButton * l_pInputsButton_remove         = GTK_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "sg-box-inputs-remove-button"));
	
	g_signal_connect(l_pInputsButton_add,    "pressed",G_CALLBACK(button_add_input_cb), this);
	g_signal_connect(l_pInputsButton_remove, "pressed",G_CALLBACK(button_remove_input_cb), this);
	
	//'outputs' buttons
	::GtkButton * l_pOutputsButton_add            = GTK_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "sg-box-outputs-add-button"));
	::GtkButton * l_pOutputsButton_remove         = GTK_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "sg-box-outputs-remove-button"));
	
	g_signal_connect(l_pOutputsButton_add,    "pressed",G_CALLBACK(button_add_output_cb), this);
	g_signal_connect(l_pOutputsButton_remove, "pressed",G_CALLBACK(button_remove_output_cb), this);
	
	//'settings' buttons
	::GtkButton * l_pSettingsButton_add            = GTK_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "sg-box-settings-add-button"));
	::GtkButton * l_pSettingsButton_remove         = GTK_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "sg-box-settings-remove-button"));
	
	g_signal_connect(l_pSettingsButton_add,    "pressed",G_CALLBACK(button_add_setting_cb), this);
	g_signal_connect(l_pSettingsButton_remove, "pressed",G_CALLBACK(button_remove_setting_cb), this);

	//'algos' buttons
	::GtkButton * l_pAlgorithmsButton_add          = GTK_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "sg-box-algorithms-add-button"));
	::GtkButton * l_pAlgorithmsButton_remove       = GTK_BUTTON(gtk_builder_get_object(m_pBuilderInterface, "sg-box-algorithms-remove-button"));
	
	g_signal_connect(l_pAlgorithmsButton_add,    "pressed",G_CALLBACK(button_add_algorithm_cb), this);
	g_signal_connect(l_pAlgorithmsButton_remove, "pressed",G_CALLBACK(button_remove_algorithm_cb), this);

	//Add IO dialog buttons 
	::GtkWidget * l_pDialog = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-IO-add-dialog"));
	gtk_dialog_add_button (GTK_DIALOG (l_pDialog),
		GTK_STOCK_APPLY,
		GTK_RESPONSE_APPLY);

	gtk_dialog_add_button (GTK_DIALOG (l_pDialog),
		GTK_STOCK_CANCEL,
		GTK_RESPONSE_CANCEL);

	//Add Setting dialog buttons 
	l_pDialog = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-settings-add-dialog"));
	gtk_dialog_add_button (GTK_DIALOG (l_pDialog),
		GTK_STOCK_APPLY,
		GTK_RESPONSE_APPLY);

	gtk_dialog_add_button (GTK_DIALOG (l_pDialog),
		GTK_STOCK_CANCEL,
		GTK_RESPONSE_CANCEL);

	//Add Algo dialog buttons 
	l_pDialog = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-algorithms-add-dialog"));
	gtk_dialog_add_button (GTK_DIALOG (l_pDialog),
		GTK_STOCK_APPLY,
		GTK_RESPONSE_APPLY);

	gtk_dialog_add_button (GTK_DIALOG (l_pDialog),
		GTK_STOCK_CANCEL,
		GTK_RESPONSE_CANCEL);

	//initialize the icon combo box with gtk stock items
	::GtkWidget * l_pIconCombobox = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-icon-combobox"));
	::GtkTreeModel * l_pIconListStore = gtk_combo_box_get_model(GTK_COMBO_BOX(l_pIconCombobox));
	::GSList * l_StockIdList = gtk_stock_list_ids();
	while(l_StockIdList->next!=NULL)
	{
		GtkTreeIter l_iter;
		gtk_list_store_append(GTK_LIST_STORE(l_pIconListStore), &l_iter);
		gtk_list_store_set (GTK_LIST_STORE(l_pIconListStore), &l_iter, 0, (char *)l_StockIdList->data,1,(char *)l_StockIdList->data,-1);
		l_StockIdList = g_slist_next(l_StockIdList);
	}
	g_slist_free(l_StockIdList);

	//types when adding IOS
	::GtkWidget * l_pTypeCombobox = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-IO-add-type-combobox"));
	::GtkTreeModel * l_pTypeListStore = gtk_combo_box_get_model(GTK_COMBO_BOX(l_pTypeCombobox));
	::GtkWidget * l_pSettingTypeCombobox = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-settings-add-type-combobox"));
	::GtkTreeModel * l_pSettingTypeListStore = gtk_combo_box_get_model(GTK_COMBO_BOX(l_pSettingTypeCombobox));
	//we iterate over all identifiers
	CIdentifier l_Identifier = m_rKernelContext.getTypeManager().getNextTypeIdentifier(OV_UndefinedIdentifier);
	while(l_Identifier != OV_UndefinedIdentifier)
	{
		CString l_sType = m_rKernelContext.getTypeManager().getTypeName(l_Identifier);
		CString l_sTypeId = l_Identifier.toString();
		//Streams are possible inputs and outputs
		if(m_rKernelContext.getTypeManager().isStream(l_Identifier))
		{
			GtkTreeIter l_iter;
			gtk_list_store_append(GTK_LIST_STORE(l_pTypeListStore), &l_iter);
			gtk_list_store_set (GTK_LIST_STORE(l_pTypeListStore), &l_iter, 0, (const char *)l_sType,1,(const char *)l_sTypeId,-1);
		}
		else // other types are possible settings
		{
			GtkTreeIter l_iter;
			gtk_list_store_append(GTK_LIST_STORE(l_pSettingTypeListStore), &l_iter);
			gtk_list_store_set (GTK_LIST_STORE(l_pSettingTypeListStore), &l_iter, 0, (const char *)l_sType,1,(const char *)l_sTypeId,-1);
		}
		l_Identifier = m_rKernelContext.getTypeManager().getNextTypeIdentifier(l_Identifier);
	}
	gtk_combo_box_set_active(GTK_COMBO_BOX(l_pTypeCombobox),0);
	gtk_combo_box_set_active(GTK_COMBO_BOX(l_pSettingTypeCombobox),0);
	//types when adding Algorithms
	m_vParameterType_EnumTypeCorrespondance.resize(ParameterType_Pointer+1);
	m_vParameterType_EnumTypeCorrespondance[ParameterType_None]           = "TYPE-NOT-AVAILABLE";
	m_vParameterType_EnumTypeCorrespondance[ParameterType_Integer]        = "OpenViBE::int64";
	m_vParameterType_EnumTypeCorrespondance[ParameterType_UInteger]       = "OpenViBE::uint64";
	m_vParameterType_EnumTypeCorrespondance[ParameterType_Enumeration]    = "ENUMERATION-NOT-AVAILABLE";
	m_vParameterType_EnumTypeCorrespondance[ParameterType_Boolean]        = "OpenViBE::boolean";
	m_vParameterType_EnumTypeCorrespondance[ParameterType_Float]          = "OpenViBE::float64";
	m_vParameterType_EnumTypeCorrespondance[ParameterType_String]         = "OpenViBE::CString";
	m_vParameterType_EnumTypeCorrespondance[ParameterType_Identifier]     = "OpenViBE::CIdentifier";
	m_vParameterType_EnumTypeCorrespondance[ParameterType_Matrix]         = "OpenViBE::IMatrix *";
	m_vParameterType_EnumTypeCorrespondance[ParameterType_StimulationSet] = "OpenViBE::IStimulationSet *";
	m_vParameterType_EnumTypeCorrespondance[ParameterType_MemoryBuffer]   = "OpenViBE::IMemoryBuffer *";
	m_vParameterType_EnumTypeCorrespondance[ParameterType_Object]         = "OpenViBE::IObject *";
	m_vParameterType_EnumTypeCorrespondance[ParameterType_Pointer]        = "OpenViBE::uint8*";


	// CODECS INITIALISATION:
	::GtkWidget * l_pCodecCheckbutton = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-algorithms-toolkit-checkbutton"));
	m_bUseCodecToolkit = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(l_pCodecCheckbutton));

	::GtkWidget * l_pAlgoCombobox = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-algorithms-add-combobox"));
	::GtkTreeModel * l_pAlgoListStore = gtk_combo_box_get_model(GTK_COMBO_BOX(l_pAlgoCombobox));
	//we iterate over all plugin descriptor identifiers
	l_Identifier = m_rKernelContext.getPluginManager().getNextPluginObjectDescIdentifier(OV_UndefinedIdentifier);
	while(l_Identifier != OV_UndefinedIdentifier)
	{
		const IPluginObjectDesc * l_pDesc =  m_rKernelContext.getPluginManager().getPluginObjectDesc(l_Identifier);
		if(l_pDesc != NULL && l_pDesc->isDerivedFromClass(OV_ClassId_Plugins_AlgorithmDesc)) // we select only algorithm descriptors
		{
			CString l_sAlgo = l_pDesc->getName();
			string l_sTest((const char *) l_sAlgo);

			// we only keep decoders and encoders
			// and reject the master acquisition stream
			if((l_sTest.find("encoder")!=string::npos || l_sTest.find("decoder")!=string::npos) && l_sTest.find("Master")==string::npos)
			{
				CString l_sAlgoID = l_Identifier.toString();
				GtkTreeIter l_iter;
				gtk_list_store_append(GTK_LIST_STORE(l_pAlgoListStore), &l_iter);
				gtk_list_store_set (GTK_LIST_STORE(l_pAlgoListStore), &l_iter, 0, (const char *)l_sAlgo,1,(const char *)l_sAlgoID,-1);
			
				// now we map every decoder/encoder to its string description that will be added in the skeleton (algorithmProxy + parameter handlers for I/O)
				CString l_sHeaderDeclaration ="\\t\\t\\t\\/\\/ "+ l_sAlgo +"\\n";
				CString l_sInitialisation ="\\t\\/\\/ "+ l_sAlgo +"\\n";
				CString l_sInitialisation_ReferenceTargets;
				CString l_sUninitialisation;

			
				//we need to create a dummy instance of the algorithm proto to know its input/output/triggers
				const IPluginObjectDesc * l_pDesc = m_rKernelContext.getPluginManager().getPluginObjectDesc(l_Identifier);
				CDummyAlgoProto l_oDummyProto;
				((IAlgorithmDesc *)l_pDesc)->getAlgorithmPrototype(l_oDummyProto);
				//algorithm proxy
				string l_sAlgoNameStdSTr((const char *)l_sAlgo);
				for(uint32 s=0; s<l_sAlgoNameStdSTr.length(); s++)
				{
					if(l_sAlgoNameStdSTr[s]==' ')
					{
						l_sAlgoNameStdSTr.erase(s,1);
						if(l_sAlgoNameStdSTr[s] >= 'a' && l_sAlgoNameStdSTr[s]<= 'z') l_sAlgoNameStdSTr.replace(s,1,1,(char)(l_sAlgoNameStdSTr[s]+'A'-'a'));
					}
				}
				string l_sCodecTypeStdStr = l_sAlgoNameStdSTr;
				string l_sStream("Stream");
				l_sCodecTypeStdStr.erase(l_sCodecTypeStdStr.rfind(l_sStream),6);

				CString l_sAlgorithmProxy = "m_p@" + CString(l_sAlgoNameStdSTr.c_str());
				CString l_sCodec = "m_o@" + CString(l_sCodecTypeStdStr.c_str());
				CString l_sCodecType = CString(l_sCodecTypeStdStr.c_str());
			
				if(! m_bUseCodecToolkit)
				{
					l_sInitialisation_ReferenceTargets = "\\t\\/\\/"+ l_sAlgo +" Reference Targets :\\n";
					l_sHeaderDeclaration = l_sHeaderDeclaration + "\\t\\t\\tOpenViBE::Kernel::IAlgorithmProxy* "+ l_sAlgorithmProxy +";\\n";
					l_sInitialisation = l_sInitialisation + "\\t" +
						l_sAlgorithmProxy +" = \\&this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_"+CString(l_sAlgoNameStdSTr.c_str())+"));\\n" +
						"\\t"+ l_sAlgorithmProxy +"->initialize();\\n";
					l_sUninitialisation = "\\tthis->getAlgorithmManager().releaseAlgorithm(*"+l_sAlgorithmProxy+");\\n" + l_sUninitialisation;
					//inputs of the algorithm
					for(map<CString,EParameterType>::iterator it = l_oDummyProto.m_vInputs.begin(); it!=l_oDummyProto.m_vInputs.end(); it++)
					{
						string l_sInputNameStdSTr((const char *)(*it).first);
						for(uint32 s=0; s<l_sInputNameStdSTr.length(); s++)
						{
							if(l_sInputNameStdSTr[s]==' ')
							{
								l_sInputNameStdSTr.erase(s,1);
								if(l_sInputNameStdSTr[s] >= 'a' && l_sInputNameStdSTr[s]<= 'z') l_sInputNameStdSTr.replace(s,1,1,(char)(l_sInputNameStdSTr[s]+'A'-'a'));
							}
						}
						CString l_sInputHandler ="ip_p@"+ CString(l_sInputNameStdSTr.c_str());
						// input handlers for pointer must be "const"
						CString l_sConst = "";
						if(l_oDummyProto.m_vInputs[(*it).first] >= ParameterType_Matrix)
						{
							l_sConst = "const ";
						}
						l_sHeaderDeclaration = l_sHeaderDeclaration + "\\t\\t\\tOpenViBE::Kernel::TParameterHandler < " + l_sConst + (const char *)m_vParameterType_EnumTypeCorrespondance[l_oDummyProto.m_vInputs[(*it).first]] + "> "+ l_sInputHandler +";\\n";
						l_sInitialisation = l_sInitialisation + "\\t" +
							 l_sInputHandler + ".initialize("+ l_sAlgorithmProxy +"->getInputParameter(OVP_GD_Algorithm_"+CString(l_sAlgoNameStdSTr.c_str())+"_InputParameterId_"+CString(l_sInputNameStdSTr.c_str())+"));\\n";
						l_sInitialisation_ReferenceTargets = l_sInitialisation_ReferenceTargets + "\\t\\/\\/"+l_sInputHandler +".setReferenceTarget( ... )\\n";
						l_sUninitialisation = "\\t"+l_sInputHandler+ ".uninitialize();\\n" + l_sUninitialisation;
					}
					//outputs of the algorithm
					for(map<CString,EParameterType>::iterator it = l_oDummyProto.m_vOutputs.begin(); it!=l_oDummyProto.m_vOutputs.end(); it++)
					{
						string l_sOutputNameStdSTr((const char *)(*it).first);
						for(uint32 s=0; s<l_sOutputNameStdSTr.length(); s++)
						{
							if(l_sOutputNameStdSTr[s]==' ')
							{
								l_sOutputNameStdSTr.erase(s,1);
								if(l_sOutputNameStdSTr[s] >= 'a' && l_sOutputNameStdSTr[s]<= 'z') l_sOutputNameStdSTr.replace(s,1,1,(char)(l_sOutputNameStdSTr[s]+'A'-'a'));
							}
						}
						CString l_sOutputHandler ="op_p@"+ CString(l_sOutputNameStdSTr.c_str());
						l_sHeaderDeclaration = l_sHeaderDeclaration + "\\t\\t\\tOpenViBE::Kernel::TParameterHandler < " + (const char *)m_vParameterType_EnumTypeCorrespondance[l_oDummyProto.m_vOutputs[(*it).first]] + "> "+ l_sOutputHandler +";\\n";
						l_sInitialisation = l_sInitialisation + "\\t" +
							l_sOutputHandler+ ".initialize("+ l_sAlgorithmProxy +"->getOutputParameter(OVP_GD_Algorithm_"+CString(l_sAlgoNameStdSTr.c_str())+"_OutputParameterId_"+CString(l_sOutputNameStdSTr.c_str())+"));\\n";
						l_sUninitialisation = "\\t"+ l_sOutputHandler+ ".uninitialize();\\n" + l_sUninitialisation;
					}
					l_sHeaderDeclaration = l_sHeaderDeclaration + "\\n";
					l_sInitialisation = l_sInitialisation + "\\n";
					l_sInitialisation_ReferenceTargets = l_sInitialisation_ReferenceTargets + "\\n";
					l_sUninitialisation ="\\t\\/\\/ "+ l_sAlgo +"\\n" + l_sUninitialisation + "\\t"+l_sAlgorithmProxy+" = NULL;\\n\\n";
				}
				else // use the Codec Toolkit
				{
					l_sHeaderDeclaration = l_sHeaderDeclaration + "\\t\\t\\tOpenViBEToolkit::T"+ l_sCodecType + " < @@ClassName@@ > " + l_sCodec +";\\n";
					l_sInitialisation = l_sInitialisation + "\\t" + l_sCodec + ".initialize(*this);\\n";
					l_sUninitialisation = "\\t" + l_sCodec + ".uninitialize();\\n" + l_sUninitialisation;
					l_sInitialisation_ReferenceTargets = "";
					//l_sInitialisation_ReferenceTargets = l_sInitialisation_ReferenceTargets + "\\t\\/\\/ If you need to, you can manually set the reference targets to link the codecs input and output. To do so, you can use :\n";
					//l_sInitialisation_ReferenceTargets = l_sInitialisation_ReferenceTargets + "\\t\\/\\/"+l_sCodec + ".getInputX().setReferenceTarget( ... )\\n";
				}
				m_mAlgorithmHeaderDeclaration[l_sAlgo]               = l_sHeaderDeclaration;
				m_mAlgorithmInitialisation[l_sAlgo]                  = l_sInitialisation;
				m_mAlgorithmUninitialisation[l_sAlgo]                = l_sUninitialisation;
				m_mAlgorithmInitialisation_ReferenceTargets[l_sAlgo] = l_sInitialisation_ReferenceTargets;
				m_rKernelContext.getLogManager() << LogLevel_Debug << "The algorithm [" << l_sAlgo << "] has description [" << l_sHeaderDeclaration << "\n";
			}
		}
		l_Identifier =m_rKernelContext.getPluginManager().getNextPluginObjectDescIdentifier(l_Identifier);
	}

	

	gtk_combo_box_set_active(GTK_COMBO_BOX(l_pAlgoCombobox),-1);
	//callback to update algo description
	g_signal_connect(G_OBJECT(l_pAlgoCombobox),"changed", G_CALLBACK(algorithm_selected_cb),this);
	
	//Close with X and "cancel" button
	g_signal_connect (G_OBJECT(l_pBox),"delete_event",G_CALLBACK(::gtk_exit),0);
	::GtkWidget * l_pButtonCancel = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-exit-button"));
	g_signal_connect(l_pButtonCancel,"pressed", G_CALLBACK(button_exit_cb), this);

	//load everything from config file
	load(m_sConfigurationFile);
	::GtkWidget * l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-checkbutton"));
	toggleListenerCheckbuttonsStateCB(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(l_pListenerWidget)));

	gtk_widget_show_all(l_pBox);

	return true;
}

boolean CBoxAlgorithmSkeletonGenerator::save(CString sFileName)
{
	FILE* l_pFile=::fopen(sFileName.toASCIIString(), "ab");
	if(!l_pFile)
	{
		m_rKernelContext.getLogManager() << LogLevel_Warning << "Saving the box entries in [" << sFileName << "] failed !\n";
		return false;
	}
	::fprintf(l_pFile, "# ----------------------BOX GENERATOR-------------------------\n");
	// we need to replace the \ by / in the path for cross compatibility
	string::iterator it_directory;
	string l_sTempTargetDirectory(m_sTargetDirectory.toASCIIString());
	for(it_directory=l_sTempTargetDirectory.begin(); it_directory<l_sTempTargetDirectory.end(); it_directory++)
	{
		if((*it_directory)=='\\')
		{
			l_sTempTargetDirectory.replace(it_directory, it_directory+1, 1, '/');
		}
	}
	::fprintf(l_pFile, "SkeletonGenerator_Box_TargetDirectory = %s\n", l_sTempTargetDirectory.c_str());

	::fprintf(l_pFile, "SkeletonGenerator_Box_Name = %s\n",(const char *)m_sName);
	::fprintf(l_pFile, "SkeletonGenerator_Box_Version = %s\n",(const char *) m_sVersion);
	::fprintf(l_pFile, "SkeletonGenerator_Box_Category = %s\n",(const char *)m_sCategory);
	::fprintf(l_pFile, "SkeletonGenerator_Box_ClassName = %s\n",(const char *)m_sClassName);
	
	//we need to escape with '\' the special characters of the configuration manager files
	string l_sTempShortDescr((const char *)m_sShortDescription);
	l_sTempShortDescr.reserve(1000); // if we need to insert characters
	string::iterator it_sdescr=l_sTempShortDescr.begin();
	while(it_sdescr<l_sTempShortDescr.end())
	{
		//characters to escape
		if((*it_sdescr)=='\\' || (*it_sdescr)=='=' || (*it_sdescr)=='$' || (*it_sdescr)=='\t')
		{
			l_sTempShortDescr.insert(it_sdescr, '\\');
			it_sdescr++;
		}
		it_sdescr++;
	}
	::fprintf(l_pFile, "SkeletonGenerator_Box_ShortDescription = %s\n", l_sTempShortDescr.c_str());
	
	//we need to escape with '\' the special characters of the configuration manager files
	string l_sTempDetailedDescr((const char *)m_sDetailedDescription);
	m_rKernelContext.getLogManager() << LogLevel_Debug << "SAVE > DESCRIPTION FROM WIDGET: "<<l_sTempDetailedDescr.c_str()<<"\n";
	l_sTempDetailedDescr.reserve(1000); // if we need to insert characters
	string::iterator it_descr=l_sTempDetailedDescr.begin();
	while(it_descr<l_sTempDetailedDescr.end())
	{
		//characters to escape
		if((*it_descr)=='\\' || (*it_descr)=='=' || (*it_descr)=='$' || (*it_descr)=='\t')
		{
			l_sTempDetailedDescr.insert(it_descr, '\\');
			it_descr++;
		}
		//the special character we use for \n must also be escaped when used in the text
		else if((*it_descr)=='@')
		{
			l_sTempDetailedDescr.insert(it_descr, '\\');
			l_sTempDetailedDescr.insert(it_descr, '\\');
			it_descr+=2;
		}
		//we add a special character @ representing a \n for further loading. the \ ensure that the config manager will read the token past the \n
		else if((*it_descr)=='\n')
		{
			l_sTempDetailedDescr.insert(it_descr, '\\');
			l_sTempDetailedDescr.insert(it_descr, '@');
			it_descr+=2;
		}
		it_descr++;
	}
	m_rKernelContext.getLogManager() << LogLevel_Debug << "SAVE > DESCR MODIFIED: "<<l_sTempDetailedDescr.c_str()<<"\n";
	
	::fprintf(l_pFile, "SkeletonGenerator_Box_DetailedDescription = %s\n", (const char *) l_sTempDetailedDescr.c_str());
	
	::fprintf(l_pFile, "SkeletonGenerator_Box_IconIndex = %i\n",m_i32GtkStockItemIndex);
	::fprintf(l_pFile, "SkeletonGenerator_Box_IconName = %s\n",(const char *)m_sGtkStockItemName);
	
	// ADD/MODIFY FLAGS
	::fprintf(l_pFile, "SkeletonGenerator_Box_CanModifyInputs = %s\n",(m_bCanModifyInputs?"TRUE":"FALSE"));
	::fprintf(l_pFile, "SkeletonGenerator_Box_CanAddInputs = %s\n",(m_bCanAddInputs?"TRUE":"FALSE"));
	::fprintf(l_pFile, "SkeletonGenerator_Box_CanModifyOutputs = %s\n",(m_bCanModifyOutputs?"TRUE":"FALSE"));
	::fprintf(l_pFile, "SkeletonGenerator_Box_CanAddOutputs = %s\n",(m_bCanModifyOutputs?"TRUE":"FALSE"));
	::fprintf(l_pFile, "SkeletonGenerator_Box_CanModifySettings = %s\n",(m_bCanModifySettings?"TRUE":"FALSE"));
	::fprintf(l_pFile, "SkeletonGenerator_Box_CanAddSettings = %s\n",(m_bCanAddSettings?"TRUE":"FALSE"));
	
	// Inputs
	::fprintf(l_pFile, "SkeletonGenerator_Box_InputCount = %i\n",m_vInputs.size());
	for(uint32 i = 0; i < m_vInputs.size(); i++)
	{
		::fprintf(l_pFile, "SkeletonGenerator_Box_Input%i_Name = %s\n",i,(const char *)m_vInputs[i]._name);
		::fprintf(l_pFile, "SkeletonGenerator_Box_Input%i_Type = %s\n",i,(const char *)m_vInputs[i]._type);
		::fprintf(l_pFile, "SkeletonGenerator_Box_Input%i_TypeId = %s\n",i,(const char *)m_vInputs[i]._typeId);
	}
	// Outputs
	::fprintf(l_pFile, "SkeletonGenerator_Box_OutputCount = %i\n",m_vOutputs.size());
	for(uint32 i = 0; i < m_vOutputs.size(); i++)
	{
		::fprintf(l_pFile, "SkeletonGenerator_Box_Output%i_Name = %s\n",i,(const char *)m_vOutputs[i]._name);
		::fprintf(l_pFile, "SkeletonGenerator_Box_Output%i_Type = %s\n",i,(const char *)m_vOutputs[i]._type);
		::fprintf(l_pFile, "SkeletonGenerator_Box_Output%i_TypeId = %s\n",i,(const char *)m_vOutputs[i]._typeId);
	}
	// Inputs
	::fprintf(l_pFile, "SkeletonGenerator_Box_SettingCount = %i\n",m_vSettings.size());
	for(uint32 i = 0; i < m_vSettings.size(); i++)
	{
		::fprintf(l_pFile, "SkeletonGenerator_Box_Setting%i_Name = %s\n",i,(const char *)m_vSettings[i]._name);
		::fprintf(l_pFile, "SkeletonGenerator_Box_Setting%i_Type = %s\n",i,(const char *)m_vSettings[i]._type);
		::fprintf(l_pFile, "SkeletonGenerator_Box_Setting%i_TypeId = %s\n",i,(const char *)m_vSettings[i]._typeId);
		::fprintf(l_pFile, "SkeletonGenerator_Box_Setting%i_DefaultValue = %s\n",i,(const char *)m_vSettings[i]._defaultValue);
	}
	// Algorithms
	::fprintf(l_pFile, "SkeletonGenerator_Box_AlgorithmCount = %i\n",m_vAlgorithms.size());
	for(uint32 i = 0; i < m_vAlgorithms.size(); i++)
	{
		::fprintf(l_pFile, "SkeletonGenerator_Box_Algorithm%i_Name = %s\n",i,(const char *)m_vAlgorithms[i]);
	}

	// Listener
	::fprintf(l_pFile, "SkeletonGenerator_Box_UseListener = %s\n",(m_bUseBoxListener?"TRUE":"FALSE"));
	::fprintf(l_pFile, "SkeletonGenerator_Box_ListenerOnInputAdded = %s\n",(m_bBoxListenerOnInputAdded?"TRUE":"FALSE"));
	::fprintf(l_pFile, "SkeletonGenerator_Box_ListenerOnInputRemoved = %s\n",(m_bBoxListenerOnInputRemoved?"TRUE":"FALSE"));
	::fprintf(l_pFile, "SkeletonGenerator_Box_ListenerOnInputTypeChanged = %s\n",(m_bBoxListenerOnInputTypeChanged?"TRUE":"FALSE"));
	::fprintf(l_pFile, "SkeletonGenerator_Box_ListenerOnInputNameChanged = %s\n",(m_bBoxListenerOnInputNameChanged?"TRUE":"FALSE"));
	::fprintf(l_pFile, "SkeletonGenerator_Box_ListenerOnInputConnected = %s\n",(m_bBoxListenerOnInputConnected?"TRUE":"FALSE"));
	::fprintf(l_pFile, "SkeletonGenerator_Box_ListenerOnInputDisconnected = %s\n",(m_bBoxListenerOnInputDisconnected?"TRUE":"FALSE"));
	::fprintf(l_pFile, "SkeletonGenerator_Box_ListenerOnOutputAdded = %s\n",(m_bBoxListenerOnOutputAdded?"TRUE":"FALSE"));
	::fprintf(l_pFile, "SkeletonGenerator_Box_ListenerOnOutputRemoved = %s\n",(m_bBoxListenerOnOutputRemoved?"TRUE":"FALSE"));
	::fprintf(l_pFile, "SkeletonGenerator_Box_ListenerOnOutputTypeChanged = %s\n",(m_bBoxListenerOnOutputTypeChanged?"TRUE":"FALSE"));
	::fprintf(l_pFile, "SkeletonGenerator_Box_ListenerOnOutputNameChanged = %s\n",(m_bBoxListenerOnOutputNameChanged?"TRUE":"FALSE"));
	::fprintf(l_pFile, "SkeletonGenerator_Box_ListenerOnOutputConnected = %s\n",(m_bBoxListenerOnOutputConnected?"TRUE":"FALSE"));
	::fprintf(l_pFile, "SkeletonGenerator_Box_ListenerOnOutputDisconnected = %s\n",(m_bBoxListenerOnOutputDisconnected?"TRUE":"FALSE"));
	::fprintf(l_pFile, "SkeletonGenerator_Box_ListenerOnSettingAdded = %s\n",(m_bBoxListenerOnSettingAdded?"TRUE":"FALSE"));
	::fprintf(l_pFile, "SkeletonGenerator_Box_ListenerOnSettingRemoved = %s\n",(m_bBoxListenerOnSettingRemoved?"TRUE":"FALSE"));
	::fprintf(l_pFile, "SkeletonGenerator_Box_ListenerOnSettingTypeChanged = %s\n",(m_bBoxListenerOnSettingTypeChanged?"TRUE":"FALSE"));
	::fprintf(l_pFile, "SkeletonGenerator_Box_ListenerOnSettingNameChanged = %s\n",(m_bBoxListenerOnSettingNameChanged?"TRUE":"FALSE"));
	::fprintf(l_pFile, "SkeletonGenerator_Box_ListenerOnSettingDefaultValueChanged = %s\n",(m_bBoxListenerOnSettingDefaultValueChanged?"TRUE":"FALSE"));
	::fprintf(l_pFile, "SkeletonGenerator_Box_ListenerOnSettingValueChanged = %s\n",(m_bBoxListenerOnSettingValueChanged?"TRUE":"FALSE"));

	::fprintf(l_pFile, "SkeletonGenerator_Box_ProcessInput = %s\n",(m_bProcessInput?"TRUE":"FALSE"));
	::fprintf(l_pFile, "SkeletonGenerator_Box_ProcessClock = %s\n",(m_bProcessClock?"TRUE":"FALSE"));
	stringstream ssListener; ssListener << m_ui32ClockFrequency;
	::fprintf(l_pFile, "SkeletonGenerator_Box_ClockFrequency = %s\n",ssListener.str().c_str());
	



	::fprintf(l_pFile, "# --------------------------------------------------\n");
	::fclose(l_pFile);
	m_rKernelContext.getLogManager() << LogLevel_Info << "box entries saved in [" << sFileName << "]\n";

	m_bConfigurationFileLoaded = false; 

	return true;
}

boolean CBoxAlgorithmSkeletonGenerator::load(CString sFileName)
{
	if(!m_bConfigurationFileLoaded && !m_rKernelContext.getConfigurationManager().addConfigurationFromFile(sFileName))
	{
		m_rKernelContext.getLogManager() << LogLevel_Warning << "box: Configuration file [" << sFileName << "] could not be loaded.\n";
		return false;
	}

	::GtkWidget * l_pClassNameEntry = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-class-name-entry"));
	CString l_sClassName = m_rKernelContext.getConfigurationManager().expand("${SkeletonGenerator_Box_ClassName}");
	gtk_entry_set_text(GTK_ENTRY(l_pClassNameEntry),(const char *) l_sClassName);
	::GtkWidget * l_pCategoryEntry = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-category-entry"));
	CString l_sCategory = m_rKernelContext.getConfigurationManager().expand("${SkeletonGenerator_Box_Category}");
	gtk_entry_set_text(GTK_ENTRY(l_pCategoryEntry),(const char *) l_sCategory);
	::GtkWidget * l_pNameEntry = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-box-name-entry"));
	CString l_sName = m_rKernelContext.getConfigurationManager().expand("${SkeletonGenerator_Box_Name}");
	gtk_entry_set_text(GTK_ENTRY(l_pNameEntry),(const char *) l_sName);
	::GtkWidget * l_pVersionEntry = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-version-entry"));
	CString l_sVersion = m_rKernelContext.getConfigurationManager().expand("${SkeletonGenerator_Box_Version}");
	gtk_entry_set_text(GTK_ENTRY(l_pVersionEntry),(const char *) l_sVersion);
	::GtkWidget * l_pSDEntry = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-short-description-entry"));
	
	CString l_sShortDescr = m_rKernelContext.getConfigurationManager().expand("${SkeletonGenerator_Box_ShortDescription}");
	//we need to UNescape the special characters of the configuration manager files
	string::iterator it_sdescr;
	string l_sTempShortDescr(l_sShortDescr.toASCIIString());
	for(it_sdescr=l_sTempShortDescr.begin(); it_sdescr<l_sTempShortDescr.end(); it_sdescr++)
	{
		// juste erase the escape character
		if((*it_sdescr)=='\\' && (it_sdescr+1) != l_sTempShortDescr.end())
		{
			if((*(it_sdescr+1))=='\\' || (*(it_sdescr+1))=='=' || (*(it_sdescr+1))=='$' || (*(it_sdescr+1))=='\t' || (*(it_sdescr+1))=='@')
			{
				l_sTempShortDescr.erase(it_sdescr);
			}
		}
		// replace the special character @ by \n in the textview
		else if((*it_sdescr)=='@')
		{
			l_sTempShortDescr.erase(it_sdescr);
			l_sTempShortDescr.insert(it_sdescr,'\n');
		}
	}	
	gtk_entry_set_text(GTK_ENTRY(l_pSDEntry),l_sTempShortDescr.c_str());
	
	::GtkWidget * l_pDetailedDescrTextView = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-detailed-description-textview"));
	::GtkTextBuffer * l_pDetailedDescrTextBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(l_pDetailedDescrTextView));
	CString l_sDetailedDescr = m_rKernelContext.getConfigurationManager().expand("${SkeletonGenerator_Box_DetailedDescription}");
	
	//we need to UNescape the special characters of the configuration manager files
	string::iterator it_descr;
	string l_sTempDetailedDescr(l_sDetailedDescr.toASCIIString());
	m_rKernelContext.getLogManager() << LogLevel_Debug << "LOAD > DESCR LOADED: "<<l_sTempDetailedDescr.c_str()<<"\n";
	for(it_descr=l_sTempDetailedDescr.begin(); it_descr<l_sTempDetailedDescr.end(); it_descr++)
	{
		// juste erase the escape character
		if((*it_descr)=='\\' && (it_descr+1) != l_sTempDetailedDescr.end())
		{
			if((*(it_sdescr+1))=='\\' || (*(it_sdescr+1))=='=' || (*(it_sdescr+1))=='$' || (*(it_sdescr+1))=='\t' || (*(it_sdescr+1))=='@')
			{
				l_sTempDetailedDescr.erase(it_descr);
			}
		}
		// replace the special character @ by \n in the textview
		else if((*it_descr)=='@')
		{
			l_sTempDetailedDescr.erase(it_descr);
			l_sTempDetailedDescr.insert(it_descr,'\n');
		}
	}	
	m_rKernelContext.getLogManager() << LogLevel_Debug << "LOAD > DESCR MODIFIED: "<<l_sTempDetailedDescr.c_str()<<"\n";
	gtk_text_buffer_set_text(l_pDetailedDescrTextBuffer,l_sTempDetailedDescr.c_str(),((string)l_sTempDetailedDescr).length());

	GtkWidget * l_pIconCombobox =  GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-icon-combobox"));
	int64 l_iIconSelected = m_rKernelContext.getConfigurationManager().expandAsInteger("${SkeletonGenerator_Box_IconIndex}");
	gtk_combo_box_set_active(GTK_COMBO_BOX(l_pIconCombobox),(gint)l_iIconSelected);
	
	GtkWidget * l_pCanModifyInputsCheckbox =  GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-inputs-modify-checkbutton"));
	boolean l_pCanModifyInputs = m_rKernelContext.getConfigurationManager().expandAsBoolean("${SkeletonGenerator_Box_CanModifyInputs}");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(l_pCanModifyInputsCheckbox),l_pCanModifyInputs);

	GtkWidget * l_pCanAddInputsCheckbox =  GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-inputs-add-checkbutton"));
	boolean l_pCanAddInputs = m_rKernelContext.getConfigurationManager().expandAsBoolean("${SkeletonGenerator_Box_CanAddInputs}",false);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(l_pCanAddInputsCheckbox),l_pCanAddInputs);

	GtkWidget * l_pCanModifyOutputsCheckbox =  GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-outputs-modify-checkbutton"));
	boolean l_pCanModifyOutputs = m_rKernelContext.getConfigurationManager().expandAsBoolean("${SkeletonGenerator_Box_CanModifyOutputs}",false);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(l_pCanModifyOutputsCheckbox),l_pCanModifyOutputs);

	GtkWidget * l_pCanAddOutputsCheckbox =  GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-outputs-add-checkbutton"));
	boolean l_pCanAddOutputs = m_rKernelContext.getConfigurationManager().expandAsBoolean("${SkeletonGenerator_Box_CanAddOutputs}",false);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(l_pCanAddOutputsCheckbox),l_pCanAddOutputs);

	GtkWidget * l_pCanModifySettingsCheckbox =  GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-settings-modify-checkbutton"));
	boolean l_pCanModifySettings = m_rKernelContext.getConfigurationManager().expandAsBoolean("${SkeletonGenerator_Box_CanModifySettings}",false);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(l_pCanModifySettingsCheckbox),l_pCanModifySettings);

	GtkWidget * l_pCanAddSettingsCheckbox =  GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-settings-add-checkbutton"));
	boolean l_pCanAddSettings = m_rKernelContext.getConfigurationManager().expandAsBoolean("${SkeletonGenerator_Box_CanAddSettings}",false);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(l_pCanAddSettingsCheckbox),l_pCanAddSettings);

	::GtkWidget * l_pInputsTreeView = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-inputs-treeview"));
	::GtkTreeModel * l_pInputsListStore = gtk_tree_view_get_model(GTK_TREE_VIEW(l_pInputsTreeView));
	int64 l_i32InputCount = m_rKernelContext.getConfigurationManager().expandAsInteger("${SkeletonGenerator_Box_InputCount}",0);
	for(int32 i = 0; i < l_i32InputCount; i++)
	{
		char l_sTokenName [128];
		sprintf(l_sTokenName,"${SkeletonGenerator_Box_Input%i_Name}",i);
		CString l_sName =  m_rKernelContext.getConfigurationManager().expand(CString(l_sTokenName));
		char l_sTokenType [128];
		sprintf(l_sTokenType,"${SkeletonGenerator_Box_Input%i_Type}",i);
		CString l_sType =  m_rKernelContext.getConfigurationManager().expand(CString(l_sTokenType));
		char l_sTokenTypeId [128];
		sprintf(l_sTokenTypeId,"${SkeletonGenerator_Box_Input%i_TypeId}",i);
		CString l_sTypeId =  m_rKernelContext.getConfigurationManager().expand(CString(l_sTokenTypeId));
		GtkTreeIter l_iter;
		gtk_list_store_append(GTK_LIST_STORE(l_pInputsListStore), &l_iter);
		gtk_list_store_set (GTK_LIST_STORE(l_pInputsListStore), &l_iter, 0, (const char *)l_sName,1,(const char *)l_sType,2,(const char *)l_sTypeId,-1);
	}

	::GtkWidget * l_pOutputsTreeView = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-outputs-treeview"));
	::GtkTreeModel * l_pOutputsListStore = gtk_tree_view_get_model(GTK_TREE_VIEW(l_pOutputsTreeView));
	int64 l_i32OutputCount = m_rKernelContext.getConfigurationManager().expandAsInteger("${SkeletonGenerator_Box_OutputCount}",0);
	for(int32 i = 0; i < l_i32OutputCount; i++)
	{
		char l_sTokenName [128];
		sprintf(l_sTokenName,"${SkeletonGenerator_Box_Output%i_Name}",i);
		CString l_sName =  m_rKernelContext.getConfigurationManager().expand(CString(l_sTokenName));
		char l_sTokenType [128];
		sprintf(l_sTokenType,"${SkeletonGenerator_Box_Output%i_Type}",i);
		CString l_sType =  m_rKernelContext.getConfigurationManager().expand(CString(l_sTokenType));
		char l_sTokenTypeId [128];
		sprintf(l_sTokenTypeId,"${SkeletonGenerator_Box_Output%i_TypeId}",i);
		CString l_sTypeId =  m_rKernelContext.getConfigurationManager().expand(CString(l_sTokenTypeId));
		GtkTreeIter l_iter;
		gtk_list_store_append(GTK_LIST_STORE(l_pOutputsListStore), &l_iter);
		gtk_list_store_set (GTK_LIST_STORE(l_pOutputsListStore), &l_iter, 0, (const char *)l_sName,1,(const char *)l_sType,2,(const char *)l_sTypeId,-1);
	}

	::GtkWidget * l_pSettingsTreeView = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-settings-treeview"));
	::GtkTreeModel * l_pSettingsListStore = gtk_tree_view_get_model(GTK_TREE_VIEW(l_pSettingsTreeView));
	int64 l_i32SettingCount = m_rKernelContext.getConfigurationManager().expandAsInteger("${SkeletonGenerator_Box_SettingCount}",0);
	for(int32 i = 0; i < l_i32SettingCount; i++)
	{
		char l_sTokenName [128];
		sprintf(l_sTokenName,"${SkeletonGenerator_Box_Setting%i_Name}",i);
		CString l_sName =  m_rKernelContext.getConfigurationManager().expand(CString(l_sTokenName));
		char l_sTokenType [128];
		sprintf(l_sTokenType,"${SkeletonGenerator_Box_Setting%i_Type}",i);
		CString l_sType =  m_rKernelContext.getConfigurationManager().expand(CString(l_sTokenType));
		char l_sTokenTypeId [128];
		sprintf(l_sTokenTypeId,"${SkeletonGenerator_Box_Setting%i_TypeId}",i);
		CString l_sTypeId =  m_rKernelContext.getConfigurationManager().expand(CString(l_sTokenTypeId));
		char l_sTokenDefaultValue [128];
		sprintf(l_sTokenDefaultValue,"${SkeletonGenerator_Box_Setting%i_DefaultValue}",i);
		CString l_sDefaultValue =  m_rKernelContext.getConfigurationManager().expand(CString(l_sTokenDefaultValue));
		GtkTreeIter l_iter;
		gtk_list_store_append(GTK_LIST_STORE(l_pSettingsListStore), &l_iter);
		gtk_list_store_set (GTK_LIST_STORE(l_pSettingsListStore), &l_iter, 0, (const char *)l_sName,1,(const char *)l_sType,2,(const char *)l_sDefaultValue,3,(const char *)l_sTypeId,-1);
	}
	
	::GtkWidget * l_pAlgoTreeView = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-algorithms-treeview"));
	::GtkTreeModel * l_pAlgoListStore = gtk_tree_view_get_model(GTK_TREE_VIEW(l_pAlgoTreeView));
	int64 l_i32AlgoCount = m_rKernelContext.getConfigurationManager().expandAsInteger("${SkeletonGenerator_Box_AlgorithmCount}",0);
	for(int32 i = 0; i < l_i32AlgoCount; i++)
	{
		char l_sTokenName [128];
		sprintf(l_sTokenName,"${SkeletonGenerator_Box_Algorithm%i_Name}",i);
		CString l_sName =  m_rKernelContext.getConfigurationManager().expand(CString(l_sTokenName));
		GtkTreeIter l_iter;
		gtk_list_store_append(GTK_LIST_STORE(l_pAlgoListStore), &l_iter);
		gtk_list_store_set (GTK_LIST_STORE(l_pAlgoListStore), &l_iter, 0, (const char *)l_sName,-1);
	}

	::GtkWidget * l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-checkbutton"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(l_pListenerWidget),m_rKernelContext.getConfigurationManager().expandAsBoolean("${SkeletonGenerator_Box_UseListener}",false));

	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-input-added-checkbutton"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(l_pListenerWidget),m_rKernelContext.getConfigurationManager().expandAsBoolean("${SkeletonGenerator_Box_ListenerOnInputAdded}",false));
	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-input-removed-checkbutton"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(l_pListenerWidget),m_rKernelContext.getConfigurationManager().expandAsBoolean("${SkeletonGenerator_Box_ListenerOnInputRemoved}",false));
	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-input-type-checkbutton"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(l_pListenerWidget),m_rKernelContext.getConfigurationManager().expandAsBoolean("${SkeletonGenerator_Box_ListenerOnInputTypeChanged}",false));
	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-input-name-checkbutton"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(l_pListenerWidget),m_rKernelContext.getConfigurationManager().expandAsBoolean("${SkeletonGenerator_Box_ListenerOnInputNameChanged}",false));
	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-input-connected-checkbutton"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(l_pListenerWidget),m_rKernelContext.getConfigurationManager().expandAsBoolean("${SkeletonGenerator_Box_ListenerOnInputConnected}",false));
	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-input-disconnected-checkbutton"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(l_pListenerWidget),m_rKernelContext.getConfigurationManager().expandAsBoolean("${SkeletonGenerator_Box_ListenerOnInputDisconnected}",false));
	
	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-output-added-checkbutton"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(l_pListenerWidget),m_rKernelContext.getConfigurationManager().expandAsBoolean("${SkeletonGenerator_Box_ListenerOnOutputAdded}",false));
	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-output-removed-checkbutton"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(l_pListenerWidget),m_rKernelContext.getConfigurationManager().expandAsBoolean("${SkeletonGenerator_Box_ListenerOnOutputRemoved}",false));
	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-output-type-checkbutton"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(l_pListenerWidget),m_rKernelContext.getConfigurationManager().expandAsBoolean("${SkeletonGenerator_Box_ListenerOnOutputTypeChanged}",false));
	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-output-name-checkbutton"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(l_pListenerWidget),m_rKernelContext.getConfigurationManager().expandAsBoolean("${SkeletonGenerator_Box_ListenerOnOutputNameChanged}",false));
	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-output-connected-checkbutton"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(l_pListenerWidget),m_rKernelContext.getConfigurationManager().expandAsBoolean("${SkeletonGenerator_Box_ListenerOnOutputConnected}",false));
	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-output-disconnected-checkbutton"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(l_pListenerWidget),m_rKernelContext.getConfigurationManager().expandAsBoolean("${SkeletonGenerator_Box_ListenerOnOutputDisconnected}",false));
	
	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-setting-added-checkbutton"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(l_pListenerWidget),m_rKernelContext.getConfigurationManager().expandAsBoolean("${SkeletonGenerator_Box_ListenerOnSettingAdded}",false));
	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-setting-removed-checkbutton"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(l_pListenerWidget),m_rKernelContext.getConfigurationManager().expandAsBoolean("${SkeletonGenerator_Box_ListenerOnSettingRemoved}",false));
	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-setting-type-checkbutton"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(l_pListenerWidget),m_rKernelContext.getConfigurationManager().expandAsBoolean("${SkeletonGenerator_Box_ListenerOnSettingTypeChanged}",false));
	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-setting-name-checkbutton"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(l_pListenerWidget),m_rKernelContext.getConfigurationManager().expandAsBoolean("${SkeletonGenerator_Box_ListenerOnSettingNameChanged}",false));
	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-setting-default-checkbutton"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(l_pListenerWidget),m_rKernelContext.getConfigurationManager().expandAsBoolean("${SkeletonGenerator_Box_ListenerOnSettingDefaultValueChanged}",false));
	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-setting-value-checkbutton"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(l_pListenerWidget),m_rKernelContext.getConfigurationManager().expandAsBoolean("${SkeletonGenerator_Box_ListenerOnSettingValueChanged}",false));
	
	::GtkWidget * l_pProcessingMethod = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-process-input-checkbutton"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(l_pProcessingMethod),m_rKernelContext.getConfigurationManager().expandAsBoolean("${SkeletonGenerator_Box_ProcessInput}",false));
	l_pProcessingMethod = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-process-clock-checkbutton"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(l_pProcessingMethod),m_rKernelContext.getConfigurationManager().expandAsBoolean("${SkeletonGenerator_Box_ProcessClock}",false));
	l_pProcessingMethod = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-process-frequency-spinbutton"));
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(l_pProcessingMethod),m_rKernelContext.getConfigurationManager().expandAsUInteger("${SkeletonGenerator_Box_ProcessClock}",1));

	m_rKernelContext.getLogManager() << LogLevel_Info << "box entries from [" << m_sConfigurationFile << "] loaded.\n";
	
	return true;
}

void CBoxAlgorithmSkeletonGenerator::getCurrentParameters(void){
	::GtkWidget * l_pEntryBoxName = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-box-name-entry"));
	m_sName = gtk_entry_get_text(GTK_ENTRY(l_pEntryBoxName));
	
	::GtkWidget * l_pEntryClassName = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-class-name-entry"));
	m_sClassName = gtk_entry_get_text(GTK_ENTRY(l_pEntryClassName));
	
	::GtkWidget * l_pEntryCategory = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-category-entry"));
	m_sCategory = gtk_entry_get_text(GTK_ENTRY(l_pEntryCategory));

	::GtkWidget * l_pEntryVersion = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-version-entry"));
	m_sVersion = gtk_entry_get_text(GTK_ENTRY(l_pEntryVersion));
	
	::GtkWidget * l_pEntryShortDescr = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-short-description-entry"));
	m_sShortDescription = gtk_entry_get_text(GTK_ENTRY(l_pEntryShortDescr));
	
	::GtkWidget * l_pEntryDetailedDescr = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-detailed-description-textview"));
	::GtkTextBuffer * l_pTextBufferDetailedDescr = gtk_text_view_get_buffer(GTK_TEXT_VIEW(l_pEntryDetailedDescr));
	::GtkTextIter l_TextIterStart,l_TextIterEnd;
	gtk_text_buffer_get_start_iter(l_pTextBufferDetailedDescr,&l_TextIterStart);
	gtk_text_buffer_get_end_iter(l_pTextBufferDetailedDescr,&l_TextIterEnd);
	m_sDetailedDescription = gtk_text_buffer_get_text(l_pTextBufferDetailedDescr,&l_TextIterStart,&l_TextIterEnd, false);
	
	::GtkWidget * l_pIconCombobox = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-icon-combobox"));
	::GtkTreeModel * l_pIconListStore = gtk_combo_box_get_model(GTK_COMBO_BOX(l_pIconCombobox));
	::GtkTreeIter l_iterIcon;
	m_i32GtkStockItemIndex = gtk_combo_box_get_active(GTK_COMBO_BOX(l_pIconCombobox)); // can be -1 if nothing selected
	if(m_i32GtkStockItemIndex != -1)
	{
		gtk_tree_model_iter_nth_child(l_pIconListStore,&l_iterIcon,NULL,m_i32GtkStockItemIndex);
		gchar * l_sData;
		gtk_tree_model_get(l_pIconListStore, &l_iterIcon,0, &l_sData,-1);
		m_sGtkStockItemName = CString((const char *)l_sData);
	}
	else
	{
		m_sGtkStockItemName = "";
	}
	::GtkWidget * l_pCanModifyInputsCheckbox = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-inputs-modify-checkbutton"));
	m_bCanModifyInputs = (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(l_pCanModifyInputsCheckbox)) ? true : false);
	::GtkWidget * l_pCanAddInputsCheckbox = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-inputs-add-checkbutton"));
	m_bCanAddInputs = (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(l_pCanAddInputsCheckbox)) ? true : false);
	::GtkWidget * l_pCanModifyOutputsCheckbox = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-outputs-modify-checkbutton"));
	m_bCanModifyOutputs = (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(l_pCanModifyOutputsCheckbox)) ? true : false);
	::GtkWidget * l_pCanAddOutputsCheckbox = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-outputs-add-checkbutton"));
	m_bCanAddOutputs = (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(l_pCanAddOutputsCheckbox)) ? true : false);
	::GtkWidget * l_pCanModifySettingsCheckbox = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-settings-modify-checkbutton"));
	m_bCanModifySettings = (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(l_pCanModifySettingsCheckbox)) ? true : false);
	::GtkWidget * l_pCanAddSettingsCheckbox = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-settings-add-checkbutton"));
	m_bCanAddSettings = (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(l_pCanAddSettingsCheckbox)) ? true : false);

	::GtkWidget * l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-checkbutton"));
	m_bUseBoxListener = (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(l_pListenerWidget)) ? true : false);

	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-input-added-checkbutton"));
	m_bBoxListenerOnInputAdded = (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(l_pListenerWidget)) ? true : false);
	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-input-removed-checkbutton"));
	m_bBoxListenerOnInputRemoved = (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(l_pListenerWidget)) ? true : false);
	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-input-type-checkbutton"));
	m_bBoxListenerOnInputTypeChanged = (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(l_pListenerWidget)) ? true : false);
	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-input-name-checkbutton"));
	m_bBoxListenerOnInputNameChanged = (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(l_pListenerWidget)) ? true : false);
	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-input-connected-checkbutton"));
	m_bBoxListenerOnInputConnected = (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(l_pListenerWidget)) ? true : false);
	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-input-disconnected-checkbutton"));
	m_bBoxListenerOnInputDisconnected = (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(l_pListenerWidget)) ? true : false);
	
	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-output-added-checkbutton"));
	m_bBoxListenerOnOutputAdded = (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(l_pListenerWidget)) ? true : false);
	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-output-removed-checkbutton"));
	m_bBoxListenerOnOutputRemoved = (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(l_pListenerWidget)) ? true : false);
	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-output-type-checkbutton"));
	m_bBoxListenerOnOutputTypeChanged = (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(l_pListenerWidget)) ? true : false);
	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-output-name-checkbutton"));
	m_bBoxListenerOnOutputNameChanged = (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(l_pListenerWidget)) ? true : false);
	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-output-connected-checkbutton"));
	m_bBoxListenerOnOutputConnected = (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(l_pListenerWidget)) ? true : false);
	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-output-disconnected-checkbutton"));
	m_bBoxListenerOnOutputDisconnected = (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(l_pListenerWidget)) ? true : false);

	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-setting-added-checkbutton"));
	m_bBoxListenerOnSettingAdded = (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(l_pListenerWidget)) ? true : false);
	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-setting-removed-checkbutton"));
	m_bBoxListenerOnSettingRemoved = (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(l_pListenerWidget)) ? true : false);
	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-setting-type-checkbutton"));
	m_bBoxListenerOnSettingTypeChanged = (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(l_pListenerWidget)) ? true : false);
	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-setting-name-checkbutton"));
	m_bBoxListenerOnSettingNameChanged = (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(l_pListenerWidget)) ? true : false);
	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-setting-default-checkbutton"));
	m_bBoxListenerOnSettingDefaultValueChanged = (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(l_pListenerWidget)) ? true : false);
	l_pListenerWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-listener-setting-value-checkbutton"));
	m_bBoxListenerOnSettingValueChanged = (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(l_pListenerWidget)) ? true : false);

	::GtkWidget * l_pProcessingMethod = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-process-input-checkbutton"));
	m_bProcessInput = (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(l_pProcessingMethod)) ? true : false);
	l_pProcessingMethod = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-process-clock-checkbutton"));
	m_bProcessClock = (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(l_pProcessingMethod)) ? true : false);
	l_pProcessingMethod = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-process-frequency-spinbutton"));
	m_ui32ClockFrequency = (uint32) gtk_spin_button_get_value(GTK_SPIN_BUTTON(l_pProcessingMethod));

	::GtkWidget * l_pUseCodecToolkitCheckbox = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-algorithms-toolkit-checkbutton"));
	m_bUseCodecToolkit = (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(l_pUseCodecToolkitCheckbox)) ? true : false);

	::GtkWidget * l_pInputsTreeview = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-inputs-treeview"));
	::GtkTreeModel * l_pInputListStore = gtk_tree_view_get_model(GTK_TREE_VIEW(l_pInputsTreeview));
	GtkTreeIter l_iter;
	boolean l_bValid = (gtk_tree_model_get_iter_first(l_pInputListStore,&l_iter) ? true : false);
	gint l_iRowCount = 0;
	m_vInputs.clear();
	while(l_bValid)
	{
		/* Walk through the list, reading each row */
		gchar * l_sName;
		gchar * l_sType;
		gchar * l_sTypeOv;
		gtk_tree_model_get(l_pInputListStore, &l_iter,0, &l_sName,1, &l_sType,2,&l_sTypeOv,-1);
		
		IOSStruct l_struct;
		l_struct._name=l_sName;
		l_struct._type=l_sType;
		l_struct._typeId=l_sTypeOv;
		m_vInputs.push_back(l_struct);
		
		g_free(l_sName);g_free (l_sType);g_free(l_sTypeOv);
		l_iRowCount++;
		l_bValid = (gtk_tree_model_iter_next (l_pInputListStore, &l_iter) ? true : false);
	}

	::GtkWidget * l_pOutputsTreeview = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-outputs-treeview"));
	::GtkTreeModel * l_pOutputListStore = gtk_tree_view_get_model(GTK_TREE_VIEW(l_pOutputsTreeview));
	GtkTreeIter l_iterOutput;
	l_bValid = (gtk_tree_model_get_iter_first(l_pOutputListStore,&l_iterOutput) ? true : false);
	l_iRowCount = 0;
	m_vOutputs.clear();
	while(l_bValid)
	{
		/* Walk through the list, reading each row */
		gchar * l_sName;
		gchar * l_sType;
		gchar * l_sTypeOv;
		gtk_tree_model_get(l_pOutputListStore, &l_iterOutput,0, &l_sName,1, &l_sType,2,&l_sTypeOv,-1);
		
		IOSStruct l_struct;
		l_struct._name=l_sName;
		l_struct._type=l_sType;
		l_struct._typeId=l_sTypeOv;
		m_vOutputs.push_back(l_struct);
			
		g_free(l_sName);g_free(l_sType);g_free(l_sTypeOv);
		l_iRowCount++;
		l_bValid = (gtk_tree_model_iter_next (l_pOutputListStore, &l_iterOutput) ? true : false);
	}

	::GtkWidget * l_pSettingsTreeview = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-settings-treeview"));
	::GtkTreeModel * l_pSettingListStore = gtk_tree_view_get_model(GTK_TREE_VIEW(l_pSettingsTreeview));
	GtkTreeIter l_iterSetting;
	l_bValid = (gtk_tree_model_get_iter_first(l_pSettingListStore,&l_iterSetting) ? true : false);
	l_iRowCount = 0;
	m_vSettings.clear();
	while(l_bValid)
	{
		/* Walk through the list, reading each row */
		gchar * l_sName;
		gchar * l_sType;gchar * l_sTypeOv;
		gchar * l_sDefaultValue;
		gtk_tree_model_get(l_pSettingListStore, &l_iterSetting,0, &l_sName,1, &l_sType, 2, &l_sDefaultValue,3,&l_sTypeOv,-1);
		
		IOSStruct l_struct;
		l_struct._name = l_sName;
		l_struct._type = l_sType;
		l_struct._typeId=l_sTypeOv;
		l_struct._defaultValue = l_sDefaultValue;
		m_vSettings.push_back(l_struct);

		g_free(l_sName);g_free(l_sType);g_free(l_sDefaultValue);g_free(l_sTypeOv);
		l_iRowCount++;
		l_bValid = (gtk_tree_model_iter_next (l_pSettingListStore, &l_iterSetting) ? true : false);
	}

	::GtkWidget * l_pAlgosTreeview = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-algorithms-treeview"));
	::GtkTreeModel * l_pAlgosListStore = gtk_tree_view_get_model(GTK_TREE_VIEW(l_pAlgosTreeview));
	GtkTreeIter l_iterAlgo;
	l_bValid = (gtk_tree_model_get_iter_first(l_pAlgosListStore,&l_iterAlgo) ? true : false);
	l_iRowCount = 0;
	m_vAlgorithms.clear();
	while(l_bValid)
	{
		/* Walk through the list, reading each row */
		gchar * l_sName;
		gtk_tree_model_get(l_pAlgosListStore, &l_iterAlgo,0, &l_sName,-1);

		m_vAlgorithms.push_back(l_sName);

		g_free(l_sName);
		l_iRowCount++;
		l_bValid = (gtk_tree_model_iter_next (l_pAlgosListStore, &l_iterAlgo) ? true : false);
	}
}

CString  CBoxAlgorithmSkeletonGenerator::getRandomIdentifierString(void)
{
	srand((unsigned int)time(NULL));
	unsigned short int l_ui16Value1=(rand()&0xffff);
	unsigned short int l_ui16Value2=(rand()&0xffff);
	unsigned short int l_ui16Value3=(rand()&0xffff);
	unsigned short int l_ui16Value4=(rand()&0xffff);

	char buffer[50];
	sprintf(buffer,"OpenViBE::CIdentifier(0x%04X%04X, 0x%04X%04X)",(int)l_ui16Value1, (int)l_ui16Value2, (int)l_ui16Value3, (int)l_ui16Value4);
	return CString(buffer);
}
