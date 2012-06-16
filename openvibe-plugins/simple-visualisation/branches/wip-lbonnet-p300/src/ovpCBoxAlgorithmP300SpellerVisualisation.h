#ifndef __OpenViBEPlugins_BoxAlgorithm_P300SpellerVisualisation_H__
#define __OpenViBEPlugins_BoxAlgorithm_P300SpellerVisualisation_H__

#include "ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include <gtk/gtk.h>
#include <map>
#include <list>

// TODO:
// - please move the identifier definitions in ovp_defines.h
// - please include your desciptor in ovp_main.cpp

#define OVP_ClassId_BoxAlgorithm_P300SpellerVisualisation     OpenViBE::CIdentifier(0x195E41D6, 0x6E684D47)
#define OVP_ClassId_BoxAlgorithm_P300SpellerVisualisationDesc OpenViBE::CIdentifier(0x31DE2B0D, 0x028202E7)

namespace OpenViBEPlugins
{
	namespace SimpleVisualisation
	{
		class CBoxAlgorithmP300SpellerVisualisation : public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32Index);
			virtual OpenViBE::uint64 getClockFrequency(void){ return (128LL<<32); }
			virtual OpenViBE::boolean processClock(OpenViBE::CMessageClock& rMessageClock);
			virtual OpenViBE::boolean process(void);

			void redraw(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_P300SpellerVisualisation);

		private:

			typedef struct
			{
				::GtkWidget* pWidget;
				::GtkWidget* pChildWidget;
				::GdkColor oBackgroundColor;
				::GdkColor oForegroundColor;
				::PangoFontDescription* pFontDescription;
			} SWidgetStyle;

			typedef void (CBoxAlgorithmP300SpellerVisualisation::*_cache_callback_)(CBoxAlgorithmP300SpellerVisualisation::SWidgetStyle& rWidgetStyle, void* pUserData);

			void _cache_build_from_table_(::GtkTable* pTable);
			void _cache_for_each_(_cache_callback_ fpCallback, void* pUserData);
			void _cache_for_each_if_(int iLine, int iColumn, _cache_callback_ fpIfCallback, _cache_callback_ fpElseCallback, void* pIfUserData, void* pElseUserData);
			void _cache_change_null_cb_(CBoxAlgorithmP300SpellerVisualisation::SWidgetStyle& rWidgetStyle, void* pUserData);
			void _cache_change_background_cb_(CBoxAlgorithmP300SpellerVisualisation::SWidgetStyle& rWidgetStyle, void* pUserData);
			void _cache_change_foreground_cb_(CBoxAlgorithmP300SpellerVisualisation::SWidgetStyle& rWidgetStyle, void* pUserData);
			void _cache_change_font_cb_(CBoxAlgorithmP300SpellerVisualisation::SWidgetStyle& rWidgetStyle, void* pUserData);
			void _cache_collect_widget_cb_(CBoxAlgorithmP300SpellerVisualisation::SWidgetStyle& rWidgetStyle, void* pUserData);
			void _cache_collect_child_widget_cb_(CBoxAlgorithmP300SpellerVisualisation::SWidgetStyle& rWidgetStyle, void* pUserData);
			void _cache_emit_signal_cb_(CBoxAlgorithmP300SpellerVisualisation::SWidgetStyle& rWidgetStyle, void* pUserData);
			
		protected:

			OpenViBE::CString m_sInterfaceFilename;
			OpenViBE::uint64 m_ui64RowStimulationBase;
			OpenViBE::uint64 m_ui64ColumnStimulationBase;

			::GdkColor m_oFlashBackgroundColor;
			::GdkColor m_oFlashForegroundColor;
			OpenViBE::uint64 m_ui64FlashFontSize;
			::PangoFontDescription* m_pFlashFontDescription;
			::GdkColor m_oNoFlashBackgroundColor;
			::GdkColor m_oNoFlashForegroundColor;
			OpenViBE::uint64 m_ui64NoFlashFontSize;
			::PangoFontDescription* m_pNoFlashFontDescription;
			::GdkColor m_oTargetBackgroundColor;
			::GdkColor m_oTargetForegroundColor;
			OpenViBE::uint64 m_ui64TargetFontSize;
			::PangoFontDescription* m_pTargetFontDescription;
			::GdkColor m_oSelectedBackgroundColor;
			::GdkColor m_oSelectedForegroundColor;
			OpenViBE::uint64 m_ui64SelectedFontSize;
			::PangoFontDescription* m_pSelectedFontDescription;

		private:

			OpenViBE::Kernel::IAlgorithmProxy* m_pSequenceStimulationDecoder;
			OpenViBE::Kernel::IAlgorithmProxy* m_pTargetStimulationDecoder;
			
			OpenViBE::Kernel::IAlgorithmProxy* m_pRowSelectionStimulationDecoder;
			OpenViBE::Kernel::IAlgorithmProxy* m_pColumnSelectionStimulationDecoder;

