#include "ovpiCPluginObjectDescEnumAlgorithmGlobalDefinesGenerator.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

CPluginObjectDescEnumAlgorithmGlobalDefinesGenerator::CPluginObjectDescEnumAlgorithmGlobalDefinesGenerator(const IKernelContext& rKernelContext, const CString& sFilename)
	:CPluginObjectDescEnum(rKernelContext)
{
	m_oFile.open((sFilename+CString("/ovp_global_defines.h")).toASCIIString());

	if(!m_oFile.good()) return;

	m_oFile << "#ifndef __OpenViBEPlugins_Global_Defines_H__\n";
	m_oFile << "#define __OpenViBEPlugins_Global_Defines_H__\n";
	m_oFile << "\n";
}

CPluginObjectDescEnumAlgorithmGlobalDefinesGenerator::~CPluginObjectDescEnumAlgorithmGlobalDefinesGenerator(void)
{
	if(!m_oFile.good()) return;

	m_oFile << "#endif // __OpenViBEPlugins_Global_Defines_H__\n";
	m_oFile.close();
}

boolean CPluginObjectDescEnumAlgorithmGlobalDefinesGenerator::callback(const IPluginObjectDesc& rPluginObjectDesc)
{
	if(!m_oFile.good()) return false;

	int l_iWidth=120;

	m_rKernelContext.getLogManager() << LogLevel_Info << "Dumping [" << rPluginObjectDesc.getName() << "]\n";

	IAlgorithmManager& l_rAlgorithmManager=m_rKernelContext.getAlgorithmManager();
	IAlgorithmProxy& l_rAlgorithmProxy=l_rAlgorithmManager.getAlgorithm(l_rAlgorithmManager.createAlgorithm(rPluginObjectDesc.getCreatedClass()));
	CIdentifier l_oIdentifier;

	m_oFile << "// -----------------------------------------------------\n";
	m_oFile << "// " << rPluginObjectDesc.getName().toASCIIString() << "\n";
	m_oFile << "// -----------------------------------------------------\n";
	m_oFile << "\n";

	// l_rAlgorithmProxy.initialize();

	{
		std::string l_sDefName;
		l_sDefName+="OVP_GD_ClassId_Algorithm_";
		l_sDefName+=transform(rPluginObjectDesc.getName().toASCIIString(), true);
		l_sDefName+="";

		std::string l_sSpaces;
		l_sSpaces.resize(l_iWidth-l_sDefName.length(), ' ');

		m_oFile << "#define " << l_sDefName << l_sSpaces << "OpenViBE::CIdentifier" << rPluginObjectDesc.getCreatedClassIdentifier().toString().toASCIIString() << "\n";
	}

	{
		std::string l_sDefName;
		l_sDefName+="OVP_GD_ClassId_Algorithm_";
		l_sDefName+=transform(rPluginObjectDesc.getName().toASCIIString(), true);
		l_sDefName+="Desc";

		std::string l_sSpaces;
		l_sSpaces.resize(l_iWidth-l_sDefName.length(), ' ');

		m_oFile << "#define " << l_sDefName << l_sSpaces << "OpenViBE::CIdentifier" << rPluginObjectDesc.getClassIdentifier().toString().toASCIIString() << "\n";
	}

	while((l_oIdentifier=l_rAlgorithmProxy.getNextInputParameterIdentifier(l_oIdentifier))!=OV_UndefinedIdentifier)
	{
		std::string l_sDefName;
		l_sDefName+="OVP_GD_Algorithm_";
		l_sDefName+=transform(rPluginObjectDesc.getName().toASCIIString(), true);
		l_sDefName+="_InputParameterId_";
		l_sDefName+=transform(l_rAlgorithmProxy.getInputParameterName(l_oIdentifier).toASCIIString(), true);

		std::string l_sSpaces;
		l_sSpaces.resize(l_iWidth-l_sDefName.length(), ' ');

		m_oFile << "#define " << l_sDefName << l_sSpaces << "OpenViBE::CIdentifier" << l_oIdentifier.toString().toASCIIString() << "\n";
	}

	while((l_oIdentifier=l_rAlgorithmProxy.getNextOutputParameterIdentifier(l_oIdentifier))!=OV_UndefinedIdentifier)
	{
		std::string l_sDefName;
		l_sDefName+="OVP_GD_Algorithm_";
		l_sDefName+=transform(rPluginObjectDesc.getName().toASCIIString(), true);
		l_sDefName+="_OutputParameterId_";
		l_sDefName+=transform(l_rAlgorithmProxy.getOutputParameterName(l_oIdentifier).toASCIIString(), true);

		std::string l_sSpaces;
		l_sSpaces.resize(l_iWidth-l_sDefName.length(), ' ');

		m_oFile << "#define " << l_sDefName << l_sSpaces << "OpenViBE::CIdentifier" << l_oIdentifier.toString().toASCIIString() << "\n";
	}

	while((l_oIdentifier=l_rAlgorithmProxy.getNextInputTriggerIdentifier(l_oIdentifier))!=OV_UndefinedIdentifier)
	{
		std::string l_sDefName;
		l_sDefName+="OVP_GD_Algorithm_";
		l_sDefName+=transform(rPluginObjectDesc.getName().toASCIIString(), true);
		l_sDefName+="_InputTriggerId_";
		l_sDefName+=transform(l_rAlgorithmProxy.getInputTriggerName(l_oIdentifier).toASCIIString(), true);

		std::string l_sSpaces;
		l_sSpaces.resize(l_iWidth-l_sDefName.length(), ' ');

		m_oFile << "#define " << l_sDefName << l_sSpaces << "OpenViBE::CIdentifier" << l_oIdentifier.toString().toASCIIString() << "\n";
	}

	while((l_oIdentifier=l_rAlgorithmProxy.getNextOutputTriggerIdentifier(l_oIdentifier))!=OV_UndefinedIdentifier)
	{
		std::string l_sDefName;
		l_sDefName+="OVP_GD_Algorithm_";
		l_sDefName+=transform(rPluginObjectDesc.getName().toASCIIString(), true);
		l_sDefName+="_OutputTriggerId_";
		l_sDefName+=transform(l_rAlgorithmProxy.getOutputTriggerName(l_oIdentifier).toASCIIString(), true);

		std::string l_sSpaces;
		l_sSpaces.resize(l_iWidth-l_sDefName.length(), ' ');

		m_oFile << "#define " << l_sDefName << l_sSpaces << "OpenViBE::CIdentifier" << l_oIdentifier.toString().toASCIIString() << "\n";
	}

	// l_rAlgorithmProxy.uninitialize();

	m_oFile << "\n";

	l_rAlgorithmManager.releaseAlgorithm(l_rAlgorithmProxy);

	return true;
}
