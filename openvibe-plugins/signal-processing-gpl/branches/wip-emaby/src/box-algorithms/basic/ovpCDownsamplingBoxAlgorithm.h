#ifndef __OpenViBEPlugins_SignalProcessingGpl_BoxAlgorithms_Filter_CDownsamplingBoxAlgorithm_H__
#define __OpenViBEPlugins_SignalProcessingGpl_BoxAlgorithms_Filter_CDownsamplingBoxAlgorithm_H__

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

namespace OpenViBEPlugins
{
	namespace SignalProcessingGpl
	{
		class CDownsamplingBoxAlgorithm : virtual public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);

			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, OVP_ClassId_Box_DownsamplingBoxAlgorithm)

		protected:

			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamDecoder;
			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamEncoder;
			OpenViBE::Kernel::IAlgorithmProxy* m_pComputeTemporalFilterCoefficients;
			OpenViBE::Kernel::IAlgorithmProxy* m_pApplyTemporalFilter;
			OpenViBE::Kernel::IAlgorithmProxy* m_pDownsampling;

			OpenViBE::Kernel::TParameterHandler < const OpenViBE::IMemoryBuffer* > ip_pMemoryBufferToDecode;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMemoryBuffer* > op_pEncodedMemoryBuffer;

			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > m_pInputSignal;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > m_pOutputSignal;
			OpenViBE::IMatrix* m_pOutputSignalDescription;
			OpenViBE::uint64 m_ui64NewSamplingRate;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > m_ui64SamplingRate;

			OpenViBE::uint64 m_ui64LastEndTime;
			OpenViBE::boolean m_bFlagFirstTime;
			OpenViBE::uint64 m_ui64LastBufferSize;
			OpenViBE::uint64 m_ui64CurrentBufferSize;
			OpenViBE::uint64 m_ui64SignalType;
		};

		class CDownsamplingBoxAlgorithmDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }
			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Downsampling - GPL"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("G. Gibert - E. Maby - P.E. Aguera"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INSERM/U821"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Filters and downsamples input buffer."); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("First, applies a low-pass (Butterworth, Chebychev or Yule-Walker) filter (frequency cut is 1/4, 1/3 or 1/2 of the new sampling rate) to input buffers of signal for anti-aliasing. Then, the input buffers of signal is downsampled."); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Signal processing/Basic"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.01"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString(""); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_Box_DownsamplingBoxAlgorithm; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SignalProcessingGpl::CDownsamplingBoxAlgorithm(); }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rPrototype) const
			{
				rPrototype.addInput  ("Input signal",           OV_TypeId_Signal);
				rPrototype.addOutput ("Output signal",          OV_TypeId_Signal);
				rPrototype.addSetting("New sampling rate (Hz)", OV_TypeId_Integer, "40");
				rPrototype.addSetting("Frequency cutoff ratio", OVP_TypeId_FrequencyCutOffRatio, "1/4");
				rPrototype.addSetting("Name of filter",         OVP_TypeId_FilterMethod, "Butterworth");
				rPrototype.addSetting("Filter order",           OV_TypeId_Integer, "4");
				rPrototype.addSetting("Pass band ripple (dB)",  OV_TypeId_Float, "0.5");
				rPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_IsUnstable);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_Box_DownsamplingBoxAlgorithmDesc)
		};
	};
};

#endif // __OpenViBEPlugins_SignalProcessingGpl_BoxAlgorithms_Filter_CDownsamplingBoxAlgorithm_H__
