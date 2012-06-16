#ifndef __OpenViBEPlugins_file_io_CBCICompetitionIIIbReader_H__
#define __OpenViBEPlugins_file_io_CBCICompetitionIIIbReader_H__

#include "ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>
#include <ebml/TWriterCallbackProxy.h>
#include <ebml/IWriter.h>

#include <vector>
#include <string>
#include <sstream>
#include <fstream>

namespace OpenViBEPlugins
{
	namespace FileIO
	{

		class CBCICompetitionIIIbReader : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{

			public:

				CBCICompetitionIIIbReader();

				virtual void release(void) { delete this; }

				virtual OpenViBE::boolean initialize();

				virtual OpenViBE::boolean uninitialize();

				virtual OpenViBE::boolean process();

				virtual OpenViBE::boolean processClock(OpenViBE::CMessageClock& rMessageClock);

				virtual OpenViBE::uint64 getClockFrequency(){ return m_ui64ClockFrequency; }

			public:
				void writeSignalInformation();

				virtual void writeSignalOutput(const void* pBuffer, const EBML::uint64 ui64BufferSize);
				virtual void writeStimulationOutput(const void* pBuffer, const EBML::uint64 ui64BufferSize);

				_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithm, OVP_ClassId_BCICompetitionIIIbReader)


			public:
				OpenViBE::boolean m_bErrorOccured;	//true if an error has occured while reading the GDF file

				//The filename and handle
				std::ifstream m_oSignalFile;
				OpenViBE::uint64 m_ui64FileSize;

				//EBML handling
				EBML::TWriterCallbackProxy1<OpenViBEPlugins::FileIO::CBCICompetitionIIIbReader> * m_pOutputWriterCallbackProxy[2];
				EBML::IWriter* m_pWriter[2];

				OpenViBEToolkit::IBoxAlgorithmSignalOutputWriter * m_pSignalOutputWriterHelper;
				OpenViBEToolkit::IBoxAlgorithmStimulationOutputWriter * m_pStimulationOutputWriterHelper;

				OpenViBE::uint64 m_ui64ClockFrequency;

				std::vector<OpenViBE::uint64> m_oTriggerTime;
				std::vector<OpenViBE::uint64> m_oEndOfTrial;
				std::vector<OpenViBE::uint64> m_oCueDisplayStart;
				std::vector<OpenViBE::uint64> m_oFeedbackStart;
				std::vector<OpenViBE::uint64> m_oClassLabels;
				std::vector<OpenViBE::boolean> m_oArtifacts;
				std::vector<OpenViBE::uint64> m_oTrueLabels;


				OpenViBE::uint32 m_ui32SamplesPerBuffer;
				OpenViBE::uint32 m_ui32SamplingRate;
				OpenViBE::uint32 m_ui32SentSampleCount;

				std::vector<OpenViBE::float64> m_oMatrixBuffer;
				OpenViBE::boolean m_bEndOfFile;

				OpenViBE::uint32 m_ui32CurrentTrial;

				OpenViBE::boolean m_bKeepTrainingSamples;
				OpenViBE::boolean m_bKeepTestSamples;
				OpenViBE::boolean m_bKeepArtifactSamples;

				OpenViBE::float64 m_f64TrialLength;
				OpenViBE::float64 m_f64CueDisplayStart;
				OpenViBE::float64 m_f64FeedbackStart;

		};



		class CBCICompetitionIIIbReaderDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
			public:
				virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("BCI competition IIIb reader"); }
				virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Bruno Renier"); }
				virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
				virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Reads ASCII version of BCI competition IIIb datasets."); }
				virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("Reads signal samples, stimulations and class labels from the BCI competition IIIb ASCII datasets."); }
				virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("File reading and writing/BCI Competition"); }
				virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("0.7"); }
				virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-open"); }

				virtual void release(void)                                   { }
				virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BCICompetitionIIIbReader; }
				virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::FileIO::CBCICompetitionIIIbReader(); }

				virtual OpenViBE::boolean getBoxPrototype(OpenViBE::Kernel::IBoxProto& rPrototype) const
				{
					// Adds box outputs
					rPrototype.addOutput("Signal", OV_TypeId_Signal);
					rPrototype.addOutput("Stimulations", OV_TypeId_Stimulations);

					// Adds settings
					rPrototype.addSetting("Signal file", OV_TypeId_Filename, "");
					rPrototype.addSetting("Triggers file", OV_TypeId_Filename, "");
					rPrototype.addSetting("Labels file", OV_TypeId_Filename, "");
					rPrototype.addSetting("Artifact file", OV_TypeId_Filename, "");
					rPrototype.addSetting("True labels file", OV_TypeId_Filename, "");

					rPrototype.addSetting("Samples per buffer", OV_TypeId_Integer, "32");

					rPrototype.addSetting("Offline", OV_TypeId_Boolean, "false");
					rPrototype.addSetting("Train?", OV_TypeId_Boolean, "true");
					rPrototype.addSetting("Test?", OV_TypeId_Boolean, "false");
					rPrototype.addSetting("Keep artifacts?", OV_TypeId_Boolean, "false");

					rPrototype.addSetting("Trial length", OV_TypeId_Float, "8.0");
					rPrototype.addSetting("CUE display Start", OV_TypeId_Float, "3.0");
					rPrototype.addSetting("Feedback start", OV_TypeId_Float, "4.0");

					rPrototype.addFlag(OpenViBE::Kernel::BoxFlag_IsUnstable);
					return true;
				}

				_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BCICompetitionIIIbReaderDesc)
		};
	};
};

#endif
