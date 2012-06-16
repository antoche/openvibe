#ifndef __OpenViBEPlugins_Stimulation_CFeatureAggregator_H__
#define __OpenViBEPlugins_Stimulation_CFeatureAggregator_H__

#include "ovp_defines.h"

#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include <ebml/IWriter.h>
#include <ebml/TWriterCallbackProxy.h>

#include <string>
#include <vector>
#include <queue>
#include <cstdio>

namespace OpenViBEPlugins
{
	namespace FeatureExtraction
	{
		/**
		 * Main plugin class of the feature aggregator plugins.
		 * Aggregates the features received in a feature vector then outputs it.
		 * */
		class CFeatureAggregator : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, virtual public OpenViBEToolkit::IBoxAlgorithmStreamedMatrixInputReaderCallback::ICallback
		{
		public:

			CFeatureAggregator(void);

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize();
			virtual OpenViBE::boolean uninitialize();

			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);

			virtual OpenViBE::boolean process();

			virtual void writeFeatureVectorOutput(const void* pBuffer, const EBML::uint64 ui64BufferSize);

			virtual void setMatrixDimmensionCount(const OpenViBE::uint32 ui32DimmensionCount);
			virtual void setMatrixDimmensionSize(const OpenViBE::uint32 ui32DimmensionIndex, const OpenViBE::uint32 ui32DimmensionSize);
			virtual void setMatrixDimmensionLabel(const OpenViBE::uint32 ui32DimmensionIndex, const OpenViBE::uint32 ui32DimmensionEntryIndex, const char* sDimmensionLabel);
			virtual void setMatrixBuffer(const OpenViBE::float64* pBuffer);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, OVP_ClassId_FeatureAggregator)

		public:
				//EBML writing stuff
				EBML::IWriter* m_pWriter;
				EBML::TWriterCallbackProxy1<OpenViBEPlugins::FeatureExtraction::CFeatureAggregator> * m_pOutputWriterCallbackProxy;
				OpenViBEToolkit::IBoxAlgorithmFeatureVectorOutputWriter * m_pFeatureVectorOutputWriterHelper;

				// Needed to read the input
				EBML::IReader* m_pReader;
				OpenViBEToolkit::IBoxAlgorithmStreamedMatrixInputReaderCallback* m_pMatrixReaderCallBack;

				// contains the labels for each dimmension for each input
				std::vector<std::vector<std::vector<std::string> > > m_oFeatureNames;

				// contains the dimmension size for each dimmension of each input
				std::vector<std::vector<OpenViBE::uint32> > m_oDimmensionSize;

				// contains the input buffer's total size for each input
				std::vector<OpenViBE::uint64> m_oInputBufferSizes;

				//start time and end time of the last arrived chunk
				OpenViBE::uint64 m_ui64LastChunkStartTime;
				OpenViBE::uint64 m_ui64LastChunkEndTime;

				// number of inputs
				OpenViBE::uint32 m_ui32NumberOfInput;

				// current input (usesful in the EBML callbacks)
				OpenViBE::uint32 m_ui32CurrentInput;

				// the feature vector
				OpenViBE::float64 * m_pVectorBuffer;
				// the feature vector size
				OpenViBE::uint32 m_ui32VectorSize;

				// error flag
				OpenViBE::boolean m_bError;
		};

		class CFeatureAggregatorListener : public OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >
		{
		public:

			OpenViBE::boolean check(OpenViBE::Kernel::IBox& rBox)
			{
				char l_sName[1024];
				OpenViBE::uint32 i;

				for(i=0; i<rBox.getInputCount(); i++)
				{
					sprintf(l_sName, "Input stream %u", i+1);
					rBox.setInputName(i, l_sName);
					rBox.setInputType(i, OV_TypeId_StreamedMatrix);
				}

				return true;
			}

			virtual OpenViBE::boolean onInputRemoved(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return this->check(rBox); }
			virtual OpenViBE::boolean onInputAdded(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return this->check(rBox); };

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier);
		};

		/**
		* Plugin's description
		*/
		class CFeatureAggregatorDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Feature aggregator"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Bruno Renier"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Aggregates features in a vector"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("Aggregates features in a vector"); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Feature extraction"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-convert"); }
			virtual void release(void)                                   { }
			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_FeatureAggregator; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::FeatureExtraction::CFeatureAggregator(); }
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener(void) const               { return new CFeatureAggregatorListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }

			virtual OpenViBE::boolean getBoxPrototype(OpenViBE::Kernel::IBoxProto& rPrototype) const
			{
				rPrototype.addInput("Input stream 1", OV_TypeId_StreamedMatrix);
				// rPrototype.addInput("Input stream 2", OV_TypeId_StreamedMatrix);
				rPrototype.addOutput("Feature vector stream", OV_TypeId_FeatureVector);
				rPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddInput);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_FeatureAggregatorDesc)

		};

	};
};

#endif

