#ifndef __SamplePlugin_Algorithms_CAcquisitionEncoder_H__
#define __SamplePlugin_Algorithms_CAcquisitionEncoder_H__

#include "ovpCEBMLBaseEncoder.h"

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CAcquisitionEncoder : public OpenViBEPlugins::StreamCodecs::CEBMLBaseEncoder
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processHeader(void);
			virtual OpenViBE::boolean processBuffer(void);

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseEncoder, OVP_ClassId_Algorithm_AcquisitionStreamEncoder);

		protected:

			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > ip_ui64BufferDuration;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMemoryBuffer* > ip_pExperimentInformationStream;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMemoryBuffer* > ip_pSignalStream;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMemoryBuffer* > ip_pStimulationStream;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMemoryBuffer* > ip_pChannelLocalisationStream;

		private:

			OpenViBE::uint64 m_ui64MatrixBufferSize;
		};

		class CAcquisitionEncoderDesc : public OpenViBEPlugins::StreamCodecs::CEBMLBaseEncoderDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Acquisition stream encoder"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Stream codecs/Encoders"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_Algorithm_AcquisitionStreamEncoder; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::StreamCodecs::CAcquisitionEncoder(); }

			virtual OpenViBE::boolean getAlgorithmPrototype(
				OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				OpenViBEPlugins::StreamCodecs::CEBMLBaseEncoderDesc::getAlgorithmPrototype(rAlgorithmPrototype);

				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_AcquisitionStreamEncoder_InputParameterId_BufferDuration,              "Buffer duration",               OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_AcquisitionStreamEncoder_InputParameterId_ExperimentInformationStream, "Experiment information stream", OpenViBE::Kernel::ParameterType_MemoryBuffer);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_AcquisitionStreamEncoder_InputParameterId_SignalStream,                "Signal stream",                 OpenViBE::Kernel::ParameterType_MemoryBuffer);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_AcquisitionStreamEncoder_InputParameterId_StimulationStream,           "Stimulation stream",            OpenViBE::Kernel::ParameterType_MemoryBuffer);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_AcquisitionStreamEncoder_InputParameterId_ChannelLocalisationStream,   "Channel localisation stream",   OpenViBE::Kernel::ParameterType_MemoryBuffer);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseEncoderDesc, OVP_ClassId_Algorithm_AcquisitionStreamEncoderDesc);
		};
	};
};

#endif // __SamplePlugin_Algorithms_CAcquisitionEncoder_H__
