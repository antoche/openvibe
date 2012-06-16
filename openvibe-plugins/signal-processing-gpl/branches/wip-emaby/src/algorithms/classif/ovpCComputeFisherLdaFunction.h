#ifndef __OpenViBEPlugins_SignalProcessingGpl_Algorithms_Filter_CComputeFisherLdaFunction_H__
#define __OpenViBEPlugins_SignalProcessingGpl_Algorithms_Filter_CComputeFisherLdaFunction_H__

#include "../../ovp_defines.h"

#include <openvibe/ov_all.h>

#include <openvibe-toolkit/ovtk_all.h>

#include <itpp/itstat.h>
#include <itpp/itsignal.h>

namespace OpenViBEPlugins
{
	namespace SignalProcessingGpl
	{
		class CComputeFisherLdaFunction : virtual public OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);

			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >, OVP_ClassId_Algorithm_ComputeFisherLdaFunction);

		protected:

			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > ip_pMatrixFirstClass;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > ip_pMatrixSecondClass;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > op_pMatrixGlobalMean;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > op_pMatrixGlobalCovariance;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > op_pMatrixGlobalProbability;
		};

		class CComputeFisherLdaFunctionDesc : virtual public OpenViBE::Plugins::IAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Compute Fisher Lda function"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Guillaume Gibert"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INSERM/U821"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Algorithm/Classification"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_Algorithm_ComputeFisherLdaFunction; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SignalProcessingGpl::CComputeFisherLdaFunction(); }

			virtual OpenViBE::boolean getAlgorithmPrototype(
				OpenViBE::Kernel::IAlgorithmProto& rAlgorithmProto) const
			{
				rAlgorithmProto.addInputParameter (OVP_Algorithm_ComputeFisherLdaFunction_InputParameterId_MatrixFirstClass,         "Matrix first class",        OpenViBE::Kernel::ParameterType_Matrix);
				rAlgorithmProto.addInputParameter (OVP_Algorithm_ComputeFisherLdaFunction_InputParameterId_MatrixSecondClass,        "Matrix second class",       OpenViBE::Kernel::ParameterType_Matrix);
				rAlgorithmProto.addOutputParameter(OVP_Algorithm_ComputeFisherLdaFunction_OutputParameterId_MatrixGlobalMean,        "Matrix global mean",        OpenViBE::Kernel::ParameterType_Matrix);
				rAlgorithmProto.addOutputParameter(OVP_Algorithm_ComputeFisherLdaFunction_OutputParameterId_MatrixGlobalCovariance,  "Matrix global covariance",  OpenViBE::Kernel::ParameterType_Matrix);
				rAlgorithmProto.addOutputParameter(OVP_Algorithm_ComputeFisherLdaFunction_OutputParameterId_MatrixGlobalProbability, "Matrix global probability", OpenViBE::Kernel::ParameterType_Matrix);
				rAlgorithmProto.addInputTrigger   (OVP_Algorithm_ComputeFisherLdaFunction_InputTriggerId_Initialize,                 "Initialize");
				rAlgorithmProto.addInputTrigger   (OVP_Algorithm_ComputeFisherLdaFunction_InputTriggerId_ComputeFunction,            "Compute function");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IAlgorithmDesc, OVP_ClassId_Algorithm_ComputeFisherLdaFunctionDesc);
		};
	};
};

#endif // __OpenViBEPlugins_SignalProcessingGpl_Algorithms_Filter_CComputeFisherLdaFunction_H__
