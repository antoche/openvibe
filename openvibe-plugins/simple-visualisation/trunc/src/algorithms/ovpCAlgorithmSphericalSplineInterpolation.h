#ifndef __OpenViBEPlugins_Algorithm_SphericalSplineInterpolation_H__
#define __OpenViBEPlugins_Algorithm_SphericalSplineInterpolation_H__

#include "../ovp_defines.h"

#include <openvibe/ov_all.h>

#include <openvibe-toolkit/ovtk_all.h>

namespace OpenViBEPlugins
{
	namespace Test
	{
		class CAlgorithmSphericalSplineInterpolation : public OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >, OVP_ClassId_Algorithm_SphericalSplineInterpolation);

		protected:

			//input parameters
			//----------------
			OpenViBE::Kernel::TParameterHandler < OpenViBE::int64 > ip_i64SplineOrder;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::int64 > ip_i64ControlPointsCount;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > ip_pControlPointsCoords;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > ip_pControlPointsValues;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > ip_pSamplePointsCoords;

			//output parameters
			//-----------------
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > op_pSamplePointsValues;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::float64 > op_f64MinSamplePointValue;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::float64 > op_f64MaxSamplePointValue;

			//internal data
			//-------------
			OpenViBE::boolean m_bFirstProcess;
			double* m_pDoubleCoords;
			double** m_pInsermCoords;
			double m_ScdTable[2004];
			double m_PotTable[2004];
			double* m_pSplineCoefs;
			double* m_pLaplacianSplineCoefs;
		};

		class CAlgorithmSphericalSplineInterpolationDesc : public OpenViBE::Plugins::IAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Spherical spline interpolation"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Vincent Delannoy"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Interpolates potentials/laplacians using a spherical spline"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Algorithm/Signal processing"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_Algorithm_SphericalSplineInterpolation; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::Test::CAlgorithmSphericalSplineInterpolation(); }

			virtual OpenViBE::boolean getAlgorithmPrototype(
				OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				//input parameters
				rAlgorithmPrototype.addInputParameter (OVP_Algorithm_SphericalSplineInterpolation_InputParameterId_SplineOrder, "Spline order",  OpenViBE::Kernel::ParameterType_Integer);
				rAlgorithmPrototype.addInputParameter (OVP_Algorithm_SphericalSplineInterpolation_InputParameterId_ControlPointsCount, "Number of values",  OpenViBE::Kernel::ParameterType_Integer);
				rAlgorithmPrototype.addInputParameter (OVP_Algorithm_SphericalSplineInterpolation_InputParameterId_ControlPointsCoordinates, "Values coordinates",  OpenViBE::Kernel::ParameterType_Matrix);
				rAlgorithmPrototype.addInputParameter (OVP_Algorithm_SphericalSplineInterpolation_InputParameterId_ControlPointsValues, "Values",  OpenViBE::Kernel::ParameterType_Matrix);
				rAlgorithmPrototype.addInputParameter (OVP_Algorithm_SphericalSplineInterpolation_InputParameterId_SamplePointsCoordinates,"Coordinates where to interpolate values", OpenViBE::Kernel::ParameterType_Matrix);
				//input triggers
				rAlgorithmPrototype.addInputTrigger   (OVP_Algorithm_SphericalSplineInterpolation_InputTriggerId_PrecomputeTables, OpenViBE::CString("Precomputation"));
				rAlgorithmPrototype.addInputTrigger   (OVP_Algorithm_SphericalSplineInterpolation_InputTriggerId_ComputeSplineCoefs, OpenViBE::CString("Spline coefficients computation"));
				rAlgorithmPrototype.addInputTrigger   (OVP_Algorithm_SphericalSplineInterpolation_InputTriggerId_ComputeLaplacianCoefs, OpenViBE::CString("Laplacian coefficients computation"));
				rAlgorithmPrototype.addInputTrigger   (OVP_Algorithm_SphericalSplineInterpolation_InputTriggerId_InterpolateSpline, OpenViBE::CString("Interpolation using spline coefficients"));
				rAlgorithmPrototype.addInputTrigger   (OVP_Algorithm_SphericalSplineInterpolation_InputTriggerId_InterpolateLaplacian, OpenViBE::CString("Interpolation using laplacian coefficients"));
				//output parameters
				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_SphericalSplineInterpolation_OutputParameterId_SamplePointsValues, "Interpolated values", OpenViBE::Kernel::ParameterType_Matrix);
				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_SphericalSplineInterpolation_OutputParameterId_MinSamplePointValue, "Min interpolated value", OpenViBE::Kernel::ParameterType_Float);
				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_SphericalSplineInterpolation_OutputParameterId_MaxSamplePointValue, "Max interpolated value", OpenViBE::Kernel::ParameterType_Float);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IAlgorithmDesc, OVP_ClassId_Algorithm_SphericalSplineInterpolationDesc);
		};
	};
};

#endif // __OpenViBEPlugins_Algorithm_SphericalSplineInterpolation_H__
