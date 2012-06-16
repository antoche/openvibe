#ifndef __OpenViBEToolkit_Reader_IBoxAlgorithmFeatureVectorInputReaderCallback_H__
#define __OpenViBEToolkit_Reader_IBoxAlgorithmFeatureVectorInputReaderCallback_H__

#include "ovtkIBoxAlgorithmStreamedMatrixInputReaderCallback.h"

namespace OpenViBEToolkit
{
	class OVTK_API IBoxAlgorithmFeatureVectorInputReaderCallback : public OpenViBEToolkit::IBoxAlgorithmStreamedMatrixInputReaderCallback
	{
	public:

		class OVTK_API ICallback
		{
		public:
			virtual void setFeatureCount(const OpenViBE::uint32 ui32FeatureCount)=0;
			virtual void setFeatureName(const OpenViBE::uint32 ui32FeatureIndex, const char* sFeatureName)=0;
			virtual void setFeatureVector(const OpenViBE::float64* pFeatureBuffer)=0;
			virtual ~ICallback(void) { }
		};

		template <class COwnerClass>
		class TCallbackProxy1 : public OpenViBEToolkit::IBoxAlgorithmFeatureVectorInputReaderCallback::ICallback
		{
		public:
			TCallbackProxy1(
				COwnerClass& rOwnerObject,
				void (COwnerClass::*mfpSetFeatureCount)(const OpenViBE::uint32 ui32FeatureCount),
				void (COwnerClass::*mfpSetFeatureName)(const OpenViBE::uint32 ui32FeatureIndex, const char* sFeatureName),
				void (COwnerClass::*mfpSetFeatureVector)(const OpenViBE::float64* pFeatureBuffer))
				:m_rOwnerObject(rOwnerObject)
				,m_mfpSetFeatureCount(mfpSetFeatureCount)
				,m_mfpSetFeatureName(mfpSetFeatureName)
				,m_mfpSetFeatureVector(mfpSetFeatureVector)
			{
			}
			virtual void setFeatureCount(const OpenViBE::uint32 ui32FeatureCount)
			{
				if(m_mfpSetFeatureCount)
				{
					(m_rOwnerObject.*m_mfpSetFeatureCount)(ui32FeatureCount);
				}
			}
			virtual void setFeatureName(const OpenViBE::uint32 ui32FeatureIndex, const char* sFeatureName)
			{
				if(m_mfpSetFeatureName)
				{
					(m_rOwnerObject.*m_mfpSetFeatureName)(ui32FeatureIndex, sFeatureName);
				}
			}
			virtual void setFeatureVector(const OpenViBE::float64* pFeatureBuffer)
			{
				if(m_mfpSetFeatureVector)
				{
					(m_rOwnerObject.*m_mfpSetFeatureVector)(pFeatureBuffer);
				}
			}
		protected:
			COwnerClass& m_rOwnerObject;
			void (COwnerClass::*m_mfpSetFeatureCount)(const OpenViBE::uint32 ui32FeatureCount);
			void (COwnerClass::*m_mfpSetFeatureName)(const OpenViBE::uint32 ui32FeatureIndex, const char* sFeatureName);
			void (COwnerClass::*m_mfpSetFeatureVector)(const OpenViBE::float64* pFeatureBuffer);
		};

		_IsDerivedFromClass_(OpenViBEToolkit::IBoxAlgorithmStreamedMatrixInputReaderCallback, OVTK_ClassId_);
	};

	OVTK_API OpenViBEToolkit::IBoxAlgorithmFeatureVectorInputReaderCallback* createBoxAlgorithmFeatureVectorInputReaderCallback(OpenViBEToolkit::IBoxAlgorithmFeatureVectorInputReaderCallback::ICallback& rCallback);
	OVTK_API void releaseBoxAlgorithmFeatureVectorInputReaderCallback(OpenViBEToolkit::IBoxAlgorithmFeatureVectorInputReaderCallback* pBoxAlgorithmFeatureVectorInputReaderCallback);
};

#endif // __OpenViBEToolkit_Reader_IBoxAlgorithmFeatureVectorInputReaderCallback_H__
