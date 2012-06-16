#ifndef __OpenViBEPlugins_BoxAlgorithm_ClassifierProcessor_H__
#define __OpenViBEPlugins_BoxAlgorithm_ClassifierProcessor_H__

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include "ovpCBoxAlgorithmCommonClassifierListener.inl"

#include <map>

#define OVP_ClassId_BoxAlgorithm_ClassifierProcessor     OpenViBE::CIdentifier(0x40AB4835, 0x1AE96E9E)
#define OVP_ClassId_BoxAlgorithm_ClassifierProcessorDesc OpenViBE::CIdentifier(0x786B436B, 0x5B327327)

namespace OpenViBEPlugins
{
	namespace Classification
	{
		class CBoxAlgorithmClassifierProcessor : virtual public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_ClassifierProcessor);

		protected:

			OpenViBE::Kernel::IAlgorithmProxy* m_pFeaturesDecoder;
			OpenViBE::Kernel::IAlgorithmProxy* m_pLabelsEncoder;
			OpenViBE::Kernel::IAlgorithmProxy* m_pClassificationStateEncoder;
			OpenViBE::Kernel::IAlgorithmProxy* m_pClassifier;

			std::map < OpenViBE::float64, OpenViBE::uint64 > m_vStimulation;
			OpenViBE::boolean m_bOutputHeaderSent;
		};

		class CBoxAlgorithmClassifierProcessorDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Classifier processor"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Generic classification, relying on several box algorithms"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Classification"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-apply"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_ClassifierProcessor; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::Classification::CBoxAlgorithmClassifierProcessor; }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput  ("Features",                            OV_TypeId_FeatureVector);
				rBoxAlgorithmPrototype.addOutput ("Labels",                              OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addOutput ("Classification state",                OV_TypeId_StreamedMatrix);
				rBoxAlgorithmPrototype.addSetting("Classifier to use",                   OVTK_TypeId_ClassificationAlgorithm, "");
				rBoxAlgorithmPrototype.addSetting("Filename to load configuration from", OV_TypeId_Filename,    "");
				rBoxAlgorithmPrototype.addSetting("Reject class label",                  OV_TypeId_Stimulation, "OVTK_StimulationId_Label_00");
				rBoxAlgorithmPrototype.addSetting("Class 1 label",                       OV_TypeId_Stimulation, "OVTK_StimulationId_Label_01");
				rBoxAlgorithmPrototype.addSetting("Class 2 label",                       OV_TypeId_Stimulation, "OVTK_StimulationId_Label_02");
				return true;
			}

			// virtual OpenViBE::Plugins::IBoxListener* createBoxListener(void) const { return new CBoxAlgorithmCommonClassifierListener(5); }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) { delete pBoxListener; }

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_ClassifierProcessorDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_ClassifierProcessor_H__
