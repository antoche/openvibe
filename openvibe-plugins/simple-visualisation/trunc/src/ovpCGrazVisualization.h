#ifndef __OpenViBEPlugins_SimpleVisualisation_CGrazVisualization_H__
#define __OpenViBEPlugins_SimpleVisualisation_CGrazVisualization_H__

#include "ovp_defines.h"

#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include <ebml/IReader.h>

#include <gtk/gtk.h>

#include <vector>
#include <string>
#include <map>
#include <deque>

#define Stimulation_Idle	0x320

namespace OpenViBEPlugins
{
	namespace SimpleVisualisation
	{
		enum EArrowDirection
		{
			EArrowDirection_None,
			EArrowDirection_Left,
			EArrowDirection_Right,
			EArrowDirection_Up,
			EArrowDirection_Down,
		};

		enum EGrazVisualizationState
		{
			EGrazVisualizationState_Idle,
			EGrazVisualizationState_Reference,
			EGrazVisualizationState_Cue,
			EGrazVisualizationState_ContinousFeedback
		};

		/**
		*/
		class CGrazVisualization :
			virtual public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>,
			virtual public OpenViBEToolkit::IBoxAlgorithmStimulationInputReaderCallback::ICallback,
			virtual public OpenViBEToolkit::IBoxAlgorithmStreamedMatrixInputReaderCallback::ICallback
		{
		public:

			CGrazVisualization(void);

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize();
			virtual OpenViBE::boolean uninitialize();
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process();

			virtual void setStimulationCount(const OpenViBE::uint32 ui32StimulationCount);
			virtual void setStimulation(const OpenViBE::uint32 ui32StimulationIndex, const OpenViBE::uint64 ui64StimulationIdentifier, const OpenViBE::uint64 ui64StimulationDate);

			virtual void setMatrixDimmensionCount(const OpenViBE::uint32 ui32DimmensionCount);
			virtual void setMatrixDimmensionSize(const OpenViBE::uint32 ui32DimmensionIndex, const OpenViBE::uint32 ui32DimmensionSize);
			virtual void setMatrixDimmensionLabel(const OpenViBE::uint32 ui32DimmensionIndex, const OpenViBE::uint32 ui32DimmensionEntryIndex, const char* sDimmensionLabel);
			virtual void setMatrixBuffer(const OpenViBE::float64* pBuffer);

			virtual void processState();

			virtual void redraw();
			virtual void resize(OpenViBE::uint32 ui32Width, OpenViBE::uint32 ui32Height);
			virtual void drawReferenceCross();
			virtual void drawArrow(EArrowDirection eDirection);
			virtual void drawBar();

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithm, OVP_ClassId_GrazVisualization)

		public:
			//! The Builder handler used to create the interface
			::GtkBuilder* m_pBuilderInterface;

			GtkWidget * m_pMainWindow;

			GtkWidget * m_pDrawingArea;

			//ebml
			EBML::IReader* m_pReader[2];
			OpenViBEToolkit::IBoxAlgorithmStimulationInputReaderCallback* m_pStimulationReaderCallBack;
			OpenViBEToolkit::IBoxAlgorithmStreamedMatrixInputReaderCallback* m_pStreamedMatrixReaderCallBack;

			EGrazVisualizationState m_eCurrentState;
			EArrowDirection m_eCurrentDirection;

			OpenViBE::float64 m_f64MaxAmplitude;
			OpenViBE::float64 m_f64BarScale;

			//Start and end time of the last buffer
			OpenViBE::uint64 m_ui64StartTime;
			OpenViBE::uint64 m_ui64EndTime;

			OpenViBE::boolean m_bError;

			GdkPixbuf * m_pOriginalBar;
			GdkPixbuf * m_pLeftBar;
			GdkPixbuf * m_pRightBar;

			GdkPixbuf * m_pOriginalLeftArrow;
			GdkPixbuf * m_pOriginalRightArrow;
			GdkPixbuf * m_pOriginalUpArrow;
			GdkPixbuf * m_pOriginalDownArrow;

			GdkPixbuf * m_pLeftArrow;
			GdkPixbuf * m_pRightArrow;
			GdkPixbuf * m_pUpArrow;
			GdkPixbuf * m_pDownArrow;

			GdkColor m_oBackgroundColor;
			GdkColor m_oForegroundColor;

			// Score
			std::map<OpenViBE::uint32, OpenViBE::uint32> m_vWindowFailCount;
			std::map<OpenViBE::uint32, OpenViBE::uint32> m_vWindowSuccessCount;
			std::deque<OpenViBE::float64> m_vAmplitude;
			OpenViBE::uint32 m_ui32WindowIndex;

			OpenViBE::boolean m_bShowInstruction;
			OpenViBE::boolean m_bShowFeedback;
		};

		/**
		* Plugin's description
		*/
		class CGrazVisualizationDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Graz visualization"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Bruno Renier"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Visualization plugin for the Graz experiment"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("Visualization/Feedback plugin for the Graz experiment"); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Visualisation/Presentation"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("0.1"); }
			virtual void release(void)                                   { }
			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_GrazVisualization; }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-fullscreen"); }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SimpleVisualisation::CGrazVisualization(); }

			virtual OpenViBE::boolean hasFunctionality(OpenViBE::Kernel::EPluginFunctionality ePF) const
			{
				return ePF == OpenViBE::Kernel::PluginFunctionality_Visualization;
			}

			virtual OpenViBE::boolean getBoxPrototype(OpenViBE::Kernel::IBoxProto& rPrototype) const
			{
				rPrototype.addInput("Stimulations", OV_TypeId_Stimulations);
				rPrototype.addInput("Amplitude", OV_TypeId_StreamedMatrix);

				rPrototype.addSetting("Show instruction", OV_TypeId_Boolean, "true");
				rPrototype.addSetting("Show feedback", OV_TypeId_Boolean, "false");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_GrazVisualizationDesc)

		};

	};
};

#endif
