#include "ovasCConfigurationNetworkBuilder.h"

#include <openvibe-toolkit/ovtk_all.h>

#include <iostream>
#include <fstream>
#include <list>

using namespace OpenViBEAcquisitionServer;
using namespace OpenViBE;
using namespace std;

//___________________________________________________________________//
//                                                                   //

CConfigurationNetworkBuilder::CConfigurationNetworkBuilder(const char* sGtkBuilderFileName)
	:CConfigurationBuilder(sGtkBuilderFileName)
	,m_sHostName("localhost")
	,m_ui32HostPort(4000)
{
}

CConfigurationNetworkBuilder::~CConfigurationNetworkBuilder(void)
{
}

//___________________________________________________________________//
//                                                                   //

boolean CConfigurationNetworkBuilder::setHostName(const CString& sHostName)
{
	m_sHostName=sHostName;
	return true;
}

boolean CConfigurationNetworkBuilder::setHostPort(const uint32 ui32HostPort)
{
	m_ui32HostPort=ui32HostPort;
	return true;
}

//___________________________________________________________________//
//                                                                   //

CString CConfigurationNetworkBuilder::getHostName(void) const
{
	return m_sHostName;
}

uint32 CConfigurationNetworkBuilder::getHostPort(void) const
{
	return m_ui32HostPort;
}

//___________________________________________________________________//
//                                                                   //

boolean CConfigurationNetworkBuilder::preConfigure(void)
{
	boolean l_bParentResult=CConfigurationBuilder::preConfigure();

	m_pHostName=GTK_WIDGET(gtk_builder_get_object(m_pBuilderConfigureInterface, "entry_host_name"));
	m_pHostPort=GTK_WIDGET(gtk_builder_get_object(m_pBuilderConfigureInterface, "spinbutton_host_port"));

	gtk_spin_button_set_value(
		GTK_SPIN_BUTTON(m_pHostPort),
		m_ui32HostPort);
	gtk_entry_set_text(
		GTK_ENTRY(m_pHostName),
		m_sHostName.toASCIIString());

	return l_bParentResult;
}

boolean CConfigurationNetworkBuilder::postConfigure(void)
{
	if(m_bApplyConfiguration)
	{
		m_ui32HostPort=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(m_pHostPort));
		m_sHostName=gtk_entry_get_text(GTK_ENTRY(m_pHostName));
	}

	return CConfigurationBuilder::postConfigure();
}
