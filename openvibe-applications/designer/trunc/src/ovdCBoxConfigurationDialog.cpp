#include "ovdCBoxConfigurationDialog.h"
#include "ovdCSettingCollectionHelper.h"

#include <vector>
#include <string>
#include <fstream>

#include <xml/IReader.h>
#include <xml/IWriter.h>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBEDesigner;
using namespace std;

#include <iostream>
namespace
{
	typedef struct
	{
		const IKernelContext& m_rKernelContext;
		vector< ::GtkWidget* >& m_vSettingValue;
		CSettingCollectionHelper& m_rHelper;
		::GtkWidget* m_pSettingOverrideValue;
		IBox& m_rBox;
	} SButtonCB;

	class CXMLWriterCallback : public XML::IWriterCallback
	{
	public:

		CXMLWriterCallback(const string& sFilename)
		{
			m_pFile=fopen(sFilename.c_str(), "wt");
		}

		virtual ~CXMLWriterCallback(void)
		{
			if(m_pFile)
			{
				fclose(m_pFile);
			}
		}

		virtual void write(const char* sString)
		{
			if(m_pFile)
			{
				fprintf(m_pFile, "%s", sString);
			}
		}

	protected:

		FILE* m_pFile;
	};

	class CXMLReaderCallback : public XML::IReaderCallback
	{
	public:

		CXMLReaderCallback(SButtonCB& rButtonCB)
			:m_ui32Status(Status_ParsingNone)
			,m_rButtonCB(rButtonCB)
		{
		}

		virtual ~CXMLReaderCallback(void)
		{
		}

		virtual void openChild(const char* sName, const char** sAttributeName, const char** sAttributeValue, XML::uint64 ui64AttributeCount)
		{
			string l_sName(sName);
			switch(m_ui32Status)
			{
				case Status_ParsingNone:
					if(l_sName=="OpenViBE-SettingsOverride")
					{
						m_ui32Status=Status_ParsingSettingsOverride;
						m_ui32SettingIndex=(uint32)-1;
					}
					break;

				case Status_ParsingSettingsOverride:
					if(l_sName=="SettingValue")
					{
						m_ui32Status=Status_ParsingSettingValue;
						m_ui32SettingIndex++;
					}
					break;
			}
		}

		virtual void processChildData(const char* sData)
		{
			CIdentifier l_oSettingType;
			switch(m_ui32Status)
			{
				case Status_ParsingSettingValue:
					if(m_ui32SettingIndex<m_rButtonCB.m_rBox.getSettingCount())
					{
						m_rButtonCB.m_rBox.getSettingType(m_ui32SettingIndex, l_oSettingType);
						m_rButtonCB.m_rHelper.setValue(l_oSettingType, m_rButtonCB.m_vSettingValue[m_ui32SettingIndex], sData);
					}
					break;
			}
		}

		virtual void closeChild(void)
		{
			switch(m_ui32Status)
			{
				case Status_ParsingSettingValue:
					m_ui32Status=Status_ParsingSettingsOverride;
					break;

				case Status_ParsingSettingsOverride:
					m_ui32Status=Status_ParsingNone;
					break;
			}
		}

	protected:

		enum
		{
			Status_ParsingNone,
			Status_ParsingSettingsOverride,
			Status_ParsingSettingValue,
		};

		uint32 m_ui32Status;
		uint32 m_ui32SettingIndex;
		SButtonCB& m_rButtonCB;
	};
};

static void on_file_override_check_toggled(::GtkToggleButton* pToggleButton, gpointer pUserData)
{
	gtk_widget_set_sensitive((::GtkWidget*)pUserData, !gtk_toggle_button_get_active(pToggleButton));
}

