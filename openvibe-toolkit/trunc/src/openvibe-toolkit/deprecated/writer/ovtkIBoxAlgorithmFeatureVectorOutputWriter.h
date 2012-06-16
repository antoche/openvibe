#ifndef __OpenViBEToolkit_Writer_IBoxAlgorithmFeatureVectorOutputWriter_H__
#define __OpenViBEToolkit_Writer_IBoxAlgorithmFeatureVectorOutputWriter_H__

#include "ovtkIBoxAlgorithmEBMLOutputWriter.h"

namespace OpenViBEToolkit
{
	class OVTK_API IBoxAlgorithmFeatureVectorOutputWriter : public OpenViBEToolkit::IBoxAlgorithmEBMLOutputWriter
	{
	public:

		virtual OpenViBE::boolean setFeatureCount(const OpenViBE::uint32 ui32FeatureCount)=0;
		virtual OpenViBE::boolean setFeatureName(const OpenViBE::uint32 ui32FeatureIndex, const char* sFeatureName)=0;
		virtual OpenViBE::boolean setFeatureVector(const OpenViBE::float64* pFeatureVector)=0;

		_IsDerivedFromClass_(OpenViBEToolkit::IBoxAlgorithmEBMLOutputWriter, OVTK_ClassId_);
	};

	extern OVTK_API OpenViBEToolkit::IBoxAlgorithmFeatureVectorOutputWriter* createBoxAlgorithmFeatureVectorOutputWriter(void);
	extern OVTK_API void releaseBoxAlgorithmFeatureVectorOutputWriter(OpenViBEToolkit::IBoxAlgorithmFeatureVectorOutputWriter* pOutputWriter);
};

#endif // __OpenViBEToolkit_Writer_IBoxAlgorithmFeatureVectorOutputWriter_H__
