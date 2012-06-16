#ifndef __OpenViBEPlugins_SignalProcessingGpl_BoxAlgorithms_Filter_CLDABoxAlgorithm_H__
#define __OpenViBEPlugins_SignalProcessingGpl_BoxAlgorithms_Filter_CLDABoxAlgorithm_H__

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>
#include <vector>
#include <cstdio>

namespace OpenViBEPlugins
{
	namespace SignalProcessingGpl
	{
		class CLDABoxAlgorithm : virtual public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);

			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, OVP_ClassId_Box_LDABoxAlgorithm)

		protected:

			std::vector < OpenViBE::Kernel::IAlgorithmProxy* > m_vStreamDecoder;
			std::vector < OpenViBE::uint64 > m_vStreamDecoderEndTime;

			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamEncoder;

			OpenViBE::Kernel::IAlgorithmProxy* m_pComputeFisherLdaFunction;
			OpenViBE::Kernel::IAlgorithmProxy* m_pApplyFisherLdaFunction;
			OpenViBE::Kernel::IAlgorithmProxy* m_pFeatureExtractionLda;

			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* >  m_oSignalIntputMatrixHandle;

			OpenViBE::CMatrix m_oClass1SamplesNumbersInputMatrix;
			OpenViBE::CMatrix* m_pEpochTable;
			OpenViBE::float64* m_pf64EpochTable ;

			OpenViBE::uint64 m_ui64NbEntries;
			OpenViBE::uint64 m_ui64NbEntriesDecoded;

			OpenViBE::uint64 m_ui64LastStartTime;
			OpenViBE::uint64 m_ui64LastEndTime;

			std::vector<OpenViBE::uint64> m_oCoefficients;

			OpenViBE::uint64 m_ui64TrainingIndex;
			OpenViBE::uint64 m_ui64NbTrainingSamples;
			OpenViBE::boolean m_bLoadTrainingParms;
			OpenViBE::CString m_sLoadTrainingParmsFilename;
			
			OpenViBE::boolean m_bFirstTime;

			// Save training data flag and file name
			OpenViBE::boolean m_bSaveTrainingData;
			OpenViBE::CString m_sSaveTrainingDataFilename;
			
			// Load training data flag and file name
			OpenViBE::boolean m_bLoadTrainingData;
			OpenViBE::CString m_sLoadTrainingDataFilename;
			
			OpenViBE::boolean m_bNeedTraining;
			OpenViBE::boolean m_bNeedApply;

			OpenViBE::boolean m_bHasSentHeader;
		};

		class CLDABoxAlgorithmListener : public OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >
		{
		public:

			OpenViBE::boolean check(OpenViBE::Kernel::IBox& rBox)
			{
				OpenViBE::uint32 i;

				for(i=0; i<rBox.getInputCount(); i++)
				{//					sprintf(l_sName, "Input epochs")
					rBox.setInputName(i, "Input epochs");
					rBox.setInputType(i, OV_TypeId_StreamedMatrix);
				}

				return true;
			}

			virtual OpenViBE::boolean onInputRemoved(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				return this->check(rBox);
			}

			virtual OpenViBE::boolean onInputAdded(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				return this->check(rBox);
			};

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier);
		};
		
		class CLDABoxAlgorithmDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }
			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Linear Discriminant Analysis"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Guillaume Gibert/Manu Maby/P.E. Aguera"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INSERM/U821"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Signal processing/Classification"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.01"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString(""); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_Box_LDABoxAlgorithm; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SignalProcessingGpl::CLDABoxAlgorithm(); }

			virtual OpenViBE::Plugins::IBoxListener* createBoxListener(void) const               { return new CLDABoxAlgorithmListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }
			
			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rPrototype) const
			{
				rPrototype.addInput  ("Input epochs",               OV_TypeId_StreamedMatrix);
				rPrototype.addInput  ("Input epochs",               OV_TypeId_StreamedMatrix);
				rPrototype.addInput  ("Input epochs",               OV_TypeId_StreamedMatrix);
				rPrototype.addInput  ("Input epochs",               OV_TypeId_StreamedMatrix);
				rPrototype.addInput  ("Input epochs",               OV_TypeId_StreamedMatrix);
				rPrototype.addInput  ("Input epochs",               OV_TypeId_StreamedMatrix);
        
				rPrototype.addOutput ("Decision value",             OV_TypeId_StreamedMatrix);
        
				rPrototype.addSetting("Number of training samples", OV_TypeId_Integer, "5");
				rPrototype.addSetting("Class 1 samples numbers",    OV_TypeId_String, "2 5 1 3 6");
				rPrototype.addSetting("Or load training samples from file", OV_TypeId_Boolean, "false");
				rPrototype.addSetting("Training samples file name", OV_TypeId_Filename, "training_parms.txt");
				
				rPrototype.addSetting("Save training data", OV_TypeId_Boolean, "true");
				rPrototype.addSetting("Output training data file name", OV_TypeId_Filename, "LDA_training.bin");
				rPrototype.addSetting("Load training data", OV_TypeId_Boolean, "false");
				rPrototype.addSetting("Input training data file name", OV_TypeId_Filename, "LDA_training.bin");
	
				rPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_CanAddInput);
				rPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_IsDeprecated);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_Box_LDABoxAlgorithmDesc)
		};
	};
};

#endif // __OpenViBEPlugins_SignalProcessingGpl_BoxAlgorithms_Filter_CLDABoxAlgorithm_H__
