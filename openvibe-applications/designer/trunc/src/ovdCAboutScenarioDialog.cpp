#include "ovdCAboutScenarioDialog.h"

using namespace OpenViBEDesigner;
using namespace OpenViBE;
using namespace OpenViBE::Plugins;
using namespace OpenViBE::Kernel;

CAboutScenarioDialog::CAboutScenarioDialog(const IKernelContext& rKernelContext, IScenario& rScenario, const char* sGUIFilename)
	:m_rKernelContext(rKernelContext)
	,m_rScenario(rScenario)
	,m_sGUIFilename(sGUIFilename)
{
}

CAboutScenarioDialog::~CAboutScenarioDialog(void)
{
}

boolean CAboutScenarioDialog::run(void)
{
	::GtkBuilder* l_pInterface=gtk_builder_new(); // glade_xml_new(m_sGUIFilename.toASCIIString(), "scenario_about", NULL);
	gtk_builder_add_from_file(l_pInterface, m_sGUIFilename.toASCIIString(), NULL);
	gtk_builder_connect_signals(l_pInterface, NULL);

	::GtkWidget* l_pDialog=GTK_WIDGET(gtk_builder_get_object(l_pInterface, "scenario_about"));
	::GtkWidget* l_pName=GTK_WIDGET(gtk_builder_get_object(l_pInterface, "scenario_about-entry_name"));
	::GtkWidget* l_pAuthorName=GTK_WIDGET(gtk_builder_get_object(l_pInterface, "scenario_about-entry_author_name"));
	::GtkWidget* l_pAuthorCompanyName=GTK_WIDGET(gtk_builder_get_object(l_pInterface, "scenario_about-entry_company_name"));
	::GtkWidget* l_pCategory=GTK_WIDGET(gtk_builder_get_object(l_pInterface, "scenario_about-entry_category"));
	::GtkWidget* l_pVersion=GTK_WIDGET(gtk_builder_get_object(l_pInterface, "scenario_about-entry_version"));
	::GtkWidget* l_pDocumentationPage=GTK_WIDGET(gtk_builder_get_object(l_pInterface, "scenario_about-entry_documentation_page"));
	g_object_unref(l_pInterface);

	gtk_entry_set_text(GTK_ENTRY(l_pName), m_rScenario.getAttributeValue(OV_AttributeId_Scenario_Name).toASCIIString());
	gtk_entry_set_text(GTK_ENTRY(l_pAuthorName), m_rScenario.getAttributeValue(OV_AttributeId_Scenario_Author).toASCIIString());
	gtk_entry_set_text(GTK_ENTRY(l_pAuthorCompanyName), m_rScenario.getAttributeValue(OV_AttributeId_Scenario_Company).toASCIIString());
	gtk_entry_set_text(GTK_ENTRY(l_pCategory), m_rScenario.getAttributeValue(OV_AttributeId_Scenario_Category).toASCIIString());
	gtk_entry_set_text(GTK_ENTRY(l_pVersion), m_rScenario.getAttributeValue(OV_AttributeId_Scenario_Version).toASCIIString());
	gtk_entry_set_text(GTK_ENTRY(l_pDocumentationPage), m_rScenario.getAttributeValue(OV_AttributeId_Scenario_DocumentationPage).toASCIIString());

	gtk_dialog_run(GTK_DIALOG(l_pDialog));

	m_rScenario.setAttributeValue(OV_AttributeId_Scenario_Name, gtk_entry_get_text(GTK_ENTRY(l_pName)));
	m_rScenario.setAttributeValue(OV_AttributeId_Scenario_Author, gtk_entry_get_text(GTK_ENTRY(l_pAuthorName)));
	m_rScenario.setAttributeValue(OV_AttributeId_Scenario_Company, gtk_entry_get_text(GTK_ENTRY(l_pAuthorCompanyName)));
	m_rScenario.setAttributeValue(OV_AttributeId_Scenario_Category, gtk_entry_get_text(GTK_ENTRY(l_pCategory)));
	m_rScenario.setAttributeValue(OV_AttributeId_Scenario_Version, gtk_entry_get_text(GTK_ENTRY(l_pVersion)));
	m_rScenario.setAttributeValue(OV_AttributeId_Scenario_DocumentationPage, gtk_entry_get_text(GTK_ENTRY(l_pDocumentationPage)));

	gtk_widget_destroy(l_pDialog);

	return true;
}
