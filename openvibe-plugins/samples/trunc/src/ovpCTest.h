#ifndef __SamplePlugin_CTest_H__
#define __SamplePlugin_CTest_H__

#include "ovp_defines.h"

#include <openvibe-toolkit/ovtk_all.h>

#include <ebml/CReader.h>
#include <ebml/CReaderHelper.h>

namespace OpenViBEPlugins
{
	namespace Samples
	{
		class CTest : public OpenViBEToolkit::TTrainingBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:

			CTest(void);
			virtual ~CTest(void);

			virtual void release(void);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			virtual OpenViBE::CIdentifier getStimulationIdentifierTrialStart(void);
			virtual OpenViBE::CIdentifier getStimulationIdentifierTrialEnd(void);
			virtual OpenViBE::CIdentifier getStimulationIdentifierTrialLabelRangeStart(void);
			virtual OpenViBE::CIdentifier getStimulationIdentifierTrialLabelRangeEnd(void);
			virtual OpenViBE::CIdentifier getStimulationIdentifierTrain(void);
			virtual OpenViBE::boolean train(OpenViBEToolkit::ISignalTrialSet& rTrialSet);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TTrainingBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, OVP_ClassId_Test)

			OpenViBEToolkit::IBoxAlgorithmSignalInputReaderCallback::TCallbackProxy1<OpenViBEToolkit::TTrainingBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm> > m_oSignalReaderCallbackProxy;
			OpenViBEToolkit::IBoxAlgorithmStimulationInputReaderCallback::TCallbackProxy1<OpenViBEToolkit::TTrainingBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm> > m_oStimulationReaderCallbackProxy;

			OpenViBEToolkit::IBoxAlgorithmSignalInputReaderCallback* m_pSignalReaderCallback;
			OpenViBEToolkit::IBoxAlgorithmStimulationInputReaderCallback* m_pStimulationReaderCallback;

			EBML::CReader m_oSignalReader;
			EBML::CReader m_oStimulationReader;

			OpenViBE::uint64 m_ui64SignalLatestSampleTime;
		};

		class CTestDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }
			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Test"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Samples"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_Test; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::Samples::CTest(); }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rPrototype) const
			{
				// Adds box inputs
				rPrototype.addInput("Signal stream", OV_TypeId_Signal);
				rPrototype.addInput("Stimulation stream", OV_TypeId_Stimulations);

				// Adds box outputs

				// Adds box settings
				rPrototype.addSetting("Trial start stimulation", OVTK_TypeId_Stimulation, "OVTK_Stimulation_TrialStart");
				rPrototype.addSetting("Trial end stimulation", OVTK_TypeId_Stimulation, "OVTK_Stimulation_TrialEnd");
				rPrototype.addSetting("Trial label range start stimulation", OVTK_TypeId_Stimulation, "OVTK_Stimulation_LabelStart");
				rPrototype.addSetting("Trial label range end stimulation", OVTK_TypeId_Stimulation, "OVTK_Stimulation_LabelEnd");
				rPrototype.addSetting("Train stimulation", OVTK_TypeId_Stimulation, "OVTK_Stimulation_Train");

				return true;
			}

			virtual OpenViBE::CString getStockItemName(void) const
			{
				return OpenViBE::CString("gtk-execute");
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_TestDesc)
		};
	};
};

#endif // __SamplePlugin_CTest_H__
