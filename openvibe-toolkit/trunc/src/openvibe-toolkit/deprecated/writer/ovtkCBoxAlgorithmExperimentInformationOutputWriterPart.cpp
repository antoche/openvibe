#include "ovtkCBoxAlgorithmExperimentInformationOutputWriterPart.inl"

using namespace OpenViBEToolkit;
using namespace OpenViBE;
using namespace std;

#define _NoValueI_ 0xffffffff
#define _NoValueS_ "_unspecified_"

// ________________________________________________________________________________________________________________
//

CBoxAlgorithmExperimentInformationOutputWriterPart::CBoxAlgorithmExperimentInformationOutputWriterPart(void)
	:m_ui32ExperimentIdentifier(_NoValueI_)
	,m_sExperimentDate(_NoValueS_)
	,m_ui32SubjectIdentifier(_NoValueI_)
	,m_sSubjectName(_NoValueS_)
	,m_ui32SubjectAge(_NoValueI_)
	,m_ui32SubjectSex(_NoValueI_)
	,m_ui32LaboratoryIdentifier(_NoValueI_)
	,m_sLaboratoryName(_NoValueS_)
	,m_ui32TechnicianIdentifier(_NoValueI_)
	,m_sTechnicianName(_NoValueS_)
{
}

// ________________________________________________________________________________________________________________
//

boolean CBoxAlgorithmExperimentInformationOutputWriterPart::setValue(const uint32 ui32ValueIdentifier, const uint32 ui32Value)
{
	switch(ui32ValueIdentifier)
	{
		case IBoxAlgorithmExperimentInformationOutputWriter::Value_ExperimentIdentifier: m_ui32ExperimentIdentifier=ui32Value; break;
		case IBoxAlgorithmExperimentInformationOutputWriter::Value_SubjectIdentifier:    m_ui32SubjectIdentifier   =ui32Value; break;
		case IBoxAlgorithmExperimentInformationOutputWriter::Value_SubjectAge:           m_ui32SubjectAge          =ui32Value; break;
		case IBoxAlgorithmExperimentInformationOutputWriter::Value_SubjectSex:           m_ui32SubjectSex          =ui32Value; break;
		case IBoxAlgorithmExperimentInformationOutputWriter::Value_LaboratoryIdentifier: m_ui32LaboratoryIdentifier=ui32Value; break;
		case IBoxAlgorithmExperimentInformationOutputWriter::Value_TechnicianIdentifier: m_ui32TechnicianIdentifier=ui32Value; break;
		default : return false;
	}
	return true;
}

boolean CBoxAlgorithmExperimentInformationOutputWriterPart::setValue(const uint32 ui32ValueIdentifier, const char* sValue)
{
	switch(ui32ValueIdentifier)
	{
		case IBoxAlgorithmExperimentInformationOutputWriter::Value_ExperimentDate: m_sExperimentDate=sValue; break;
		case IBoxAlgorithmExperimentInformationOutputWriter::Value_SubjectName:    m_sSubjectName   =sValue; break;
		case IBoxAlgorithmExperimentInformationOutputWriter::Value_LaboratoryName: m_sLaboratoryName=sValue; break;
		case IBoxAlgorithmExperimentInformationOutputWriter::Value_TechnicianName: m_sTechnicianName=sValue; break;
		default : return false;
	}
	return true;
}

// ________________________________________________________________________________________________________________
//

