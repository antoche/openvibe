#ifndef __OpenViBEPlugins_SignChangeDetector_H__
#define __OpenViBEPlugins_SignChangeDetector_H__

#include "ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include <ebml/IReader.h>

namespace OpenViBEPlugins
{
	namespace Stimulation
	{
		class CSignChangeDetector :
				virtual public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_SignChangeDetector);

		protected:

			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamedMatrixDecoder;
			OpenViBE::Kernel::IAlgorithmProxy* m_pStimulationEncoder;

			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > op_pMatrix;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IStimulationSet* > ip_pStimulationSet;

			OpenViBE::uint64 m_ui64OnStimulationId;
			OpenViBE::uint64 m_ui64OffStimulationId;
			OpenViBE::uint64 m_ui64ChannelIndex;
			OpenViBE::uint64 m_ui64SamplesPerChannel;
			OpenViBE::float64 m_f64Lastsample;
			OpenViBE::boolean m_bNextStimIsOn; // if true next stimulation will be m_ui64OnStimulationId and the next one m_ui64OffStimulationId and so on
			OpenViBE::boolean m_bError;
		};

		class CSignChangeDetectorDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Sign Change Detector"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Joan Fruitet and Jozef Legény"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("Inria Sophia"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Detects the change of input's sign"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("Triggers a stimulation when one of the input's sign changes (input gets positive or negative"); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Stimulation"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.2"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-missing-image"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_SignChangeDetector; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::Stimulation::CSignChangeDetector; }

			virtual OpenViBE::boolean getBoxPrototype(OpenViBE::Kernel::IBoxProto& rPrototype) const
			{
				rPrototype.addInput  ("Signal", OV_TypeId_StreamedMatrix);
				rPrototype.addOutput ("Generated stimulations",             OV_TypeId_Stimulations);
				rPrototype.addSetting("Sign switch to positive stimulation",                     OV_TypeId_Stimulation, "OVTK_StimulationId_Label_01");
				rPrototype.addSetting("Sign switch to negative stimulation",                    OV_TypeId_Stimulation, "OVTK_StimulationId_Label_00");
				rPrototype.addSetting("Channel Index",                      OV_TypeId_Integer, "1");
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_SignChangeDetectorDesc);
		};
	};
};

#endif // __OpenViBEPlugins_SignChangeDetector_H__
