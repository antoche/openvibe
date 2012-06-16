#if defined TARGET_OS_Windows

#include "ovasCConfigurationMitsarEEG202A.h"

#include <windows.h>
#define boolean OpenViBE::boolean

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBEAcquisitionServer;

CConfigurationMitsarEEG202A::CConfigurationMitsarEEG202A(
	const char* sGTKbuilderXMLFileName, 
	OpenViBE::uint32& rRefIndex,
	boolean& rEventAndBioChannelsState)
	:CConfigurationBuilder(sGTKbuilderXMLFileName),
	m_rRefIndex(rRefIndex),
	m_rEventAndBioChannelsState(rEventAndBioChannelsState)
{
}

boolean CConfigurationMitsarEEG202A::preConfigure(void)
{
	if (!CConfigurationBuilder::preConfigure())
	{
		return false;
	}

	::GtkComboBox* l_pComboBox_Ref = GTK_COMBO_BOX(gtk_builder_get_object(m_pBuilderConfigureInterface, "combobox_ref"));
	//::GtkComboBox* l_pComboBox_Chan = GTK_COMBO_BOX(gtk_builder_get_object(m_pBuilderConfigureInterface, "combobox_channels"));

	::GtkToggleButton* l_pCheckButton_Drift=GTK_TOGGLE_BUTTON(gtk_builder_get_object(m_pBuilderConfigureInterface, "checkbutton_driftCorrection"));
	::GtkToggleButton* l_pCheckButton_HardwareTagging=GTK_TOGGLE_BUTTON(gtk_builder_get_object(m_pBuilderConfigureInterface, "checkbutton_EventAndBioChannels"));

	::gtk_combo_box_set_active(l_pComboBox_Ref, 0);
	//::gtk_combo_box_set_active(l_pComboBox_Chan, 0);
	
	::gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(l_pCheckButton_HardwareTagging), m_rEventAndBioChannelsState);
	return true;
}

boolean CConfigurationMitsarEEG202A::postConfigure(void)
{
	::GtkComboBox* l_pComboBox_Ref=GTK_COMBO_BOX(gtk_builder_get_object(m_pBuilderConfigureInterface, "combobox_ref"));
	//::GtkComboBox* l_pComboBox_Chan=GTK_COMBO_BOX(gtk_builder_get_object(m_pBuilderConfigureInterface, "combobox_channels"));

	::GtkToggleButton* l_pCheckButton_Drift=GTK_TOGGLE_BUTTON(gtk_builder_get_object(m_pBuilderConfigureInterface, "checkbutton_driftCorrection"));
	::GtkToggleButton* l_pCheckButton_HardwareTagging=GTK_TOGGLE_BUTTON(gtk_builder_get_object(m_pBuilderConfigureInterface, "checkbutton_EventAndBioChannels"));

	m_rRefIndex=(uint32)::gtk_combo_box_get_active(l_pComboBox_Ref);

	m_rEventAndBioChannelsState=(boolean)::gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(l_pCheckButton_HardwareTagging));

	if (!CConfigurationBuilder::postConfigure())
	{
		return false;
	}
	return true;
}

#endif