#ifndef __OpenViBEPlugins_SignalProcessing_CReferenceChannelOld_H__
#define __OpenViBEPlugins_SignalProcessing_CReferenceChannelOld_H__

#include "ovp_defines.h"

#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include <ebml/IReader.h>
#include <ebml/IReaderHelper.h>
#include <ebml/IWriter.h>
#include <ebml/IWriterHelper.h>

#include <ebml/TReaderCallbackProxy.h>
#include <ebml/TWriterCallbackProxy.h>

#include <vector>
#include <string>

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{

		class CReferenceChannelOld : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, public OpenViBEToolkit::IBoxAlgorithmSignalInputReaderCallback::ICallback
		{
		public:

			CReferenceChannelOld();

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize();

			virtual OpenViBE::boolean uninitialize();

			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);

			virtual OpenViBE::boolean process();

			virtual void writeSignalOutput(const void* pBuffer, const EBML::uint64 ui64BufferSize);

			virtual void setChannelCount(const OpenViBE::uint32 ui32ChannelCount);
			virtual void setChannelName(const OpenViBE::uint32 ui32ChannelIndex, const char* sChannelName);
			virtual void setSampleCountPerBuffer(const OpenViBE::uint32 ui32SampleCountPerBuffer);
			virtual void setSamplingRate(const OpenViBE::uint32 ui32SamplingFrequency);
			virtual void setSampleBuffer(const OpenViBE::float64* pBuffer);

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithm, OVP_ClassId_ReferenceChannel)

		protected:

			// Needed to read the input
			EBML::IReader* m_pReader;
			OpenViBEToolkit::IBoxAlgorithmSignalInputReaderCallback* m_pSignalReaderCallBack;

			OpenViBE::uint64 m_ui64LastChunkStartTime;
			OpenViBE::uint64 m_ui64LastChunkEndTime;

			// Needed to write on the plugin output
			EBML::IWriter* m_pWriter;
			EBML::TWriterCallbackProxy1<OpenViBEPlugins::SignalProcessing::CReferenceChannelOld> m_oSignalOutputWriterCallbackProxy;
			OpenViBEToolkit::IBoxAlgorithmSignalOutputWriter* m_pSignalOutputWriterHelper;

			OpenViBE::uint32 m_ui32SamplingFrequency;
			OpenViBE::uint32 m_ui32SamplesPerBuffer;
			std::vector<std::string> m_oChannelNames;

			//! The reference channel
			OpenViBE::uint32 m_ui32ReferenceChannel;

			//! True if an error has occured
			OpenViBE::boolean m_bError;

			OpenViBE::float64 * m_pMatrixBuffer;
			OpenViBE::uint64 m_ui64MatrixBufferSize;
		};

		class CReferenceChannelOldDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Reference channel (deprecated)"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Bruno Renier"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Sets a reference channel in the signal stream."); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("Substracts the value of the samples from the reference channel from the other channels' samples."); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Signal processing/Basic"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("0.5"); }
			virtual void release(void)                                   { }
			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_ReferenceChannel; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SignalProcessing::CReferenceChannelOld(); }

			virtual OpenViBE::boolean getBoxPrototype(OpenViBE::Kernel::IBoxProto& rPrototype) const
			{
				rPrototype.addInput("Input signal", OV_TypeId_Signal);
				rPrototype.addSetting("Reference channel", OV_TypeId_Integer, "0");
				rPrototype.addOutput("Filtered signal", OV_TypeId_Signal);
				rPrototype.addFlag(OpenViBE::Kernel::BoxFlag_IsDeprecated);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_ReferenceChannelDesc)
		};
	};
};

#endif

