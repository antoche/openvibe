#ifndef __OpenViBEPlugins_BoxAlgorithm_P300IdentifierStimulator_H__
#define __OpenViBEPlugins_BoxAlgorithm_P300IdentifierStimulator_H__

#include "ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include <map>

namespace OpenViBEPlugins
{
	namespace Stimulation
	{
		class CBoxAlgorithmP300IdentifierStimulator : public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::uint64 getClockFrequency(void);
			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32Index);
			virtual OpenViBE::boolean processClock(OpenViBE::CMessageClock& rMessageClock);
			virtual OpenViBE::boolean process(void);
			virtual OpenViBE::boolean reset();

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_P300IdentifierStimulator);

		protected:

			OpenViBE::uint64 m_ui64StartStimulation;
			OpenViBE::uint64 m_ui64StimulationBase;

			OpenViBE::uint64 m_ui64ImagesCount;

			OpenViBE::float64 m_f64PercentRepetitionTarget;

			OpenViBE::uint64 m_ui64RepetitionCountInTrial;
			OpenViBE::uint64 m_ui64TrialCount;
			OpenViBE::uint64 m_ui64FlashDuration;
			OpenViBE::uint64 m_ui64NoFlashDuration;
			OpenViBE::uint64 m_ui64InterRepetitionDuration;
			OpenViBE::uint64 m_ui64InterTrialDuration;

			// OpenViBE::boolean m_bAvoidNeighborFlashing;

		private:

			OpenViBE::Kernel::IAlgorithmProxy* m_pStimulationEncoder;
			OpenViBE::Kernel::IAlgorithmProxy* m_pStimulationDecoder;
			OpenViBE::Kernel::IAlgorithmProxy* m_pStimulationTargetDecoder;
			OpenViBE::uint64 m_ui64LastTime;
			OpenViBE::boolean m_bHeaderSent;
			OpenViBE::boolean m_bStartReceived;

			OpenViBE::uint32 m_ui32LastState;
			OpenViBE::uint64 m_ui64TrialStartTime;

			OpenViBE::uint64 m_ui64FlashCountInRepetition;
			OpenViBE::uint64 m_ui64FlashCountInRepetitionWithoutTarget;
			OpenViBE::uint64 m_ui64RepetitionDuration;
			OpenViBE::uint64 m_ui64RepetitionDurationWithoutTarget;
			OpenViBE::uint64 m_ui64TrialDuration;
			OpenViBE::uint64 m_ui64TrialIndex;

			OpenViBE::boolean m_bRepetitionWithoutTarget; //true if the repetition doesn't contains the target

			OpenViBE::uint64 m_ui64RepetitionIndex;
			OpenViBE::int64 m_i64TargetNumber;

			OpenViBE::uint64 *m_pRepetitionTarget; //for every Repetition of one trial this table indicate the number of target should see for the begin of the trial
			std::vector < OpenViBE::uint32 > m_vImages;

			void generate_sequence(void);
			void generate_trial_vars(void);
			OpenViBE::int64 getCurrentTimeInRepetition(OpenViBE::uint64 ui64CurrentTimeInTrial);
		};

		class CBoxAlgorithmP300IdentifierStimulatorDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("P300 Identifier Stimulator"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Baptiste Payan"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Stimulation"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-select-font"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_P300IdentifierStimulator; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::Stimulation::CBoxAlgorithmP300IdentifierStimulator; }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput  ("Incoming stimulations",           OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addOutput ("Produced stimulations",           OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addInput  ("Target Stimulations",             OV_TypeId_Stimulations);

				rBoxAlgorithmPrototype.addSetting("Start stimulation",               OV_TypeId_Stimulation, "OVTK_StimulationId_Label_00");
				rBoxAlgorithmPrototype.addSetting("Stimulation base",                OV_TypeId_Stimulation, "OVTK_StimulationId_Label_01");

				rBoxAlgorithmPrototype.addSetting("Number of identifiable objects",  OV_TypeId_Integer,     "6");

				rBoxAlgorithmPrototype.addSetting("Percent of repetitions containing the target", OV_TypeId_Float, "100");

				rBoxAlgorithmPrototype.addSetting("Number of repetitions",           OV_TypeId_Integer,     "5");
				rBoxAlgorithmPrototype.addSetting("Number of trials",                OV_TypeId_Integer,     "5");
				rBoxAlgorithmPrototype.addSetting("Flash duration (in sec)",         OV_TypeId_Float,       "0.075");
				rBoxAlgorithmPrototype.addSetting("No flash duration (in sec)",      OV_TypeId_Float,       "0.125");
				rBoxAlgorithmPrototype.addSetting("Inter-repetition delay (in sec)", OV_TypeId_Float,       "2");
				rBoxAlgorithmPrototype.addSetting("Inter-trial delay (in sec)",      OV_TypeId_Float,       "5");

				// rBoxAlgorithmPrototype.addSetting("Avoid neighbor flashing",         OV_TypeId_Boolean,     "false");

				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_IsUnstable);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_P300IdentifierStimulatorDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_P300IdentifierStimulator_H__
