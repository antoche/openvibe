#ifndef __SamplePlugin_Algorithms_CMasterAcquisitionEncoder_H__
#define __SamplePlugin_Algorithms_CMasterAcquisitionEncoder_H__

#include "ovpCEBMLBaseEncoder.h"

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CMasterAcquisitionEncoder : public OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >, OVP_ClassId_Algorithm_MasterAcquisitionStreamEncoder);

		protected:

			OpenViBE::Kernel::IAlgorithmProxy* m_pAcquisitionStreamEncoder;
			OpenViBE::Kernel::IAlgorithmProxy* m_pExperimentInformationStreamEncoder;
			OpenViBE::Kernel::IAlgorithmProxy* m_pSignalStreamEncoder;
			OpenViBE::Kernel::IAlgorithmProxy* m_pStimulationStreamEncoder;
			OpenViBE::Kernel::IAlgorithmProxy* m_pChannelLocalisationStreamEncoder;
		};

		class CMasterAcquisitionEncoderDesc : public OpenViBE::Plugins::IAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Master acquisition stream encoder"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Stream codecs/Encoders"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_Algorithm_MasterAcquisitionStreamEncoder; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::StreamCodecs::CMasterAcquisitionEncoder(); }

			virtual OpenViBE::boolean getAlgorithmPrototype(
				OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				rAlgorithmPrototype.addInputTrigger(OVP_Algorithm_EBMLStreamEncoder_InputTriggerId_EncodeHeader, "Encode header");
				rAlgorithmPrototype.addInputTrigger(OVP_Algorithm_EBMLStreamEncoder_InputTriggerId_EncodeBuffer, "Encode buffer");
				rAlgorithmPrototype.addInputTrigger(OVP_Algorithm_EBMLStreamEncoder_InputTriggerId_EncodeEnd, "Encode end");

				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_MasterAcquisitionStreamEncoder_InputParameterId_SubjectIdentifier,    "Subject identifier",    OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_MasterAcquisitionStreamEncoder_InputParameterId_SubjectAge,           "Subject age",           OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_MasterAcquisitionStreamEncoder_InputParameterId_SubjectGender,        "Subject gender",        OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_MasterAcquisitionStreamEncoder_InputParameterId_SignalMatrix,         "Signal matrix",         OpenViBE::Kernel::ParameterType_Matrix);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_MasterAcquisitionStreamEncoder_InputParameterId_SignalSamplingRate,   "Signal sampling rate",  OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_MasterAcquisitionStreamEncoder_InputParameterId_StimulationSet,       "Stimulation set",       OpenViBE::Kernel::ParameterType_StimulationSet);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_MasterAcquisitionStreamEncoder_InputParameterId_BufferDuration,       "Buffer duration",       OpenViBE::Kernel::ParameterType_UInteger);

				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_EBMLStreamEncoder_OutputParameterId_EncodedMemoryBuffer, "Encoded memory buffer", OpenViBE::Kernel::ParameterType_MemoryBuffer);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IAlgorithmDesc, OVP_ClassId_Algorithm_MasterAcquisitionStreamEncoderDesc);
		};
	};
};

#endif // __SamplePlugin_Algorithms_CMasterAcquisitionEncoder_H__
