#ifndef __OpenViBEToolkit_CFeatureVector_H__
#define __OpenViBEToolkit_CFeatureVector_H__

#include "ovtkCVector.hpp"
#include "../../ovtkIFeatureVector.h"

namespace OpenViBEToolkit
{
	template <class CParent>
	class TFeatureVector : public OpenViBEToolkit::TVector < CParent >
	{
	public:

		TFeatureVector(OpenViBE::IMatrix& rMatrix)
			:OpenViBEToolkit::TVector < CParent >(rMatrix)
		{
		}

		virtual OpenViBE::float64 getLabel(void) const
		{
			return 0;
		}

		virtual OpenViBE::boolean setLabel(const OpenViBE::float64 f64Label)
		{
			return false;
		}

		_IsDerivedFromClass_Final_(OpenViBEToolkit::TVector < CParent >, OV_UndefinedIdentifier);
	};

	typedef OpenViBEToolkit::TFeatureVector < OpenViBEToolkit::IFeatureVector > CFeatureVector;
};

#endif // __OpenViBEToolkit_CFeatureVector_H__
