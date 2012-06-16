#include "ovsgCSkeletonGenerator.h"

#include <string>
#include <cstdlib>
#include <ctime>
#include <sstream>

using namespace std;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBESkeletonGenerator;

CSkeletonGenerator::CSkeletonGenerator(IKernelContext & rKernelContext, ::GtkBuilder * pBuilderInterface)
	:m_rKernelContext(rKernelContext)
	,m_pBuilderInterface(pBuilderInterface)
	,m_bConfigurationFileLoaded(false)
{
	m_sConfigurationFile = m_rKernelContext.getConfigurationManager().expand("${CustomConfigurationApplication}");
	m_rKernelContext.getLogManager() << LogLevel_Trace << "Configuration file [" << m_sConfigurationFile << "]\n";
	loadCommonParameters(m_sConfigurationFile);
}

CSkeletonGenerator::~CSkeletonGenerator(void)
{
}

void CSkeletonGenerator::getCommonParameters()
{
	//Author and Company
	::GtkWidget* l_pEntryCompany = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "entry_company_name"));
	m_sCompany = gtk_entry_get_text(GTK_ENTRY(l_pEntryCompany));

	::GtkWidget* l_pEntryAuthor = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "entry_author_name"));
	m_sAuthor=gtk_entry_get_text(GTK_ENTRY(l_pEntryAuthor));

}

boolean CSkeletonGenerator::saveCommonParameters(CString sFileName)
{
	// we get the latest values
	getCommonParameters();

	FILE* l_pFile=::fopen(sFileName.toASCIIString(), "ab");
	if(!l_pFile)
	{
		m_rKernelContext.getLogManager() << LogLevel_Warning << "Saving the common entries in [" << sFileName << "] failed !\n";
		return false;
	}

	// generator selected
	CString l_sActive;
	::GtkWidget* l_pWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-driver-selection-radio-button"));
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(l_pWidget)))
	{
		l_sActive = "0";
	}
	l_pWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-algo-selection-radio-button"));
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(l_pWidget)))
	{
		l_sActive = "1";
	}
	l_pWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-selection-radio-button"));
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(l_pWidget)))
	{
		l_sActive = "2";
	}

	::fprintf(l_pFile, "SkeletonGenerator_GeneratorSelected = %s\n", l_sActive.toASCIIString());
	::fprintf(l_pFile, "SkeletonGenerator_Common_Author = %s\n", m_sAuthor.toASCIIString());
	::fprintf(l_pFile, "SkeletonGenerator_Common_Company = %s\n", m_sCompany.toASCIIString());
	::fclose(l_pFile);
	m_rKernelContext.getLogManager() << LogLevel_Info << "Common entries saved in [" << sFileName << "]\n";

	//we can reload the file, it may have changed
	m_bConfigurationFileLoaded = false;

	return true;
}

boolean CSkeletonGenerator::cleanConfigurationFile(CString sFileName)
{
	FILE* l_pFile=::fopen(sFileName.toASCIIString(), "wb");
	if(!l_pFile)
	{
		m_rKernelContext.getLogManager() << LogLevel_Warning << "Failed to clean [" << sFileName << "]\n";
		return false;
	}

	m_rKernelContext.getLogManager() << LogLevel_Info << "Configuration file [" << sFileName << "] cleaned.\n";
	::fclose(l_pFile);
	return true;
}

boolean CSkeletonGenerator::loadCommonParameters(CString sFileName)
{
	if(!m_bConfigurationFileLoaded && !m_rKernelContext.getConfigurationManager().addConfigurationFromFile(sFileName))
	{
		m_rKernelContext.getLogManager() << LogLevel_Warning << "Common: Configuration file [" << sFileName << "] could not be loaded. \n";
		return false;
	}

	::GtkWidget* l_pWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-driver-selection-radio-button"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(l_pWidget), (m_rKernelContext.getConfigurationManager().expandAsUInteger("${SkeletonGenerator_GeneratorSelected}") == 0));
	l_pWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-algo-selection-radio-button"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(l_pWidget), (m_rKernelContext.getConfigurationManager().expandAsUInteger("${SkeletonGenerator_GeneratorSelected}") == 1));
	l_pWidget = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "sg-box-selection-radio-button"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(l_pWidget), (m_rKernelContext.getConfigurationManager().expandAsUInteger("${SkeletonGenerator_GeneratorSelected}") == 2));
	
	::GtkWidget* l_pEntryCompany = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "entry_company_name"));
	gtk_entry_set_text(GTK_ENTRY(l_pEntryCompany),m_rKernelContext.getConfigurationManager().expand("${SkeletonGenerator_Common_Company}"));

	::GtkWidget * l_pEntryAuthorName = GTK_WIDGET(gtk_builder_get_object(m_pBuilderInterface, "entry_author_name"));
	gtk_entry_set_text(GTK_ENTRY(l_pEntryAuthorName),m_rKernelContext.getConfigurationManager().expand("${SkeletonGenerator_Common_Author}"));

	m_rKernelContext.getLogManager() << LogLevel_Info << "Common entries from [" << sFileName << "] loaded.\n";

	m_bConfigurationFileLoaded = true;

	return true;
}

