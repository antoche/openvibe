#ifndef __OpenViBEPlugins_SignalProcessingGpl_BoxAlgorithms_Filter_CTemporalFilterBoxAlgorithm_H__
#define __OpenViBEPlugins_SignalProcessingGpl_BoxAlgorithms_Filter_CTemporalFilterBoxAlgorithm_H__

#include "../../ovp_defines.h"

#include <openvibe-toolkit/ovtk_all.h>

namespace OpenViBEPlugins
{
	namespace SignalProcessingGpl
	{
		class CTemporalFilterBoxAlgorithm : virtual public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);

			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, OVP_ClassId_Box_TemporalFilterBoxAlgorithm)

		protected:

			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamDecoder;
			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamEncoder;
			OpenViBE::Kernel::IAlgorithmProxy* m_pComputeTemporalFilterCoefficients;
			OpenViBE::Kernel::IAlgorithmProxy* m_pApplyTemporalFilter;

			OpenViBE::Kernel::TParameterHandler < const OpenViBE::IMemoryBuffer* > ip_pMemoryBufferToDecode;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMemoryBuffer* > op_pEncodedMemoryBuffer;
//			OpenViBE::uint64 m_ui64LastStartTime;
			OpenViBE::uint64 m_ui64LastEndTime;
		};

		class CTemporalFilterBoxAlgorithmDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }
			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Temporal filter - GPL"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Guillaume Gibert"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INSERM/U821"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Signal processing/Filtering"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString(""); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_Box_TemporalFilterBoxAlgorithm; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SignalProcessingGpl::CTemporalFilterBoxAlgorithm(); }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rPrototype) const
			{
				rPrototype.addInput  ("Input signal",            OV_TypeId_Signal);
				rPrototype.addOutput ("Filtered signal",         OV_TypeId_Signal);
				rPrototype.addSetting("Filter method",           OVP_TypeId_FilterMethod, OVP_TypeId_FilterMethod_Butterworth.toString());
				rPrototype.addSetting("Filter type",             OVP_TypeId_FilterType,   OVP_TypeId_FilterType_BandPass.toString());
				rPrototype.addSetting("Filter order",            OV_TypeId_Integer,       "4");
				rPrototype.addSetting("Low cut frequency (Hz)",  OV_TypeId_Float,         "29");
				rPrototype.addSetting("High cut frequency (Hz)", OV_TypeId_Float,         "40");
				rPrototype.addSetting("Pass band ripple (dB)",   OV_TypeId_Float,         "0.5");
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_Box_TemporalFilterBoxAlgorithmDesc)
		};
	};
};

#endif // __OpenViBEPlugins_SignalProcessingGpl_BoxAlgorithms_Filter_CTemporalFilterBoxAlgorithm_H__
