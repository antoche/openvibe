#ifndef __OpenViBEToolkit_IFeatureVector_H__
#define __OpenViBEToolkit_IFeatureVector_H__

#include "ovtkIVector.h"

namespace OpenViBEToolkit
{
	class OVTK_API IFeatureVector : public OpenViBEToolkit::IVector
	{
	public:

		virtual OpenViBE::float64 getLabel(void) const=0;
		virtual OpenViBE::boolean setLabel(const OpenViBE::float64 f64Label)=0;

		_IsDerivedFromClass_(OpenViBEToolkit::IObject, OVTK_ClassId_FeatureVector);
	};
};

#endif // __OpenViBEToolkit_IFeatureVector_H__
