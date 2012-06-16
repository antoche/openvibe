#ifndef __OpenViBEPlugins_BoxAlgorithm_XDAWNSpatialFilterTrainer_H__
#define __OpenViBEPlugins_BoxAlgorithm_XDAWNSpatialFilterTrainer_H__

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

// TODO:
// - please move the identifier definitions in ovp_defines.h
// - please include your desciptor in ovp_main.cpp

#define OVP_ClassId_BoxAlgorithm_XDAWNSpatialFilterTrainer     OpenViBE::CIdentifier(0x27542F6E, 0x14AA3548)
#define OVP_ClassId_BoxAlgorithm_XDAWNSpatialFilterTrainerDesc OpenViBE::CIdentifier(0x128A6013, 0x370B5C2C)


namespace OpenViBEPlugins
{
	namespace SignalProcessingGpl
	{
		class CBoxAlgorithmXDAWNSpatialFilterTrainer : public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_XDAWNSpatialFilterTrainer);

		protected:

			OpenViBE::Kernel::IAlgorithmProxy* m_pStimulationDecoder;
			OpenViBE::Kernel::IAlgorithmProxy* m_pSignalDecoder;
			OpenViBE::Kernel::IAlgorithmProxy* m_pEvokedPotentialDecoder;

			OpenViBE::uint64 m_ui64StimulationIdentifier;
			OpenViBE::CString m_sSpatialFilterConfigurationFilename;
			OpenViBE::uint64 m_ui64FilterDimension;
		};

		class CBoxAlgorithmXDAWNSpatialFilterTrainerDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("xDAWN Spatial Filter Trainer"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Computes spatial filter coeffcients in order to get better evoked potential classification (typically used for P300 detection)"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Signal processing/Filtering"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-missing-image"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_XDAWNSpatialFilterTrainer; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SignalProcessingGpl::CBoxAlgorithmXDAWNSpatialFilterTrainer; }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput  ("Stimulations", OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addInput  ("Session signal", OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addInput  ("Evoked potential epochs", OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addSetting("Train stimulation", OV_TypeId_Stimulation, "OVTK_StimulationId_Train");
				rBoxAlgorithmPrototype.addSetting("Spatial filter configuration", OV_TypeId_Filename, "");
				rBoxAlgorithmPrototype.addSetting("Filter dimension", OV_TypeId_Integer, "4");
				// rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_IsUnstable);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_XDAWNSpatialFilterTrainerDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_XDAWNSpatialFilterTrainer_H__
