#ifndef __OpenViBEPlugins_BoxAlgorithm_SignalDecimation_H__
#define __OpenViBEPlugins_BoxAlgorithm_SignalDecimation_H__

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

// TODO:
// - please move the identifier definitions in ovp_defines.h
// - please include your desciptor in ovp_main.cpp

#define OVP_ClassId_BoxAlgorithm_SignalDecimation     OpenViBE::CIdentifier(0x012F4BEA, 0x3BE37C66)
#define OVP_ClassId_BoxAlgorithm_SignalDecimationDesc OpenViBE::CIdentifier(0x1C5F1356, 0x1E685777)

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CBoxAlgorithmSignalDecimation : public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_SignalDecimation);

		protected:

			OpenViBE::int64 m_i64DecimationFactor;
			OpenViBE::uint32 m_ui32ChannelCount;
			OpenViBE::uint32 m_ui32InputSampleIndex;
			OpenViBE::uint32 m_ui32InputSampleCountPerSentBlock;
			OpenViBE::uint64 m_ui64InputSamplingFrequency;
			OpenViBE::uint32 m_ui32OutputSampleIndex;
			OpenViBE::uint32 m_ui32OutputSampleCountPerSentBlock;
			OpenViBE::uint64 m_ui64OutputSamplingFrequency;

			OpenViBE::uint64 m_ui64TotalSampleCount;
			OpenViBE::uint64 m_ui64StartTimeBase;
			OpenViBE::uint64 m_ui64LastStartTime;
			OpenViBE::uint64 m_ui64LastEndTime;

			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamDecoder;
			OpenViBE::Kernel::TParameterHandler < const OpenViBE::IMemoryBuffer* > ip_pMemoryBuffer;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > op_pMatrix;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > op_ui64SamplingRate;

			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamEncoder;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > ip_ui64SamplingRate;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > ip_pMatrix;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMemoryBuffer* > op_pMemoryBuffer;
		};

		class CBoxAlgorithmSignalDecimationDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Signal Decimation"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Reduces the sampling frequency to a divider of the original sampling frequency"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("No pre filtering applied - Number of samples per block have to be a multiple of the decimation factor"); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Signal processing/Basic"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-missing-image"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_SignalDecimation; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SignalProcessing::CBoxAlgorithmSignalDecimation; }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput  ("Input signal",  OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addOutput ("Output signal", OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addSetting("Decimation factor", OV_TypeId_Integer, "8");
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_SignalDecimationDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_SignalDecimation_H__
