#ifndef __OpenViBEDesigner_CInterfacedScenario_H__
#define __OpenViBEDesigner_CInterfacedScenario_H__

#include "ovd_base.h"

#include "ovdCInterfacedObject.h"

#include <map>
#include <string>

namespace OpenViBEDesigner
{
	class CDesignerVisualisation;
	class CPlayerVisualisation;

	class CInterfacedScenario
	{
		public:

		CInterfacedScenario(OpenViBE::Kernel::IKernel& rKernel, OpenViBE::Kernel::IScenario& rScenario, OpenViBE::CIdentifier& rScenarioIdentifier,
			::GtkNotebook& rNotebook, const char* sGUIFilename);
		virtual ~CInterfacedScenario(void);

		virtual OpenViBE::boolean isLocked(void) const;
		virtual void redraw(void);
		virtual void redrawBox(OpenViBE::Kernel::IBox& rBox);
		virtual void redrawLink(OpenViBE::Kernel::ILink& rLink);
		virtual void updateScenarioLabel(void);
		OpenViBE::uint32 pickInterfacedObject(int x, int y);
		OpenViBE::boolean pickInterfacedObject(int x, int y, int iSizeX, int iSizeY);

		void scenarioDrawingAreaExposeCB(::GdkEventExpose* pEvent);
		void scenarioDrawingAreaDragDataReceivedCB(::GdkDragContext* pDragContext, gint iX, gint iY, ::GtkSelectionData* pSelectionData, guint uiInfo, guint uiT);
		void scenarioDrawingAreaMotionNotifyCB(::GtkWidget* pWidget, ::GdkEventMotion* pEvent);
		void scenarioDrawingAreaButtonPressedCB(::GtkWidget* pWidget, ::GdkEventButton* pEvent);
		void scenarioDrawingAreaButtonReleasedCB(::GtkWidget* pWidget, ::GdkEventButton* pEvent);
		void scenarioDrawingAreaKeyPressEventCB(::GtkWidget* pWidget, ::GdkEventKey* pEvent);
		void scenarioDrawingAreaKeyReleaseEventCB(::GtkWidget* pWidget, ::GdkEventKey* pEvent);

		void contextMenuModifyInputCB(OpenViBE::uint32 ui32Index);
		void contextMenuAddInputCB(void);
		void contextMenuRemoveInputCB(void);
		void contextMenuModifySettingCB(OpenViBE::uint32 ui32Index);
		void contextMenuAddSettingCB(void);
		void contextMenuRemoveSettingCB(void);
		void contextMenuModifyOutputCB(OpenViBE::uint32 ui32Index);
		void contextMenuAddOutputCB(void);
		void contextMenuRemoveOutputCB(void);

		void toggleDesignerVisualisation();
		OpenViBE::boolean isDesignerVisualisationToggled();

		void showCurrentVisualisation();
		void hideCurrentVisualisation();

		void createPlayerVisualisation(void);
		void releasePlayerVisualisation(void);

	private:

		void generateDisplayPluginName(OpenViBE::Kernel::IBox* pDisplayBox, OpenViBE::CString& rDisplayBoxName);

	public:

		OpenViBE::CIdentifier m_oScenarioIdentifier;
		OpenViBE::CIdentifier m_oPlayerIdentifier;
		OpenViBE::CIdentifier m_oVisualisationTreeIdentifier;
		OpenViBE::Kernel::IKernel& m_rKernel;
		OpenViBE::Kernel::IScenario& m_rScenario;
		OpenViBE::Kernel::IPlayer* m_pPlayer;
		OpenViBE::uint64 m_ui64LastLoopTime;
		::GtkNotebook& m_rNotebook;
		OpenViBE::Kernel::IVisualisationTree* m_pVisualisationTree;
		OpenViBEDesigner::CDesignerVisualisation* m_pDesignerVisualisation;
		OpenViBE::boolean m_bDesignerVisualisationToggled;
		OpenViBEDesigner::CPlayerVisualisation* m_pPlayerVisualisation;
		::GladeXML* m_pGladeDummyScenarioNotebookTitle;
		::GladeXML* m_pGladeDummyScenarioNotebookClient;
		::GladeXML* m_pGladeTooltip;
		::GtkWidget* m_pNotebookPageTitle;
		::GtkWidget* m_pNotebookPageContent;
		::GtkDrawingArea* m_pScenarioDrawingArea;
		::GdkPixmap* m_pStencilBuffer;
		OpenViBE::boolean m_bHasFileName;
		OpenViBE::boolean m_bHasBeenModified;
		OpenViBE::boolean m_bButtonPressed;
		OpenViBE::boolean m_bShiftPressed;
		OpenViBE::boolean m_bControlPressed;
		OpenViBE::boolean m_bAltPressed;
		std::string m_sFileName;
		std::string m_sGUIFilename;
		OpenViBE::float64 m_f64PressMouseX;
		OpenViBE::float64 m_f64PressMouseY;
		OpenViBE::float64 m_f64ReleaseMouseX;
		OpenViBE::float64 m_f64ReleaseMouseY;
		OpenViBE::float64 m_f64CurrentMouseX;
		OpenViBE::float64 m_f64CurrentMouseY;
		OpenViBE::int32 m_i32ViewOffsetX;
		OpenViBE::int32 m_i32ViewOffsetY;
		OpenViBE::uint32 m_ui32CurrentMode;

		OpenViBE::uint32 m_ui32InterfacedObjectId;
		std::map<OpenViBE::uint32, OpenViBEDesigner::CInterfacedObject> m_vInterfacedObject;
		std::map<OpenViBE::CIdentifier, OpenViBE::boolean> m_vCurrentObject;
		OpenViBEDesigner::CInterfacedObject m_oCurrentObject;
	};
};

#endif // __OpenViBEDesigner_CInterfacedScenario_H__
