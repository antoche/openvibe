#ifndef __OpenViBEPlugins_SignalProcessing_Algorithms_Basic_CMatrixAverage_H__
#define __OpenViBEPlugins_SignalProcessing_Algorithms_Basic_CMatrixAverage_H__

#include "../../ovp_defines.h"

#include <openvibe/ov_all.h>

#include <openvibe-toolkit/ovtk_all.h>

#include <deque>

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CMatrixAverage : public OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);

			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >, OVP_ClassId_Algorithm_MatrixAverage);

		protected:

			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > ip_ui64AveragingMethod;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > ip_ui64MatrixCount;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > ip_pMatrix;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > op_pAveragedMatrix;

			std::deque < OpenViBE::CMatrix* > m_vHistory;
		};

		class CMatrixAverageDesc : public OpenViBE::Plugins::IAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Matrix average"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Signal processing/Averaging"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_Algorithm_MatrixAverage; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SignalProcessing::CMatrixAverage(); }

			virtual OpenViBE::boolean getAlgorithmPrototype(
				OpenViBE::Kernel::IAlgorithmProto& rAlgorithmProto) const
			{
				rAlgorithmProto.addInputParameter (OVP_Algorithm_MatrixAverage_InputParameterId_Matrix,                    "Matrix",              OpenViBE::Kernel::ParameterType_Matrix);
				rAlgorithmProto.addInputParameter (OVP_Algorithm_MatrixAverage_InputParameterId_MatrixCount,               "Matrix count",        OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmProto.addInputParameter (OVP_Algorithm_MatrixAverage_InputParameterId_AveragingMethod,           "Averaging Method",    OpenViBE::Kernel::ParameterType_UInteger);

				rAlgorithmProto.addOutputParameter(OVP_Algorithm_MatrixAverage_OutputParameterId_AveragedMatrix,           "Averaged matrix",     OpenViBE::Kernel::ParameterType_Matrix);

				rAlgorithmProto.addInputTrigger   (OVP_Algorithm_MatrixAverage_InputTriggerId_Reset,                       "Reset");
				rAlgorithmProto.addInputTrigger   (OVP_Algorithm_MatrixAverage_InputTriggerId_FeedMatrix,                  "Feed matrix");
				rAlgorithmProto.addInputTrigger   (OVP_Algorithm_MatrixAverage_InputTriggerId_ForceAverage,                "Force average");

				rAlgorithmProto.addOutputTrigger  (OVP_Algorithm_MatrixAverage_OutputTriggerId_AveragePerformed,           "Average performed");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IAlgorithmDesc, OVP_ClassId_Algorithm_MatrixAverageDesc);
		};
	};
};

#endif // __OpenViBEPlugins_SignalProcessing_Algorithms_Basic_CMatrixAverage_H__
