#ifndef __OpenViBEPlugins_SignalProcessingGpl_BoxAlgorithms_Classif_CNaiveBayesApplyBoxAlgorithm_H__
#define __OpenViBEPlugins_SignalProcessingGpl_BoxAlgorithms_Classif_CNaiveBayesApplyBoxAlgorithm_H__

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>
//#include <vector>
#include <cstdio>

namespace OpenViBEPlugins
{
	namespace SignalProcessingGpl
	{
		class CNaiveBayesApplyBoxAlgorithm : virtual public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
			public:

				virtual void release(void) { delete this; }

				virtual OpenViBE::boolean initialize(void);
				virtual OpenViBE::boolean uninitialize(void);

				virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
				virtual OpenViBE::boolean process(void);

				_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, OVP_ClassId_Box_NaiveBayesApplyBoxAlgorithm)

			protected:

				std::vector < OpenViBE::Kernel::IAlgorithmProxy* > m_vStreamDecoder;
				std::vector < OpenViBE::uint64 > m_vStreamDecoderEndTime;

				std::vector < OpenViBE::Kernel::IAlgorithmProxy* > m_pStreamEncoder;

				OpenViBE::Kernel::IAlgorithmProxy* m_pNaiveBayesApplyFunction;

//				OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* >  m_oSignalIntputMatrixHandle;
				OpenViBE::uint64 m_ui64LastStartTime;
				OpenViBE::uint64 m_ui64LastEndTime;
				OpenViBE::uint64 m_ui64NbEntries;
				OpenViBE::uint64 m_ui64NbEntriesDecoded;

				OpenViBE::boolean m_bFirstTime;

				OpenViBE::CString m_sLoadTrainingDataFilename;
				OpenViBE::uint32 m_ui32NbClasses;
				OpenViBE::uint32 m_ui32NbFeatures;
				OpenViBE::CMatrix m_oMeanClassFeaturesMatrix;
				OpenViBE::CMatrix m_oVarianceClassFeaturesMatrix;
				OpenViBE::CMatrix m_oLogTermVector;
				OpenViBE::CMatrix m_oEncodeDecisionMatrix;
				OpenViBE::CMatrix m_oEncodeClassificationProbabilitiesMatrix;				
				OpenViBE::CMatrix m_oOutputFunctionalNB;
				OpenViBE::float64 **m_pf64AllEntriesClassFunctionalNB;
				OpenViBE::uint32 *m_pui32IndexClass2Max;
				OpenViBE::uint32 m_ui32Class2MaxNumber;

				OpenViBE::boolean m_bHasSentHeader;
		};

		class CNaiveBayesApplyBoxAlgorithmListener : public OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >
		{
			public:

				OpenViBE::boolean check(OpenViBE::Kernel::IBox& rBox)
				{
					OpenViBE::uint32 i;

					for(i=0; i<rBox.getInputCount(); i++)
					{
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

		class CNaiveBayesApplyBoxAlgorithmDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
			public:

				virtual void release(void) { }
				virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Linear Naive Bayes Apply"); }
				virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Guillaume Gibert/Manu Maby/P.E. Aguera"); }
				virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INSERM/U821"); }
				virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
				virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
				virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Signal processing-gpl/Classification"); }
				virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.00"); }
				virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString(""); }

				virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_Box_NaiveBayesApplyBoxAlgorithm; }
				virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SignalProcessingGpl::CNaiveBayesApplyBoxAlgorithm(); }

				virtual OpenViBE::Plugins::IBoxListener* createBoxListener(void) const               { return new CNaiveBayesApplyBoxAlgorithmListener; }
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
							rPrototype.addOutput ("Classification probabilities",             OV_TypeId_StreamedMatrix);

							rPrototype.addSetting("Input training data file name", OV_TypeId_Filename, "NB_training.bin");

							rPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_CanAddInput);
							//rPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_IsDeprecated);

							return true;
						}

						_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_Box_NaiveBayesApplyBoxAlgorithmDesc)
		};
	};
};

#endif // __OpenViBEPlugins_SignalProcessingGpl_BoxAlgorithms_Classif_CNaiveBayesApplyBoxAlgorithm_H__
