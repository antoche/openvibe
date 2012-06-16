#ifndef __OpenViBEPlugins_SignalProcessing_CChannelSelector_H__
#define __OpenViBEPlugins_SignalProcessing_CChannelSelector_H__

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
				std::vector<std::string> m_oChannelName;
				EBML::uint32 m_ui32CurrentChannel;

				OpenViBE::boolean m_bReadyToSend;
		};

		/**
		* This plugin may be used to filter a signal stream in order to select only a few desired channels.
		* The desired output channels are to be selected in the plugin configuration box. The different entries
		* needs to be seperated by a ";". Channels names not present in the original stream are ignored.
		*/
		class CChannelSelector : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, virtual public OpenViBEToolkit::IBoxAlgorithmSignalInputReaderCallback::ICallback
		{
		public:

			CChannelSelector(void);

			virtual void release(void);

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);

			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);

			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithm, OVP_ClassId_ChannelSelector)

		public:

			virtual void writeSignalOutput(const void* pBuffer, const EBML::uint64 ui64BufferSize);

			virtual void setChannelCount(const OpenViBE::uint32 ui32ChannelCount);
			virtual void setChannelName(const OpenViBE::uint32 ui32ChannelIndex, const char* sChannelName);
			virtual void setSampleCountPerBuffer(const OpenViBE::uint32 ui32SampleCountPerBuffer);
			virtual void setSamplingRate(const OpenViBE::uint32 ui32SamplingFrequency);
			virtual void setSampleBuffer(const OpenViBE::float64* pBuffer);

		public:

			std::vector<std::string> m_vSelectedChannelNames;		//! Contains the names of the channels to select
			std::vector<OpenViBE::uint32> m_vSelectedChannelIndexes;	//! Contains the indexes of the channels to select

			// Needed to read the input
			EBML::IReader* m_pReader;

			OpenViBEToolkit::IBoxAlgorithmSignalInputReaderCallback* m_pSignalReaderCallBack;

			// the current node identifier
			EBML::CIdentifier m_oCurrentIdentifier;

			OpenViBE::uint64 m_ui64LastChunkStartTime;
			OpenViBE::uint64 m_ui64LastChunkEndTime;

			// if false then the next Dimension will be the channels' one
			//    true  then the samples' one
			OpenViBE::boolean m_bCurrentDimension;

			// stores the indexes of the selected channels in the original channel array
			std::vector<std::string> m_vChannelNames;
			std::vector<OpenViBE::uint32> m_vChannelsToKeep;

			// Needed to write on the plugin output
			EBML::IWriter* m_pWriter;

			EBML::TWriterCallbackProxy1<OpenViBEPlugins::SignalProcessing::CChannelSelector> m_oSignalOutputWriterCallbackProxy;

			OpenViBEToolkit::IBoxAlgorithmSignalOutputWriter* m_pSignalOutputWriterHelper;

			OpenViBE::boolean m_bSelectionbyIndex;		//! If true the channels are selected by their index in the stream instead of their name

			CSignalDescription * m_pSignalDescription;	//! Structure containing information about the signal stream

			OpenViBE::uint64 m_ui64MatrixBufferSize; 	//! Size of the matrix buffer (output signal)
			EBML::float64* m_pMatrixBuffer;			//! Output signal's matrix buffer
		};

		/**
		* Description of the channel selection plugin
		*/
		class CChannelSelectorDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }
			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Channel selector (deprecated)"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Bruno Renier"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Channel selector filter"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Signal processing/Basic"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("0.6"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_ChannelSelector; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SignalProcessing::CChannelSelector(); }

			virtual OpenViBE::boolean getBoxPrototype(OpenViBE::Kernel::IBoxProto& rPrototype) const
			{
				rPrototype.addInput("Input signal", OV_TypeId_Signal);
				rPrototype.addOutput("Filtered signal", OV_TypeId_Signal);
				rPrototype.addSetting("Channels list", OV_TypeId_String, "");
				rPrototype.addSetting("Selection by index", OV_TypeId_Boolean, "false");
				rPrototype.addFlag(OpenViBE::Kernel::BoxFlag_IsDeprecated);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_ChannelSelectorDesc)
		};
	}
}

#endif // __SamplePlugin_CChannelSelector_H__
