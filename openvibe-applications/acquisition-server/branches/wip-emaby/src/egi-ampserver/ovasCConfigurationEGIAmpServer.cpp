#include "ovasCConfigurationEGIAmpServer.h"

#include <openvibe-toolkit/ovtk_all.h>

#include <iostream>
#include <fstream>
#include <list>

using namespace OpenViBEAcquisitionServer;
using namespace OpenViBE;
using namespace std;

//___________________________________________________________________//
//                                                                   //

CConfigurationEGIAmpServer::CConfigurationEGIAmpServer(const char* sGtkBuilderFileName)
	:CConfigurationBuilder(sGtkBuilderFileName)
	,m_sHostName("localhost")
	,m_ui32CommandPort(9877)
	,m_ui32StreamPort(9879)
{
}

CConfigurationEGIAmpServer::~CConfigurationEGIAmpServer(void)
{
}

//___________________________________________________________________//
//                                                                   //

boolean CConfigurationEGIAmpServer::setHostName(const CString& sHostName)
{
	m_sHostName=sHostName;
	return true;
}

boolean CConfigurationEGIAmpServer::setCommandPort(const uint32 ui32CommandPort)
{
	m_ui32CommandPort=ui32CommandPort;
	return true;
}

boolean CConfigurationEGIAmpServer::setStreamPort(const uint32 ui32StreamPort)
{
	m_ui32StreamPort=ui32StreamPort;
	return true;
}

//___________________________________________________________________//
//                                                                   //

CString CConfigurationEGIAmpServer::getHostName(void) const
{
	return m_sHostName;
}

uint32 CConfigurationEGIAmpServer::getCommandPort(void) const
{
	return m_ui32CommandPort;
}

uint32 CConfigurationEGIAmpServer::getStreamPort(void) const
{
	return m_ui32StreamPort;
}

//___________________________________________________________________//
//                                                                   //

boolean CConfigurationEGIAmpServer::preConfigure(void)
{
	boolean l_bParentResult=CConfigurationBuilder::preConfigure();

	m_pHostName=GTK_WIDGET(gtk_builder_get_object(m_pBuilderConfigureInterface, "entry_host_name"));
	m_pCommandPort=GTK_WIDGET(gtk_builder_get_object(m_pBuilderConfigureInterface, "spinbutton_command_port"));
	m_pStreamPort=GTK_WIDGET(gtk_builder_get_object(m_pBuilderConfigureInterface, "spinbutton_stream_port"));

	gtk_spin_button_set_value(
		GTK_SPIN_BUTTON(m_pCommandPort),
		m_ui32CommandPort);

	gtk_spin_button_set_value(
		GTK_SPIN_BUTTON(m_pStreamPort),
		m_ui32StreamPort);

	gtk_entry_set_text(
		GTK_ENTRY(m_pHostName),
		m_sHostName.toASCIIString());

	return l_bParentResult;
}

boolean CConfigurationEGIAmpServer::postConfigure(void)
{
	if(m_bApplyConfiguration)
	{
		m_ui32StreamPort=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(m_pStreamPort));
		m_ui32CommandPort=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(m_pCommandPort));
		m_sHostName=gtk_entry_get_text(GTK_ENTRY(m_pHostName));
	}

	return CConfigurationBuilder::postConfigure();
}
