#if defined TARGET_HAS_ThirdPartyEmotivAPI

#include "ovasCConfigurationEmotivEPOC.h"



using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBEAcquisitionServer;
using namespace std;

#define boolean OpenViBE::boolean

//____________________________________________________________________________________

CConfigurationEmotivEPOC::CConfigurationEmotivEPOC(IDriverContext& rDriverContext, const char* sGtkBuilderFileName, boolean& rUseGyroscope,OpenViBE::CString& rPathToEmotivResearchSDK)
	:CConfigurationBuilder(sGtkBuilderFileName)
	,m_rDriverContext(rDriverContext)
	,m_rUseGyroscope(rUseGyroscope)
	,m_rPathToEmotivResearchSDK(rPathToEmotivResearchSDK)
{
}

boolean CConfigurationEmotivEPOC::preConfigure(void)
{
	if(! CConfigurationBuilder::preConfigure())
	{
		return false;
	}

	::GtkToggleButton* l_pCheckbuttonGyro = GTK_TOGGLE_BUTTON(gtk_builder_get_object(m_pBuilderConfigureInterface, "checkbutton_gyro"));
	gtk_toggle_button_set_active(l_pCheckbuttonGyro,m_rUseGyroscope);

	::GtkFileChooser* l_pFileChooserButton = GTK_FILE_CHOOSER(gtk_builder_get_object(m_pBuilderConfigureInterface, "filechooserbutton"));
	gtk_file_chooser_set_current_folder(l_pFileChooserButton, (const char *) m_rPathToEmotivResearchSDK);
		
	return true;
}

boolean CConfigurationEmotivEPOC::postConfigure(void)
{

	if(m_bApplyConfiguration)
	{
		::GtkToggleButton* l_pCheckbuttonGyro = GTK_TOGGLE_BUTTON(gtk_builder_get_object(m_pBuilderConfigureInterface, "checkbutton_gyro"));
		m_rUseGyroscope = ::gtk_toggle_button_get_active(l_pCheckbuttonGyro);

		::GtkFileChooser* l_pFileChooserButton = GTK_FILE_CHOOSER(gtk_builder_get_object(m_pBuilderConfigureInterface, "filechooserbutton"));
		gchar * l_sDir = gtk_file_chooser_get_filename(l_pFileChooserButton);
		string::iterator it_directory;
		string l_sTempTargetDirectory(l_sDir);
		for(it_directory=l_sTempTargetDirectory.begin(); it_directory<l_sTempTargetDirectory.end(); it_directory++)
		{
			if((*it_directory)=='\\')
			{
				l_sTempTargetDirectory.replace(it_directory, it_directory+1, 1, '/');
			}
		}
		l_sTempTargetDirectory.push_back('/');
		m_rPathToEmotivResearchSDK = CString(l_sTempTargetDirectory.c_str());

		if(m_rDriverContext.getConfigurationManager().lookUpConfigurationTokenIdentifier("AcquisitionServer_PathToEmotivResearchSDK") == OV_UndefinedIdentifier)
		{
			//it's possible at first start of the acquisition server/ emotiv driver selection
			m_rDriverContext.getConfigurationManager().createConfigurationToken("AcquisitionServer_PathToEmotivResearchSDK","");
		}
		else
		{
			m_rDriverContext.getConfigurationManager().setConfigurationTokenValue(m_rDriverContext.getConfigurationManager().lookUpConfigurationTokenIdentifier("AcquisitionServer_PathToEmotivResearchSDK"),m_rPathToEmotivResearchSDK);
			m_rDriverContext.getLogManager() << LogLevel_Info << "Path to Emotiv Research SDK is set to ["<< m_rDriverContext.getConfigurationManager().expand("${AcquisitionServer_PathToEmotivResearchSDK}")<< "]\n";
		}
	}

	if(! CConfigurationBuilder::postConfigure()) // normal header is filled, ressources are realesed
	{
		return false;
	}

	return true;
}

#endif // TARGET_HAS_ThirdPartyEmotivAPI
