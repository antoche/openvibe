#ifndef __OpenViBEPlugins_SignalProcessing_CEpoching_H__
#define __OpenViBEPlugins_SignalProcessing_CEpoching_H__

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

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		namespace Epoching
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
		* The Epoching plugin's main class.
		*/
		class CEpoching : virtual public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, virtual public OpenViBEToolkit::IBoxAlgorithmSignalInputReaderCallback::ICallback
		{
		public:

			CEpoching(void);

			virtual void release(void);

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);

			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);

			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithm, OVP_ClassId_BoxAlgorithm_Epoching)

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
			EBML::TWriterCallbackProxy1<OpenViBEPlugins::SignalProcessing::CEpoching> m_oSignalOutputWriterCallbackProxy;
			OpenViBEToolkit::IBoxAlgorithmSignalOutputWriter* m_pSignalOutputWriterHelper;

			//! Structure containing information about the signal stream
			Epoching::CSignalDescription * m_pSignalDescription;

			//! Size of the matrix buffer (output signal)
			OpenViBE::uint64 m_ui64MatrixBufferSize;

			//! Output signal's matrix buffer
			EBML::float64* m_pMatrixBuffer;

			OpenViBE::uint32 m_ui32SizeInputBuffer;
			OpenViBE::uint32 m_ui32InputChannelCount;

			OpenViBE::float64 m_float64SizeTimeOutputBuffer;
			OpenViBE::float64 m_float64SizeTimeIntervalInterTrigger;

			OpenViBE::uint32 m_ui32SizeSampleOutputBuffer;
			OpenViBE::uint32 m_ui32SizeSampleIntervalInterTrigger;

			OpenViBE::float64* m_pTempMatrixBuffer;
			OpenViBE::float64* m_pTempMatrixDbBuffer;

			OpenViBE::uint32 m_ui32IndexTempMatrixBuffer;
			OpenViBE::uint32 m_ui32SizeTempMatrixBuffer;

			OpenViBE::uint32 m_ui32IndexIn;
			OpenViBE::uint64 m_ui32IndexInGlobal;

			OpenViBE::uint32 m_ui32IndexLoopBuffer;

			OpenViBE::boolean m_bFlagFirstTime;
		};

		/**
		* Description of the Epoching plugin
		*/
		class CEpochingDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }
			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Epoching"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Guillaume Gibert"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INSERM"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Epochs buffer of data"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Signal processing/Epoching"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("0.1"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_Epoching; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SignalProcessing::CEpoching(); }

			virtual OpenViBE::boolean getBoxPrototype(OpenViBE::Kernel::IBoxProto& rPrototype) const
			{
				rPrototype.addInput("Input signal", OV_TypeId_Signal);
				rPrototype.addSetting("Interval Inter Trigger (ms)", OV_TypeId_Float, "40");
				rPrototype.addSetting("Size of epoch (ms)", OV_TypeId_Float, "55");
				rPrototype.addOutput("Output signal", OV_TypeId_Signal);
				rPrototype.addFlag(OpenViBE::Kernel::BoxFlag_IsDeprecated);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_EpochingDesc)
		};
	}
}

#endif // __SamplePlugin_CEpoching_H__
