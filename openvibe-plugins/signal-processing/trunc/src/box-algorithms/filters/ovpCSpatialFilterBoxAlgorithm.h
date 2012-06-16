#if 0
#ifndef __OpenViBEPlugins_SignalProcessing_BoxAlgorithms_Filter_CSpatialFilterBoxAlgorithm_H__
#define __OpenViBEPlugins_SignalProcessing_BoxAlgorithms_Filter_CSpatialFilterBoxAlgorithm_H__

#include "../../ovp_defines.h"

#include <openvibe-toolkit/ovtk_all.h>

#define OVP_ClassId_Box_SpatialFilterBoxAlgorithm     OpenViBE::CIdentifier(0xDD332C6C, 0x195B4FD4)
#define OVP_ClassId_Box_SpatialFilterBoxAlgorithmDesc OpenViBE::CIdentifier(0x72A01C92, 0xF8C1FA24)

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CSpatialFilterBoxAlgorithm : virtual public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);

			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, OVP_ClassId_Box_SpatialFilterBoxAlgorithm)

		protected:

			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamDecoder;
			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamEncoder;
			OpenViBE::Kernel::IAlgorithmProxy* m_pApplySpatialFilter;

			std::vector<OpenViBE::float64> m_oCoefficients;
		};

		class CSpatialFilterBoxAlgorithmDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }
			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Spatial filter Deprecated"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Guillaume Gibert"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INSERM/U821"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Signal processing/Filtering"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString(""); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_Box_SpatialFilterBoxAlgorithm; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SignalProcessing::CSpatialFilterBoxAlgorithm; }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rPrototype) const
			{
				rPrototype.addInput("Input signal", OV_TypeId_Signal);

				rPrototype.addOutput("Filtered signal", OV_TypeId_Signal);

				rPrototype.addSetting("Spatial filter coefficients", OV_TypeId_String, "1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1");
				rPrototype.addSetting("Number of output channels", OV_TypeId_Integer, "4");
				rPrototype.addSetting("Number of input channels", OV_TypeId_Integer, "4");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_Box_SpatialFilterBoxAlgorithmDesc)
		};
	};
};

#endif // __OpenViBEPlugins_SignalProcessing_BoxAlgorithms_Filter_CSpatialFilterBoxAlgorithm_H__
#endif
