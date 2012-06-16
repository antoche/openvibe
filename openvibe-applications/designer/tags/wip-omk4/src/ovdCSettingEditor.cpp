#include "ovdCSettingEditor.h"

#include <vector>
#include <sstream>
#include <iostream>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBEDesigner;
using namespace std;

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

static void collect_widget_cb(::GtkWidget* pWidget, gpointer pUserData)
{
	static_cast< vector< ::GtkWidget* > *>(pUserData)->push_back(pWidget);
}

static void on_button_setting_filename_browse_pressed(::GtkButton* pButton, gpointer pUserData)
{
	vector< ::GtkWidget* > l_vWidget;
	gtk_container_foreach(GTK_CONTAINER(gtk_widget_get_parent(GTK_WIDGET(pButton))), collect_widget_cb, &l_vWidget);
	::GtkEntry* l_pWidget=GTK_ENTRY(l_vWidget[0]);

	::GtkWidget* l_pWidgetDialogOpen=gtk_file_chooser_dialog_new(
		"Select file to open...",
		NULL,
		GTK_FILE_CHOOSER_ACTION_SAVE,
		GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
		GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
		NULL);

	const char* l_sInitialFileName=gtk_entry_get_text(l_pWidget);
	if(g_path_is_absolute(l_sInitialFileName))
	{
		gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(l_pWidgetDialogOpen), l_sInitialFileName);
	}
	else
	{
		char* l_sFullPath=g_build_filename(g_get_current_dir(), l_sInitialFileName, NULL);
		gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(l_pWidgetDialogOpen), l_sFullPath);
		g_free(l_sFullPath);
	}

	gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(l_pWidgetDialogOpen), false);

	if(gtk_dialog_run(GTK_DIALOG(l_pWidgetDialogOpen))==GTK_RESPONSE_ACCEPT)
	{
		char* l_sFileName=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(l_pWidgetDialogOpen));
		gtk_entry_set_text(l_pWidget, l_sFileName);
		g_free(l_sFileName);
	}
	gtk_widget_destroy(l_pWidgetDialogOpen);
}

static void on_file_override_check_toggled(::GtkToggleButton* pToggleButton, gpointer pUserData)
{
	gtk_widget_set_sensitive((::GtkWidget*)pUserData, !gtk_toggle_button_get_active(pToggleButton));
}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

CSettingEditor::CSettingEditor(IKernel& rKernel, IBox& rBox, const char* sGUIFilename)
	:m_rKernel(rKernel)
	,m_rBox(rBox)
	,m_sGUIFilename(sGUIFilename)
{
}

CSettingEditor::~CSettingEditor(void)
{
}

