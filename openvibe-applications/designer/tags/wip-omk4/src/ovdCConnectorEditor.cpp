#include "ovdCConnectorEditor.h"

#include <map>
#include <string>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBEDesigner;
using namespace std;

CConnectorEditor::CConnectorEditor(IKernel& rKernel, IBox& rBox, uint32 ui32ConnectorType, uint32 ui32ConnectorIndex, const char* sGUIFilename)
	:m_rKernel(rKernel)
	,m_rBox(rBox)
	,m_ui32ConnectorType(ui32ConnectorType)
	,m_ui32ConnectorIndex(ui32ConnectorIndex)
	,m_sGUIFilename(sGUIFilename)
{
}

CConnectorEditor::~CConnectorEditor(void)
{
}

void CConnectorEditor::run(void)
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
			return;
	}

	CString l_oConnectorName;
	CIdentifier l_oConnectorType;
	(m_rBox.*getConnectorName)(m_ui32ConnectorIndex, l_oConnectorName);
	(m_rBox.*getConnectorType)(m_ui32ConnectorIndex, l_oConnectorType);

	::GladeXML* l_pGladeInterfaceConnector=glade_xml_new(m_sGUIFilename.c_str(), "openvibe_connector", NULL);
	::GtkWidget* l_pConnectorDialog=glade_xml_get_widget(l_pGladeInterfaceConnector, "openvibe_connector");
	::GtkEntry* l_pConnectorNameEntry=GTK_ENTRY(glade_xml_get_widget(l_pGladeInterfaceConnector, "connector_name_entry"));
	::GtkComboBox* l_pConnectorTypeComboBox=GTK_COMBO_BOX(glade_xml_get_widget(l_pGladeInterfaceConnector,"connector_type_combobox"));
	gtk_list_store_clear(GTK_LIST_STORE(gtk_combo_box_get_model(l_pConnectorTypeComboBox)));
	gtk_window_set_title(GTK_WINDOW(l_pConnectorDialog), m_rBox.getName());

	map<string, CIdentifier> m_vStreamTypes;
	CIdentifier l_oCurrentTypeIdentifier;
	gint l_iActive=-1;
	while((l_oCurrentTypeIdentifier=m_rKernel.getContext()->getTypeManager().getNextTypeIdentifier(l_oCurrentTypeIdentifier))!=OV_UndefinedIdentifier)
	{
		if(m_rKernel.getContext()->getTypeManager().isStream(l_oCurrentTypeIdentifier))
		{
			gtk_combo_box_append_text(l_pConnectorTypeComboBox, m_rKernel.getContext()->getTypeManager().getTypeName(l_oCurrentTypeIdentifier).toASCIIString());
			if(l_oCurrentTypeIdentifier==l_oConnectorType)
			{
				l_iActive=m_vStreamTypes.size();
				gtk_combo_box_set_active(l_pConnectorTypeComboBox, l_iActive);
			}
			m_vStreamTypes[m_rKernel.getContext()->getTypeManager().getTypeName(l_oCurrentTypeIdentifier).toASCIIString()]=l_oCurrentTypeIdentifier;
		}
	}

	gtk_entry_set_text(l_pConnectorNameEntry, l_oConnectorName.toASCIIString());

	boolean l_bFinished=false;
	while(!l_bFinished)
	{
		gint l_iResult=gtk_dialog_run(GTK_DIALOG(l_pConnectorDialog));
		if(l_iResult==0) // revert
		{
			(m_rBox.*getConnectorName)(m_ui32ConnectorIndex, l_oConnectorName);
			(m_rBox.*getConnectorType)(m_ui32ConnectorIndex, l_oConnectorType);

			gtk_entry_set_text(l_pConnectorNameEntry, l_oConnectorName.toASCIIString());
			gtk_combo_box_set_active(l_pConnectorTypeComboBox, l_iActive);
		}
		else if(l_iResult==GTK_RESPONSE_APPLY)
		{
			map<string, CIdentifier>::const_iterator it=m_vStreamTypes.find(gtk_combo_box_get_active_text(l_pConnectorTypeComboBox));
			if(it!=m_vStreamTypes.end())
			{
				(m_rBox.*setConnectorType)(m_ui32ConnectorIndex, it->second);
				(m_rBox.*setConnectorName)(m_ui32ConnectorIndex, gtk_entry_get_text(l_pConnectorNameEntry));
				l_bFinished=true;
			}
		}
		else
		{
			l_bFinished=true;
		}
	}

	gtk_widget_destroy(l_pConnectorDialog);

	g_object_unref(l_pGladeInterfaceConnector);
}
