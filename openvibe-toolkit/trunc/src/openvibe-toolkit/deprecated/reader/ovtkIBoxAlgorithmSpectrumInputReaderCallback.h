#ifndef __OpenViBEToolkit_Reader_IBoxAlgorithmSpectrumInputReaderCallback_H__
#define __OpenViBEToolkit_Reader_IBoxAlgorithmSpectrumInputReaderCallback_H__

#include "ovtkIBoxAlgorithmEBMLInputReaderCallback.h"

namespace OpenViBEToolkit
{
	class OVTK_API IBoxAlgorithmSpectrumInputReaderCallback : public OpenViBEToolkit::IBoxAlgorithmEBMLInputReaderCallback
	{
	public:

		class OVTK_API ICallback
		{
		public:
			virtual void setChannelCount(const OpenViBE::uint32 ui32ChannelCount)=0;
			virtual void setChannelName(const OpenViBE::uint32 ui32ChannelIndex, const char* sChannelName)=0;
			virtual void setFrequencyBandCount(const OpenViBE::uint32 ui32FrequencyBandCount)=0;
			virtual void setFrequencyBandName(const OpenViBE::uint32 ui32FrequencyBandIndex, const char* sFrequencyBandName)=0;
			virtual void setFrequencyBandStart(const OpenViBE::uint32 ui32FrequencyBandIndex, const OpenViBE::float64 f64FrequencyBandStart)=0;
			virtual void setFrequencyBandStop(const OpenViBE::uint32 ui32FrequencyBandIndex, const OpenViBE::float64 f64FrequencyBandStop)=0;
			virtual void setBuffer(const OpenViBE::float64* pBuffer)=0;
			virtual ~ICallback(void) { }
		};

		template <class COwnerClass>
		class TCallbackProxy1 : public OpenViBEToolkit::IBoxAlgorithmSpectrumInputReaderCallback::ICallback
		{
		public:
			TCallbackProxy1(
				COwnerClass& rOwnerObject,
				void (COwnerClass::*mfpSetChannelCount)(const OpenViBE::uint32 ui32ChannelCount),
				void (COwnerClass::*mfpSetChannelName)(const OpenViBE::uint32 ui32ChannelIndex, const char* sChannelName),
				void (COwnerClass::*mfpSetFrequencyBandCount)(const OpenViBE::uint32 ui32FrequencyBandCount),
				void (COwnerClass::*mfpSetFrequencyBandName)(const OpenViBE::uint32 ui32FrequencyBandIndex, const char* sFrequencyBandName),
				void (COwnerClass::*mfpSetFrequencyBandStart)(const OpenViBE::uint32 ui32FrequencyBandIndex, const OpenViBE::float64 f64FrequencyBandStart),
				void (COwnerClass::*mfpSetFrequencyBandStop)(const OpenViBE::uint32 ui32FrequencyBandIndex, const OpenViBE::float64 f64FrequencyBandStop),
				void (COwnerClass::*mfpSetBuffer)(const OpenViBE::float64* pBuffer))
				:m_rOwnerObject(rOwnerObject)
				,m_mfpSetChannelCount(mfpSetChannelCount)
				,m_mfpSetChannelName(mfpSetChannelName)
				,m_mfpSetFrequencyBandCount(mfpSetFrequencyBandCount)
				,m_mfpSetFrequencyBandName(mfpSetFrequencyBandName)
				,m_mfpSetFrequencyBandStart(mfpSetFrequencyBandStart)
				,m_mfpSetFrequencyBandStop(mfpSetFrequencyBandStop)
				,m_mfpSetBuffer(mfpSetBuffer)
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
			virtual void setFrequencyBandCount(const OpenViBE::uint32 ui32FrequencyBandCount)
			{
				if(m_mfpSetFrequencyBandCount)
				{
					(m_rOwnerObject.*m_mfpSetFrequencyBandCount)(ui32FrequencyBandCount);
				}
			}
			virtual void setFrequencyBandName(const OpenViBE::uint32 ui32FrequencyBandIndex, const char* sFrequencyBandName)
			{
				if(m_mfpSetFrequencyBandName)
				{
					(m_rOwnerObject.*m_mfpSetFrequencyBandName)(ui32FrequencyBandIndex, sFrequencyBandName);
				}
			}
			virtual void setFrequencyBandStart(const OpenViBE::uint32 ui32FrequencyBandIndex, const OpenViBE::float64 f64FrequencyBandStart)
			{
				if(m_mfpSetFrequencyBandStart)
				{
					(m_rOwnerObject.*m_mfpSetFrequencyBandStart)(ui32FrequencyBandIndex, f64FrequencyBandStart);
				}
			}
			virtual void setFrequencyBandStop(const OpenViBE::uint32 ui32FrequencyBandIndex, const OpenViBE::float64 f64FrequencyBandStop)
			{
				if(m_mfpSetFrequencyBandStop)
				{
					(m_rOwnerObject.*m_mfpSetFrequencyBandStop)(ui32FrequencyBandIndex, f64FrequencyBandStop);
				}
			}
			virtual void setBuffer(const OpenViBE::float64* pBuffer)
			{
				if(m_mfpSetBuffer)
				{
					(m_rOwnerObject.*m_mfpSetBuffer)(pBuffer);
				}
			}
		protected:
			COwnerClass& m_rOwnerObject;
			void (COwnerClass::*m_mfpSetChannelCount)(const OpenViBE::uint32 ui32ChannelCount);
			void (COwnerClass::*m_mfpSetChannelName)(const OpenViBE::uint32 ui32ChannelIndex, const char* sChannelName);
			void (COwnerClass::*m_mfpSetFrequencyBandCount)(const OpenViBE::uint32 ui32FrequencyBandCount);
			void (COwnerClass::*m_mfpSetFrequencyBandName)(const OpenViBE::uint32 ui32FrequencyBandIndex, const char* sFrequencyBandName);
			void (COwnerClass::*m_mfpSetFrequencyBandStart)(const OpenViBE::uint32 ui32FrequencyBandIndex, const OpenViBE::float64 f64FrequencyBandStart);
			void (COwnerClass::*m_mfpSetFrequencyBandStop)(const OpenViBE::uint32 ui32FrequencyBandIndex, const OpenViBE::float64 f64FrequencyBandStop);
			void (COwnerClass::*m_mfpSetBuffer)(const OpenViBE::float64* pBuffer);
		};

		_IsDerivedFromClass_(OpenViBEToolkit::IBoxAlgorithmEBMLInputReaderCallback, OVTK_ClassId_);
	};

	OVTK_API OpenViBEToolkit::IBoxAlgorithmSpectrumInputReaderCallback* createBoxAlgorithmSpectrumInputReaderCallback(OpenViBEToolkit::IBoxAlgorithmSpectrumInputReaderCallback::ICallback& rCallback);
	OVTK_API void releaseBoxAlgorithmSpectrumInputReaderCallback(OpenViBEToolkit::IBoxAlgorithmSpectrumInputReaderCallback* pBoxAlgorithmSpectrumInputReaderCallback);
};

#endif // __OpenViBEToolkit_Reader_IBoxAlgorithmSpectrumInputReaderCallback_H__
