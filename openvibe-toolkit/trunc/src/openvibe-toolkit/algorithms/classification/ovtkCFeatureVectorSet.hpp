#ifndef __OpenViBEToolkit_CFeatureVectorSet_H__
#define __OpenViBEToolkit_CFeatureVectorSet_H__

#include "../../ovtkIFeatureVector.h"
#include "../../ovtkIFeatureVectorSet.h"

#include <map>

namespace OpenViBEToolkit
{
	class CInternalFeatureVector : public OpenViBEToolkit::IFeatureVector
	{
	public:

		CInternalFeatureVector(void);

		virtual OpenViBE::uint32 getSize(void) const;
		virtual OpenViBE::boolean setSize(const OpenViBE::uint32 ui32Size);
		virtual OpenViBE::float64* getBuffer(void);
		virtual const OpenViBE::float64* getBuffer(void) const;
		virtual const char* getElementLabel(const OpenViBE::uint32 ui32Index) const;
		virtual OpenViBE::boolean setElementLabel(const OpenViBE::uint32 ui32Index, const char* sElementLabel);

		virtual OpenViBE::float64 getLabel(void) const;
		virtual OpenViBE::boolean setLabel(const OpenViBE::float64 f64Label);

		_IsDerivedFromClass_Final_(OpenViBEToolkit::IFeatureVector, OV_UndefinedIdentifier);

	public:

		const OpenViBE::IMatrix* m_pMatrix;
		OpenViBE::uint32 m_ui32DimensionIndex;
		OpenViBE::uint32 m_ui32BufferElementCount;
		const OpenViBE::float64* m_pBuffer;
	};

	class CFeatureVectorSet : public OpenViBEToolkit::IFeatureVectorSet
	{
	public:

		CFeatureVectorSet(const OpenViBE::IMatrix& rMatrix);

		virtual OpenViBE::uint32 getFeatureVectorCount(void) const;
		virtual OpenViBE::boolean setFeatureVectorCount(const OpenViBE::uint32 ui32FeatureVectorCount);
		virtual OpenViBE::boolean addFeatureVector(const OpenViBEToolkit::IFeatureVector& rFeatureVector);
		virtual OpenViBEToolkit::IFeatureVector& getFeatureVector(const OpenViBE::uint32 ui32Index);
		virtual const OpenViBEToolkit::IFeatureVector& getFeatureVector(const OpenViBE::uint32 ui32Index) const;
		virtual OpenViBE::uint32 getLabelCount(void) const;

		_IsDerivedFromClass_Final_(OpenViBEToolkit::IFeatureVectorSet, OV_UndefinedIdentifier);

	protected:

		const OpenViBE::IMatrix& m_rMatrix;
		std::map < OpenViBE::uint32, CInternalFeatureVector > m_vFeatureVector;
	};
};

#endif // __OpenViBEToolkit_CFeatureVectorSet_H__
