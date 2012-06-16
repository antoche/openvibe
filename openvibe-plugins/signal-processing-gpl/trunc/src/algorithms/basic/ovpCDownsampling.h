#ifndef __OpenViBEPlugins_SignalProcessingGpl_Algorithms_Basic_CDownsampling_H__
#define __OpenViBEPlugins_SignalProcessingGpl_Algorithms_Basic_CDownsampling_H__

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

namespace OpenViBEPlugins
{
	namespace SignalProcessingGpl
	{
		class CDownsampling : virtual public OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }
			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >, OVP_ClassId_Algorithm_Downsampling);

		protected:

			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > ip_ui64SamplingFrequency;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > ip_ui64NewSamplingFrequency;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > ip_pSignalMatrix;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > op_pSignalMatrix;
			OpenViBE::float64 *m_f64LastValueOrigSignal;
			OpenViBE::float64 m_f64LastTimeOrigSignal;
			OpenViBE::float64 m_f64LastTimeNewSignal;
			OpenViBE::boolean m_bFirst;
		};

		class CDownsamplingDesc : virtual public OpenViBE::Plugins::IAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Downsampling"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("G. Gibert - E. Maby - P.E. Aguera"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INSERM/U821"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Downsamples input signal."); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("Downsamples input signal respect of the new sampling rate choosen by user."); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Signal processing Gpl/Basic"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.1"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_Algorithm_Downsampling; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SignalProcessingGpl::CDownsampling(); }

			virtual OpenViBE::boolean getAlgorithmPrototype(
				OpenViBE::Kernel::IAlgorithmProto& rAlgorithmProto) const
			{
				rAlgorithmProto.addInputParameter (OVP_Algorithm_Downsampling_InputParameterId_SamplingFrequency,    "Sampling frequency",     OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmProto.addInputParameter (OVP_Algorithm_Downsampling_InputParameterId_NewSamplingFrequency, "New sampling frequency", OpenViBE::Kernel::ParameterType_UInteger);
				
				rAlgorithmProto.addInputParameter (OVP_Algorithm_Downsampling_InputParameterId_SignalMatrix,         "Signal matrix",          OpenViBE::Kernel::ParameterType_Matrix);
				rAlgorithmProto.addOutputParameter(OVP_Algorithm_Downsampling_OutputParameterId_SignalMatrix,        "Signal matrix",          OpenViBE::Kernel::ParameterType_Matrix);
				rAlgorithmProto.addInputTrigger   (OVP_Algorithm_Downsampling_InputTriggerId_Initialize,             "Initialize");
				rAlgorithmProto.addInputTrigger   (OVP_Algorithm_Downsampling_InputTriggerId_Resample,               "Resample");
				rAlgorithmProto.addInputTrigger   (OVP_Algorithm_Downsampling_InputTriggerId_ResampleWithHistoric, "Resample with historic");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IAlgorithmDesc, OVP_ClassId_Algorithm_DownsamplingDesc);
		};
	};
};

#endif // __OpenViBEPlugins_SignalProcessingGpl_Algorithms_Basic_CDownsampling_H__
