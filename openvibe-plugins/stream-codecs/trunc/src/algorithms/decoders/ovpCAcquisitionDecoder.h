#ifndef __SamplePlugin_Algorithms_CAcquisitionDecoder_H__
#define __SamplePlugin_Algorithms_CAcquisitionDecoder_H__

#include "ovpCEBMLBaseDecoder.h"

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CAcquisitionDecoder : public OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoder
		{
		public:

			CAcquisitionDecoder(void);

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoder, OVP_ClassId_Algorithm_AcquisitionStreamDecoder);

			// ebml callbacks
			virtual EBML::boolean isMasterChild(const EBML::CIdentifier& rIdentifier);
			virtual void openChild(const EBML::CIdentifier& rIdentifier);
			virtual void processChildData(const void* pBuffer, const EBML::uint64 ui64BufferSize);
			virtual void closeChild(void);

		protected:

			void appendMemoryBuffer(OpenViBE::IMemoryBuffer* pMemoryBuffer, const void* pBuffer, const EBML::uint64 ui64BufferSize);

			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > op_ui64BufferDuration;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMemoryBuffer* > op_pExperimentInformationStream;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMemoryBuffer* > op_pSignalStream;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMemoryBuffer* > op_pStimulationStream;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMemoryBuffer* > op_pChannelLocalisationStream;

		private:

			std::stack<EBML::CIdentifier> m_vNodes;
		};

		class CAcquisitionDecoderDesc : public OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoderDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Acquisition stream decoder"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Stream codecs/Decoders"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_Algorithm_AcquisitionStreamDecoder; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::StreamCodecs::CAcquisitionDecoder(); }

			virtual OpenViBE::boolean getAlgorithmPrototype(
				OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoderDesc::getAlgorithmPrototype(rAlgorithmPrototype);

				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_AcquisitionStreamDecoder_OutputParameterId_BufferDuration,              "Buffer duration",               OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_AcquisitionStreamDecoder_OutputParameterId_ExperimentInformationStream, "Experiment information stream", OpenViBE::Kernel::ParameterType_MemoryBuffer);
				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_AcquisitionStreamDecoder_OutputParameterId_SignalStream,                "Signal stream",                 OpenViBE::Kernel::ParameterType_MemoryBuffer);
				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_AcquisitionStreamDecoder_OutputParameterId_StimulationStream,           "Stimulation stream",            OpenViBE::Kernel::ParameterType_MemoryBuffer);
				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_AcquisitionStreamDecoder_OutputParameterId_ChannelLocalisationStream,   "Channel localisation stream",   OpenViBE::Kernel::ParameterType_MemoryBuffer);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoderDesc, OVP_ClassId_Algorithm_AcquisitionStreamDecoderDesc);
		};
	};
};

#endif // __SamplePlugin_Algorithms_CAcquisitionDecoder_H__
