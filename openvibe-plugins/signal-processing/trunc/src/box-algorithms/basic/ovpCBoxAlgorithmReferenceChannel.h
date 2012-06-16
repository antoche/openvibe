#ifndef __OpenViBEPlugins_BoxAlgorithm_ReferenceChannel_H__
#define __OpenViBEPlugins_BoxAlgorithm_ReferenceChannel_H__

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#define OVP_ClassId_BoxAlgorithm_ReferenceChannel     OpenViBE::CIdentifier(0x444721AD, 0x78342CF5)
#define OVP_ClassId_BoxAlgorithm_ReferenceChannelDesc OpenViBE::CIdentifier(0x42856103, 0x45B125AD)

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CBoxAlgorithmReferenceChannel : public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_ReferenceChannel);

		protected:

			OpenViBEToolkit::TSignalDecoder < CBoxAlgorithmReferenceChannel > m_oDecoder;
			OpenViBEToolkit::TSignalEncoder < CBoxAlgorithmReferenceChannel > m_oEncoder;
			OpenViBE::uint32 m_ui32ReferenceChannelIndex;
		};

		class CBoxAlgorithmReferenceChannelDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Reference Channel"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("..."); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("..."); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Signal processing/Basic"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-missing-image"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_ReferenceChannel; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SignalProcessing::CBoxAlgorithmReferenceChannel; }
			// virtual OpenViBE::Plugins::IBoxListener* createBoxListener(void) const               { return new CBoxAlgorithmReferenceChannelListener; }
			// virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput  ("Input signal",  OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addOutput ("Output signal", OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addSetting("Channel",                  OV_TypeId_String, "Ref_Nose");
				rBoxAlgorithmPrototype.addSetting("Channel Matching Method",  OVP_TypeId_MatchMethod,     OVP_TypeId_MatchMethod_Smart.toString());
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_ReferenceChannelDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_ReferenceChannel_H__
