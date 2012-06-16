#ifndef __OpenViBEPlugins_SignalProcessing_CSpectralAnalysis_H__
#define __OpenViBEPlugins_SignalProcessing_CSpectralAnalysis_H__

#include "ovp_defines.h"

#include <openvibe-toolkit/ovtk_all.h>

#include <ebml/IReader.h>
#include <ebml/IReaderHelper.h>
#include <ebml/IWriter.h>
#include <ebml/IWriterHelper.h>

#include <ebml/TReaderCallbackProxy.h>
#include <ebml/TWriterCallbackProxy.h>

#include <vector>
#include <map>
#include <string>

#ifndef  CString2Boolean
	#define CString2Boolean(string) (strcmp(string,"true"))?0:1
#endif

namespace OpenViBEPlugins
{
	namespace SignalProcessingGpl
	{
		/**
		* The Spectral Anlaysis plugin's main class.
		*/
		class CSpectralAnalysis : virtual public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:

			CSpectralAnalysis(void);

			virtual void release(void);
			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, OVP_ClassId_SpectralAnalysis)

		public:

			virtual void writeAmplitudeSpectrumOutput(const void* pBuffer, const EBML::uint64 ui64BufferSize);
			virtual void writePhaseSpectrumOutput(const void* pBuffer, const EBML::uint64 ui64BufferSize);
			virtual void writeRealPartSpectrumOutput(const void* pBuffer, const EBML::uint64 ui64BufferSize);
			virtual void writeImagPartSpectrumOutput(const void* pBuffer, const EBML::uint64 ui64BufferSize);

			virtual void setChannelCount(const OpenViBE::uint32 ui32ChannelCount);
			virtual void setChannelName(const OpenViBE::uint32 ui32ChannelIndex, const char* sChannelName);
			virtual void setSampleCountPerBuffer(const OpenViBE::uint32 ui32SampleCountPerBuffer);
			virtual void setSamplingRate(const OpenViBE::uint32 ui32SamplingFrequency);
			virtual void setSampleBuffer(const OpenViBE::float64* pBuffer);

			virtual void setBuffer(const OpenViBE::float64* pBuffer);

		public:

			// Needed to read the input
			EBML::IReader* m_pSignalReader;
			OpenViBEToolkit::IBoxAlgorithmSignalInputReaderCallback* m_pSignalReaderCallBack;
			OpenViBEToolkit::IBoxAlgorithmSignalInputReaderCallback::TCallbackProxy1<OpenViBEPlugins::SignalProcessingGpl::CSpectralAnalysis> m_oSignalReaderCallBackProxy;

			//start time and end time of the last arrived chunk
			OpenViBE::uint64 m_ui64LastChunkStartTime;
			OpenViBE::uint64 m_ui64LastChunkEndTime;

			// Needed to write on the plugin output
			EBML::IWriter* m_pWriter[4];
			EBML::TWriterCallbackProxy1<OpenViBEPlugins::SignalProcessingGpl::CSpectralAnalysis> m_oAmplitudeSpectrumOutputWriterCallbackProxy;
			EBML::TWriterCallbackProxy1<OpenViBEPlugins::SignalProcessingGpl::CSpectralAnalysis> m_oPhaseSpectrumOutputWriterCallbackProxy;
			EBML::TWriterCallbackProxy1<OpenViBEPlugins::SignalProcessingGpl::CSpectralAnalysis> m_oRealPartSpectrumOutputWriterCallbackProxy;
			EBML::TWriterCallbackProxy1<OpenViBEPlugins::SignalProcessingGpl::CSpectralAnalysis> m_oImagPartSpectrumOutputWriterCallbackProxy;

			OpenViBEToolkit::IBoxAlgorithmSpectrumOutputWriter* m_pSpectrumOutputWriterHelper;

			///number of channels
			OpenViBE::uint32 m_ui32ChannelCount;
			OpenViBE::uint32 m_ui32SamplingRate;
			OpenViBE::uint32 m_ui32FrequencyBandCount;
			OpenViBE::uint32 m_ui32SampleCount;

			//the current input buffer being processed
			const OpenViBE::float64* m_pInputBuffer;

			//the current output buffer being generated
			OpenViBE::float64* m_pOutputBuffer;

			OpenViBE::boolean m_bCoefComputed;
			OpenViBE::uint32 m_ui32FFTSize;

			OpenViBE::boolean m_bAmplitudeSpectrum;
			OpenViBE::boolean m_bPhaseSpectrum;
			OpenViBE::boolean m_bRealPartSpectrum;
			OpenViBE::boolean m_bImagPartSpectrum;
		};

		class CSpectralAnalysisDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }
			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Spectral analysis - GPL"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Guillaume Gibert"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INSERM"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Compute spectral analysis using Fast Fourier Transform"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Signal processing/Spectral analysis"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("0.1"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_SpectralAnalysis; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SignalProcessingGpl::CSpectralAnalysis(); }

			virtual OpenViBE::boolean getBoxPrototype(OpenViBE::Kernel::IBoxProto& rPrototype) const
			{
				rPrototype.addInput("Input signal", OV_TypeId_Signal);

				rPrototype.addOutput("Amplitude", OV_TypeId_Spectrum);
				rPrototype.addOutput("Phase", OV_TypeId_Spectrum);
				rPrototype.addOutput("Real Part", OV_TypeId_Spectrum);
				rPrototype.addOutput("Imag Part", OV_TypeId_Spectrum);

				rPrototype.addSetting("Spectral components", OVP_TypeId_SpectralComponent, "Amplitude");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_SpectralAnalysisDesc)
		};
	}
}

#endif // __SamplePlugin_CSpectralAnalysis_H__
