#if defined TARGET_HAS_ThirdPartyEmokit

#include "ovasCConfigurationEmokit.h"



using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBEAcquisitionServer;
using namespace std;

#define boolean OpenViBE::boolean

//____________________________________________________________________________________

CConfigurationEmokit::CConfigurationEmokit(IDriverContext& rDriverContext, const char* sGtkBuilderFileName, boolean& rUseGyroscope, OpenViBE::uint32&  rUserID)
	:CConfigurationBuilder(sGtkBuilderFileName)
	,m_rDriverContext(rDriverContext)
	,m_rUseGyroscope(rUseGyroscope)
	,m_rUserID(rUserID)
{
}

boolean CConfigurationEmokit::preConfigure(void)
{
	if(! CConfigurationBuilder::preConfigure())
	{
		return false;
	}

	::GtkToggleButton* l_pCheckbuttonGyro = GTK_TOGGLE_BUTTON(gtk_builder_get_object(m_pBuilderConfigureInterface, "checkbutton_gyro"));
	gtk_toggle_button_set_active(l_pCheckbuttonGyro,m_rUseGyroscope);

	::GtkSpinButton* l_pSpinButtonUserID = GTK_SPIN_BUTTON(gtk_builder_get_object(m_pBuilderConfigureInterface, "spinbutton_userid"));
	gtk_spin_button_set_value(l_pSpinButtonUserID, m_rUserID);
	return true;
}

boolean CConfigurationEmokit::postConfigure(void)
{

	if(m_bApplyConfiguration)
	{
		::GtkToggleButton* l_pCheckbuttonGyro = GTK_TOGGLE_BUTTON(gtk_builder_get_object(m_pBuilderConfigureInterface, "checkbutton_gyro"));
		m_rUseGyroscope = ::gtk_toggle_button_get_active(l_pCheckbuttonGyro);

		::GtkSpinButton* l_pSpinButtonUserID = GTK_SPIN_BUTTON(gtk_builder_get_object(m_pBuilderConfigureInterface, "spinbutton_userid"));
		m_rUserID = gtk_spin_button_get_value(l_pSpinButtonUserID);

	}

	if(! CConfigurationBuilder::postConfigure()) // normal header is filled, ressources are realesed
	{
		return false;
	}

	return true;
}

#endif // TARGET_HAS_ThirdPartyEmokit