CString CSkeletonGenerator::ensureSedCompliancy(CString sExpression)
{
	string l_sExpression((const char*)sExpression);
	string::iterator it=l_sExpression.begin();
	while(it<l_sExpression.end())
	{
		if((*it)=='\\')
		{
			it = l_sExpression.insert(it,'\\');
			it++;
			it = l_sExpression.insert(it,'\\');
			it++;
			it = l_sExpression.insert(it,'\\');
			it++;
#ifdef OV_OS_Linux
			it = l_sExpression.insert(it,'\\');
			it = l_sExpression.insert(it,'\\');
			it+=2;
			it = l_sExpression.insert(it,'\\');
			it = l_sExpression.insert(it,'\\');
			it+=2;
#endif
		}
		else if((*it)=='/')
		{
			it = l_sExpression.insert(it,'\\');
			it++;
		}
		else if((*it)=='"')
		{
			it = l_sExpression.insert(it,'\\');
			it++;
			it = l_sExpression.insert(it,'\\');
			it++;
			it = l_sExpression.insert(it,'\\');
			it++;
			it = l_sExpression.insert(it,'\\');
			it++;
			it = l_sExpression.insert(it,'\\');
			it++;
		}
		else if((*it)=='\n')
		{
			it = l_sExpression.erase(it);
#ifdef OV_OS_Linux
			it = l_sExpression.insert(it,'\\');
			it = l_sExpression.insert(it,'\\');
			it+=2;
#endif
			it = l_sExpression.insert(it,'\\');
			it = l_sExpression.insert(it,'\\');
			it+=2;
			it = l_sExpression.insert(it,'n');
			//it++;
		}
		it++;
	}

	return CString(l_sExpression.c_str());
}

boolean CSkeletonGenerator::executeSedCommand(CString sTemplateFile, CString sCommand, CString sDestinationFile)
{
	CString l_sSed;
	CString l_sMove;
	CString l_sNull;
#ifdef OV_OS_Windows
	l_sSed = "..\\share\\openvibe-applications\\skeleton-generator\\sed";
	l_sMove = "move";
	l_sNull = "NULL";
#else
#ifdef OV_OS_Linux
	l_sSed = "sed";
	l_sMove = "mv";
	l_sNull = "/dev/null";
#endif
#endif

	boolean l_bSuccess = false;

	CString l_sCommandSed = l_sSed + " \"" + sCommand+"\" \"" + sTemplateFile + "\"";
	if(string(sDestinationFile) != string(""))
	{
		l_sCommandSed =  l_sCommandSed + " > \"" + sDestinationFile + "\"";
		l_bSuccess = (system(((string)l_sCommandSed).c_str()) == 0);
	}
	else
	{
		l_sCommandSed =  l_sCommandSed + " > tmp-sed";
		l_bSuccess = (system(l_sCommandSed) == 0);
		CString l_sMoveCommand = l_sMove + " tmp-sed \"" + sTemplateFile + "\" >> "+l_sNull;
		l_bSuccess &= (system(l_sMoveCommand) == 0);
		m_rKernelContext.getLogManager() << LogLevel_Trace << " -- Move command : [" << l_sMoveCommand << "]\n";
	}
	
	l_bSuccess = (system(((string)l_sCommandSed).c_str()) == 0);

	if(l_bSuccess)
	{
		m_rKernelContext.getLogManager() << LogLevel_Trace << " -- Sed command successfully called : [" << l_sCommandSed << "]\n";
	}
	else
	{
		m_rKernelContext.getLogManager() << LogLevel_Error << " -- Faulty substitution command : [" << l_sCommandSed <<"]\n";
	}

	return l_bSuccess;

}

