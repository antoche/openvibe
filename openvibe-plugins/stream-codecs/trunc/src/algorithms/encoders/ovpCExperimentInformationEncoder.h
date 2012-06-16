#ifndef __SamplePlugin_Algorithms_CExperimentInformationEncoder_H__
#define __SamplePlugin_Algorithms_CExperimentInformationEncoder_H__

#include "ovpCEBMLBaseEncoder.h"

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CExperimentInformationEncoder : public OpenViBEPlugins::StreamCodecs::CEBMLBaseEncoder
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processHeader(void);

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseEncoder, OVP_ClassId_Algorithm_ExperimentInformationStreamEncoder);

		protected:

			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > ip_ui64ExperimentIdentifier;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::CString* > ip_pExperimentDate;

			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > ip_ui64SubjectIdentifier;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::CString* > ip_pSubjectName;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > ip_ui64SubjectAge;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > ip_ui64SubjectGender;

			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > ip_ui64LaboratoryIdentifier;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::CString* > ip_pLaboratoryName;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > ip_ui64TechnicianIdentifier;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::CString* > ip_pTechnicianName;
		};

		class CExperimentInformationEncoderDesc : public OpenViBEPlugins::StreamCodecs::CEBMLBaseEncoderDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Experiment information stream encoder"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Stream codecs/Encoders"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_Algorithm_ExperimentInformationStreamEncoder; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::StreamCodecs::CExperimentInformationEncoder(); }

			virtual OpenViBE::boolean getAlgorithmPrototype(
				OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				OpenViBEPlugins::StreamCodecs::CEBMLBaseEncoderDesc::getAlgorithmPrototype(rAlgorithmPrototype);

				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_ExperimentIdentifier, "Experiment identifier", OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_ExperimentDate,       "Experiment date",       OpenViBE::Kernel::ParameterType_String);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_SubjectIdentifier,    "Subject identifier",    OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_SubjectName,          "Subject name",          OpenViBE::Kernel::ParameterType_String);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_SubjectAge,           "Subject age",           OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_SubjectGender,        "Subject gender",        OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_LaboratoryIdentifier, "Laboratory identifier", OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_LaboratoryName,       "Laboratory name",       OpenViBE::Kernel::ParameterType_String);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_TechnicianIdentifier, "Technician identifier", OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_TechnicianName,       "Technician name",       OpenViBE::Kernel::ParameterType_String);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseEncoderDesc, OVP_ClassId_Algorithm_ExperimentInformationStreamEncoderDesc);
		};
	};
};

#endif // __SamplePlugin_Algorithms_CExperimentInformationEncoder_H__
