#ifndef __SamplePlugin_Algorithms_CEBMLBaseDecoder_H__
#define __SamplePlugin_Algorithms_CEBMLBaseDecoder_H__

#include "../../ovp_defines.h"

#include <openvibe/ov_all.h>

#include <openvibe-toolkit/ovtk_all.h>

#include <ebml/IReader.h>
#include <ebml/IReaderHelper.h>
#include <ebml/TReaderCallbackProxy.h>

#include <stack>

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CEBMLBaseDecoder : public OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >
		{
		public:

			CEBMLBaseDecoder(void);

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >, OVP_ClassId_Algorithm_EBMLBaseStreamDecoder);

			// ebml callbacks
			virtual EBML::boolean isMasterChild(const EBML::CIdentifier& rIdentifier);
			virtual void openChild(const EBML::CIdentifier& rIdentifier);
			virtual void processChildData(const void* pBuffer, const EBML::uint64 ui64BufferSize);
			virtual void closeChild(void);

		protected:

			EBML::IReaderHelper* m_pEBMLReaderHelper;
			EBML::IReader* m_pEBMLReader;
			EBML::TReaderCallbackProxy1 < OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoder > m_oEBMLReaderCallbackProxy;

			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMemoryBuffer* > ip_pMemoryBufferToDecode;
		};

		class CEBMLBaseDecoderDesc : public OpenViBE::Plugins::IAlgorithmDesc
		{
		public:

			virtual OpenViBE::boolean getAlgorithmPrototype(
				OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_EBMLStreamDecoder_InputParameterId_MemoryBufferToDecode, "Memory buffer to decode", OpenViBE::Kernel::ParameterType_MemoryBuffer);

				rAlgorithmPrototype.addOutputTrigger(OVP_Algorithm_EBMLStreamDecoder_OutputTriggerId_ReceivedHeader, "Received header");
				rAlgorithmPrototype.addOutputTrigger(OVP_Algorithm_EBMLStreamDecoder_OutputTriggerId_ReceivedBuffer, "Received buffer");
				rAlgorithmPrototype.addOutputTrigger(OVP_Algorithm_EBMLStreamDecoder_OutputTriggerId_ReceivedEnd, "Received end");

				return true;
			}

			_IsDerivedFromClass_(OpenViBE::Plugins::IAlgorithmDesc, OVP_ClassId_Algorithm_EBMLBaseStreamDecoderDesc);
		};
	};
};

#endif // __SamplePlugin_Algorithms_CEBMLBaseDecoder_H__
