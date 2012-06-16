#ifndef __OpenViBEPlugins_BoxAlgorithm_ConfusionMatrix_H__
#define __OpenViBEPlugins_BoxAlgorithm_ConfusionMatrix_H__

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include <vector>
#include <sstream>
#include <cstdio>

#define OVP_ClassId_BoxAlgorithm_ConfusionMatrix     OpenViBE::CIdentifier(0x1AB625DA, 0x3B2502CE)
#define OVP_ClassId_BoxAlgorithm_ConfusionMatrixDesc OpenViBE::CIdentifier(0x52237A64, 0x63555613)
#define FIRST_CLASS_SETTING_INDEX 2

namespace OpenViBEPlugins
{
	namespace Classification
	{
		class CBoxAlgorithmConfusionMatrix : virtual public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_ConfusionMatrix);

		protected:

			// input TARGET
			OpenViBE::Kernel::IAlgorithmProxy* m_pTargetStimulationDecoder;
			OpenViBE::Kernel::TParameterHandler < const OpenViBE::IMemoryBuffer* > ip_pTargetMemoryBufferToDecode;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IStimulationSet* > op_pTargetStimulationSetDecoded;

			// input CLASSIFIER
			OpenViBE::Kernel::IAlgorithmProxy* m_pClassifierStimulationDecoder;
			OpenViBE::Kernel::TParameterHandler < const OpenViBE::IMemoryBuffer* > ip_pClassifierMemoryBufferToDecode;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IStimulationSet* > op_pClassifierStimulationSetDecoded;

			//CONFUSION MATRIX computing
			OpenViBE::Kernel::IAlgorithmProxy* m_pConfusionMatrixAlgorithm;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IStimulationSet* > ip_pTargetStimulationSet;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IStimulationSet* > ip_pClassifierStimulationSet;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IStimulationSet* > ip_pClassesCodes;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::boolean > ip_bPercentages;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::boolean > ip_bSums;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > op_pConfusionMatrix;

			OpenViBE::uint32 m_ui32ClassCount;
			OpenViBE::uint64 m_ui64CurrentProcessingTimeLimit;

			//OUTPUT MATRIX
			OpenViBE::Kernel::IAlgorithmProxy* m_pConfusionMatrixEncoder;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > ip_pConfusionMatrixToEncode;
			OpenViBE::Kernel::TParameterHandler < const OpenViBE::IMemoryBuffer* > op_pConfusionMatrixEncoded;
		};

		class CBoxAlgorithmConfusionMatrixListener : public OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >
		{
		public:

			virtual OpenViBE::boolean onSettingAdded(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				char l_sName[1024];
				char l_sValue[1024];
				::sprintf(l_sName, "Class %i", ui32Index - 1);
				::sprintf(l_sValue, "OVTK_StimulationId_Label_%02i", ui32Index- 2);
				rBox.setSettingName(ui32Index, l_sName);
				rBox.setSettingType(ui32Index, OV_TypeId_Stimulation);
				rBox.setSettingValue(ui32Index, l_sValue);
				return true;
			}

			virtual OpenViBE::boolean onSettingRemoved(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				OpenViBE::uint32 l_ui32SettingCount = rBox.getSettingCount();
				OpenViBE::uint32 l_ui32ClassCount = l_ui32SettingCount - FIRST_CLASS_SETTING_INDEX;

				for(OpenViBE::uint32 i = 0; i<l_ui32ClassCount; i++)
				{
					char l_sName[1024];
					::sprintf(l_sName, "Class %i", i+1);
					rBox.setSettingName(FIRST_CLASS_SETTING_INDEX + i, l_sName);
				}

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier);
		};

		class CBoxAlgorithmConfusionMatrixDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Confusion Matrix"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Laurent Bonnet"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Make a confusion matrix out of classification results coming from one classifier."); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Classification"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-execute"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_ConfusionMatrix; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::Classification::CBoxAlgorithmConfusionMatrix; }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput  ("Targets",                OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addInput  ("Classification results", OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addOutput ("Confusion Matrix",       OV_TypeId_StreamedMatrix);

				rBoxAlgorithmPrototype.addSetting("Percentages",            OV_TypeId_Boolean, "true");
				rBoxAlgorithmPrototype.addSetting("Sums",                   OV_TypeId_Boolean, "false");

				rBoxAlgorithmPrototype.addSetting("Class 1",                OV_TypeId_Stimulation, "OVTK_StimulationId_Label_00");
				rBoxAlgorithmPrototype.addSetting("Class 2",                OV_TypeId_Stimulation, "OVTK_StimulationId_Label_01");

				// rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_IsUnstable);
				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddSetting);

				return true;
			}

			virtual OpenViBE::Plugins::IBoxListener* createBoxListener(void) const { return new CBoxAlgorithmConfusionMatrixListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) { delete pBoxListener; }

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_ConfusionMatrixDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_ConfusionMatrix_H__
