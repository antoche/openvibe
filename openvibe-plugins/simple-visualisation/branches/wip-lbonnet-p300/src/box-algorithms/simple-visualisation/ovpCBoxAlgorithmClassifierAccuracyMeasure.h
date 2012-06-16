#ifndef __OpenViBEPlugins_BoxAlgorithm_ClassifierAccuracyMeasure_H__
#define __OpenViBEPlugins_BoxAlgorithm_ClassifierAccuracyMeasure_H__

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>
#include <gtk/gtk.h>
#include <map>
#include <vector>

#define OVP_ClassId_BoxAlgorithm_ClassifierAccuracyMeasure      OpenViBE::CIdentifier(0x48395CE7, 0x17D62550)
#define OVP_ClassId_BoxAlgorithm_ClassifierAccuracyMeasureDesc  OpenViBE::CIdentifier(0x067F38CC, 0x084A6ED3)

namespace OpenViBEPlugins
{
	namespace SimpleVisualisation
	{
		class CBoxAlgorithmClassifierAccuracyMeasure : public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_ClassifierAccuracyMeasure);

		protected:

			// we need an algorithm to decode the EBML stream (memory buffer) into a StimulationSet

			// for the TARGET
			OpenViBE::Kernel::IAlgorithmProxy* m_pTargetStimulationDecoder;
			OpenViBE::Kernel::TParameterHandler < const OpenViBE::IMemoryBuffer* > ip_pTargetMemoryBuffer;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IStimulationSet* > op_pTargetStimulationSet;
			// deduced timeline:
			std::map<OpenViBE::uint64,OpenViBE::uint64> m_mTargetsTimeLine;
			OpenViBE::uint64 m_ui64CurrentProcessingTimeLimit;

			// For the CLASSIFIERS
			std::vector<OpenViBE::Kernel::IAlgorithmProxy*> m_vpClassifierStimulationDecoder;
			std::vector<OpenViBE::Kernel::TParameterHandler < const OpenViBE::IMemoryBuffer* > > m_vInputClassifierMemoryBuffer;
			std::vector<OpenViBE::Kernel::TParameterHandler < OpenViBE::IStimulationSet* > > m_vOutputClassifierStimulationSet;

			// Outputs: visualization in a gtk window
			::GtkBuilder* m_pMainWidgetInterface;
			::GtkBuilder* m_pToolbarWidgetInterface;
			::GtkWidget* m_pMainWidget;
			::GtkWidget* m_pToolbarWidget;

		public:
			typedef struct
			{
				::GtkLabel* m_pLabelClassifier;
				::GtkProgressBar* m_pProgressBar;
				OpenViBE::uint32 m_ui32Score;
				OpenViBE::uint32 m_ui32StimulationCount;
			} SProgressBar;

			std::vector<SProgressBar> m_vProgressBar;
			OpenViBE::boolean m_bShowPercentages;
			OpenViBE::boolean m_bShowScores;

		};

		class CBoxAlgorithmClassifierAccuracyMeasureListener : public OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >
		{

		public:

			virtual OpenViBE::boolean onInputTypeChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				rBox.setInputType(ui32Index, OV_TypeId_Stimulations); // all inputs must be stimulations
				return true;
			}

			virtual OpenViBE::boolean onInputNameChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				if(ui32Index == 0)
				{
					rBox.setInputName(0, "Targets"); // forced
				}
				return true;
			}

			virtual OpenViBE::boolean onInputAdded(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				rBox.setInputType(ui32Index, OV_TypeId_Stimulations); // all inputs must be stimulations
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier);
		};

		class CBoxAlgorithmClassifierAccuracyMeasureDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Classifier Accuracy Measure"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Laurent Bonnet"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Visualisation/Classification"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-sort-ascending"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_ClassifierAccuracyMeasure; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SimpleVisualisation::CBoxAlgorithmClassifierAccuracyMeasure; }

			virtual OpenViBE::Plugins::IBoxListener* createBoxListener(void) const               { return new CBoxAlgorithmClassifierAccuracyMeasureListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }

			virtual OpenViBE::boolean hasFunctionality(OpenViBE::Kernel::EPluginFunctionality ePluginFunctionality) const
			{
				return ePluginFunctionality == OpenViBE::Kernel::PluginFunctionality_Visualization;
			}

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput("Targets",      OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addInput("Classifier 1", OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addFlag (OpenViBE::Kernel::BoxFlag_CanAddInput);
				rBoxAlgorithmPrototype.addFlag (OpenViBE::Kernel::BoxFlag_CanModifyInput);
				// rBoxAlgorithmPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_IsUnstable);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_ClassifierAccuracyMeasureDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_ClassifierAccuracyMeasure_H__
