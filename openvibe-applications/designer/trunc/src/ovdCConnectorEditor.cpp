#include "ovdCConnectorEditor.h"

#include <map>
#include <string>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBEDesigner;
using namespace std;

CConnectorEditor::CConnectorEditor(const IKernelContext& rKernelContext, IBox& rBox, uint32 ui32ConnectorType, uint32 ui32ConnectorIndex, const char* sTitle, const char* sGUIFilename)
	:m_rKernelContext(rKernelContext)
	,m_rBox(rBox)
	,m_ui32ConnectorType(ui32ConnectorType)
	,m_ui32ConnectorIndex(ui32ConnectorIndex)
	,m_sGUIFilename(sGUIFilename)
	,m_sTitle(sTitle?sTitle:"")
{
}

CConnectorEditor::~CConnectorEditor(void)
{
}

boolean CConnectorEditor::run(void)
{
	t_getConnectorName getConnectorName=NULL;
	t_setConnectorName setConnectorName=NULL;
	t_getConnectorType getConnectorType=NULL;
	t_setConnectorType setConnectorType=NULL;

	switch(m_ui32ConnectorType)
	{
		case Connector_Input:
			getConnectorName=&IBox::getInputName;
			setConnectorName=&IBox::setInputName;
			getConnectorType=&IBox::getInputType;
			setConnectorType=&IBox::setInputType;
			break;

		case Connector_Output:
			getConnectorName=&IBox::getOutputName;
			setConnectorName=&IBox::setOutputName;
			getConnectorType=&IBox::getOutputType;
			setConnectorType=&IBox::setOutputType;
			break;

		default:
			return false;
	}

	CString l_oConnectorName;
	CIdentifier l_oConnectorType;
	(m_rBox.*getConnectorName)(m_ui32ConnectorIndex, l_oConnectorName);
	(m_rBox.*getConnectorType)(m_ui32ConnectorIndex, l_oConnectorType);

	::GtkBuilder* l_pBuilderInterfaceConnector=gtk_builder_new(); // glade_xml_new(m_sGUIFilename.c_str(), "connector_editor", NULL);
	gtk_builder_add_from_file(l_pBuilderInterfaceConnector, m_sGUIFilename.c_str(), NULL);
	gtk_builder_connect_signals(l_pBuilderInterfaceConnector, NULL);

	::GtkWidget* l_pConnectorDialog=GTK_WIDGET(gtk_builder_get_object(l_pBuilderInterfaceConnector, "connector_editor"));
	::GtkEntry* l_pConnectorNameEntry=GTK_ENTRY(gtk_builder_get_object(l_pBuilderInterfaceConnector, "connector_editor-connector_name_entry"));
	::GtkComboBox* l_pConnectorTypeComboBox=GTK_COMBO_BOX(gtk_builder_get_object(l_pBuilderInterfaceConnector,"connector_editor-connector_type_combobox"));
	gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(l_pConnectorTypeComboBox)));
	gtk_window_set_title(GTK_WINDOW(l_pConnectorDialog), m_sTitle.c_str());

	//get a list of stream types and display connector type
	map<string, CIdentifier> m_vStreamTypes;
	CIdentifier l_oCurrentTypeIdentifier;
	gint l_iActive=-1;
	while((l_oCurrentTypeIdentifier=m_rKernelContext.getTypeManager().getNextTypeIdentifier(l_oCurrentTypeIdentifier))!=OV_UndefinedIdentifier)
	{
		if(m_rKernelContext.getTypeManager().isStream(l_oCurrentTypeIdentifier))
		{
			gtk_combo_box_append_text(l_pConnectorTypeComboBox, m_rKernelContext.getTypeManager().getTypeName(l_oCurrentTypeIdentifier).toASCIIString());
			if(l_oCurrentTypeIdentifier==l_oConnectorType)
			{
				l_iActive=m_vStreamTypes.size();
				gtk_combo_box_set_active(l_pConnectorTypeComboBox, l_iActive);
			}
			m_vStreamTypes[m_rKernelContext.getTypeManager().getTypeName(l_oCurrentTypeIdentifier).toASCIIString()]=l_oCurrentTypeIdentifier;
		}
	}

	//display connector name
	gtk_entry_set_text(l_pConnectorNameEntry, l_oConnectorName.toASCIIString());

	boolean l_bFinished=false;
	boolean l_bResult=false;
	while(!l_bFinished)
	{
		gint l_iResult=gtk_dialog_run(GTK_DIALOG(l_pConnectorDialog));
		if(l_iResult==GTK_RESPONSE_APPLY)
		{
			char* l_sActiveText=gtk_combo_box_get_active_text(l_pConnectorTypeComboBox);
			if(l_sActiveText)
			{
				(m_rBox.*setConnectorType)(m_ui32ConnectorIndex, m_vStreamTypes[l_sActiveText]);
				(m_rBox.*setConnectorName)(m_ui32ConnectorIndex, gtk_entry_get_text(l_pConnectorNameEntry));
				l_bFinished=true;
				l_bResult=true;
			}
		}
		else if(l_iResult==2) // revert
		{
			(m_rBox.*getConnectorName)(m_ui32ConnectorIndex, l_oConnectorName);
			(m_rBox.*getConnectorType)(m_ui32ConnectorIndex, l_oConnectorType);

			gtk_entry_set_text(l_pConnectorNameEntry, l_oConnectorName.toASCIIString());
			gtk_combo_box_set_active(l_pConnectorTypeComboBox, l_iActive);
		}
		else
		{
			l_bFinished=true;
			l_bResult=false;
		}
	}

	gtk_widget_destroy(l_pConnectorDialog);

	g_object_unref(l_pBuilderInterfaceConnector);

	return l_bResult;
}