boolean CBoxAlgorithmExperimentInformationOutputWriterPart::writeHeaderStart(EBML::IWriter& rWriter)
{
	m_oWriterHelper.connect(&rWriter);

	 m_oWriterHelper.openChild(OVTK_NodeId_Header_ExperimentInformation);
	  m_oWriterHelper.openChild(OVTK_NodeId_Header_ExperimentInformation_Experiment);
	   if(m_ui32ExperimentIdentifier!=_NoValueI_)
	   {
	    m_oWriterHelper.openChild(OVTK_NodeId_Header_ExperimentInformation_Experiment_Identifier);
	     m_oWriterHelper.setUIntegerAsChildData(m_ui32ExperimentIdentifier);
	    m_oWriterHelper.closeChild();
	   }
	   if(m_sExperimentDate!=_NoValueS_)
	   {
	    m_oWriterHelper.openChild(OVTK_NodeId_Header_ExperimentInformation_Experiment_Date);
	     m_oWriterHelper.setASCIIStringAsChildData(m_sExperimentDate.c_str());
	    m_oWriterHelper.closeChild();
	   }
	   m_oWriterHelper.closeChild();
	   m_oWriterHelper.openChild(OVTK_NodeId_Header_ExperimentInformation_Subject);
	   if(m_ui32SubjectIdentifier!=_NoValueI_)
	   {
	    m_oWriterHelper.openChild(OVTK_NodeId_Header_ExperimentInformation_Subject_Identifier);
	    m_oWriterHelper.setUIntegerAsChildData(m_ui32SubjectIdentifier);
	    m_oWriterHelper.closeChild();
	   }
	   if(m_sSubjectName!=_NoValueS_)
	   {
	    m_oWriterHelper.openChild(OVTK_NodeId_Header_ExperimentInformation_Subject_Name);
	     m_oWriterHelper.setASCIIStringAsChildData(m_sSubjectName.c_str());
	    m_oWriterHelper.closeChild();
	   }
	   if(m_ui32SubjectAge!=_NoValueI_)
	   {
	    m_oWriterHelper.openChild(OVTK_NodeId_Header_ExperimentInformation_Subject_Age);
	     m_oWriterHelper.setUIntegerAsChildData(m_ui32SubjectAge);
	    m_oWriterHelper.closeChild();
	   }
	   if(m_ui32SubjectSex!=_NoValueI_)
	   {
	    m_oWriterHelper.openChild(OVTK_NodeId_Header_ExperimentInformation_Subject_Gender);
	     m_oWriterHelper.setUIntegerAsChildData(m_ui32SubjectSex);
	    m_oWriterHelper.closeChild();
	   }
	   m_oWriterHelper.closeChild();
	   m_oWriterHelper.openChild(OVTK_NodeId_Header_ExperimentInformation_Context);
	   if(m_ui32LaboratoryIdentifier!=_NoValueI_)
	   {
	    m_oWriterHelper.openChild(OVTK_NodeId_Header_ExperimentInformation_Context_LaboratoryIdentifier);
	     m_oWriterHelper.setUIntegerAsChildData(m_ui32LaboratoryIdentifier);
	    m_oWriterHelper.closeChild();
	   }
	   if(m_sLaboratoryName!=_NoValueS_)
	   {
	    m_oWriterHelper.openChild(OVTK_NodeId_Header_ExperimentInformation_Context_LaboratoryName);
	     m_oWriterHelper.setASCIIStringAsChildData(m_sLaboratoryName.c_str());
	    m_oWriterHelper.closeChild();
	   }
	   if(m_ui32TechnicianIdentifier!=_NoValueI_)
	   {
	    m_oWriterHelper.openChild(OVTK_NodeId_Header_ExperimentInformation_Context_TechnicianIdentifier);
	     m_oWriterHelper.setUIntegerAsChildData(m_ui32TechnicianIdentifier);
	    m_oWriterHelper.closeChild();
	   }
	   if(m_sTechnicianName!=_NoValueS_)
	   {
	    m_oWriterHelper.openChild(OVTK_NodeId_Header_ExperimentInformation_Context_TechnicianName);
	     m_oWriterHelper.setASCIIStringAsChildData(m_sTechnicianName.c_str());
	    m_oWriterHelper.closeChild();
	   }
	  m_oWriterHelper.closeChild();

	m_oWriterHelper.disconnect();
	return true;
}

boolean CBoxAlgorithmExperimentInformationOutputWriterPart::writeHeaderStop(EBML::IWriter& rWriter)
{
	m_oWriterHelper.connect(&rWriter);

	 m_oWriterHelper.closeChild();

	m_oWriterHelper.disconnect();
	return true;
}
