#ifndef __OpenViBEToolkit_IFeatureVectorSet_H__
#define __OpenViBEToolkit_IFeatureVectorSet_H__

#include "ovtkIObject.h"

namespace OpenViBEToolkit
{
	class IFeatureVector;

	class OVTK_API IFeatureVectorSet : public OpenViBEToolkit::IObject
	{
	public:

		virtual OpenViBE::uint32 getFeatureVectorCount(void) const=0;
		virtual OpenViBE::boolean setFeatureVectorCount(const OpenViBE::uint32 ui32FeatureVectorCount)=0;
		virtual OpenViBE::boolean addFeatureVector(const OpenViBEToolkit::IFeatureVector& rFeatureVector)=0;

		virtual OpenViBEToolkit::IFeatureVector& getFeatureVector(const OpenViBE::uint32 ui32Index)=0;
		virtual const OpenViBEToolkit::IFeatureVector& getFeatureVector(const OpenViBE::uint32 ui32Index) const=0;
		virtual OpenViBE::uint32 getLabelCount(void) const=0;

		_IsDerivedFromClass_(OpenViBEToolkit::IObject, OVTK_ClassId_FeatureVectorSet);

		const OpenViBEToolkit::IFeatureVector& operator [] (const OpenViBE::uint32 ui32Index) const
		{
			return this->getFeatureVector(ui32Index);
		}

		OpenViBEToolkit::IFeatureVector& operator [] (const OpenViBE::uint32 ui32Index)
		{
			return this->getFeatureVector(ui32Index);
		}
	};
};

#endif // __OpenViBEToolkit_IFeatureVectorSet_H__
