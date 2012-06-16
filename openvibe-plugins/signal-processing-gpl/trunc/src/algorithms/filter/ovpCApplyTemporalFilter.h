#ifndef __OpenViBEPlugins_SignalProcessingGpl_Algorithms_Filter_CApplyTemporalFilter_H__
#define __OpenViBEPlugins_SignalProcessingGpl_Algorithms_Filter_CApplyTemporalFilter_H__

#include "../../ovp_defines.h"

#include <openvibe/ov_all.h>

#include <openvibe-toolkit/ovtk_all.h>

#include <itpp/itstat.h>
#include <itpp/itsignal.h>

namespace OpenViBEPlugins
{
	namespace SignalProcessingGpl
	{
		class CApplyTemporalFilter : virtual public OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);

			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >, OVP_ClassId_Algorithm_ApplyTemporalFilter);

		protected:

			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > ip_pSignalMatrix;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > ip_pFilterCoefficientsMatrix;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > op_pSignalMatrix;

		protected:

			itpp::vec m_vecDenomCoefFilter;
			itpp::vec m_vecNumCoefFilter;
			std::vector<itpp::vec> m_oCurrentStates;

			OpenViBE::boolean m_bFlagInitialize;
		};

		class CApplyTemporalFilterDesc : virtual public OpenViBE::Plugins::IAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Apply Temporal Filter"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Guillaume Gibert"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INSERM/U821"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Algorithm/Signal processing/Filter"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_Algorithm_ApplyTemporalFilter; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SignalProcessingGpl::CApplyTemporalFilter(); }

			virtual OpenViBE::boolean getAlgorithmPrototype(
				OpenViBE::Kernel::IAlgorithmProto& rAlgorithmProto) const
			{
				rAlgorithmProto.addInputParameter (OVP_Algorithm_ApplyTemporalFilter_InputParameterId_SignalMatrix,             "Signal matrix",              OpenViBE::Kernel::ParameterType_Matrix);
				rAlgorithmProto.addInputParameter (OVP_Algorithm_ApplyTemporalFilter_InputParameterId_FilterCoefficientsMatrix, "Filter coefficients matrix", OpenViBE::Kernel::ParameterType_Matrix);
				rAlgorithmProto.addOutputParameter(OVP_Algorithm_ApplyTemporalFilter_OutputParameterId_FilteredSignalMatrix,    "Filtered signal matrix",     OpenViBE::Kernel::ParameterType_Matrix);
				rAlgorithmProto.addInputTrigger   (OVP_Algorithm_ApplyTemporalFilter_InputTriggerId_Initialize,                 "Initialize");
				rAlgorithmProto.addInputTrigger   (OVP_Algorithm_ApplyTemporalFilter_InputTriggerId_ApplyFilter,                "Apply filter");
				rAlgorithmProto.addInputTrigger   (OVP_Algorithm_ApplyTemporalFilter_InputTriggerId_ApplyFilterWithHistoric,    "Apply filter with historic");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IAlgorithmDesc, OVP_ClassId_Algorithm_ApplyTemporalFilterDesc);
		};
	};
};

#endif // __OpenViBEPlugins_SignalProcessingGpl_Algorithms_Filter_CApplyTemporalFilter_H__
