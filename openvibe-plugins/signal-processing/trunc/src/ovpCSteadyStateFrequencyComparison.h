#ifndef __OpenViBEPlugins_SignalProcessing_CSteadyStateFrequencyComparison_H__
#define __OpenViBEPlugins_SignalProcessing_CSteadyStateFrequencyComparison_H__

#include "ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>
#include <ebml/TWriterCallbackProxy.h>

#include <vector>

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{

		class CSteadyStateFrequencyComparison :  public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
			public:

				CSteadyStateFrequencyComparison();

				virtual void release(void);
				virtual OpenViBE::boolean initialize();
				virtual OpenViBE::boolean uninitialize();
				virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
				virtual OpenViBE::boolean process();

				_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithm, OVP_ClassId_BoxAlgorithm_SteadyStateFrequencyComparison)

			public:

				virtual void setFeatureCount(const OpenViBE::uint32 ui32FeatureCount);
				virtual void setFeatureName(const OpenViBE::uint32 ui32FeatureIndex, const char* sFeatureName);
				virtual void setFeatureVector(const OpenViBE::float64* pFeatureBuffer);

				virtual void writeToAmplitudeOutput(const void* pBuffer, const EBML::uint64 ui64BufferSize);

			public:

				// Needed to read the input
				EBML::IReader* m_pFeatureVectorReader;
				OpenViBEToolkit::IBoxAlgorithmFeatureVectorInputReaderCallback * m_pFeatureVectorInputReaderCallback;
				OpenViBEToolkit::IBoxAlgorithmFeatureVectorInputReaderCallback::TCallbackProxy1<OpenViBEPlugins::SignalProcessing::CSteadyStateFrequencyComparison> m_oFeatureVectorReaderCallbackProxy;

				//start time and end time of the last arrived chunk
				OpenViBE::uint64 m_ui64LastChunkStartTime;
				OpenViBE::uint64 m_ui64LastChunkEndTime;

				//EBML writing stuff
				EBML::IWriter* m_pWriter;
				EBML::TWriterCallbackProxy1<OpenViBEPlugins::SignalProcessing::CSteadyStateFrequencyComparison> m_oAmplitudeOutputWriterCallbackProxy;
				OpenViBEToolkit::IBoxAlgorithmStreamedMatrixOutputWriter * m_pStreamedMatrixOutputWriterHelper;

				// error flag
				OpenViBE::boolean m_bError;

				//the current input buffer being processed
				const OpenViBE::float64* m_pInputBuffer;
				//the current output buffer being generated
				OpenViBE::float64* m_pOutputBuffer;

				OpenViBE::boolean m_bRatio;
				OpenViBE::boolean m_bSubstraction;
				OpenViBE::boolean m_bLateralityIndex;

				OpenViBE::uint32 m_ui32NumberOfInput;
				OpenViBE::uint32 m_ui32CurrentInput;

				OpenViBE::float64 m_float64Amplitude;
		};

		class CSteadyStateFrequencyComparisonDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
			public:

				virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Frequency Comparison"); }
				virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Guillaume Gibert"); }
				virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INSERM"); }
				virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Comparison of Frequencies"); }
				virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("Comparison of Frequencies"); }
				virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Signal processing/Basic"); }
				virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("0.1"); }
				virtual void release(void)                                   { }
				virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_SteadyStateFrequencyComparison; }
				virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SignalProcessing::CSteadyStateFrequencyComparison(); }

				virtual OpenViBE::boolean getBoxPrototype(OpenViBE::Kernel::IBoxProto& rPrototype) const
				{
					rPrototype.addInput  ("Feature vector 1",     OV_TypeId_FeatureVector);
					rPrototype.addInput  ("Feature vector 2",     OV_TypeId_FeatureVector);
					rPrototype.addOutput ("Comparison Amplitude", OV_TypeId_StreamedMatrix);
					rPrototype.addSetting("Kind of Comparison",   OVP_TypeId_ComparisonMethod, OVP_TypeId_ComparisonMethod_Ratio.toString());
					rPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_IsUnstable);
					rPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_IsDeprecated);
					return true;
				}

				_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_SteadyStateFrequencyComparisonDesc)
		};
	};
};

#endif


