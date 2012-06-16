#ifndef __SamplePlugin_Algorithms_CFeatureVectorDecoder_H__
#define __SamplePlugin_Algorithms_CFeatureVectorDecoder_H__

#include "ovpCStreamedMatrixDecoder.h"

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CFeatureVectorDecoder : public OpenViBEPlugins::StreamCodecs::CStreamedMatrixDecoder
		{
		public:

			virtual void release(void) { delete this; }

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CStreamedMatrixDecoder, OVP_ClassId_Algorithm_FeatureVectorStreamDecoder);

		};

		class CFeatureVectorDecoderDesc : public OpenViBEPlugins::StreamCodecs::CStreamedMatrixDecoderDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Feature vector stream decoder"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Stream codecs/Decoders"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_Algorithm_FeatureVectorStreamDecoder; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::StreamCodecs::CFeatureVectorDecoder(); }

			virtual OpenViBE::boolean getAlgorithmPrototype(
				OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				OpenViBEPlugins::StreamCodecs::CStreamedMatrixDecoderDesc::getAlgorithmPrototype(rAlgorithmPrototype);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CStreamedMatrixDecoderDesc, OVP_ClassId_Algorithm_FeatureVectorStreamDecoderDesc);
		};
	};
};

#endif // __SamplePlugin_Algorithms_CFeatureVectorDecoder_H__