void CSettingEditor::run(void)
{
	if(m_rBox.getSettingCount())
	{
		uint32 i;
		CString l_oSettingName;
		CString l_oSettingValue;
		CIdentifier l_oSettingType;

		::GladeXML* l_pGladeInterfaceSetting=glade_xml_new(m_sGUIFilename.c_str(), "setting", NULL);
		::GtkWidget* l_pSettingDialog=glade_xml_get_widget(l_pGladeInterfaceSetting, "setting");
		::GtkTable* l_pSettingTable=GTK_TABLE(glade_xml_get_widget(l_pGladeInterfaceSetting, "table_setting"));
		::GtkContainer* l_pFileOverrideContainer=GTK_CONTAINER(glade_xml_get_widget(l_pGladeInterfaceSetting, "hbox_filename_override"));
		::GtkCheckButton* l_pFileOverrideCheck=GTK_CHECK_BUTTON(glade_xml_get_widget(l_pGladeInterfaceSetting, "checkbutton_filename_override"));
		g_object_unref(l_pGladeInterfaceSetting);

		gtk_table_resize(l_pSettingTable, m_rBox.getSettingCount(), 3);
		gtk_window_set_title(GTK_WINDOW(l_pSettingDialog), m_rBox.getName());

		vector< ::GtkWidget* > l_vSettingValue;
		for(i=0; i<m_rBox.getSettingCount(); i++)
		{
			m_rBox.getSettingName(i, l_oSettingName);
			m_rBox.getSettingValue(i, l_oSettingValue);
			m_rBox.getSettingType(i, l_oSettingType);

			::GladeXML* l_pGladeInterfaceDummy=glade_xml_new(m_sGUIFilename.c_str(), "dummy_setting_content", NULL);
			::GtkWidget* l_pSettingName=glade_xml_get_widget(l_pGladeInterfaceDummy, "label_setting_name");
			::GtkWidget* l_pSettingRevert=glade_xml_get_widget(l_pGladeInterfaceDummy, "button_setting_revert");
			g_object_unref(l_pGladeInterfaceDummy);

			string l_sWidgetName=getSettingWidgetName(l_oSettingType);
			::GladeXML* l_pGladeInterfaceSettingCollection=glade_xml_new(m_sGUIFilename.c_str(), l_sWidgetName.c_str(), NULL);
			::GtkWidget* l_pSettingValue=glade_xml_get_widget(l_pGladeInterfaceSettingCollection, l_sWidgetName.c_str());
			g_object_unref(l_pGladeInterfaceSettingCollection);

			gtk_widget_ref(l_pSettingName);
			gtk_widget_ref(l_pSettingValue);
			gtk_widget_ref(l_pSettingRevert);
			gtk_widget_unparent(l_pSettingName);
			gtk_widget_unparent(l_pSettingValue);
			gtk_widget_unparent(l_pSettingRevert);
			gtk_table_attach(l_pSettingTable, l_pSettingName,   0, 1, i, i+1, ::GtkAttachOptions(GTK_FILL),            ::GtkAttachOptions(GTK_FILL),            0, 0);
			gtk_table_attach(l_pSettingTable, l_pSettingValue,  1, 2, i, i+1, ::GtkAttachOptions(GTK_FILL|GTK_EXPAND), ::GtkAttachOptions(GTK_FILL|GTK_EXPAND), 0, 0);
			gtk_table_attach(l_pSettingTable, l_pSettingRevert, 2, 3, i, i+1, ::GtkAttachOptions(GTK_SHRINK),          ::GtkAttachOptions(GTK_SHRINK),          0, 0);
			gtk_widget_unref(l_pSettingRevert);
			gtk_widget_unref(l_pSettingValue);
			gtk_widget_unref(l_pSettingName);

			l_vSettingValue.push_back(l_pSettingValue);

			this->setValue(l_oSettingType, l_pSettingValue, l_oSettingValue);
			gtk_label_set_text(GTK_LABEL(l_pSettingName), l_oSettingName);
		}

#if 1
		g_signal_connect(G_OBJECT(l_pFileOverrideCheck), "toggled", G_CALLBACK(on_file_override_check_toggled), GTK_WIDGET(l_pSettingTable));

		string l_sSettingOverrideWidgetName=getSettingWidgetName(OV_TypeId_Filename);
		::GladeXML* l_pGladeInterfaceSettingCollection=glade_xml_new(m_sGUIFilename.c_str(), l_sSettingOverrideWidgetName.c_str(), NULL);
		::GtkWidget* l_pSettingOverrideValue=glade_xml_get_widget(l_pGladeInterfaceSettingCollection, l_sSettingOverrideWidgetName.c_str());
		g_object_unref(l_pGladeInterfaceSettingCollection);

		gtk_widget_ref(l_pSettingOverrideValue);
		gtk_widget_unparent(l_pSettingOverrideValue);
		gtk_container_add(l_pFileOverrideContainer, l_pSettingOverrideValue);
		gtk_widget_unref(l_pSettingOverrideValue);

		if(m_rBox.hasAttribute(OVD_AttributeId_SettingOverrideFilename))
		{
			this->setValue(OV_TypeId_Filename, l_pSettingOverrideValue, m_rBox.getAttributeValue(OVD_AttributeId_SettingOverrideFilename));
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(l_pFileOverrideCheck), true);
			gtk_widget_set_sensitive(GTK_WIDGET(l_pSettingTable), false);
		}
		else
		{
			this->setValue(OV_TypeId_Filename, l_pSettingOverrideValue, "");
		}
#endif

		boolean l_bFinished=false;
		while(!l_bFinished)
		{
			gint l_iResult=gtk_dialog_run(GTK_DIALOG(l_pSettingDialog));
			if(l_iResult==0) // revert
			{
				for(i=0; i<m_rBox.getSettingCount(); i++)
				{
					m_rBox.getSettingType(i, l_oSettingType);
					m_rBox.getSettingDefaultValue(i, l_oSettingValue);
					setValue(l_oSettingType, l_vSettingValue[i], l_oSettingValue);
				}
				if(m_rBox.hasAttribute(OVD_AttributeId_SettingOverrideFilename))
				{
					this->setValue(OV_TypeId_Filename, l_pSettingOverrideValue, m_rBox.getAttributeValue(OVD_AttributeId_SettingOverrideFilename));
					gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(l_pFileOverrideCheck), true);
					gtk_widget_set_sensitive(GTK_WIDGET(l_pSettingTable), false);
				}
			}
			else if(l_iResult==GTK_RESPONSE_APPLY)
			{
				for(i=0; i<m_rBox.getSettingCount(); i++)
				{
					m_rBox.getSettingType(i, l_oSettingType);
					m_rBox.setSettingValue(i, this->getValue(l_oSettingType, l_vSettingValue[i]));
				}
				if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(l_pFileOverrideCheck)))
				{
					if(m_rBox.hasAttribute(OVD_AttributeId_SettingOverrideFilename))
					{
						m_rBox.setAttributeValue(OVD_AttributeId_SettingOverrideFilename, this->getValue(OV_TypeId_Filename, l_pSettingOverrideValue));
					}
					else
					{
						m_rBox.addAttribute(OVD_AttributeId_SettingOverrideFilename, this->getValue(OV_TypeId_Filename, l_pSettingOverrideValue));
					}
				}
				else
				{
					if(m_rBox.hasAttribute(OVD_AttributeId_SettingOverrideFilename))
					{
						m_rBox.removeAttribute(OVD_AttributeId_SettingOverrideFilename);
					}
				}

				l_bFinished=true;
			}
			else
			{
				l_bFinished=true;
			}
		}

		gtk_widget_destroy(l_pSettingDialog);
	}
}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