static void on_button_load_clicked(::GtkButton* pButton, gpointer pUserData)
{
	SButtonCB* l_pUserData=static_cast < SButtonCB* >(pUserData);

	::GtkWidget* l_pWidgetDialogOpen=gtk_file_chooser_dialog_new(
		"Select file to load settings from...",
		NULL,
		GTK_FILE_CHOOSER_ACTION_OPEN,
		GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
		GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
		NULL);

	CString l_sInitialFileName=l_pUserData->m_rKernelContext.getConfigurationManager().expand(l_pUserData->m_rHelper.getValue(OV_TypeId_Filename, l_pUserData->m_pSettingOverrideValue));
	if(g_path_is_absolute(l_sInitialFileName.toASCIIString()))
	{
		gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(l_pWidgetDialogOpen), l_sInitialFileName.toASCIIString());
	}
	else
	{
		char* l_sFullPath=g_build_filename(g_get_current_dir(), l_sInitialFileName.toASCIIString(), NULL);
		gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(l_pWidgetDialogOpen), l_sFullPath);
		g_free(l_sFullPath);
	}

	if(gtk_dialog_run(GTK_DIALOG(l_pWidgetDialogOpen))==GTK_RESPONSE_ACCEPT)
	{
		char* l_sFileName=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(l_pWidgetDialogOpen));

		CXMLReaderCallback l_oCB(*l_pUserData);
		XML::IReader* l_pReader=XML::createReader(l_oCB);

		ifstream l_oFile(l_sFileName, ios::binary);
		if(l_oFile.is_open())
		{
			bool l_bStatusOk=true;
			char l_sBuffer[1024];
			size_t l_iBufferLen=0;
			size_t l_iFileLen;
			l_oFile.seekg(0, ios::end);
			l_iFileLen=l_oFile.tellg();
			l_oFile.seekg(0, ios::beg);
			while(l_iFileLen && l_bStatusOk)
			{
				l_iBufferLen=(l_iFileLen>sizeof(l_sBuffer)?sizeof(l_sBuffer):l_iFileLen);
				l_oFile.read(l_sBuffer, l_iBufferLen);
				l_iFileLen-=l_iBufferLen;
				l_bStatusOk=l_pReader->processData(l_sBuffer, l_iBufferLen);
			}
			l_oFile.close();
		}
		l_pReader->release();

		g_free(l_sFileName);
	}
	gtk_widget_destroy(l_pWidgetDialogOpen);
}

static void on_button_save_clicked(::GtkButton* pButton, gpointer pUserData)
{
	SButtonCB* l_pUserData=static_cast < SButtonCB* >(pUserData);

	::GtkWidget* l_pWidgetDialogOpen=gtk_file_chooser_dialog_new(
		"Select file to save settings to...",
		NULL,
		GTK_FILE_CHOOSER_ACTION_SAVE,
		GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
		GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
		NULL);

	CString l_sInitialFileName=l_pUserData->m_rKernelContext.getConfigurationManager().expand(l_pUserData->m_rHelper.getValue(OV_TypeId_Filename, l_pUserData->m_pSettingOverrideValue));
	if(g_path_is_absolute(l_sInitialFileName.toASCIIString()))
	{
		gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(l_pWidgetDialogOpen), l_sInitialFileName.toASCIIString());
	}
	else
	{
		char* l_sFullPath=g_build_filename(g_get_current_dir(), l_sInitialFileName.toASCIIString(), NULL);
		gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(l_pWidgetDialogOpen), l_sFullPath);
		g_free(l_sFullPath);
	}

	if(gtk_dialog_run(GTK_DIALOG(l_pWidgetDialogOpen))==GTK_RESPONSE_ACCEPT)
	{
		char* l_sFileName=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(l_pWidgetDialogOpen));

		CXMLWriterCallback l_oCB(l_sFileName);
		XML::IWriter* l_pWriter=XML::createWriter(l_oCB);
		l_pWriter->openChild("OpenViBE-SettingsOverride");
		for(unsigned int i=0; i<l_pUserData->m_vSettingValue.size(); i++)
		{
			CIdentifier l_oSettingType;
			l_pUserData->m_rBox.getSettingType(i, l_oSettingType);

			l_pWriter->openChild("SettingValue");
			l_pWriter->setChildData(l_pUserData->m_rHelper.getValue(l_oSettingType, l_pUserData->m_vSettingValue[i]));
			l_pWriter->closeChild();
		}
		l_pWriter->closeChild();
		l_pWriter->release();

		g_free(l_sFileName);
	}
	gtk_widget_destroy(l_pWidgetDialogOpen);
}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

CBoxConfigurationDialog::CBoxConfigurationDialog(const IKernelContext& rKernelContext, IBox& rBox, const char* sGUIFilename, const char* sGUISettingsFilename)
	:m_rKernelContext(rKernelContext)
	,m_rBox(rBox)
	,m_sGUIFilename(sGUIFilename)
	,m_sGUISettingsFilename(sGUISettingsFilename)
{
}

