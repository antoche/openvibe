#ifndef __OpenViBEDesigner_CApplication_H__
#define __OpenViBEDesigner_CApplication_H__

#include "ovd_base.h"

#include <vector>


namespace OpenViBEDesigner
{
	class CInterfacedScenario;

	class CLogListenerDesigner;

	class CApplication
	{
	public:
		CApplication(const OpenViBE::Kernel::IKernelContext& rKernelContext);

		void initialize(OpenViBEDesigner::ECommandLineFlag eCommandLineFlags);

		OpenViBE::boolean openScenario(const char* sFileName);

		/** \name Drag and drop management */
		//@{

		void dragDataGetCB(
			::GtkWidget* pWidget,
			::GdkDragContext* pDragContex,
			::GtkSelectionData* pSelectionData,
			guint uiInfo,
			guint uiT);

		//@}

		/** \name Selection management */
		//@{

		void undoCB(void);
		void redoCB(void);

		void copySelectionCB(void);
		void cutSelectionCB(void);
		void pasteSelectionCB(void);
		void deleteSelectionCB(void);
		void preferencesCB(void);

		//@}

		/** \name Scenario management */
		//@{

		OpenViBE::CString getWorkingDirectory(void);

		OpenViBE::boolean hasRunningScenario(void);
		OpenViBE::boolean hasUnsavedScenario(void);

		OpenViBEDesigner::CInterfacedScenario* getCurrentInterfacedScenario(void);

		void testCB(void);
		void newScenarioCB(void);
		void openScenarioCB(void);
		void saveScenarioCB(OpenViBEDesigner::CInterfacedScenario* pInterfacedScenario=NULL); // defaults to current scenario if NULL
		void saveScenarioAsCB(OpenViBEDesigner::CInterfacedScenario* pInterfacedScenario=NULL); // defaults to current scenario if NULL
		void closeScenarioCB(
			OpenViBEDesigner::CInterfacedScenario* pInterfacedScenario);

		void stopScenarioCB(void);
		void pauseScenarioCB(void);
		void nextScenarioCB(void);
		void playScenarioCB(void);
		void forwardScenarioCB(void);

		void addCommentCB(
			OpenViBEDesigner::CInterfacedScenario* pScenario);

		void changeCurrentScenario(
			OpenViBE::int32 i32PageIndex);

		//@}

		/** \name Designer visualisation management */
		//@{

		void deleteDesignerVisualisationCB();

		void toggleDesignerVisualisationCB();

		//@}

		/** \name Player management */
		//@{

		OpenViBE::Kernel::IPlayer* getPlayer(void);

		void createPlayer(void);

		void releasePlayer(void);

		//@}

		/** \name Application management */
		//@{

		OpenViBE::boolean quitApplicationCB(void);
		void aboutOpenViBECB(void);
		void aboutScenarioCB(OpenViBEDesigner::CInterfacedScenario* pScenario);
		void browseDocumentationCB(void);

		//@}

		/** \name Log management */
		//@{

		void logLevelCB(void);
		void logLevelMessagesCB(void);

		//@}

		/** \name CPU usage */
		//@{

		void CPUUsageCB(void);

		//@}

	public:

		const OpenViBE::Kernel::IKernelContext& m_rKernelContext;
		OpenViBE::Kernel::IPluginManager* m_pPluginManager;
		OpenViBE::Kernel::IScenarioManager* m_pScenarioManager;
		OpenViBE::Kernel::IVisualisationManager* m_pVisualisationManager;
		OpenViBE::Kernel::IScenario* m_pClipboardScenario;

		OpenViBEDesigner::CLogListenerDesigner* m_pLogListenerDesigner;

		OpenViBEDesigner::ECommandLineFlag m_eCommandLineFlags;

		::GtkBuilder* m_pBuilderInterface;
		::GtkWidget* m_pMainWindow;
		::GtkNotebook* m_pScenarioNotebook;
		::GtkNotebook* m_pResourceNotebook;
		::GtkTreeStore* m_pBoxAlgorithmTreeModel;
		::GtkTreeModel* m_pBoxAlgorithmTreeModelFilter;
		::GtkTreeModel* m_pBoxAlgorithmTreeModelFilter2;
		::GtkTreeModel* m_pBoxAlgorithmTreeModelFilter3;
		::GtkTreeModel* m_pBoxAlgorithmTreeModelFilter4;
		::GtkTreeView* m_pBoxAlgorithmTreeView;
		::GtkTreeStore* m_pAlgorithmTreeModel;
		::GtkTreeView* m_pAlgorithmTreeView;
		gint m_giFilterTimeout;

		const gchar* m_sSearchTerm;
		
		OpenViBE::uint64 m_ui64LastTimeRefresh;
		OpenViBE::boolean m_bIsQuitting;

		std::vector < OpenViBEDesigner::CInterfacedScenario* > m_vInterfacedScenario;
	};
};

#endif // __OpenViBEDesigner_CApplication_H__
