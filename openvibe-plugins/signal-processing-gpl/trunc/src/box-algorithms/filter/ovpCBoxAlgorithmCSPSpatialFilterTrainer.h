#ifndef __OpenViBEPlugins_BoxAlgorithm_CSPSpatialFilterTrainer_H__
#define __OpenViBEPlugins_BoxAlgorithm_CSPSpatialFilterTrainer_H__

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#define OVP_ClassId_BoxAlgorithm_CSPSpatialFilterTrainer     OpenViBE::CIdentifier(0x51DB0D64, 0x2109714E)
#define OVP_ClassId_BoxAlgorithm_CSPSpatialFilterTrainerDesc OpenViBE::CIdentifier(0x05120978, 0x14E061CD)


namespace OpenViBEPlugins
{
	namespace SignalProcessingGpl
	{
		class CBoxAlgorithmCSPSpatialFilterTrainer : public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_CSPSpatialFilterTrainer);

		protected:

			OpenViBE::Kernel::IAlgorithmProxy* m_pStimulationDecoder;
			OpenViBE::Kernel::IAlgorithmProxy* m_pSignalDecoderCondition1;
			OpenViBE::Kernel::IAlgorithmProxy* m_pSignalDecoderCondition2;

			OpenViBEToolkit::TStimulationEncoder<CBoxAlgorithmCSPSpatialFilterTrainer> m_oStimulationEncoder;

			OpenViBE::uint64 m_ui64StimulationIdentifier;
			OpenViBE::CString m_sSpatialFilterConfigurationFilename;
			OpenViBE::uint64 m_ui64FilterDimension;
		};

		class CBoxAlgorithmCSPSpatialFilterTrainerDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:


			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("CSP Spatial Filter Trainer"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Dieter Devlaminck"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("Ghent University"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Computes spatial filter coeffcients according to the Common Spatial Pattern algorithm."); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("The CSP algortihm increases the signal variance for one condition while minimizing the variance for the other condition."); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Signal processing/Filtering"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString(""); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_CSPSpatialFilterTrainer; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SignalProcessingGpl::CBoxAlgorithmCSPSpatialFilterTrainer; }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput  ("Stimulations",                 OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addInput  ("Signal condition 1",           OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addInput  ("Signal condition 2",           OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addSetting("Train Trigger",                OV_TypeId_Stimulation, "OVTK_GDF_End_Of_Session");
				rBoxAlgorithmPrototype.addSetting("Spatial filter configuration", OV_TypeId_Filename, "");
				rBoxAlgorithmPrototype.addSetting("Filter dimension",             OV_TypeId_Integer, "2");
				
				rBoxAlgorithmPrototype.addOutput ("Train-completed Flag",         OV_TypeId_Stimulations);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_CSPSpatialFilterTrainerDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_XDAWNSpatialFilterTrainer_H__
