#ifndef __OpenViBEPlugins_BoxAlgorithm_StimulationBasedEpoching_H__
#define __OpenViBEPlugins_BoxAlgorithm_StimulationBasedEpoching_H__

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include <vector>
#include <map>

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CBoxAlgorithmStimulationBasedEpoching : public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_StimulationBasedEpoching);

		protected:

			typedef struct
			{
				OpenViBE::Kernel::IAlgorithmProxy* m_pEpocher;
				OpenViBE::uint64 m_ui64StimulationTime;
				OpenViBE::uint64 m_ui64StartTime;
				OpenViBE::uint64 m_ui64EndTime;
				OpenViBE::boolean m_bNeedsReset;
			} SStimulationBasedEpoching;

			OpenViBE::Kernel::IAlgorithmProxy* m_pStimulationStreamDecoder;
			OpenViBE::Kernel::IAlgorithmProxy* m_pStimulationStreamEncoder;
			OpenViBE::Kernel::IAlgorithmProxy* m_pSignalStreamDecoder;
			OpenViBE::Kernel::IAlgorithmProxy* m_pSignalStreamEncoder;
			std::vector < SStimulationBasedEpoching > m_vStimulationBasedEpoching;

			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > op_ui64SamplingRate;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IStimulationSet* > op_pStimulationSet;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IStimulationSet* > ip_pStimulationSet;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > ip_pSignal;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > op_pSignal;

			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMemoryBuffer* > op_pStimulationMemoryBuffer;
			OpenViBE::Kernel::TParameterHandler < const OpenViBE::IMemoryBuffer* > ip_pStimulationMemoryBuffer;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMemoryBuffer* > op_SignalMemoryBuffer;
			OpenViBE::Kernel::TParameterHandler < const OpenViBE::IMemoryBuffer* > ip_SignalMemoryBuffer;

			OpenViBE::IMatrix* m_pOutputSignalDescription;

			std::map < OpenViBE::uint64, OpenViBE::boolean > m_vStimulationId;
			OpenViBE::uint64 m_ui64EpochDuration;
			OpenViBE::int64 m_i64EpochOffset;
			OpenViBE::uint64 m_ui64LastStimulationInputStartTime;
			OpenViBE::uint64 m_ui64LastStimulationInputEndTime;
			OpenViBE::uint64 m_ui64LastStimulationOutputEndTime;
		};

		class CBoxAlgorithmStimulationBasedEpochingDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Stimulation based epoching"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Performs epoching near a stimulation"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Signal processing/Epoching"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_StimulationBasedEpoching; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SignalProcessing::CBoxAlgorithmStimulationBasedEpoching; }
			virtual OpenViBE::CString getStockItemName(void) const       { return "gtk-cut"; }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput("Input signal",                OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addInput("Input stimulations",          OV_TypeId_Stimulations);

				rBoxAlgorithmPrototype.addOutput("Epoched signal",             OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addOutput("Resynced stimulations",      OV_TypeId_Stimulations);

				rBoxAlgorithmPrototype.addSetting("Epoch duration (in sec)",   OV_TypeId_Float,       "1");
				rBoxAlgorithmPrototype.addSetting("Epoch offset (in sec)",     OV_TypeId_Float,       "0.5");
				rBoxAlgorithmPrototype.addSetting("Stimulation to epoch from", OV_TypeId_Stimulation, "OVTK_GDF_VEP");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_StimulationBasedEpochingDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_StimulationBasedEpoching_H__
