#ifndef __OpenViBEPlugins_BoxAlgorithm_P300SpellerStimulator_H__
#define __OpenViBEPlugins_BoxAlgorithm_P300SpellerStimulator_H__

#include "ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include <map>

// TODO:
// - please move the identifier definitions in ovp_defines.h
// - please include your desciptor in ovp_main.cpp

#define OVP_ClassId_BoxAlgorithm_P300SpellerStimulator     OpenViBE::CIdentifier(0x88857F9A, 0xF560D3EB)
#define OVP_ClassId_BoxAlgorithm_P300SpellerStimulatorDesc OpenViBE::CIdentifier(0xCEAFBB05, 0x5DA19DCB)

namespace OpenViBEPlugins
{
	namespace Stimulation
	{
		class CBoxAlgorithmP300SpellerStimulator : public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::uint64 getClockFrequency(void);
			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32Index);
			virtual OpenViBE::boolean processClock(OpenViBE::CMessageClock& rMessageClock);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_P300SpellerStimulator);

		protected:

			OpenViBE::uint64 m_ui64StartStimulation;
			OpenViBE::uint64 m_ui64RowStimulationBase;
			OpenViBE::uint64 m_ui64ColumnStimulationBase;

			OpenViBE::uint64 m_ui64RowCount;
			OpenViBE::uint64 m_ui64ColumnCount;

			OpenViBE::uint64 m_ui64RepetitionCountInTrial;
			OpenViBE::uint64 m_ui64TrialCount;
			OpenViBE::uint64 m_ui64FlashDuration;
			OpenViBE::uint64 m_ui64NoFlashDuration;
			OpenViBE::uint64 m_ui64InterRepetitionDuration;
			OpenViBE::uint64 m_ui64InterTrialDuration;

			OpenViBE::boolean m_bAvoidNeighborFlashing;

		private:

			OpenViBE::Kernel::IAlgorithmProxy* m_pStimulationEncoder;
			OpenViBE::Kernel::IAlgorithmProxy* m_pStimulationDecoder;
			OpenViBE::uint64 m_ui64LastTime;
			OpenViBE::boolean m_bHeaderSent;
			OpenViBE::boolean m_bStartReceived;

			OpenViBE::uint32 m_ui32LastState;
			OpenViBE::uint64 m_ui64TrialStartTime;

			OpenViBE::uint64 m_ui64FlashCountInRepetition;
			OpenViBE::uint64 m_ui64RepetitionDuration;
			OpenViBE::uint64 m_ui64TrialDuration;
			OpenViBE::uint64 m_ui64TrialIndex;

			std::map < OpenViBE::uint64, OpenViBE::uint64 > m_vRow;
			std::map < OpenViBE::uint64, OpenViBE::uint64 > m_vColumn;

		private:

			void generate_sequence(void);
		};

		class CBoxAlgorithmP300SpellerStimulatorDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("P300 Speller Stimulator"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Stimulation"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-select-font"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_P300SpellerStimulator; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::Stimulation::CBoxAlgorithmP300SpellerStimulator; }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput  ("Incoming stimulations",           OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addOutput ("Produced stimulations",           OV_TypeId_Stimulations);

				rBoxAlgorithmPrototype.addSetting("Start stimulation",               OV_TypeId_Stimulation, "OVTK_StimulationId_Label_00");
				rBoxAlgorithmPrototype.addSetting("Row stimulation base",            OV_TypeId_Stimulation, "OVTK_StimulationId_Label_01");
				rBoxAlgorithmPrototype.addSetting("Column stimulation base",         OV_TypeId_Stimulation, "OVTK_StimulationId_Label_07");

				rBoxAlgorithmPrototype.addSetting("Number of rows",                  OV_TypeId_Integer,     "6");
				rBoxAlgorithmPrototype.addSetting("Number of columns",               OV_TypeId_Integer,     "6");

				rBoxAlgorithmPrototype.addSetting("Number of repetitions",           OV_TypeId_Integer,     "5");
				rBoxAlgorithmPrototype.addSetting("Number of trials",                OV_TypeId_Integer,     "5");
				rBoxAlgorithmPrototype.addSetting("Flash duration (in sec)",         OV_TypeId_Float,       "0.075");
				rBoxAlgorithmPrototype.addSetting("No flash duration (in sec)",      OV_TypeId_Float,       "0.125");
				rBoxAlgorithmPrototype.addSetting("Inter-repetition delay (in sec)", OV_TypeId_Float,       "2");
				rBoxAlgorithmPrototype.addSetting("Inter-trial delay (in sec)",      OV_TypeId_Float,       "5");

				rBoxAlgorithmPrototype.addSetting("Avoid neighbor flashing",         OV_TypeId_Boolean,     "false");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_P300SpellerStimulatorDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_P300SpellerStimulator_H__