CBoxConfigurationDialog::~CBoxConfigurationDialog(void)
{
}

boolean CBoxConfigurationDialog::run(void)
{
	boolean l_bModified=false;
	CSettingCollectionHelper l_oHelper(m_rKernelContext, m_sGUISettingsFilename.toASCIIString());
	if(m_rBox.getSettingCount())
	{
		uint32 i;
		CString l_oSettingName;
		CString l_oSettingValue;
		CIdentifier l_oSettingType;

		::GtkBuilder* l_pBuilderInterfaceSetting=gtk_builder_new(); // glade_xml_new(m_sGUIFilename.toASCIIString(), "box_configuration", NULL);
		gtk_builder_add_from_file(l_pBuilderInterfaceSetting, m_sGUIFilename.toASCIIString(), NULL);
		gtk_builder_connect_signals(l_pBuilderInterfaceSetting, NULL);

#if 1 // this approach fails to set a modal dialog

		::GtkWidget* l_pSettingDialog=GTK_WIDGET(gtk_builder_get_object(l_pBuilderInterfaceSetting, "box_configuration"));
		char l_sTitle[1024];
		sprintf(l_sTitle, "Configure %s settings", m_rBox.getName().toASCIIString());
		gtk_window_set_title(GTK_WINDOW(l_pSettingDialog), l_sTitle);
#else
		::GtkWidget *l_pSettingDialog = gtk_dialog_new_with_buttons(
			"Configure Box Settings",
			&m_rMainWindow, //set dialog transient for main window
			GTK_DIALOG_MODAL,
			"Revert", 0, "Apply", GTK_RESPONSE_APPLY, "Cancel", GTK_RESPONSE_CANCEL, NULL); //set up action buttons

		//unparent contents from builder interface
		::GtkWidget* l_pContents=GTK_WIDGET(gtk_builder_get_object(l_pBuilderInterfaceSetting, "box_configuration-table"));
		gtk_container_remove(GTK_CONTAINER(gtk_widget_get_parent(l_pContents)), l_pContents);

		//add contents to dialog
		::GtkWidget* l_pContentsArea=GTK_DIALOG(l_pSettingDialog)->vbox; //gtk_dialog_get_content_area() not available in current Gtk distribution
		gtk_container_add(GTK_CONTAINER(l_pContentsArea), l_pContents);

		//action buttons can't be unparented from builder interface and added to dialog, which is why they are added at dialog creation time
#endif
		::GtkTable* l_pSettingTable=GTK_TABLE(gtk_builder_get_object(l_pBuilderInterfaceSetting, "box_configuration-table"));
		::GtkContainer* l_pFileOverrideContainer=GTK_CONTAINER(gtk_builder_get_object(l_pBuilderInterfaceSetting, "box_configuration-hbox_filename_override"));
		::GtkCheckButton* l_pFileOverrideCheck=GTK_CHECK_BUTTON(gtk_builder_get_object(l_pBuilderInterfaceSetting, "box_configuration-checkbutton_filename_override"));
		::GtkButton* l_pButtonLoad=GTK_BUTTON(gtk_builder_get_object(l_pBuilderInterfaceSetting, "box_configuration-button_load_current_from_file"));
		::GtkButton* l_pButtonSave=GTK_BUTTON(gtk_builder_get_object(l_pBuilderInterfaceSetting, "box_configuration-button_save_current_to_file"));
		g_object_unref(l_pBuilderInterfaceSetting);

		gtk_table_resize(l_pSettingTable, m_rBox.getSettingCount(), 4);

		vector< ::GtkWidget* > l_vSettingValue;
		for(i=0; i<m_rBox.getSettingCount(); i++)
		{
			m_rBox.getSettingName(i, l_oSettingName);
			m_rBox.getSettingValue(i, l_oSettingValue);
			m_rBox.getSettingType(i, l_oSettingType);

			::GtkBuilder* l_pBuilderInterfaceDummy=gtk_builder_new(); // glade_xml_new(m_sGUIFilename.toASCIIString(), "settings_collection-dummy_setting_content", NULL);
			gtk_builder_add_from_file(l_pBuilderInterfaceDummy, m_sGUISettingsFilename.toASCIIString(), NULL);
			gtk_builder_connect_signals(l_pBuilderInterfaceDummy, NULL);

			::GtkWidget* l_pSettingName=GTK_WIDGET(gtk_builder_get_object(l_pBuilderInterfaceDummy, "settings_collection-label_setting_name"));
			::GtkWidget* l_pSettingRevert=GTK_WIDGET(gtk_builder_get_object(l_pBuilderInterfaceDummy, "settings_collection-button_setting_revert"));
			::GtkWidget* l_pSettingDefault=GTK_WIDGET(gtk_builder_get_object(l_pBuilderInterfaceDummy, "settings_collection-button_setting_default"));

			string l_sWidgetName=l_oHelper.getSettingWidgetName(l_oSettingType).toASCIIString();
			::GtkBuilder* l_pBuilderInterfaceSettingCollection=gtk_builder_new(); // glade_xml_new(m_sGUIFilename.toASCIIString(), l_sWidgetName.c_str(), NULL);
			gtk_builder_add_from_file(l_pBuilderInterfaceSettingCollection, m_sGUISettingsFilename.toASCIIString(), NULL);
			gtk_builder_connect_signals(l_pBuilderInterfaceSettingCollection, NULL);

			::GtkWidget* l_pSettingValue=GTK_WIDGET(gtk_builder_get_object(l_pBuilderInterfaceSettingCollection, l_sWidgetName.c_str()));

			gtk_container_remove(GTK_CONTAINER(gtk_widget_get_parent(l_pSettingName)), l_pSettingName);
			gtk_container_remove(GTK_CONTAINER(gtk_widget_get_parent(l_pSettingValue)), l_pSettingValue);
			gtk_container_remove(GTK_CONTAINER(gtk_widget_get_parent(l_pSettingRevert)), l_pSettingRevert);
			gtk_container_remove(GTK_CONTAINER(gtk_widget_get_parent(l_pSettingDefault)), l_pSettingDefault);

			gtk_table_attach(l_pSettingTable, l_pSettingName,    0, 1, i, i+1, ::GtkAttachOptions(GTK_FILL),            ::GtkAttachOptions(GTK_FILL),            0, 0);
			gtk_table_attach(l_pSettingTable, l_pSettingValue,   1, 2, i, i+1, ::GtkAttachOptions(GTK_FILL|GTK_EXPAND), ::GtkAttachOptions(GTK_FILL|GTK_EXPAND), 0, 0);
			gtk_table_attach(l_pSettingTable, l_pSettingRevert,  3, 4, i, i+1, ::GtkAttachOptions(GTK_SHRINK),          ::GtkAttachOptions(GTK_SHRINK),          0, 0);
			gtk_table_attach(l_pSettingTable, l_pSettingDefault, 2, 3, i, i+1, ::GtkAttachOptions(GTK_SHRINK),          ::GtkAttachOptions(GTK_SHRINK),          0, 0);

			g_object_unref(l_pBuilderInterfaceDummy);
			g_object_unref(l_pBuilderInterfaceSettingCollection);

			l_vSettingValue.push_back(l_pSettingValue);

			l_oHelper.setValue(l_oSettingType, l_pSettingValue, l_oSettingValue);
			gtk_label_set_text(GTK_LABEL(l_pSettingName), l_oSettingName);
		}

#if 1
		string l_sSettingOverrideWidgetName=l_oHelper.getSettingWidgetName(OV_TypeId_Filename).toASCIIString();
		::GtkBuilder* l_pBuilderInterfaceSettingCollection=gtk_builder_new(); // glade_xml_new(m_sGUIFilename.toASCIIString(), l_sSettingOverrideWidgetName.c_str(), NULL);
		gtk_builder_add_from_file(l_pBuilderInterfaceSettingCollection, m_sGUISettingsFilename.toASCIIString(), NULL);
		gtk_builder_connect_signals(l_pBuilderInterfaceSettingCollection, NULL);

		::GtkWidget* l_pSettingOverrideValue=GTK_WIDGET(gtk_builder_get_object(l_pBuilderInterfaceSettingCollection, l_sSettingOverrideWidgetName.c_str()));

		gtk_container_remove(GTK_CONTAINER(gtk_widget_get_parent(l_pSettingOverrideValue)), l_pSettingOverrideValue);
		gtk_container_add(l_pFileOverrideContainer, l_pSettingOverrideValue);

		g_object_unref(l_pBuilderInterfaceSettingCollection);

		SButtonCB l_oButtonCB = { m_rKernelContext, l_vSettingValue, l_oHelper, l_pSettingOverrideValue, m_rBox };

		g_signal_connect(G_OBJECT(l_pFileOverrideCheck), "toggled", G_CALLBACK(on_file_override_check_toggled), GTK_WIDGET(l_pSettingTable));
		g_signal_connect(G_OBJECT(l_pButtonLoad),        "clicked", G_CALLBACK(on_button_load_clicked), &l_oButtonCB);
		g_signal_connect(G_OBJECT(l_pButtonSave),        "clicked", G_CALLBACK(on_button_save_clicked), &l_oButtonCB);

		if(m_rBox.hasAttribute(OV_AttributeId_Box_SettingOverrideFilename))
		{
			l_oHelper.setValue(OV_TypeId_Filename, l_pSettingOverrideValue, m_rBox.getAttributeValue(OV_AttributeId_Box_SettingOverrideFilename));
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(l_pFileOverrideCheck), true);
			gtk_widget_set_sensitive(GTK_WIDGET(l_pSettingTable), false);
		}
		else
		{
			l_oHelper.setValue(OV_TypeId_Filename, l_pSettingOverrideValue, "");
		}
#endif

		boolean l_bFinished=false;
		while(!l_bFinished)
		{
			gint l_iResult=gtk_dialog_run(GTK_DIALOG(l_pSettingDialog));
			if(l_iResult==GTK_RESPONSE_APPLY)
			{
				for(i=0; i<m_rBox.getSettingCount() && i<l_vSettingValue.size(); i++)
				{
					m_rBox.getSettingType(i, l_oSettingType);
					m_rBox.setSettingValue(i, l_oHelper.getValue(l_oSettingType, l_vSettingValue[i]));
				}
				if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(l_pFileOverrideCheck)))
				{
					if(m_rBox.hasAttribute(OV_AttributeId_Box_SettingOverrideFilename))
					{
						m_rBox.setAttributeValue(OV_AttributeId_Box_SettingOverrideFilename, l_oHelper.getValue(OV_TypeId_Filename, l_pSettingOverrideValue));
					}
					else
					{
						m_rBox.addAttribute(OV_AttributeId_Box_SettingOverrideFilename, l_oHelper.getValue(OV_TypeId_Filename, l_pSettingOverrideValue));
					}
				}
				else
				{
					if(m_rBox.hasAttribute(OV_AttributeId_Box_SettingOverrideFilename))
					{
						m_rBox.removeAttribute(OV_AttributeId_Box_SettingOverrideFilename);
					}
				}

				l_bFinished=true;
				l_bModified=true;
			}
			else if(l_iResult==1) // default
			{
				for(i=0; i<m_rBox.getSettingCount(); i++)
				{
					m_rBox.getSettingType(i, l_oSettingType);
					m_rBox.getSettingDefaultValue(i, l_oSettingValue);
					l_oHelper.setValue(l_oSettingType, i<l_vSettingValue.size()?l_vSettingValue[i]:NULL, l_oSettingValue);
				}
				l_oHelper.setValue(OV_TypeId_Filename, l_pSettingOverrideValue, "");
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(l_pFileOverrideCheck), false);
				gtk_widget_set_sensitive(GTK_WIDGET(l_pSettingTable), true);
				l_bModified=false;
			}
			else if(l_iResult==2) // revert
			{
				for(i=0; i<m_rBox.getSettingCount(); i++)
				{
					m_rBox.getSettingType(i, l_oSettingType);
					m_rBox.getSettingValue(i, l_oSettingValue);
					l_oHelper.setValue(l_oSettingType, i<l_vSettingValue.size()?l_vSettingValue[i]:NULL, l_oSettingValue);
				}
				if(m_rBox.hasAttribute(OV_AttributeId_Box_SettingOverrideFilename))
				{
					l_oHelper.setValue(OV_TypeId_Filename, l_pSettingOverrideValue, m_rBox.getAttributeValue(OV_AttributeId_Box_SettingOverrideFilename));
					gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(l_pFileOverrideCheck), true);
					gtk_widget_set_sensitive(GTK_WIDGET(l_pSettingTable), false);
				}
			}
			else if(l_iResult==3) // load
			{
				l_bModified=true;
			}
			else if(l_iResult==4) // save
			{
			}
			else
			{
				l_bFinished=true;
			}
		}

		gtk_widget_destroy(l_pSettingDialog);
	}
	return l_bModified;
}
