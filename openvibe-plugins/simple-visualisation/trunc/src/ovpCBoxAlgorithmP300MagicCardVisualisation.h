#ifndef __OpenViBEPlugins_BoxAlgorithm_P300MagicCardVisualisation_H__
#define __OpenViBEPlugins_BoxAlgorithm_P300MagicCardVisualisation_H__

#include "ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include <gtk/gtk.h>
#include <map>

// TODO:
// - please move the identifier definitions in ovp_defines.h
// - please include your desciptor in ovp_main.cpp

#define OVP_ClassId_BoxAlgorithm_P300MagicCardVisualisation     OpenViBE::CIdentifier(0x841F46EF, 0x471AA2A4)
#define OVP_ClassId_BoxAlgorithm_P300MagicCardVisualisationDesc OpenViBE::CIdentifier(0x37FAFF20, 0xA74685DB)

namespace OpenViBEPlugins
{
	namespace SimpleVisualisation
	{
		class CBoxAlgorithmP300MagicCardVisualisation : public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32Index);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_P300MagicCardVisualisation);

		private:

			typedef struct
			{
				int iIndex;
				::GdkColor oBackgroundColor;
				::GtkWidget* pParent;
				::GtkWidget* pWidget;
				::GtkWidget* pImage;
			} SWidgetStyle;

			typedef void (CBoxAlgorithmP300MagicCardVisualisation::*_cache_callback_)(CBoxAlgorithmP300MagicCardVisualisation::SWidgetStyle& rWidgetStyle, void* pUserData);

			void _cache_build_from_table_(::GtkTable* pTable);
			void _cache_for_each_(_cache_callback_ fpCallback, void* pUserData);
			void _cache_for_each_if_(int iCard, _cache_callback_ fpIfCallback, _cache_callback_ fpElseCallback, void* pIfUserData, void* pElseUserData);
			void _cache_change_null_cb_(CBoxAlgorithmP300MagicCardVisualisation::SWidgetStyle& rWidgetStyle, void* pUserData);
			void _cache_change_image_cb_(CBoxAlgorithmP300MagicCardVisualisation::SWidgetStyle& rWidgetStyle, void* pUserData);
			void _cache_change_background_cb_(CBoxAlgorithmP300MagicCardVisualisation::SWidgetStyle& rWidgetStyle, void* pUserData);

		protected:

			OpenViBE::CString m_sInterfaceFilename;
			OpenViBE::uint64 m_ui64CardStimulationBase;

		private:

			OpenViBE::Kernel::IAlgorithmProxy* m_pSequenceStimulationDecoder;
			OpenViBE::Kernel::IAlgorithmProxy* m_pTargetStimulationDecoder;
			OpenViBE::Kernel::IAlgorithmProxy* m_pTargetFlaggingStimulationEncoder;
			OpenViBE::Kernel::IAlgorithmProxy* m_pCardSelectionStimulationDecoder;
			OpenViBE::Kernel::TParameterHandler<const OpenViBE::IMemoryBuffer*> ip_pSequenceMemoryBuffer;
			OpenViBE::Kernel::TParameterHandler<const OpenViBE::IMemoryBuffer*> ip_pTargetMemoryBuffer;
			OpenViBE::Kernel::TParameterHandler<const OpenViBE::IStimulationSet*> ip_pTargetFlaggingStimulationSet;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IStimulationSet*> op_pSequenceStimulationSet;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IStimulationSet*> op_pTargetStimulationSet;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> op_pTargetFlaggingMemoryBuffer;
			OpenViBE::uint64 m_ui64LastTime;

			::GtkBuilder* m_pMainWidgetInterface;
			::GtkBuilder* m_pToolbarWidgetInterface;
			::GtkWidget* m_pMainWindow;
			::GtkWidget* m_pToolbarWidget;
			::GtkTable* m_pTable;
			::GtkLabel* m_pResult;
			::GtkLabel* m_pTarget;
			::GdkColor m_oBackgroundColor;
			::GdkColor m_oTargetBackgroundColor;
			::GdkColor m_oSelectedBackgroundColor;
			OpenViBE::uint64 m_ui64TableRowCount;
			OpenViBE::uint64 m_ui64TableColumnCount;
			OpenViBE::uint64 m_ui64CardCount;

			int m_iTargetCard;

			std::vector < ::GtkWidget* > m_vForegroundImage;
			std::vector < ::GtkWidget* > m_vBackgroundImage;

			OpenViBE::boolean m_bTableInitialized;

			std::map < unsigned long, CBoxAlgorithmP300MagicCardVisualisation::SWidgetStyle > m_vCache;
		};

		class CBoxAlgorithmP300MagicCardVisualisationDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("P300 Magic Card Visualisation"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Visualisation/Presentation"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-select-font"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_P300MagicCardVisualisation; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SimpleVisualisation::CBoxAlgorithmP300MagicCardVisualisation; }

			virtual OpenViBE::boolean hasFunctionality(OpenViBE::Kernel::EPluginFunctionality ePF) const { return ePF == OpenViBE::Kernel::PluginFunctionality_Visualization; }
			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput ("Sequence stimulations",            OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addInput ("Target stimulations",              OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addInput ("Card selection stimulations",      OV_TypeId_Stimulations);

				rBoxAlgorithmPrototype.addOutput("Target / Non target flagging",     OV_TypeId_Stimulations);

				rBoxAlgorithmPrototype.addSetting("Interface filename",              OV_TypeId_Filename,    "../share/openvibe-plugins/simple-visualisation/p300-magic-card.ui");
				rBoxAlgorithmPrototype.addSetting("Background color",                OV_TypeId_Color,       "90,90,90");
				rBoxAlgorithmPrototype.addSetting("Target background color",         OV_TypeId_Color,       "10,40,10");
				rBoxAlgorithmPrototype.addSetting("Selected background color",       OV_TypeId_Color,       "70,20,20");
				rBoxAlgorithmPrototype.addSetting("Card stimulation base",           OV_TypeId_Stimulation, "OVTK_StimulationId_Label_01");
				rBoxAlgorithmPrototype.addSetting("Default background filename",     OV_TypeId_Filename,    "../share/openvibe-plugins/simple-visualisation/p300-magic-card/openvibe-logo.png-offscreen");
				rBoxAlgorithmPrototype.addSetting("Card filename",                   OV_TypeId_Filename,    "../share/openvibe-plugins/simple-visualisation/p300-magic-card/arkanoid.png");
				rBoxAlgorithmPrototype.addSetting("Card filename",                   OV_TypeId_Filename,    "../share/openvibe-plugins/simple-visualisation/p300-magic-card/bomberman.png");
				rBoxAlgorithmPrototype.addSetting("Card filename",                   OV_TypeId_Filename,    "../share/openvibe-plugins/simple-visualisation/p300-magic-card/doom.png");
				rBoxAlgorithmPrototype.addSetting("Card filename",                   OV_TypeId_Filename,    "../share/openvibe-plugins/simple-visualisation/p300-magic-card/dott.png");
				rBoxAlgorithmPrototype.addSetting("Card filename",                   OV_TypeId_Filename,    "../share/openvibe-plugins/simple-visualisation/p300-magic-card/lemmings.png");
				rBoxAlgorithmPrototype.addSetting("Card filename",                   OV_TypeId_Filename,    "../share/openvibe-plugins/simple-visualisation/p300-magic-card/mario.png");
				rBoxAlgorithmPrototype.addSetting("Card filename",                   OV_TypeId_Filename,    "../share/openvibe-plugins/simple-visualisation/p300-magic-card/megaman.png");
				rBoxAlgorithmPrototype.addSetting("Card filename",                   OV_TypeId_Filename,    "../share/openvibe-plugins/simple-visualisation/p300-magic-card/pacman.png");
				rBoxAlgorithmPrototype.addSetting("Card filename",                   OV_TypeId_Filename,    "../share/openvibe-plugins/simple-visualisation/p300-magic-card/sonic.png");
				rBoxAlgorithmPrototype.addSetting("Card filename",                   OV_TypeId_Filename,    "../share/openvibe-plugins/simple-visualisation/p300-magic-card/space-invaders.png");
				rBoxAlgorithmPrototype.addSetting("Card filename",                   OV_TypeId_Filename,    "../share/openvibe-plugins/simple-visualisation/p300-magic-card/worms.png");
				rBoxAlgorithmPrototype.addSetting("Card filename",                   OV_TypeId_Filename,    "../share/openvibe-plugins/simple-visualisation/p300-magic-card/zelda.png");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_P300MagicCardVisualisationDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_P300MagicCardVisualisation_H__
