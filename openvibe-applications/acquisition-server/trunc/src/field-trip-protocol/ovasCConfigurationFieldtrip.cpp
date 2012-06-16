#include "ovasCConfigurationFieldtrip.h"

#include <openvibe-toolkit/ovtk_all.h>

#include <iostream>
#include <fstream>
#include <list>

using namespace OpenViBEAcquisitionServer;
using namespace OpenViBE;
using namespace std;

//___________________________________________________________________//
//                                                                   //

CConfigurationFieldtrip::CConfigurationFieldtrip(const char* sGtkBuilderFileName)
	:CConfigurationBuilder(sGtkBuilderFileName)
	,m_sHostName("localhost")
	,m_ui32HostPort(4000)
    ,m_ui32MinSamples(1)
    ,m_bSRCorrection(true)
{
}

CConfigurationFieldtrip::~CConfigurationFieldtrip(void)
{
}

//___________________________________________________________________//
//                                                                   //

boolean CConfigurationFieldtrip::setHostName(const CString& sHostName)
{
	m_sHostName=sHostName;
	return true;
}

boolean CConfigurationFieldtrip::setHostPort(const uint32 ui32HostPort)
{
	m_ui32HostPort=ui32HostPort;
	return true;
}

boolean CConfigurationFieldtrip::setMinSamples(const uint32 ui32MinSamples)
{
	m_ui32MinSamples=ui32MinSamples;
	return true;
}

boolean CConfigurationFieldtrip::setSRCorrection(const boolean bSRCorrection)
{
	m_bSRCorrection=bSRCorrection;
	return true;
}
//___________________________________________________________________//
//                                                                   //

CString CConfigurationFieldtrip::getHostName(void) const
{
	return m_sHostName;
}

uint32 CConfigurationFieldtrip::getHostPort(void) const
{
	return m_ui32HostPort;
}

uint32 CConfigurationFieldtrip::getMinSamples(void) const
{
	return m_ui32MinSamples;
}

boolean CConfigurationFieldtrip::getSRCorrection(void) const
{
	return m_bSRCorrection;
}
//___________________________________________________________________//
//                                                                   //

boolean CConfigurationFieldtrip::preConfigure(void)
{
	boolean l_bParentResult=CConfigurationBuilder::preConfigure();

	m_pHostName=GTK_WIDGET(gtk_builder_get_object(m_pBuilderConfigureInterface, "entry_host_name"));
	m_pHostPort=GTK_WIDGET(gtk_builder_get_object(m_pBuilderConfigureInterface, "spinbutton_host_port"));
	m_pMinSamples=GTK_WIDGET(gtk_builder_get_object(m_pBuilderConfigureInterface, "spinbutton_minSamples"));
	m_pSRCorrection=GTK_WIDGET(gtk_builder_get_object(m_pBuilderConfigureInterface, "checkbutton_SRCorrection"));

    gtk_spin_button_set_range(
		GTK_SPIN_BUTTON(m_pMinSamples),
		1.0, 10000.0);
    gtk_spin_button_set_value(
		GTK_SPIN_BUTTON(m_pMinSamples),
		m_ui32MinSamples);
	gtk_spin_button_set_value(
		GTK_SPIN_BUTTON(m_pHostPort),
		m_ui32HostPort);
	gtk_entry_set_text(
		GTK_ENTRY(m_pHostName),
		m_sHostName.toASCIIString());
	gtk_toggle_button_set_active(
		GTK_TOGGLE_BUTTON(m_pSRCorrection),
		m_bSRCorrection);

	return l_bParentResult;
}

boolean CConfigurationFieldtrip::postConfigure(void)
{
	if(m_bApplyConfiguration)
	{
		m_ui32MinSamples=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(m_pMinSamples));
		m_ui32HostPort=gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(m_pHostPort));
		m_sHostName=gtk_entry_get_text(GTK_ENTRY(m_pHostName));
        m_bSRCorrection=gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(m_pSRCorrection));
	}

	return CConfigurationBuilder::postConfigure();
}