string CSettingEditor::getSettingWidgetName(const CIdentifier& rTypeIdentifier)
{
	if(rTypeIdentifier==OV_TypeId_Boolean)  return "check_button_setting_boolean";
	if(rTypeIdentifier==OV_TypeId_Integer)  return "spin_button_setting_integer";
	if(rTypeIdentifier==OV_TypeId_Float)    return "spin_button_setting_float";
	if(rTypeIdentifier==OV_TypeId_String)   return "entry_setting_string";
	if(rTypeIdentifier==OV_TypeId_Filename) return "hbox_setting_filename";
	if(m_rKernel.getContext()->getTypeManager().isEnumeration(rTypeIdentifier)) return "combobox_setting_enumeration";
	if(m_rKernel.getContext()->getTypeManager().isBitMask(rTypeIdentifier))     return "table_setting_bitmask";
	return "entry_setting_string";
}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

CString CSettingEditor::getValue(const CIdentifier& rTypeIdentifier, ::GtkWidget* pWidget)
{
	if(rTypeIdentifier==OV_TypeId_Boolean)  return getValueBoolean(pWidget);
	if(rTypeIdentifier==OV_TypeId_Integer)  return getValueInteger(pWidget);
	if(rTypeIdentifier==OV_TypeId_Float)    return getValueFloat(pWidget);
	if(rTypeIdentifier==OV_TypeId_String)   return getValueString(pWidget);
	if(rTypeIdentifier==OV_TypeId_Filename) return getValueFilename(pWidget);
	if(m_rKernel.getContext()->getTypeManager().isEnumeration(rTypeIdentifier)) return getValueEnumeration(rTypeIdentifier, pWidget);
	if(m_rKernel.getContext()->getTypeManager().isBitMask(rTypeIdentifier))     return getValueBitMask(rTypeIdentifier, pWidget);
	return getValueString(pWidget);
}

CString CSettingEditor::getValueBoolean(::GtkWidget* pWidget)
{
	::GtkToggleButton* l_pWidget=GTK_TOGGLE_BUTTON(pWidget);
	boolean l_bActive=gtk_toggle_button_get_active(l_pWidget)?true:false;
	return CString(l_bActive?"true":"false");
}

CString CSettingEditor::getValueInteger(::GtkWidget* pWidget)
{
	::GtkSpinButton* l_pWidget=GTK_SPIN_BUTTON(pWidget);
	int l_iValue=gtk_spin_button_get_value_as_int(l_pWidget);
	char l_sValue[1024];
	sprintf(l_sValue, "%i", l_iValue);
	return CString(l_sValue);
}

