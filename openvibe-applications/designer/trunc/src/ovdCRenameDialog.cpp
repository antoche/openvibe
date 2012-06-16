#include "ovdCRenameDialog.h"

using namespace OpenViBEDesigner;
using namespace OpenViBE;
using namespace OpenViBE::Plugins;
using namespace OpenViBE::Kernel;

CRenameDialog::CRenameDialog(const IKernelContext& rKernelContext, const CString& rInitialName, const CString& rDefaultName, const char* sGUIFilename)
	:m_rKernelContext(rKernelContext)
	,m_sInitialName(rInitialName)
	,m_sDefaultName(rDefaultName)
	,m_sResult(rInitialName)
	,m_sGUIFilename(sGUIFilename)
{
}

CRenameDialog::~CRenameDialog(void)
{
}

boolean CRenameDialog::run(void)
{
	::GtkBuilder* l_pInterface=gtk_builder_new(); // glade_xml_new(m_sGUIFilename.toASCIIString(), "rename", NULL);
	gtk_builder_add_from_file(l_pInterface, m_sGUIFilename.toASCIIString(), NULL);
	gtk_builder_connect_signals(l_pInterface, NULL);

	::GtkWidget* l_pDialog=GTK_WIDGET(gtk_builder_get_object(l_pInterface, "rename"));
	::GtkWidget* l_pName=GTK_WIDGET(gtk_builder_get_object(l_pInterface, "rename-entry"));
	g_object_unref(l_pInterface);

	gtk_entry_set_text(GTK_ENTRY(l_pName), m_sInitialName.toASCIIString());

	boolean l_bFinished=false;
	boolean l_bResult;
	while(!l_bFinished)
	{
		gint l_iResult=gtk_dialog_run(GTK_DIALOG(l_pDialog));
		if(l_iResult==GTK_RESPONSE_APPLY)
		{
			m_sResult=gtk_entry_get_text(GTK_ENTRY(l_pName));
			l_bFinished=true;
			l_bResult=true;
		}
		else if(l_iResult==1) // default
		{
			gtk_entry_set_text(GTK_ENTRY(l_pName), m_sDefaultName.toASCIIString());
		}
		else if(l_iResult==2) // revert
		{
			gtk_entry_set_text(GTK_ENTRY(l_pName), m_sInitialName.toASCIIString());
		}
		else
		{
			l_bFinished=true;
			l_bResult=false;
		}
	}

	gtk_widget_destroy(l_pDialog);

	return l_bResult;
}

CString CRenameDialog::getResult(void)
{
	return m_sResult;
}
