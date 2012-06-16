#include "ovasCConfigurationNetworkGlade.h"

#include <openvibe-toolkit/ovtk_all.h>

#include <iostream>
#include <fstream>
#include <list>

using namespace OpenViBEAcquisitionServer;
using namespace OpenViBE;
using namespace std;

//___________________________________________________________________//
//                                                                   //

CConfigurationNetworkGlade::CConfigurationNetworkGlade(const char* sGladeXMLFileName)
	:CConfigurationGlade(sGladeXMLFileName)
	,m_sHostName("localhost")
	,m_ui32HostPort(4000)
{
}

CConfigurationNetworkGlade::~CConfigurationNetworkGlade(void)
{
}

//___________________________________________________________________//
//                                                                   //

boolean CConfigurationNetworkGlade::setHostName(const CString& sHostName)
{
	m_sHostName=sHostName;
	return true;
}

boolean CConfigurationNetworkGlade::setHostPort(const uint32 ui32HostPort)
{
	m_ui32HostPort=ui32HostPort;
	return true;
}

//___________________________________________________________________//
//                                                                   //

CString CConfigurationNetworkGlade::getHostName(void) const
{
	return m_sHostName;
}

uint32 CConfigurationNetworkGlade::getHostPort(void) const
{
	return m_ui32HostPort;
}

//___________________________________________________________________//
//                                                                   //

boolean CConfigurationNetworkGlade::preConfigure(void)
{
	boolean l_bParentResult=CConfigurationGlade::preConfigure();

	m_pHostName=glade_xml_get_widget(m_pGladeConfigureInterface, "entry_host_name");
	m_pHostPort=glade_xml_get_widget(m_pGladeConfigureInterface, "spinbutton_host_port");

	gtk_spin_button_set_value(
		GTK_SPIN_BUTTON(m_pHostPort),
		m_ui32HostPort);
	gtk_entry_set_text(
		GTK_ENTRY(m_pHostName),
		m_sHostName.toASCIIString());

	return l_bParentResult;
}

boolean CConfigurationNetworkGlade::postConfigure(void)
{
	if(m_bApplyConfiguration)
	{
		m_ui32HostPort=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(m_pHostPort));
		m_sHostName=gtk_entry_get_text(GTK_ENTRY(m_pHostName));
	}

	return CConfigurationGlade::postConfigure();
}
