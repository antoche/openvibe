#include "ovdCCommentEditorDialog.h"

#include <stdlib.h>
using namespace OpenViBEDesigner;
using namespace OpenViBE;
using namespace OpenViBE::Plugins;
using namespace OpenViBE::Kernel;

CCommentEditorDialog::CCommentEditorDialog(const IKernelContext& rKernelContext, IComment& rComment, const char* sGUIFilename)
	:m_rKernelContext(rKernelContext)
	,m_rComment(rComment)
	,m_sGUIFilename(sGUIFilename)
{
}

CCommentEditorDialog::~CCommentEditorDialog(void)
{
}

boolean CCommentEditorDialog::run(void)
{
	boolean l_bResult=false;

	::GtkBuilder* l_pInterface=gtk_builder_new(); // glade_xml_new(m_sGUIFilename.toASCIIString(), "comment", NULL);
	gtk_builder_add_from_file(l_pInterface, m_sGUIFilename.toASCIIString(), NULL);
	gtk_builder_connect_signals(l_pInterface, NULL);

	::GtkWidget* l_pDialog=GTK_WIDGET(gtk_builder_get_object(l_pInterface, "comment"));
	::GtkWidget* l_pDescription=GTK_WIDGET(gtk_builder_get_object(l_pInterface, "comment-textview_description"));
	g_object_unref(l_pInterface);

	::GtkTextBuffer* l_pDescriptionBuffer=gtk_text_buffer_new(NULL);
	gtk_text_buffer_set_text(l_pDescriptionBuffer, m_rComment.getText().toASCIIString(), -1);
	gtk_text_view_set_buffer(GTK_TEXT_VIEW(l_pDescription), l_pDescriptionBuffer);
	g_object_unref(l_pDescriptionBuffer);

	gint l_iResult=gtk_dialog_run(GTK_DIALOG(l_pDialog));
	if(l_iResult==GTK_RESPONSE_APPLY)
	{
		l_bResult=true;

		::GtkTextIter l_oStartIter;
		::GtkTextIter l_oEndIter;
		l_pDescriptionBuffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(l_pDescription));
		gtk_text_buffer_get_start_iter(l_pDescriptionBuffer, &l_oStartIter);
		gtk_text_buffer_get_end_iter(l_pDescriptionBuffer, &l_oEndIter);
		m_rComment.setText(gtk_text_buffer_get_text(l_pDescriptionBuffer, &l_oStartIter, &l_oEndIter, TRUE));
	}

	gtk_widget_destroy(l_pDialog);

	return l_bResult;
}
