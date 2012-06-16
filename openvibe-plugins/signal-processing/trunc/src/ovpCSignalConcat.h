#ifndef __OpenViBEPlugins_SignalProcessing_CSignalConcatenation_H__
#define __OpenViBEPlugins_SignalProcessing_CSignalConcatenation_H__

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
#include <cstdio>

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CSignalConcatenation : virtual public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, virtual public OpenViBEToolkit::IBoxAlgorithmSignalInputReaderCallback::ICallback, virtual public OpenViBEToolkit::IBoxAlgorithmStimulationInputReaderCallback::ICallback
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

		public:

			CSignalConcatenation(void);

			virtual void release(void);

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);

			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);

			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithm, OVP_ClassId_SignalConcatenation)

		public:

			virtual void writeSignalOutput(const void* pBuffer, const EBML::uint64 ui64BufferSize);

			virtual void setChannelCount(const OpenViBE::uint32 ui32ChannelCount);
			virtual void setChannelName(const OpenViBE::uint32 ui32ChannelIndex, const char* sChannelName);
			virtual void setSampleCountPerBuffer(const OpenViBE::uint32 ui32SampleCountPerBuffer);
			virtual void setSamplingRate(const OpenViBE::uint32 ui32SamplingFrequency);
			virtual void setSampleBuffer(const OpenViBE::float64* pBuffer);

			virtual void setStimulationCount(const OpenViBE::uint32 ui32StimulationCount);
			virtual void setStimulation(const OpenViBE::uint32 ui32StimulationIndex, const OpenViBE::uint64 ui64StimulationIdentifier, const OpenViBE::uint64 ui64StimulationDate);

			virtual void writeStimulationOutput(const void* pBuffer, const EBML::uint64 ui64BufferSize);

		public:

			// Needed to read the input
			std::vector<EBML::IReader*> m_oSignalReader;
			std::vector<EBML::IReader*> m_oStimulationReader;
			OpenViBEToolkit::IBoxAlgorithmSignalInputReaderCallback* m_pSignalReaderCallBack;
			OpenViBEToolkit::IBoxAlgorithmStimulationInputReaderCallback* m_pStimulationReaderCallBack;

			// the current node identifier
			EBML::CIdentifier m_oCurrentIdentifier;

			//start time and end time of the last arrived chunk
			OpenViBE::uint64 m_ui64LastChunkStartTime;
			OpenViBE::uint64 m_ui64LastChunkEndTime;

			// Needed to write on the plugin output
			EBML::IWriter* m_pSignalWriter;
			EBML::IWriter* m_pStimulationWriter;
			EBML::TWriterCallbackProxy1<OpenViBEPlugins::SignalProcessing::CSignalConcatenation> m_oSignalOutputWriterCallbackProxy;
			EBML::TWriterCallbackProxy1<OpenViBEPlugins::SignalProcessing::CSignalConcatenation>  m_oStimulationOutputWriterCallbackProxy;
			OpenViBEToolkit::IBoxAlgorithmSignalOutputWriter* m_pSignalOutputWriterHelper;
			OpenViBEToolkit::IBoxAlgorithmStimulationOutputWriter * m_pStimulationOutputWriterHelper;

			//! Structure containing information about the signal stream
			CSignalDescription * m_pSignalDescription;

			//! Size of the matrix buffer (output signal)
			OpenViBE::uint64 m_ui64MatrixBufferSize;
			//! Output signal's matrix buffer
			EBML::float64* m_pMatrixBuffer;

			OpenViBE::uint32 m_ui32CurrentInput;
			OpenViBE::uint64 m_ui64TimeOffset;
			OpenViBE::uint64 m_ui64EndStimulation;

			std::vector<std::pair<OpenViBE::uint64, OpenViBE::uint64> > m_oStimulations;
			OpenViBE::uint32 m_ui32StimulationCount;

			OpenViBE::boolean m_bChangeInput;
		};

		class CSignalConcatenationDescListener : public OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >
		{
		public:

			OpenViBE::boolean check(OpenViBE::Kernel::IBox& rBox)
			{
				char l_sName[1024];
				OpenViBE::uint32 i;

				for(i=0; i<rBox.getInputCount()>>1; i++)
				{
					sprintf(l_sName, "Input signal %u", i+1);
					rBox.setInputName(i*2, l_sName);
					rBox.setInputType(i*2, OV_TypeId_Signal);

					sprintf(l_sName, "Input stimulations %u", i+1);
					rBox.setInputName(i*2+1, l_sName);
					rBox.setInputType(i*2+1, OV_TypeId_Stimulations);
				}

				return true;
			}

			virtual OpenViBE::boolean onInputRemoved(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				if(ui32Index&1)
				{
					rBox.removeInput(ui32Index&!1);
				}
				else
				{
					rBox.removeInput(ui32Index);
				}
				return this->check(rBox);
			}

			virtual OpenViBE::boolean onInputAdded(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				rBox.addInput("", OV_TypeId_Stimulations);
				return this->check(rBox);
			};

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier);
		};

		class CSignalConcatenationDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }
			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Signal concatenation (deprecated)"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Bruno Renier"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("\"Concatenates\" multiple signal streams and there associated stimulation streams."); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("Outputs the signal and stimulations from the first couple of inputs, then the data from the next couple of inputs and so on.\n DEPRECATED: use File Reading and Writing/Signal Concatenation instead (faster and more stable). "); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Signal processing/Basic"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("0.6"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_SignalConcatenation; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SignalProcessing::CSignalConcatenation(); }
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener(void) const               { return new CSignalConcatenationDescListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }

			virtual OpenViBE::boolean getBoxPrototype(OpenViBE::Kernel::IBoxProto& rPrototype) const
			{
				rPrototype.addInput("Input signal 1",       OV_TypeId_Signal);
				rPrototype.addInput("Input stimulations 1", OV_TypeId_Stimulations);
				rPrototype.addInput("Input signal 2",       OV_TypeId_Signal);
				rPrototype.addInput("Input stimulations 2", OV_TypeId_Stimulations);

				rPrototype.addOutput("Output signal",       OV_TypeId_Signal);
				rPrototype.addOutput("Output stimulations", OV_TypeId_Stimulations);

				rPrototype.addSetting("Stimulation to consider an input as finished", OV_TypeId_Stimulation, "OVTK_StimulationId_EndOfFile");

				rPrototype.addFlag(OpenViBE::Kernel::BoxFlag_IsUnstable);
				rPrototype.addFlag(OpenViBE::Kernel::BoxFlag_IsDeprecated);
				rPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddInput);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_SignalConcatenationDesc)
		};
	}
}

#endif // __OpenViBEPlugins_SignalProcessing_CSignalConcatenation_H__


