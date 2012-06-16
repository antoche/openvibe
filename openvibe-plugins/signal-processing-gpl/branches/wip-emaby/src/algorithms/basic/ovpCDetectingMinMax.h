#ifndef __OpenViBEPlugins_SignalProcessingGpl_Algorithms_Basic_CDetectingMinMax_H__
#define __OpenViBEPlugins_SignalProcessingGpl_Algorithms_Basic_CDetectingMinMax_H__

#include "../../ovp_defines.h"

#include <openvibe/ov_all.h>

#include <openvibe-toolkit/ovtk_all.h>

namespace OpenViBEPlugins
{
	namespace SignalProcessingGpl
	{
		class CDetectingMinMax : virtual public OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);

			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >, OVP_ClassId_Algorithm_DetectingMinMax);

		protected:

			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > ip_pSignalMatrix;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > ip_ui64SamplingFrequency;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::float64 > ip_f64TimeWindowStart;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::float64 > ip_f64TimeWindowEnd;

			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > op_pSignalMatrix;

		};

		class CDetectingMinMaxDesc : virtual public OpenViBE::Plugins::IAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Detects Min or Max of input buffer"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Guillaume Gibert"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INSERM/U821"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Algorithm/Signal processing/Basic"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_Algorithm_DetectingMinMax; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SignalProcessingGpl::CDetectingMinMax(); }

			virtual OpenViBE::boolean getAlgorithmPrototype(
				OpenViBE::Kernel::IAlgorithmProto& rAlgorithmProto) const
			{
				rAlgorithmProto.addInputParameter (OVP_Algorithm_DetectingMinMax_InputParameterId_SignalMatrix,     "Signal input matrix",  OpenViBE::Kernel::ParameterType_Matrix);
				rAlgorithmProto.addInputParameter (OVP_Algorithm_DetectingMinMax_InputParameterId_TimeWindowStart,  "Time window start",    OpenViBE::Kernel::ParameterType_Float);
				rAlgorithmProto.addInputParameter (OVP_Algorithm_DetectingMinMax_InputParameterId_TimeWindowEnd,    "Time window end",      OpenViBE::Kernel::ParameterType_Float);
				rAlgorithmProto.addInputParameter (OVP_Algorithm_DetectingMinMax_InputParameterId_SamplingFrequency,"Sampling frequency",   OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmProto.addOutputParameter(OVP_Algorithm_DetectingMinMax_OutputParameterId_SignalMatrix,    "Signal output matrix", OpenViBE::Kernel::ParameterType_Matrix);
				rAlgorithmProto.addInputTrigger   (OVP_Algorithm_DetectingMinMax_InputTriggerId_Initialize,         "Initialize");
				rAlgorithmProto.addInputTrigger   (OVP_Algorithm_DetectingMinMax_InputTriggerId_DetectsMin,         "Detects min");
				rAlgorithmProto.addInputTrigger   (OVP_Algorithm_DetectingMinMax_InputTriggerId_DetectsMax,         "Detects max");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IAlgorithmDesc, OVP_ClassId_Algorithm_DetectingMinMaxDesc);
		};
	};
};

#endif // __OpenViBEPlugins_SignalProcessingGpl_Algorithms_Basic_CDetectingMinMax_H__
