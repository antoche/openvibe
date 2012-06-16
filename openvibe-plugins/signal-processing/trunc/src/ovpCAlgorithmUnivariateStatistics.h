#ifndef __OpenViBEPlugins_UnivariateStatistics_H__
#define __OpenViBEPlugins_UnivariateStatistics_H__

#include "ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include <vector>

#define OVP_ClassId_AlgoUnivariateStatistic                                             OpenViBE::CIdentifier(0x07A71212, 0x53D93D1C)
#define OVP_ClassId_AlgoUnivariateStatisticDesc                                         OpenViBE::CIdentifier(0x408157F7, 0x4F1209F7)
#define OVP_Algorithm_UnivariateStatistic_OutputParameterId_Mean                        OpenViBE::CIdentifier(0x2E1E6A87, 0x17F37568)
#define OVP_Algorithm_UnivariateStatistic_OutputParameterId_Var                         OpenViBE::CIdentifier(0x479E18C9, 0x34A561AC)
#define OVP_Algorithm_UnivariateStatistic_OutputParameterId_Range                       OpenViBE::CIdentifier(0x3CBC7D63, 0x5BF90946)
#define OVP_Algorithm_UnivariateStatistic_OutputParameterId_Med                         OpenViBE::CIdentifier(0x2B236D6C, 0x4A37734F)
#define OVP_Algorithm_UnivariateStatistic_OutputParameterId_IQR                         OpenViBE::CIdentifier(0x7A4E5C6E, 0x16EA324E)
#define OVP_Algorithm_UnivariateStatistic_OutputParameterId_Percent                     OpenViBE::CIdentifier(0x77443BEF, 0x687B139F)
#define OVP_Algorithm_UnivariateStatistic_OutputParameterId_PercentValue                OpenViBE::CIdentifier(0x2E9B5EEA, 0x58BC5AB6)
#define OVP_Algorithm_UnivariateStatistic_OutputParameterId_Compression                 OpenViBE::CIdentifier(0x2A9C502C, 0x582959DA)
#define OVP_Algorithm_UnivariateStatistic_InputParameterId_Matrix                       OpenViBE::CIdentifier(0x1769269C, 0x41910DB9)
#define OVP_Algorithm_UnivariateStatistic_InputParameterId_MeanActive                   OpenViBE::CIdentifier(0x6CE22614, 0x3BFD4A7A)
#define OVP_Algorithm_UnivariateStatistic_InputParameterId_VarActive                    OpenViBE::CIdentifier(0x304B052D, 0x04F51601)
#define OVP_Algorithm_UnivariateStatistic_InputParameterId_RangeActive                  OpenViBE::CIdentifier(0x4EA54A91, 0x69B90629)
#define OVP_Algorithm_UnivariateStatistic_InputParameterId_MedActive                    OpenViBE::CIdentifier(0x6B0F55F1, 0x30015B5B)
#define OVP_Algorithm_UnivariateStatistic_InputParameterId_IQRActive                    OpenViBE::CIdentifier(0x4F99672C, 0x7DFF3192)
#define OVP_Algorithm_UnivariateStatistic_InputParameterId_PercentActive                OpenViBE::CIdentifier(0x3CA94023, 0x44E450C6)
#define OVP_Algorithm_UnivariateStatistic_InputParameterId_PercentValue                 OpenViBE::CIdentifier(0x0CB41979, 0x1CFF5A9C)
#define OVP_Algorithm_UnivariateStatistic_InputTriggerId_SpecialInitialize              OpenViBE::CIdentifier(0x38274F8D, 0x5FB938D2)
#define OVP_Algorithm_UnivariateStatistic_InputTriggerId_Initialize                     OpenViBE::CIdentifier(0x42CC2481, 0x70300F6D)
#define OVP_Algorithm_UnivariateStatistic_InputTriggerId_Process                        OpenViBE::CIdentifier(0x6CCD1D92, 0x02043C21)
#define OVP_Algorithm_UnivariateStatistic_OutputTriggerId_ProcessDone                   OpenViBE::CIdentifier(0x34630103, 0x3F5F0A43)

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CAlgoUnivariateStatistic : public OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >, OVP_ClassId_AlgoUnivariateStatistic);

		protected:

			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > ip_pMatrix;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > op_pProcessedMatrix_Mean;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > op_pProcessedMatrix_Variance;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > op_pProcessedMatrix_Range;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > op_pProcessedMatrix_Median;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > op_pProcessedMatrix_IQR;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > op_pProcessedMatrix_Percentile;

			OpenViBE::Kernel::TParameterHandler < OpenViBE::boolean > ip_bStatisticMeanActive;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::boolean > ip_bStatisticVarianceActive;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::boolean > ip_bStatisticRangeActive;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::boolean > ip_bStatisticMedianActive;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::boolean > ip_bStatisticIQRActive;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::boolean > ip_bStatisticPercentileActive;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 >  ip_ui64PercentileValue;

			OpenViBE::Kernel::TParameterHandler < OpenViBE::float64 > op_fCompression;

			OpenViBE::boolean m_bSumActive;
			OpenViBE::CMatrix m_oSumMatrix;
			OpenViBE::boolean m_bSqaresumActive;
			OpenViBE::CMatrix m_oSumMatrix2;
			OpenViBE::boolean m_bSortActive;
			OpenViBE::CMatrix m_oSortMatrix;

			OpenViBE::uint64 m_ui64PercentileValue;

			OpenViBE::boolean setMatrixDimension(OpenViBE::IMatrix* iomat, OpenViBE::IMatrix* ref);

		};

		class CAlgoUnivariateStatisticDesc : public OpenViBE::Plugins::IAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Signal Statistic"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Matthieu Goyat"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("Gipsa-lab"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Calculate Mean, Variance, Median, etc. on the incoming buffer"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Signal processing/Statistics"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_AlgoUnivariateStatistic; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SignalProcessing::CAlgoUnivariateStatistic(); }

			virtual OpenViBE::boolean getAlgorithmPrototype(
				OpenViBE::Kernel::IAlgorithmProto& rAlgorithmProto) const
			{
				rAlgorithmProto.addInputParameter (OVP_Algorithm_UnivariateStatistic_InputParameterId_Matrix,        "Matrix input",                OpenViBE::Kernel::ParameterType_Matrix);
				rAlgorithmProto.addInputParameter (OVP_Algorithm_UnivariateStatistic_InputParameterId_MeanActive,    "active mean",                 OpenViBE::Kernel::ParameterType_Boolean);
				rAlgorithmProto.addInputParameter (OVP_Algorithm_UnivariateStatistic_InputParameterId_VarActive,     "active variance",             OpenViBE::Kernel::ParameterType_Boolean);
				rAlgorithmProto.addInputParameter (OVP_Algorithm_UnivariateStatistic_InputParameterId_RangeActive,   "active range",                OpenViBE::Kernel::ParameterType_Boolean);
				rAlgorithmProto.addInputParameter (OVP_Algorithm_UnivariateStatistic_InputParameterId_MedActive,     "active median",               OpenViBE::Kernel::ParameterType_Boolean);
				rAlgorithmProto.addInputParameter (OVP_Algorithm_UnivariateStatistic_InputParameterId_IQRActive,     "active IQR",                  OpenViBE::Kernel::ParameterType_Boolean);
				rAlgorithmProto.addInputParameter (OVP_Algorithm_UnivariateStatistic_InputParameterId_PercentActive, "active Percentile",           OpenViBE::Kernel::ParameterType_Boolean);
				rAlgorithmProto.addInputParameter (OVP_Algorithm_UnivariateStatistic_InputParameterId_PercentValue,  "Percentile Value",            OpenViBE::Kernel::ParameterType_Integer);
				rAlgorithmProto.addOutputParameter(OVP_Algorithm_UnivariateStatistic_OutputParameterId_Mean,         "Mean output",                 OpenViBE::Kernel::ParameterType_Matrix);
				rAlgorithmProto.addOutputParameter(OVP_Algorithm_UnivariateStatistic_OutputParameterId_Var,          "Variance output",             OpenViBE::Kernel::ParameterType_Matrix);
				rAlgorithmProto.addOutputParameter(OVP_Algorithm_UnivariateStatistic_OutputParameterId_Range,        "Range output",                OpenViBE::Kernel::ParameterType_Matrix);
				rAlgorithmProto.addOutputParameter(OVP_Algorithm_UnivariateStatistic_OutputParameterId_Med,          "Median output",               OpenViBE::Kernel::ParameterType_Matrix);
				rAlgorithmProto.addOutputParameter(OVP_Algorithm_UnivariateStatistic_OutputParameterId_IQR,          "Inter-Quantile-Range output", OpenViBE::Kernel::ParameterType_Matrix);
				rAlgorithmProto.addOutputParameter(OVP_Algorithm_UnivariateStatistic_OutputParameterId_Percent,      "Percentile output",           OpenViBE::Kernel::ParameterType_Matrix);
				rAlgorithmProto.addOutputParameter(OVP_Algorithm_UnivariateStatistic_OutputParameterId_Compression,  "compression ratio",           OpenViBE::Kernel::ParameterType_Float);

				rAlgorithmProto.addInputTrigger   (OVP_Algorithm_UnivariateStatistic_InputTriggerId_Initialize,   "Initialize");
				rAlgorithmProto.addInputTrigger   (OVP_Algorithm_UnivariateStatistic_InputTriggerId_Process,      "Process");
				rAlgorithmProto.addOutputTrigger  (OVP_Algorithm_UnivariateStatistic_OutputTriggerId_ProcessDone, "Done");
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IAlgorithmDesc, OVP_ClassId_AlgoUnivariateStatisticDesc);
		};
	};
};

#endif // __OpenViBEPlugins_SignalProcessing_Algorithms_Basic_CSignalStatistic_H__
