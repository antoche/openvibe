#ifndef __OpenViBEPlugins_SimpleVisualisation_CSignalDisplay_H__
#define __OpenViBEPlugins_SimpleVisualisation_CSignalDisplay_H__

#include "ovp_defines.h"

#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include <ebml/IReader.h>

#include <vector>
#include <string>

#include "ovpCBufferDatabase.h"
#include "ovpCSignalDisplay/ovpCSignalDisplayView.h"

namespace OpenViBEPlugins
{
	namespace SimpleVisualisation
	{
		/**
		* This plugin opens a new GTK window and displays the incoming signals. The user may change the zoom level,
		* the width of the time window displayed, ...
		*/
		class CSignalDisplay : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>,
			public OpenViBEToolkit::IBoxAlgorithmStreamedMatrixInputReaderCallback::ICallback,
			public OpenViBEToolkit::IBoxAlgorithmStimulationInputReaderCallback::ICallback
		{
		public:

			CSignalDisplay(void);

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize();
			virtual OpenViBE::boolean uninitialize();
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process();

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithm, OVP_ClassId_SignalDisplay)

			virtual void setMatrixDimmensionCount(const OpenViBE::uint32 ui32DimmensionCount);
			virtual void setMatrixDimmensionSize(const OpenViBE::uint32 ui32DimmensionIndex, const OpenViBE::uint32 ui32DimmensionSize);
			virtual void setMatrixDimmensionLabel(const OpenViBE::uint32 ui32DimmensionIndex, const OpenViBE::uint32 ui32DimmensionEntryIndex, const char* sDimmensionLabel);
			virtual void setMatrixBuffer(const OpenViBE::float64* pBuffer);

			virtual void setStimulationCount(const OpenViBE::uint32 ui32StimulationCount);
			virtual void setStimulation(const OpenViBE::uint32 ui32StimulationIndex, const OpenViBE::uint64 ui64StimulationIdentifier, const OpenViBE::uint64 ui64StimulationDate);

		public:

			//ebml
			EBML::IReader* m_pStreamedMatrixReader;
			EBML::IReader* m_pStimulationReader;
			OpenViBEToolkit::IBoxAlgorithmStreamedMatrixInputReaderCallback* m_pStreamedMatrixReaderCallBack;
			OpenViBEToolkit::IBoxAlgorithmStimulationInputReaderCallback* m_pStimulationReaderCallBack;

			//The main object used for the display (contains all the GUI code)
			CSignalDisplayDrawable * m_pSignalDisplayView;

			//Contains all the data about the incoming signal
			CBufferDatabase * m_pBufferDatabase;

			//Start and end time of the last buffer
			OpenViBE::uint64 m_ui64StartTime;
			OpenViBE::uint64 m_ui64EndTime;
		};

		/**
		 * Signal Display plugin descriptor
		 */
		class CSignalDisplayDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Signal display"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Bruno Renier / Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Displays the incoming signal"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("Displays the incoming signal"); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Visualisation/Basic"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("0.2"); }
			virtual void release(void)                                   { }
			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_SignalDisplay; }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-zoom-fit"); }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SimpleVisualisation::CSignalDisplay(); }

			virtual OpenViBE::boolean hasFunctionality(OpenViBE::Kernel::EPluginFunctionality ePF) const
			{
				return ePF == OpenViBE::Kernel::PluginFunctionality_Visualization;
			}

			virtual OpenViBE::boolean getBoxPrototype(OpenViBE::Kernel::IBoxProto& rPrototype) const
			{
				rPrototype.addSetting("Time Scale", OV_TypeId_Float, "10");
				rPrototype.addSetting("Display Mode", OVP_TypeId_SignalDisplayMode, "Scan");
				rPrototype.addSetting("Manual Vertical Scale",OV_TypeId_Boolean, "false");
				rPrototype.addSetting("Vertical Scale",OV_TypeId_Float, "100");
				rPrototype.addInput("Signal", OV_TypeId_StreamedMatrix);
				rPrototype.addInput("Stimulations", OV_TypeId_Stimulations);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_SignalDisplayDesc)
		};
	};
};

#endif
