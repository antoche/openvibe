#ifndef __OpenViBEPlugins_SignalProcessing_BoxAlgorithms_CUnivariateStatistic_H__
#define __OpenViBEPlugins_SignalProcessing_BoxAlgorithms_CUnivariateStatistic_H__

#include "ovp_defines.h"

#include <openvibe-toolkit/ovtk_all.h>

#define OVP_ClassId_BoxAlgorithm_UnivariateStatistic     OpenViBE::CIdentifier(0x6118159D, 0x600C40B9)
#define OVP_ClassId_BoxAlgorithm_UnivariateStatisticDesc OpenViBE::CIdentifier(0x36F742D9, 0x6D1477B2)

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CBoxUnivariateStatistic : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);

			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_UnivariateStatistic)

		protected:

			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamDecoder;
			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamEncoderMean;
			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamEncoderVariance;
			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamEncoderRange;
			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamEncoderMedian;
			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamEncoderIQR;
			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamEncoderPercentile;
			OpenViBE::Kernel::IAlgorithmProxy* m_pMatrixStatistic;

			OpenViBE::Kernel::TParameterHandler < OpenViBE::float64 > op_fCompression;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > op_ui64SamplingRate;

			OpenViBE::Kernel::TParameterHandler < OpenViBE::boolean > ip_bStatisticMeanActive;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::boolean > ip_bStatisticVarianceActive;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::boolean > ip_bStatisticRangeActive;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::boolean > ip_bStatisticMedianActive;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::boolean > ip_bStatisticIQRActive;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::boolean > ip_bStatisticPercentileActive;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > ip_ui64StatisticParameterValue;

			OpenViBE::CIdentifier m_oInputTypeIdentifier;

		};

		class CBoxUnivariateStatisticDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }
			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Univariate Statistics"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Matthieu Goyat"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("Gipsa-lab"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Mean, Variance, Median, etc. on the incoming Signal"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Signal processing/Statistics"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-missing-image"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_UnivariateStatistic; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SignalProcessing::CBoxUnivariateStatistic(); }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rPrototype) const
			{
				rPrototype.addInput  ("Input Signals",    OV_TypeId_Signal);
				rPrototype.addOutput ("Mean",             OV_TypeId_Signal);
				rPrototype.addOutput ("Variance",         OV_TypeId_Signal);
				rPrototype.addOutput ("Range",            OV_TypeId_Signal);
				rPrototype.addOutput ("Median",           OV_TypeId_Signal);
				rPrototype.addOutput ("IQR",              OV_TypeId_Signal);
				rPrototype.addOutput ("Percentile",       OV_TypeId_Signal);
				rPrototype.addSetting("Mean",             OV_TypeId_Boolean, "true");
				rPrototype.addSetting("Variance",         OV_TypeId_Boolean, "true");
				rPrototype.addSetting("Range",            OV_TypeId_Boolean, "true");
				rPrototype.addSetting("Median",           OV_TypeId_Boolean, "true");
				rPrototype.addSetting("IQR",              OV_TypeId_Boolean, "true");
				rPrototype.addSetting("Percentile",       OV_TypeId_Boolean, "true");
				rPrototype.addSetting("Percentile value", OV_TypeId_Float,   "30");
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_UnivariateStatisticDesc)
		};
	};
};

#endif // __OpenViBEPlugins_SignalProcessing_BoxAlgorithms_CUnivariateStatistic_H__
