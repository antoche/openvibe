#ifndef __OpenViBEPlugins_SignalProcessing_CSpectralAnalysisCospectra_H__
#define __OpenViBEPlugins_SignalProcessing_CSpectralAnalysisCospectra_H__

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
		namespace SpectralAnalysisCospectra
		{
			// Used to store information about the signal stream
			class CSignalDescription
			{
				public:

					CSignalDescription(void)
						:m_ui32StreamVersion(1)
						,m_ui32SamplingRate(0)
						,m_ui32ChannelCount(0)
						,m_ui32SampleCount(0)
						,m_ui32CurrentChannel(0)
						,m_bReadyToSend(false)
					{
					}

				public:

					EBML::uint32 m_ui32StreamVersion;
					EBML::uint32 m_ui32SamplingRate;
					EBML::uint32 m_ui32ChannelCount;
					EBML::uint32 m_ui32SampleCount;
					std::vector<std::string> m_pChannelName;
					EBML::uint32 m_ui32CurrentChannel;

					OpenViBE::boolean m_bReadyToSend;
			};
		}

		/**
		* The Spectral Anlaysis plugin's main class.
		*/
		class CSpectralAnalysisCospectra : virtual public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, virtual public OpenViBEToolkit::IBoxAlgorithmSignalInputReaderCallback::ICallback
		{
		public:

			CSpectralAnalysisCospectra(void);

			virtual void release(void);

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);

			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);

			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithm, OVP_ClassId_SpectralAnalysisCospectra)

		public:

			virtual void writeSignalOutput(const void* pBuffer, const EBML::uint64 ui64BufferSize);

			virtual void setChannelCount(const OpenViBE::uint32 ui32ChannelCount);
			virtual void setChannelName(const OpenViBE::uint32 ui32ChannelIndex, const char* sChannelName);
			virtual void setSampleCountPerBuffer(const OpenViBE::uint32 ui32SampleCountPerBuffer);
			virtual void setSamplingRate(const OpenViBE::uint32 ui32SamplingFrequency);
			virtual void setSampleBuffer(const OpenViBE::float64* pBuffer);

		public:

			// Needed to read the input
			EBML::IReader* m_pReader;
			OpenViBEToolkit::IBoxAlgorithmSignalInputReaderCallback* m_pSignalReaderCallBack;

			// The current node identifier
			EBML::CIdentifier m_oCurrentIdentifier;

			//start time and end time of the last arrived chunk
			OpenViBE::uint64 m_ui64LastChunkStartTime;
			OpenViBE::uint64 m_ui64LastChunkEndTime;

			// Needed to write on the plugin output
			EBML::IWriter* m_pWriter;
			EBML::TWriterCallbackProxy1<OpenViBEPlugins::SignalProcessingGpl::CSpectralAnalysisCospectra> m_oSignalOutputWriterCallbackProxy;
			OpenViBEToolkit::IBoxAlgorithmSignalOutputWriter* m_pSignalOutputWriterHelper;

			//! Structure containing information about the signal stream
			SpectralAnalysisCospectra::CSignalDescription * m_pSignalDescription;

			//! Size of the matrix buffer (output signal)
			OpenViBE::uint64 m_ui64MatrixBufferSize;
			//! Output signal's matrix buffer
			EBML::float64* m_pMatrixBuffer;

			OpenViBE::boolean m_bHammingWindow;
			OpenViBE::boolean m_bHanningWindow;
			OpenViBE::boolean m_bHannWindow;
			OpenViBE::boolean m_bBlackmanWindow;
			OpenViBE::uint32 m_ui32WindowSize;
			OpenViBE::uint32 m_ui32PercentageOfOverlapping;
		};

		class CSpectralAnalysisCospectraDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }
			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Spectral analysis (Cospectra) - GPL"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Guillaume Gibert"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INSERM"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Computes cospectra"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Signal processing/Spectral analysis"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("0.1"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_SpectralAnalysisCospectra; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SignalProcessingGpl::CSpectralAnalysisCospectra(); }

			virtual OpenViBE::boolean getBoxPrototype(OpenViBE::Kernel::IBoxProto& rPrototype) const
			{
				rPrototype.addInput  ("Input signal",              OV_TypeId_Signal);
				rPrototype.addOutput ("Output signal",             OV_TypeId_Signal);
				rPrototype.addSetting("Window method",             OVP_TypeId_WindowMethod, "Hamming");
				rPrototype.addSetting("Window size",               OV_TypeId_Integer,       "32");
				rPrototype.addSetting("Percentage of Overlapping", OV_TypeId_Float,         "50");
				rPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_IsUnstable);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_SpectralAnalysisCospectraDesc)
		};
	}
}

#endif // __SamplePlugin_CSpectralAnalysisCospectra_H__
