#ifndef __OpenViBEPlugins_Classification_CLDAClassifier_H__
#define __OpenViBEPlugins_Classification_CLDAClassifier_H__

#include "ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>
#include <ebml/TWriterCallbackProxy.h>

#include <vector>

namespace OpenViBEPlugins
{
	namespace Classification
	{

		class CLDAClassifier : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, virtual public OpenViBEToolkit::IBoxAlgorithmFeatureVectorInputReaderCallback::ICallback
		{

			//EBML writing stuff
			EBML::IWriter* m_pWriter[2];
			EBML::TWriterCallbackProxy1<OpenViBEPlugins::Classification::CLDAClassifier> m_oClassOutputWriterCallbackProxy;
			EBML::TWriterCallbackProxy1<OpenViBEPlugins::Classification::CLDAClassifier> m_oAmplitudeOutputWriterCallbackProxy;

			OpenViBEToolkit::IBoxAlgorithmStreamedMatrixOutputWriter * m_pStreamedMatrixOutputWriterHelper;

			// Needed to read the input
			EBML::IReader* m_pReader;
			OpenViBEToolkit::IBoxAlgorithmFeatureVectorInputReaderCallback * m_pFeatureVectorInputReaderCallback;

			OpenViBE::uint64 m_ui64LastChunkStartTime;
			OpenViBE::uint64 m_ui64LastChunkEndTime;

			std::vector<OpenViBE::float64> m_oCoefficients;

			// error flag
			OpenViBE::boolean m_bError;

			public:

				CLDAClassifier();

				virtual void release(void) { delete this; }

				virtual OpenViBE::boolean initialize();

				virtual OpenViBE::boolean uninitialize();

				virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);

				virtual OpenViBE::boolean process();

				_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithm, OVP_ClassId_LDAClassifier)

				virtual void setFeatureCount(const OpenViBE::uint32 ui32FeatureCount);
				virtual void setFeatureName(const OpenViBE::uint32 ui32FeatureIndex, const char* sFeatureName);
				virtual void setFeatureVector(const OpenViBE::float64* pFeatureBuffer);

				virtual void writeToClassOutput(const void* pBuffer, const EBML::uint64 ui64BufferSize);
				virtual void writeToAmplitudeOutput(const void* pBuffer, const EBML::uint64 ui64BufferSize);

		};

		class CLDAClassifierDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
			public:
				virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("LDA classifier"); }
				virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Bruno Renier"); }
				virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
				virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Linear discriminant analysis classifier."); }
				virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("Classify incoming feature vectors by using the linear discriminant analysis method."); }
				virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Classification"); }
				virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("0.5"); }
				virtual void release(void)                                   { }
				virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_LDAClassifier; }
				virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::Classification::CLDAClassifier(); }

				virtual OpenViBE::boolean getBoxPrototype(OpenViBE::Kernel::IBoxProto& rPrototype) const
				{
					rPrototype.addInput("Feature vector", OV_TypeId_FeatureVector);

					rPrototype.addOutput("Feature class", OV_TypeId_StreamedMatrix);
					rPrototype.addOutput("Amplitude", OV_TypeId_StreamedMatrix);

					rPrototype.addSetting("LDA parameters", OV_TypeId_String, "");

					return true;
				}

				_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_LDAClassifierDesc)
		};
	};
};

#endif

