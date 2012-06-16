#ifndef __OpenViBEToolkit_Reader_IBoxAlgorithmSignalInputReaderCallback_H__
#define __OpenViBEToolkit_Reader_IBoxAlgorithmSignalInputReaderCallback_H__

#include "ovtkIBoxAlgorithmStreamedMatrixInputReaderCallback.h"

namespace OpenViBEToolkit
{
	class OVTK_API IBoxAlgorithmSignalInputReaderCallback : public OpenViBEToolkit::IBoxAlgorithmStreamedMatrixInputReaderCallback
	{
	public:

		class OVTK_API ICallback
		{
		public:
			virtual void setChannelCount(const OpenViBE::uint32 ui32ChannelCount)=0;
			virtual void setChannelName(const OpenViBE::uint32 ui32ChannelIndex, const char* sChannelName)=0;
			virtual void setSampleCountPerBuffer(const OpenViBE::uint32 ui32SampleCountPerBuffer)=0;
			virtual void setSamplingRate(const OpenViBE::uint32 ui32SamplingFrequency)=0;
			virtual void setSampleBuffer(const OpenViBE::float64* pBuffer)=0;
			virtual ~ICallback(void) { }
		};

		template <class COwnerClass>
		class TCallbackProxy1 : public OpenViBEToolkit::IBoxAlgorithmSignalInputReaderCallback::ICallback
		{
		public:
			TCallbackProxy1(
				COwnerClass& rOwnerObject,
				void (COwnerClass::*mfpSetChannelCount)(const OpenViBE::uint32 ui32ChannelCount),
				void (COwnerClass::*mfpSetChannelName)(const OpenViBE::uint32 ui32ChannelIndex, const char* sChannelName),
				void (COwnerClass::*mfpSetSampleCountPerBuffer)(const OpenViBE::uint32 ui32SampleCountPerBuffer),
				void (COwnerClass::*mfpSetSamplingRate)(const OpenViBE::uint32 ui32SamplingFrequency),
				void (COwnerClass::*mfpSetSampleBuffer)(const OpenViBE::float64* pBuffer))
				:m_rOwnerObject(rOwnerObject)
				,m_mfpSetChannelCount(mfpSetChannelCount)
				,m_mfpSetChannelName(mfpSetChannelName)
				,m_mfpSetSampleCountPerBuffer(mfpSetSampleCountPerBuffer)
				,m_mfpSetSamplingRate(mfpSetSamplingRate)
				,m_mfpSetSampleBuffer(mfpSetSampleBuffer)
			{
			}
			virtual void setChannelCount(const OpenViBE::uint32 ui32ChannelCount)
			{
				if(m_mfpSetChannelCount)
				{
					(m_rOwnerObject.*m_mfpSetChannelCount)(ui32ChannelCount);
				}
			}
			virtual void setChannelName(const OpenViBE::uint32 ui32ChannelIndex, const char* sChannelName)
			{
				if(m_mfpSetChannelName)
				{
					(m_rOwnerObject.*m_mfpSetChannelName)(ui32ChannelIndex, sChannelName);
				}
			}
			virtual void setSampleCountPerBuffer(const OpenViBE::uint32 ui32SampleCountPerBuffer)
			{
				if(m_mfpSetSampleCountPerBuffer)
				{
					(m_rOwnerObject.*m_mfpSetSampleCountPerBuffer)(ui32SampleCountPerBuffer);
				}
			}
			virtual void setSamplingRate(const OpenViBE::uint32 ui32SamplingFrequency)
			{
				if(m_mfpSetSamplingRate)
				{
					(m_rOwnerObject.*m_mfpSetSamplingRate)(ui32SamplingFrequency);
				}
			}
			virtual void setSampleBuffer(const OpenViBE::float64* pBuffer)
			{
				if(m_mfpSetSampleBuffer)
				{
					(m_rOwnerObject.*m_mfpSetSampleBuffer)(pBuffer);
				}
			}
		protected:
			COwnerClass& m_rOwnerObject;
			void (COwnerClass::*m_mfpSetChannelCount)(const OpenViBE::uint32 ui32ChannelCount);
			void (COwnerClass::*m_mfpSetChannelName)(const OpenViBE::uint32 ui32ChannelIndex, const char* sChannelName);
			void (COwnerClass::*m_mfpSetSampleCountPerBuffer)(const OpenViBE::uint32 ui32SampleCountPerBuffer);
			void (COwnerClass::*m_mfpSetSamplingRate)(const OpenViBE::uint32 ui32SamplingFrequency);
			void (COwnerClass::*m_mfpSetSampleBuffer)(const OpenViBE::float64* pBuffer);
		};

		_IsDerivedFromClass_(OpenViBEToolkit::IBoxAlgorithmStreamedMatrixInputReaderCallback, OVTK_ClassId_);
	};

	OVTK_API OpenViBEToolkit::IBoxAlgorithmSignalInputReaderCallback* createBoxAlgorithmSignalInputReaderCallback(OpenViBEToolkit::IBoxAlgorithmSignalInputReaderCallback::ICallback& rCallback);
	OVTK_API void releaseBoxAlgorithmSignalInputReaderCallback(OpenViBEToolkit::IBoxAlgorithmSignalInputReaderCallback* pBoxAlgorithmSignalInputReaderCallback);
};

#endif // __OpenViBEToolkit_Reader_IBoxAlgorithmSignalInputReaderCallback_H__
