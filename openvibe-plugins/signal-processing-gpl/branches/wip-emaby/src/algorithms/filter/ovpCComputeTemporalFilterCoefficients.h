#ifndef __OpenViBEPlugins_SignalProcessingGpl_Algorithms_Filter_CComputeTemporalFilterCoefficients_H__
#define __OpenViBEPlugins_SignalProcessingGpl_Algorithms_Filter_CComputeTemporalFilterCoefficients_H__

#include "../../ovp_defines.h"

#include <openvibe/ov_all.h>

#include <openvibe-toolkit/ovtk_all.h>

#include <itpp/itstat.h>
#include <itpp/itsignal.h>

#define PREC 27
#define MAXEXP 1024
#define MINEXP -1077
#define MAXNUM 1.79769313486231570815E308

typedef struct
{
	double real;
	double imag;
}cmplex;

namespace OpenViBEPlugins
{
	namespace SignalProcessingGpl
	{
		class CComputeTemporalFilterCoefficients : virtual public OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);

			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >, OVP_ClassId_Algorithm_ComputeTemporalFilterCoefficients);

		public:

			// Functions for Butterworth and Chebychev filters
			void findSPlanePolesAndZeros(void);
			void convertSPlanePolesAndZerosToZPlane(void);
			// Functions for Complex arithmetic
			double absComplex(cmplex *z);
			void divComplex(cmplex *a, cmplex *b, cmplex *c);
			void sqrtComplex(cmplex *z, cmplex  *w);
			void addComplex(cmplex *a, cmplex *b,cmplex *c);
			void mulComplex(cmplex *a, cmplex *b,cmplex *c);
			void subComplex(cmplex *a, cmplex *b,cmplex *c);

		protected:

			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > ip_ui64SamplingFrequency;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > ip_ui64FilterMethod;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > ip_ui64FilterType;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > ip_ui64FilterOrder;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::float64 > ip_f64LowCutFrequency;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::float64 > ip_f64HighCutFrequency;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::float64 > ip_f64BandPassRipple;

			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > op_pOutputMatrix;

			OpenViBE::uint32 m_ui32FilterOrder;
			OpenViBE::uint64 m_ui64FilterMethod;
			OpenViBE::uint64 m_ui64FilterType;
			OpenViBE::float64 m_float64LowPassBandEdge;
			OpenViBE::float64 m_float64HighPassBandEdge;
			OpenViBE::float64 m_float64PassBandRipple;
			OpenViBE::uint32 m_ui32ArraySize;
			itpp::vec m_vecNumCoefFilter;
			itpp::vec m_vecDenomCoefFilter;
			OpenViBE::float64 m_float64Phi;
			OpenViBE::float64 m_float64Scale;
			OpenViBE::float64 m_float64TanAng;
			OpenViBE::float64 m_float64CosGam;
			OpenViBE::float64 m_float64cbp;

			OpenViBE::uint32 m_ui32SamplingRate;
			OpenViBE::uint32 m_ui32NyquistFrequency;

			OpenViBE::uint32  m_ui32NbPoles;
			OpenViBE::uint32  m_ui32NbZeros;

			itpp::vec m_vecZs;
			OpenViBE::uint32  m_ui32zord;

			OpenViBE::float64 m_float64Rho;
			OpenViBE::float64 m_float64Eps;

			OpenViBE::uint32  m_ui32DimensionSize;
		};

		class CComputeTemporalFilterCoefficientsDesc : virtual public OpenViBE::Plugins::IAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Compute Filter Coefficients"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Guillaume Gibert"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INSERM/U821"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Algorithm/Signal processing/Filter"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_Algorithm_ComputeTemporalFilterCoefficients; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SignalProcessingGpl::CComputeTemporalFilterCoefficients(); }

			virtual OpenViBE::boolean getAlgorithmPrototype(
				OpenViBE::Kernel::IAlgorithmProto& rAlgorithmProto) const
			{
				rAlgorithmProto.addInputParameter (OVP_Algorithm_ComputeTemporalFilterCoefficients_InputParameterId_SamplingFrequency, "Sampling frequency",         OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmProto.addInputParameter (OVP_Algorithm_ComputeTemporalFilterCoefficients_InputParameterId_FilterMethod,      "Filter method",              OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmProto.addInputParameter (OVP_Algorithm_ComputeTemporalFilterCoefficients_InputParameterId_FilterType,        "Filter type",                OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmProto.addInputParameter (OVP_Algorithm_ComputeTemporalFilterCoefficients_InputParameterId_FilterOrder,       "Filter order",               OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmProto.addInputParameter (OVP_Algorithm_ComputeTemporalFilterCoefficients_InputParameterId_LowCutFrequency,   "Low cut frequency",          OpenViBE::Kernel::ParameterType_Float);
				rAlgorithmProto.addInputParameter (OVP_Algorithm_ComputeTemporalFilterCoefficients_InputParameterId_HighCutFrequency,  "High cut frequency",         OpenViBE::Kernel::ParameterType_Float);
				rAlgorithmProto.addInputParameter (OVP_Algorithm_ComputeTemporalFilterCoefficients_InputParameterId_BandPassRipple,    "Band pass ripple",           OpenViBE::Kernel::ParameterType_Float);
				rAlgorithmProto.addOutputParameter(OVP_Algorithm_ComputeTemporalFilterCoefficients_OutputParameterId_Matrix,           "Matrix",                     OpenViBE::Kernel::ParameterType_Matrix);
				rAlgorithmProto.addInputTrigger(OVP_Algorithm_ComputeTemporalFilterCoefficients_InputTriggerId_Initialize,             "Initialize");
				rAlgorithmProto.addInputTrigger(OVP_Algorithm_ComputeTemporalFilterCoefficients_InputTriggerId_ComputeCoefficients,    "Compute coefficients");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IAlgorithmDesc, OVP_ClassId_Algorithm_ComputeTemporalFilterCoefficientsDesc);
		};
	};
};

#endif // __OpenViBEPlugins_SignalProcessingGpl_Algorithms_Filter_CComputeTemporalFilterCoefficients_H__
