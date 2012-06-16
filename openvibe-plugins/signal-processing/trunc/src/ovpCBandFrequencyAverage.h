#ifndef __OpenViBEPlugins_SignalProcessing_CBandFrequencyAverage_H__
#define __OpenViBEPlugins_SignalProcessing_CBandFrequencyAverage_H__

#include "ovp_defines.h"

#include <openvibe-toolkit/ovtk_all.h>

#include <ebml/IReader.h>
#include <ebml/IReaderHelper.h>
#include <ebml/IWriter.h>
#include <ebml/IWriterHelper.h>

#include <ebml/TReaderCallbackProxy.h>
#include <ebml/TWriterCallbackProxy.h>

#include <string>
#include <vector>

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		/**
		* The Band Frequency Average plugin's main class.
		*/
		class CBandFrequencyAverage : virtual public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{

		public:

			CBandFrequencyAverage(void);

			virtual void release(void);
			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_BandFrequencyAverage)

		public:

			virtual void writeFeatureVectorOutput(const void* pBuffer, const EBML::uint64 ui64BufferSize);

			virtual void setChannelCount(const OpenViBE::uint32 ui32ChannelCount);
			virtual void setChannelName(const OpenViBE::uint32 ui32ChannelIndex, const char* sChannelName);
			virtual void setFrequencyBandCount(const OpenViBE::uint32 ui32FrequencyBandCount);
			virtual void setFrequencyBandName(const OpenViBE::uint32 ui32FrequencyBandIndex, const char* sFrequencyBandName);
			virtual void setFrequencyBandStart(const OpenViBE::uint32 ui32FrequencyBandIndex, const OpenViBE::float64 f64FrequencyBandStart);
			virtual void setFrequencyBandStop(const OpenViBE::uint32 ui32FrequencyBandIndex, const OpenViBE::float64 f64FrequencyBandStop);
			virtual void setBuffer(const OpenViBE::float64* pBuffer);

			virtual void setFeatureVector(const OpenViBE::float64* pBuffer);

		public:

			// Needed to read the input
			EBML::IReader* m_pSpectrumReader;
			OpenViBEToolkit::IBoxAlgorithmSpectrumInputReaderCallback* m_pSpectrumReaderCallback;
			OpenViBEToolkit::IBoxAlgorithmSpectrumInputReaderCallback::TCallbackProxy1<OpenViBEPlugins::SignalProcessing::CBandFrequencyAverage> m_oSpectrumReaderCallbackProxy;

			//start time and end time of the last arrived chunk
			OpenViBE::uint64 m_ui64LastChunkStartTime;
			OpenViBE::uint64 m_ui64LastChunkEndTime;

			// Needed to write on the plugin output
			EBML::IWriter* m_pWriter;
			EBML::TWriterCallbackProxy1<OpenViBEPlugins::SignalProcessing::CBandFrequencyAverage> m_oFeatureVectorOutputWriterCallbackProxy;
			OpenViBEToolkit::IBoxAlgorithmFeatureVectorOutputWriter* m_pFeatureVectorOutputWriterHelper;

			///number of channels
			OpenViBE::uint32 m_ui32ChannelCount;
			//number of frequency bands
			OpenViBE::uint32 m_ui32FrequencyBandCount;

			OpenViBE::float64* m_pFrequencyBandStart;
			OpenViBE::float64* m_pFrequencyBandStop;

			//the current input buffer being processed
			const OpenViBE::float64* m_pInputBuffer;
			//the current output buffer being generated
			OpenViBE::float64* m_pOutputBuffer;

			OpenViBE::uint32 m_ui32LowFrequency;
			OpenViBE::uint32 m_ui32HighFrequency;

			OpenViBE::uint32 m_ui32IndexLowFrequency;
			OpenViBE::uint32 m_ui32IndexHighFrequency;

			OpenViBE::uint32 m_ui32VectorSize;
			OpenViBE::boolean m_bCoefComputed;
		};

		/**
		* Description of the band frequency average plugin
		*/
		class CBandFrequencyAverageDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }
			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Band Frequency average"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Guillaume Gibert"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INSERM"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Computes the average of a band frequency."); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Signal processing/Averaging"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("0.1"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_BandFrequencyAverage; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SignalProcessing::CBandFrequencyAverage(); }

			virtual OpenViBE::boolean getBoxPrototype(OpenViBE::Kernel::IBoxProto& rPrototype) const
			{
				rPrototype.addInput  ("Input spectrum",            OV_TypeId_Spectrum);
				rPrototype.addOutput ("Average of Band Frequency", OV_TypeId_FeatureVector);
				rPrototype.addSetting("Low Frequency (Hz)",        OV_TypeId_String, "21");
				rPrototype.addSetting("High Frequency (Hz)",       OV_TypeId_String, "29");
				rPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_IsUnstable);
				rPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_IsDeprecated);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_BandFrequencyAverageDesc)
		};
	}
}

#endif // __OpenViBEPlugins_SignalProcessing_CBandFrequencyAverage_H__