CString CSettingEditor::getValueFloat(::GtkWidget* pWidget)
{
	::GtkSpinButton* l_pWidget=GTK_SPIN_BUTTON(pWidget);
	double l_fValue=gtk_spin_button_get_value_as_float(l_pWidget);
	char l_sValue[1024];
	sprintf(l_sValue, "%f", l_fValue);
	return CString(l_sValue);
}

CString CSettingEditor::getValueString(::GtkWidget* pWidget)
{
	::GtkEntry* l_pWidget=GTK_ENTRY(pWidget);
	return CString(gtk_entry_get_text(l_pWidget));
}

CString CSettingEditor::getValueFilename(::GtkWidget* pWidget)
{
	vector< ::GtkWidget* > l_vWidget;
	gtk_container_foreach(GTK_CONTAINER(pWidget), collect_widget_cb, &l_vWidget);
	::GtkEntry* l_pWidget=GTK_ENTRY(l_vWidget[0]);
	return CString(gtk_entry_get_text(l_pWidget));
}

CString CSettingEditor::getValueEnumeration(const CIdentifier& rTypeIdentifier, ::GtkWidget* pWidget)
{
	::GtkComboBox* l_pWidget=GTK_COMBO_BOX(pWidget);
	return CString(gtk_combo_box_get_active_text(l_pWidget));
}

// ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- ----------- -----------

CString CSettingEditor::getValueBitMask(const CIdentifier& rTypeIdentifier, ::GtkWidget* pWidget)
{
	vector< ::GtkWidget* > l_vWidget;
	gtk_container_foreach(GTK_CONTAINER(pWidget), collect_widget_cb, &l_vWidget);
	string l_sResult;
	for(unsigned int i=0; i<l_vWidget.size(); i++)
	{
		if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(l_vWidget[i])))
		{
			if(l_sResult!="")
			{
				l_sResult+=string(1, OV_Value_EnumeratedStringSeparator);
			}
			l_sResult+=gtk_button_get_label(GTK_BUTTON(l_vWidget[i]));
		}
	}

	return CString(l_sResult.c_str());
}

void CSettingEditor::setValue(const CIdentifier& rTypeIdentifier, ::GtkWidget* pWidget, const CString& rValue)
{
	if(rTypeIdentifier==OV_TypeId_Boolean)  return setValueBoolean(pWidget, rValue);
	if(rTypeIdentifier==OV_TypeId_Integer)  return setValueInteger(pWidget, rValue);
	if(rTypeIdentifier==OV_TypeId_Float)    return setValueFloat(pWidget, rValue);
	if(rTypeIdentifier==OV_TypeId_String)   return setValueString(pWidget, rValue);
	if(rTypeIdentifier==OV_TypeId_Filename) return setValueFilename(pWidget, rValue);
	if(m_rKernel.getContext()->getTypeManager().isEnumeration(rTypeIdentifier)) return setValueEnumeration(rTypeIdentifier, pWidget, rValue);
	if(m_rKernel.getContext()->getTypeManager().isBitMask(rTypeIdentifier))     return setValueBitMask(rTypeIdentifier, pWidget, rValue);
	return setValueString(pWidget, rValue);
}

void CSettingEditor::setValueBoolean(::GtkWidget* pWidget, const CString& rValue)
{
	::GtkToggleButton* l_pWidget=GTK_TOGGLE_BUTTON(pWidget);
	if(rValue==CString("true"))
	{
		gtk_toggle_button_set_active(l_pWidget, true);
	}
	else if(rValue==CString("false"))
	{
		gtk_toggle_button_set_active(l_pWidget, false);
	}
	else
	{
		gtk_toggle_button_set_active(l_pWidget, false);
	}
}

void CSettingEditor::setValueInteger(::GtkWidget* pWidget, const CString& rValue)
{
	::GtkSpinButton* l_pWidget=GTK_SPIN_BUTTON(pWidget);
	int l_iValue=0;
	sscanf(rValue, "%i", &l_iValue);
	gtk_spin_button_set_range(l_pWidget, -G_MAXDOUBLE, G_MAXDOUBLE);
	gtk_spin_button_set_value(l_pWidget, l_iValue);
}

