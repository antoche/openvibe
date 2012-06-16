#ifndef __SamplePlugin_Algorithms_CStimulationDecoder_H__
#define __SamplePlugin_Algorithms_CStimulationDecoder_H__

#include "ovpCEBMLBaseDecoder.h"

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CStimulationDecoder : public OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoder
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoder, OVP_ClassId_Algorithm_StimulationStreamDecoder);

			// ebml callbacks
			virtual EBML::boolean isMasterChild(const EBML::CIdentifier& rIdentifier);
			virtual void openChild(const EBML::CIdentifier& rIdentifier);
			virtual void processChildData(const void* pBuffer, const EBML::uint64 ui64BufferSize);
			virtual void closeChild(void);

		protected:

			OpenViBE::Kernel::TParameterHandler < OpenViBE::IStimulationSet* > op_pStimulationSet;

		private:

			std::stack<EBML::CIdentifier> m_vNodes;

			OpenViBE::uint64 m_ui64StimulationIndex;
			OpenViBE::CStimulationSet m_oStimulationSet;
		};

		class CStimulationDecoderDesc : public OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoderDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Stimulation stream decoder"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Stream codecs/Decoders"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_Algorithm_StimulationStreamDecoder; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::StreamCodecs::CStimulationDecoder(); }

			virtual OpenViBE::boolean getAlgorithmPrototype(
				OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoderDesc::getAlgorithmPrototype(rAlgorithmPrototype);

				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_StimulationStreamDecoder_OutputParameterId_StimulationSet, "Stimulation set", OpenViBE::Kernel::ParameterType_StimulationSet);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoderDesc, OVP_ClassId_Algorithm_StimulationStreamDecoderDesc);
		};
	};
};

#endif // __SamplePlugin_Algorithms_CStimulationDecoder_H__
