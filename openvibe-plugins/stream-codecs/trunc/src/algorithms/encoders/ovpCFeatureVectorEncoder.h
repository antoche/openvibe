#ifndef __SamplePlugin_Algorithms_CFeatureVectorEncoder_H__
#define __SamplePlugin_Algorithms_CFeatureVectorEncoder_H__

#include "ovpCStreamedMatrixEncoder.h"

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CFeatureVectorEncoder : public OpenViBEPlugins::StreamCodecs::CStreamedMatrixEncoder
		{
		public:

			virtual void release(void) { delete this; }

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CStreamedMatrixEncoder, OVP_ClassId_Algorithm_FeatureVectorStreamEncoder);
		};

		class CFeatureVectorEncoderDesc : public OpenViBEPlugins::StreamCodecs::CStreamedMatrixEncoderDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Feature vector stream encoder"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Stream codecs/Encoders"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_Algorithm_FeatureVectorStreamEncoder; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::StreamCodecs::CFeatureVectorEncoder(); }

			virtual OpenViBE::boolean getAlgorithmPrototype(
				OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				OpenViBEPlugins::StreamCodecs::CStreamedMatrixEncoderDesc::getAlgorithmPrototype(rAlgorithmPrototype);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CStreamedMatrixEncoderDesc, OVP_ClassId_Algorithm_FeatureVectorStreamEncoderDesc);
		};
	};
};

#endif // __SamplePlugin_Algorithms_CFeatureVectorEncoder_H__
