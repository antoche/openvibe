#ifndef __SamplePlugin_Algorithms_CExperimentInformationDecoder_H__
#define __SamplePlugin_Algorithms_CExperimentInformationDecoder_H__

#include "ovpCEBMLBaseDecoder.h"

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CExperimentInformationDecoder : public OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoder
		{
		public:

			CExperimentInformationDecoder(void);

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoder, OVP_ClassId_Algorithm_ExperimentInformationStreamDecoder);

			// ebml callbacks
			virtual EBML::boolean isMasterChild(const EBML::CIdentifier& rIdentifier);
			virtual void openChild(const EBML::CIdentifier& rIdentifier);
			virtual void processChildData(const void* pBuffer, const EBML::uint64 ui64BufferSize);
			virtual void closeChild(void);

		protected:

			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > op_ui64ExperimentIdentifier;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::CString* > op_pExperimentDate;

			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > op_ui64SubjectIdentifier;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::CString* > op_pSubjectName;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > op_ui64SubjectAge;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > op_ui64SubjectGender;

			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > op_ui64LaboratoryIdentifier;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::CString* > op_pLaboratoryName;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > op_ui64TechnicianIdentifier;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::CString* > op_pTechnicianName;

		private:

			std::stack<EBML::CIdentifier> m_vNodes;
		};

		class CExperimentInformationDecoderDesc : public OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoderDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Experiment information stream decoder"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Stream codecs/Decoders"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_Algorithm_ExperimentInformationStreamDecoder; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::StreamCodecs::CExperimentInformationDecoder(); }

			virtual OpenViBE::boolean getAlgorithmPrototype(
				OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoderDesc::getAlgorithmPrototype(rAlgorithmPrototype);

				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_ExperimentIdentifier, "Experiment identifier", OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_ExperimentDate,       "Experiment date",       OpenViBE::Kernel::ParameterType_String);
				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_SubjectIdentifier,    "Subject identifier",    OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_SubjectName,          "Subject name",          OpenViBE::Kernel::ParameterType_String);
				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_SubjectAge,           "Subject age",           OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_SubjectGender,        "Subject gender",        OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_LaboratoryIdentifier, "Laboratory identifier", OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_LaboratoryName,       "Laboratory name",       OpenViBE::Kernel::ParameterType_String);
				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_TechnicianIdentifier, "Technician identifier", OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_TechnicianName,       "Technician name",       OpenViBE::Kernel::ParameterType_String);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoderDesc, OVP_ClassId_Algorithm_ExperimentInformationStreamDecoderDesc);
		};
	};
};

#endif // __SamplePlugin_Algorithms_CExperimentInformationDecoder_H__
