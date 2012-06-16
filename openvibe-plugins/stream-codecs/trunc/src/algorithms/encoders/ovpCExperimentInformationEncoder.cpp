#include "ovpCExperimentInformationEncoder.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::StreamCodecs;

boolean CExperimentInformationEncoder::initialize(void)
{
	CEBMLBaseEncoder::initialize();

	ip_ui64ExperimentIdentifier.initialize(getInputParameter(OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_ExperimentIdentifier));
	ip_pExperimentDate.initialize(getInputParameter(OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_ExperimentDate));
	ip_ui64SubjectIdentifier.initialize(getInputParameter(OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_SubjectIdentifier));
	ip_pSubjectName.initialize(getInputParameter(OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_SubjectName));
	ip_ui64SubjectAge.initialize(getInputParameter(OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_SubjectAge));
	ip_ui64SubjectGender.initialize(getInputParameter(OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_SubjectGender));
	ip_ui64LaboratoryIdentifier.initialize(getInputParameter(OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_LaboratoryIdentifier));
	ip_pLaboratoryName.initialize(getInputParameter(OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_LaboratoryName));
	ip_ui64TechnicianIdentifier.initialize(getInputParameter(OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_TechnicianIdentifier));
	ip_pTechnicianName.initialize(getInputParameter(OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_TechnicianName));

	return true;
}

boolean CExperimentInformationEncoder::uninitialize(void)
{
	ip_pTechnicianName.uninitialize();
	ip_ui64TechnicianIdentifier.uninitialize();
	ip_pLaboratoryName.uninitialize();
	ip_ui64LaboratoryIdentifier.uninitialize();
	ip_ui64SubjectGender.uninitialize();
	ip_ui64SubjectAge.uninitialize();
	ip_pSubjectName.uninitialize();
	ip_ui64SubjectIdentifier.uninitialize();
	ip_pExperimentDate.uninitialize();
	ip_ui64ExperimentIdentifier.uninitialize();

	CEBMLBaseEncoder::uninitialize();

	return true;
}

// ________________________________________________________________________________________________________________
//

boolean CExperimentInformationEncoder::processHeader(void)
{
	m_pEBMLWriterHelper->openChild(OVTK_NodeId_Header_ExperimentInformation);
	 m_pEBMLWriterHelper->openChild(OVTK_NodeId_Header_ExperimentInformation_Experiment);
	   m_pEBMLWriterHelper->openChild(OVTK_NodeId_Header_ExperimentInformation_Experiment_Identifier);
	    m_pEBMLWriterHelper->setUIntegerAsChildData(ip_ui64ExperimentIdentifier);
	   m_pEBMLWriterHelper->closeChild();
	   m_pEBMLWriterHelper->openChild(OVTK_NodeId_Header_ExperimentInformation_Experiment_Date);
	    m_pEBMLWriterHelper->setASCIIStringAsChildData(ip_pExperimentDate->toASCIIString());
	   m_pEBMLWriterHelper->closeChild();
	  m_pEBMLWriterHelper->closeChild();
	  m_pEBMLWriterHelper->openChild(OVTK_NodeId_Header_ExperimentInformation_Subject);
	   m_pEBMLWriterHelper->openChild(OVTK_NodeId_Header_ExperimentInformation_Subject_Identifier);
	   m_pEBMLWriterHelper->setUIntegerAsChildData(ip_ui64SubjectIdentifier);
	   m_pEBMLWriterHelper->closeChild();
	   m_pEBMLWriterHelper->openChild(OVTK_NodeId_Header_ExperimentInformation_Subject_Name);
	    m_pEBMLWriterHelper->setASCIIStringAsChildData(ip_pSubjectName->toASCIIString());
	   m_pEBMLWriterHelper->closeChild();
	   m_pEBMLWriterHelper->openChild(OVTK_NodeId_Header_ExperimentInformation_Subject_Age);
	    m_pEBMLWriterHelper->setUIntegerAsChildData(ip_ui64SubjectAge);
	   m_pEBMLWriterHelper->closeChild();
	   m_pEBMLWriterHelper->openChild(OVTK_NodeId_Header_ExperimentInformation_Subject_Gender);
	    m_pEBMLWriterHelper->setUIntegerAsChildData(ip_ui64SubjectGender);
	   m_pEBMLWriterHelper->closeChild();
	  m_pEBMLWriterHelper->closeChild();
	  m_pEBMLWriterHelper->openChild(OVTK_NodeId_Header_ExperimentInformation_Context);
	   m_pEBMLWriterHelper->openChild(OVTK_NodeId_Header_ExperimentInformation_Context_LaboratoryIdentifier);
	    m_pEBMLWriterHelper->setUIntegerAsChildData(ip_ui64LaboratoryIdentifier);
	   m_pEBMLWriterHelper->closeChild();
	   m_pEBMLWriterHelper->openChild(OVTK_NodeId_Header_ExperimentInformation_Context_LaboratoryName);
	    m_pEBMLWriterHelper->setASCIIStringAsChildData(ip_pLaboratoryName->toASCIIString());
	   m_pEBMLWriterHelper->closeChild();
	   m_pEBMLWriterHelper->openChild(OVTK_NodeId_Header_ExperimentInformation_Context_TechnicianIdentifier);
	    m_pEBMLWriterHelper->setUIntegerAsChildData(ip_ui64TechnicianIdentifier);
	   m_pEBMLWriterHelper->closeChild();
	   m_pEBMLWriterHelper->openChild(OVTK_NodeId_Header_ExperimentInformation_Context_TechnicianName);
	    m_pEBMLWriterHelper->setASCIIStringAsChildData(ip_pTechnicianName->toASCIIString());
	   m_pEBMLWriterHelper->closeChild();
	 m_pEBMLWriterHelper->closeChild();
	m_pEBMLWriterHelper->closeChild();

	return true;
}
