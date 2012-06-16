#ifndef __OpenViBEPlugins_BoxAlgorithm_EDFFileWriter_H__
#define __OpenViBEPlugins_BoxAlgorithm_EDFFileWriter_H__

#include "../../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include "edflib.h"
#include <queue>
#include <deque>


namespace OpenViBEPlugins
{
	namespace FileIO
	{
		class CBoxAlgorithmEDFFileWriter : public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			//CBoxAlgorithmEDFFileWriter(void);
			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_EDFFileWriter);

		protected:

			OpenViBEToolkit::TExperimentInformationDecoder < CBoxAlgorithmEDFFileWriter > m_ExperimentInformationDecoder;
			OpenViBEToolkit::TSignalDecoder < CBoxAlgorithmEDFFileWriter > m_SignalDecoder;
			OpenViBEToolkit::TStimulationDecoder < CBoxAlgorithmEDFFileWriter > m_StimulationDecoder;
			
			OpenViBE::CString m_sFilename;
			OpenViBE::boolean m_bIsFileOpened;
			int m_iFileHandle;
			int m_iSampleFrequency;
			int m_iNumberOfChannels;
			int m_iNumberOfSamplesPerChunk;
			std::queue < OpenViBE::float64, std::deque<OpenViBE::float64> > buffer;
			double * m_pTemporyBuffer;
			double * m_pTemporyBufferToWrite;
			//int * m_pTemporyBuffer;
			//int * m_pTemporyBufferToWrite;
		};

		class CBoxAlgorithmEDFFileWriterDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("EDF File Writer"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Aurélien Van Langhenhove"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("CICIT Garches"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Writes experiment information, signal and stimulations in a EDF file"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("File reading and writing/EDF"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-save"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_EDFFileWriter; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::FileIO::CBoxAlgorithmEDFFileWriter; }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				
				rBoxAlgorithmPrototype.addInput  ("Experiment information",  OV_TypeId_ExperimentationInformation);
				rBoxAlgorithmPrototype.addInput  ("Signal",  OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addInput  ("Stimulations",  OV_TypeId_Stimulations);

				rBoxAlgorithmPrototype.addSetting("Filename",         OV_TypeId_Filename, "record-[$core{date}-$core{time}].edf");

				//rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_IsUnstable);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_EDFFileWriterDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_EDFFileWriter_H__