			OpenViBEToolkit::TStimulationEncoder<CBoxAlgorithmP300SpellerVisualisation> * m_pTargetStimulationEncoder;
			OpenViBEToolkit::TStimulationEncoder<CBoxAlgorithmP300SpellerVisualisation> * m_pResyncFlashSequenceStimulationEncoder;

			OpenViBE::Kernel::TParameterHandler<const OpenViBE::IMemoryBuffer*> ip_pSequenceMemoryBuffer;
			OpenViBE::Kernel::TParameterHandler<const OpenViBE::IMemoryBuffer*> ip_pTargetMemoryBuffer;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IStimulationSet*> op_pSequenceStimulationSet;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IStimulationSet*> op_pTargetStimulationSet;
			OpenViBE::uint64 m_ui64LastTime;

			::GtkBuilder* m_pMainWidgetInterface;
			::GtkBuilder* m_pToolbarWidgetInterface;
			::GtkWidget* m_pMainWindow;
			::GtkWidget* m_pToolbarWidget;
			::GtkTable* m_pTable;
			::GtkLabel* m_pResult;
			::GtkLabel* m_pTarget;
			OpenViBE::uint64 m_ui64RowCount;
			OpenViBE::uint64 m_ui64ColumnCount;

			int m_iLastTargetRow;
			int m_iLastTargetColumn;
			int m_iTargetRow;
			int m_iTargetColumn;
			int m_iSelectedRow;
			int m_iSelectedColumn;

			OpenViBE::boolean m_bTableInitialized;

			std::map < unsigned long, std::map < unsigned long, CBoxAlgorithmP300SpellerVisualisation::SWidgetStyle > > m_vCache;
			std::list < std::pair < int, int > > m_vTargetHistory;

			OpenViBE::boolean m_bFlashTriggered;
			
			OpenViBE::uint64 m_ui64LastFlashIdentifier;
			OpenViBE::uint64 m_ui64LastColumnTargetIdentifier;
			OpenViBE::uint64 m_ui64LastRowTargetIdentifier;
			
			OpenViBE::uint64 m_ui64LastOutputChunkDate;
			OpenViBE::uint64 m_ui64LastPlayerTime;
			OpenViBE::uint64 m_ui64LastSystemTime;
			
		};

		class CBoxAlgorithmP300SpellerVisualisationDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("P300 Speller Visualisation"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Visualisation/Presentation"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-select-font"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_P300SpellerVisualisation; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SimpleVisualisation::CBoxAlgorithmP300SpellerVisualisation; }

			virtual OpenViBE::boolean hasFunctionality(OpenViBE::Kernel::EPluginFunctionality ePF) const { return ePF == OpenViBE::Kernel::PluginFunctionality_Visualization; }
			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput ("Sequence stimulations",            OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addInput ("Target stimulations",              OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addInput ("Row selection stimulations",       OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addInput ("Column selection stimulations",    OV_TypeId_Stimulations);

				rBoxAlgorithmPrototype.addOutput("Resynched Flashing Sequence",      OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addOutput("Target / Non target flagging",     OV_TypeId_Stimulations);

				rBoxAlgorithmPrototype.addSetting("Interface filename",              OV_TypeId_Filename,    "../share/openvibe-plugins/simple-visualisation/p300-speller.ui");
				rBoxAlgorithmPrototype.addSetting("Row stimulation base",            OV_TypeId_Stimulation, "OVTK_StimulationId_Label_01");
				rBoxAlgorithmPrototype.addSetting("Column stimulation base",         OV_TypeId_Stimulation, "OVTK_StimulationId_Label_07");

				rBoxAlgorithmPrototype.addSetting("Flash background color",          OV_TypeId_Color,       "10,10,10");
				rBoxAlgorithmPrototype.addSetting("Flash foreground color",          OV_TypeId_Color,       "100,100,100");
				rBoxAlgorithmPrototype.addSetting("Flash font size",                 OV_TypeId_Integer,     "100");

				rBoxAlgorithmPrototype.addSetting("No flash background color",       OV_TypeId_Color,       "0,0,0");
				rBoxAlgorithmPrototype.addSetting("No flash foreground color",       OV_TypeId_Color,       "50,50,50");
				rBoxAlgorithmPrototype.addSetting("No flash font size",              OV_TypeId_Integer,     "75");

				rBoxAlgorithmPrototype.addSetting("Target background color",         OV_TypeId_Color,       "10,40,10");
				rBoxAlgorithmPrototype.addSetting("Target foreground color",         OV_TypeId_Color,       "60,100,60");
				rBoxAlgorithmPrototype.addSetting("Target font size",                OV_TypeId_Integer,     "100");

				rBoxAlgorithmPrototype.addSetting("Selected background color",       OV_TypeId_Color,       "70,20,20");
				rBoxAlgorithmPrototype.addSetting("Selected foreground color",       OV_TypeId_Color,       "30,10,10");
				rBoxAlgorithmPrototype.addSetting("Selected font size",              OV_TypeId_Integer,     "100");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_P300SpellerVisualisationDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_P300SpellerVisualisation_H__
