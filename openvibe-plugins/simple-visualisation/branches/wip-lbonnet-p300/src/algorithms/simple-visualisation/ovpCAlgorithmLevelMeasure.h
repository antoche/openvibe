#ifndef __OpenViBEPlugins_Algorithm_LevelMeasure_H__
#define __OpenViBEPlugins_Algorithm_LevelMeasure_H__

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include <gtk/gtk.h>
#include <vector>
#include <map>

namespace OpenViBEPlugins
{
	namespace SimpleVisualisation
	{
		class CAlgorithmLevelMeasure : public OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >, OVP_ClassId_Algorithm_LevelMeasure);

		protected:

			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > ip_pMatrix;
			OpenViBE::Kernel::TParameterHandler < ::GtkWidget* > op_pMainWidget;
			OpenViBE::Kernel::TParameterHandler < ::GtkWidget* > op_pToolbarWidget;

			::GtkBuilder* m_pMainWidgetInterface;
			::GtkBuilder* m_pToolbarWidgetInterface;
			::GtkWidget* m_pMainWindow;
			::GtkWidget* m_pToolbarWidget;

		public:

			typedef struct
			{
				::GtkProgressBar* m_pProgressBar;
				OpenViBE::uint32 m_ui32Score;
				OpenViBE::boolean m_bLastWasOverThreshold;
			} SProgressBar;

			std::vector < SProgressBar > m_vProgressBar;
			OpenViBE::boolean m_bShowPercentages;
			OpenViBE::float64 m_f64Threshold;
		};

		class CAlgorithmLevelMeasureDesc : public OpenViBE::Plugins::IAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Level measure"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Simple visualisation"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-go-up"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_Algorithm_LevelMeasure; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SimpleVisualisation::CAlgorithmLevelMeasure; }

			virtual OpenViBE::boolean getAlgorithmPrototype(
				OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				rAlgorithmPrototype.addInputParameter (OVP_Algorithm_LevelMeasure_InputParameterId_Matrix,         "Matrix",         OpenViBE::Kernel::ParameterType_Matrix);
				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_LevelMeasure_OutputParameterId_MainWidget,    "Main widget",    OpenViBE::Kernel::ParameterType_Pointer);
				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_LevelMeasure_OutputParameterId_ToolbarWidget, "Toolbar widget", OpenViBE::Kernel::ParameterType_Pointer);
				rAlgorithmPrototype.addInputTrigger   (OVP_Algorithm_LevelMeasure_InputTriggerId_Reset,            "Reset");
				rAlgorithmPrototype.addInputTrigger   (OVP_Algorithm_LevelMeasure_InputTriggerId_Refresh,          "Refresh");
				rAlgorithmPrototype.addOutputTrigger  (OVP_Algorithm_LevelMeasure_OutputTriggerId_Refreshed,       "Refreshed");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IAlgorithmDesc, OVP_ClassId_Algorithm_LevelMeasureDesc);
		};
	};
};

#endif // __OpenViBEPlugins_Algorithm_LevelMeasure_H__