void CSettingEditor::setValueFloat(::GtkWidget* pWidget, const CString& rValue)
{
	::GtkSpinButton* l_pWidget=GTK_SPIN_BUTTON(pWidget);
	double l_fValue=0;
	sscanf(rValue, "%lf", &l_fValue);
	gtk_spin_button_set_range(l_pWidget, -G_MAXDOUBLE, G_MAXDOUBLE);
	gtk_spin_button_set_value(l_pWidget, l_fValue);
}

void CSettingEditor::setValueString(::GtkWidget* pWidget, const CString& rValue)
{
	::GtkEntry* l_pWidget=GTK_ENTRY(pWidget);
	gtk_entry_set_text(l_pWidget, rValue);
}

void CSettingEditor::setValueFilename(::GtkWidget* pWidget, const CString& rValue)
{
	vector< ::GtkWidget* > l_vWidget;
	gtk_container_foreach(GTK_CONTAINER(pWidget), collect_widget_cb, &l_vWidget);
	::GtkEntry* l_pWidget=GTK_ENTRY(l_vWidget[0]);

	g_signal_connect(G_OBJECT(l_vWidget[1]), "clicked", G_CALLBACK(on_button_setting_filename_browse_pressed), NULL);

	gtk_entry_set_text(l_pWidget, rValue);
}

void CSettingEditor::setValueEnumeration(const CIdentifier& rTypeIdentifier, ::GtkWidget* pWidget, const CString& rValue)
{
	::GtkTreeIter l_oListIter;
	::GtkComboBox* l_pWidget=GTK_COMBO_BOX(pWidget);
	::GtkListStore* l_pList=GTK_LIST_STORE(gtk_combo_box_get_model(l_pWidget));

	gtk_list_store_clear(l_pList);
	for(uint64 i=0; i<m_rKernel.getContext()->getTypeManager().getEnumerationEntryCount(rTypeIdentifier); i++)
	{
		CString l_sEntryName;
		uint64 l_ui64EntryValue;
		if(m_rKernel.getContext()->getTypeManager().getEnumerationEntry(rTypeIdentifier, i, l_sEntryName, l_ui64EntryValue))
		{
			gtk_list_store_append(l_pList, &l_oListIter);
			gtk_list_store_set(l_pList, &l_oListIter, 0, (const char*)l_sEntryName, -1);
			if(l_sEntryName==rValue)
			{
				gtk_combo_box_set_active(l_pWidget, i);
			}
		}
	}
	if(gtk_combo_box_get_active(l_pWidget)==-1)
	{
		gtk_combo_box_set_active(l_pWidget, 0);
	}
}

void CSettingEditor::setValueBitMask(const CIdentifier& rTypeIdentifier, ::GtkWidget* pWidget, const CString& rValue)
{
	string l_sValue(rValue);
	::GtkTable* l_pBitMaskTable=GTK_TABLE(pWidget);
	gtk_table_resize(l_pBitMaskTable, 2, (m_rKernel.getContext()->getTypeManager().getBitMaskEntryCount(rTypeIdentifier)+1)>>1);

	for(uint64 i=0; i<m_rKernel.getContext()->getTypeManager().getBitMaskEntryCount(rTypeIdentifier); i++)
	{
		CString l_sEntryName;
		uint64 l_ui64EntryValue;
		if(m_rKernel.getContext()->getTypeManager().getBitMaskEntry(rTypeIdentifier, i, l_sEntryName, l_ui64EntryValue))
		{
			::GladeXML* l_pGladeInterfaceDummy=glade_xml_new(m_sGUIFilename.c_str(), "check_button_setting_boolean", NULL);
			::GtkWidget* l_pSettingButton=glade_xml_get_widget(l_pGladeInterfaceDummy, "check_button_setting_boolean");
			g_object_unref(l_pGladeInterfaceDummy);

			gtk_widget_ref(l_pSettingButton);
			gtk_widget_unparent(l_pSettingButton);
			gtk_table_attach_defaults(l_pBitMaskTable, l_pSettingButton, (i&1), (i&1)+1, (i>>1), (i>>1)+1);
			gtk_widget_unref(l_pSettingButton);

			gtk_button_set_label(GTK_BUTTON(l_pSettingButton), (const char*)l_sEntryName);

			if(l_sValue.find((const char*)l_sEntryName)!=string::npos)
			{
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(l_pSettingButton), true);
			}
		}
	}
}
