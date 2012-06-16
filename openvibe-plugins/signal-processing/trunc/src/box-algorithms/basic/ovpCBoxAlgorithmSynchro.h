#ifndef __OpenViBEPlugins_BoxAlgorithm_Synchro_H__
#define __OpenViBEPlugins_BoxAlgorithm_Synchro_H__

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include "ovpCInputChannel.h"
#include "ovpCOutputChannel.h"

#include <string>
#include <vector>

#define OVP_ClassId_BoxAlgorithm_Synchro     OpenViBE::CIdentifier(0x7D8C1A18, 0x4C273A91)
#define OVP_ClassId_BoxAlgorithm_SynchroDesc OpenViBE::CIdentifier(0x4E806E5E, 0x5035290D)

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CBoxAlgorithmSynchro : public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_Synchro);

		protected:

			//Intern ressources
			OpenViBE::boolean m_bStimulationReceivedStart;

			// new
			CInputChannel     m_oCInputChannel;
			COutputChannel    m_oCOutputChannel;
		};

		class CBoxAlgorithmSynchroDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Stream Synchronization"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Gelu Ionescu & Matthieu Goyat"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("GIPSA-lab"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Synchronize two acquisition servers"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Signal processing/Basic"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-missing-image"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_Synchro; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SignalProcessing::CBoxAlgorithmSynchro; }
			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput  ("Input signal", OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addInput  ("Input stimulation", OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addOutput ("Output signal", OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addOutput ("Output stimulation", OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addSetting("Synchronisation stimulation", OV_TypeId_Stimulation, "OVTK_StimulationId_ExperimentStart");
				// rBoxAlgorithmPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_CanModifyInput);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_SynchroDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_Synchro_H__