boolean CSkeletonGenerator::executeSedSubstitution(CString sTemplateFile, CString sTag, CString sSubstitute, CString sDestinationFile)
{
	CString l_sSed;
	CString l_sMove;
	CString l_sNull;
#ifdef OV_OS_Windows
	l_sSed = "..\\share\\openvibe-applications\\skeleton-generator\\sed";
	l_sMove = "move";
	l_sNull = "NULL";
#else
#ifdef OV_OS_Linux
	l_sSed = "sed";
	l_sMove = "mv";
	l_sNull = "/dev/null";
#endif
#endif

	boolean l_bSuccess = false;

	CString l_sSubstitute = ensureSedCompliancy(sSubstitute);

	CString l_sCommandSed = l_sSed + " \"s/" + sTag + "/" + l_sSubstitute+ "/g\" \"" + sTemplateFile + "\"";
	if(string(sDestinationFile) != string(""))
	{
		l_sCommandSed =  l_sCommandSed + " > \"" + sDestinationFile + "\"";
		l_bSuccess = (system(((string)l_sCommandSed).c_str()) == 0);
	}
	else
	{
		l_sCommandSed =  l_sCommandSed + " > tmp-sed";
		l_bSuccess = (system(l_sCommandSed) == 0);
		CString l_sMoveCommand = l_sMove + " tmp-sed \"" + sTemplateFile + "\" >> "+l_sNull;
		l_bSuccess &= (system(l_sMoveCommand) == 0);
		m_rKernelContext.getLogManager() << LogLevel_Trace << " -- Move command : [" << l_sMoveCommand << "]\n";
#ifdef OV_OS_Windows
		l_bSuccess &= (system("del NULL") == 0);
#endif
	}

	if(l_bSuccess)
	{
		m_rKernelContext.getLogManager() << LogLevel_Trace << " -- Sed command successfully called : [" << l_sCommandSed << "]\n";
	}
	else
	{
		m_rKernelContext.getLogManager() << LogLevel_Error << " -- Faulty substitution command : [" << l_sCommandSed <<"]\n";
	}

	return l_bSuccess;
}

CString CSkeletonGenerator::getDate()
{
	time_t rawtime;
	struct tm * timeinfo;
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	stringstream ssTime;
	string string_time(asctime (timeinfo));
	string_time = string_time.substr(0,string_time.size()-1); // the ascitime ends with a "\n"
	CString l_sDate(string_time.c_str());
	
	return l_sDate;
}

boolean CSkeletonGenerator::generate(CString sTemplateFile, CString sDestinationFile, map<CString,CString> mSubstitutions, CString& rLog)
{
	// we check if the template file is in place.
	if(! g_file_test(sTemplateFile, G_FILE_TEST_EXISTS))
	{
		rLog = rLog + "[FAILED] the template file '"+sTemplateFile+"' is missing.\n";
		m_rKernelContext.getLogManager() << LogLevel_Error << "The template file '"<<sTemplateFile<<"' is missing.\n";
		return false;
	}
	
	// we check the map
	if(mSubstitutions.size() == 0)
	{
		rLog = rLog + "[WARNING] No substitution provided.\n";
		m_rKernelContext.getLogManager() << LogLevel_Warning << "No substitution provided.\n";
		return false;
	}

	boolean l_bSuccess = true;

	rLog = rLog +  "[   OK   ] -- template file '"+sTemplateFile+"' found.\n";
	m_rKernelContext.getLogManager() << LogLevel_Info << " -- template file '" << sTemplateFile << "' found.\n";

	//we need to create the destination file by copying the template file, then do the first substitution
	map<CString,CString>::const_iterator it = mSubstitutions.begin();
	l_bSuccess &= executeSedSubstitution(sTemplateFile, it->first, it->second, sDestinationFile);
	it++;
	
	//next substitutions are done on the - incomplete - destination file itself
	while(it != mSubstitutions.end() && l_bSuccess)
	{
		m_rKernelContext.getLogManager() << LogLevel_Trace << "Executing substitution ["<<it->first<<"] ->["<<it->second<<"]\n";
		l_bSuccess &= executeSedSubstitution(sDestinationFile, it->first, it->second);
		it++;
	}

	if(! l_bSuccess)
	{
		rLog = rLog + "[FAILED] -- " + sDestinationFile + " cannot be written.\n";
		m_rKernelContext.getLogManager() << LogLevel_Error << " -- " << sDestinationFile << " cannot be written.\n";
		return false;

	}

	rLog = rLog + "[   OK   ] -- " + sDestinationFile + " written.\n";
	m_rKernelContext.getLogManager() << LogLevel_Info << " -- " << sDestinationFile << " written.\n";
	return true;
}
