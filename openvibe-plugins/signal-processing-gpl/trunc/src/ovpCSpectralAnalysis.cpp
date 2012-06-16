#include "ovpCSpectralAnalysis.h"

#include <iostream>
#include <sstream>

#include <itpp/itstat.h>
#include <itpp/itsignal.h>

using namespace itpp;
using namespace OpenViBE;
using namespace OpenViBE::Plugins;
using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessingGpl;
using namespace OpenViBEToolkit;
using namespace std;
using namespace OpenViBE::Kernel;

CSpectralAnalysis::CSpectralAnalysis(void)
	:m_pSignalReader(NULL),
	m_pSignalReaderCallBack(NULL),
	m_oSignalReaderCallBackProxy(*this,
		&CSpectralAnalysis::setChannelCount,
		&CSpectralAnalysis::setChannelName,
		&CSpectralAnalysis::setSampleCountPerBuffer,
		&CSpectralAnalysis::setSamplingRate,
		&CSpectralAnalysis::setSampleBuffer),
	m_ui64LastChunkStartTime(0),
	m_ui64LastChunkEndTime(0),
	m_oAmplitudeSpectrumOutputWriterCallbackProxy(
		*this,
		&CSpectralAnalysis::writeAmplitudeSpectrumOutput),
	m_oPhaseSpectrumOutputWriterCallbackProxy(
		*this,
		&CSpectralAnalysis::writePhaseSpectrumOutput),
	m_oRealPartSpectrumOutputWriterCallbackProxy(
		*this,
		&CSpectralAnalysis::writeRealPartSpectrumOutput),
	m_oImagPartSpectrumOutputWriterCallbackProxy(
		*this,
		&CSpectralAnalysis::writeImagPartSpectrumOutput),
	m_pSpectrumOutputWriterHelper(NULL),
	m_ui32ChannelCount(0),
	m_ui32SampleCount(0),
	m_bCoefComputed(false),
	m_ui32FFTSize(1)
{
	m_pWriter[0] = NULL;
	m_pWriter[1] = NULL;
	m_pWriter[2] = NULL;
	m_pWriter[3] = NULL;
}

void CSpectralAnalysis::release(void)
{
	delete this;
}

boolean CSpectralAnalysis::initialize()
{
	//reads the plugin settings
	CString l_sSpectralComponents;
	getBoxAlgorithmContext()->getStaticBoxContext()->getSettingValue(0, l_sSpectralComponents);
	uint64 l_ui64SpectralComponents=this->getTypeManager().getBitMaskEntryCompositionValueFromName(OVP_TypeId_SpectralComponent, l_sSpectralComponents);

	m_bAmplitudeSpectrum = (l_ui64SpectralComponents & OVP_TypeId_SpectralComponent_Amplitude.toUInteger());
	m_bPhaseSpectrum     = (l_ui64SpectralComponents & OVP_TypeId_SpectralComponent_Phase.toUInteger());
	m_bRealPartSpectrum  = (l_ui64SpectralComponents & OVP_TypeId_SpectralComponent_RealPart.toUInteger());
	m_bImagPartSpectrum  = (l_ui64SpectralComponents & OVP_TypeId_SpectralComponent_ImaginaryPart.toUInteger());

	// Prepares EBML reader
	m_pSignalReaderCallBack = createBoxAlgorithmSignalInputReaderCallback(m_oSignalReaderCallBackProxy);
	m_pSignalReader=EBML::createReader(*m_pSignalReaderCallBack);

	// Prepares EBML writer
	m_pSpectrumOutputWriterHelper=createBoxAlgorithmSpectrumOutputWriter();
	m_pWriter[0]=EBML::createWriter(m_oAmplitudeSpectrumOutputWriterCallbackProxy);
	m_pWriter[1]=EBML::createWriter(m_oPhaseSpectrumOutputWriterCallbackProxy);
	m_pWriter[2]=EBML::createWriter(m_oRealPartSpectrumOutputWriterCallbackProxy);
	m_pWriter[3]=EBML::createWriter(m_oImagPartSpectrumOutputWriterCallbackProxy);

	m_pInputBuffer = NULL;
	m_pOutputBuffer = NULL;

	return true;
}

boolean CSpectralAnalysis::uninitialize()
{
	// Cleans up EBML writer
	m_pWriter[0]->release();
	m_pWriter[0]=NULL;
	m_pWriter[1]->release();
	m_pWriter[1]=NULL;
	m_pWriter[2]->release();
	m_pWriter[2]=NULL;
	m_pWriter[3]->release();
	m_pWriter[3]=NULL;

	releaseBoxAlgorithmSpectrumOutputWriter(m_pSpectrumOutputWriterHelper);
	m_pSpectrumOutputWriterHelper=NULL;

	// Cleans up EBML reader
	releaseBoxAlgorithmSignalInputReaderCallback(m_pSignalReaderCallBack);
	m_pSignalReader->release();
	m_pSignalReaderCallBack = NULL;
	m_pSignalReader=NULL;

	delete[] m_pOutputBuffer;

	return true;
}

boolean CSpectralAnalysis::processInput(uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

void CSpectralAnalysis::setChannelCount(const uint32 ui32ChannelCount)
{
	//gets the channel count
	m_ui32ChannelCount = ui32ChannelCount;
	m_pSpectrumOutputWriterHelper->setChannelCount(ui32ChannelCount);
}

void CSpectralAnalysis::setChannelName(const uint32 ui32ChannelIndex, const char* sChannelName)
{
	m_pSpectrumOutputWriterHelper->setChannelName(ui32ChannelIndex, sChannelName);
}

void CSpectralAnalysis::setSampleCountPerBuffer(const uint32 ui32SampleCountPerBuffer)
{
	m_ui32SampleCount = ui32SampleCountPerBuffer;
}

void CSpectralAnalysis::setSamplingRate(const uint32 ui32SamplingFrequency)
{
	m_ui32SamplingRate = ui32SamplingFrequency;
}

void CSpectralAnalysis::setSampleBuffer(const float64* pBuffer)
{
	m_pInputBuffer = pBuffer;
}

void CSpectralAnalysis::setBuffer(const float64* pBuffer)
{
	m_pInputBuffer = pBuffer;
}

void CSpectralAnalysis::writeAmplitudeSpectrumOutput(const void* pBuffer, const EBML::uint64 ui64BufferSize)
{
	appendOutputChunkData<0>(pBuffer, ui64BufferSize);
}

void CSpectralAnalysis::writePhaseSpectrumOutput(const void* pBuffer, const EBML::uint64 ui64BufferSize)
{
	appendOutputChunkData<1>(pBuffer, ui64BufferSize);
}

void CSpectralAnalysis::writeRealPartSpectrumOutput(const void* pBuffer, const EBML::uint64 ui64BufferSize)
{
	appendOutputChunkData<2>(pBuffer, ui64BufferSize);
}

void CSpectralAnalysis::writeImagPartSpectrumOutput(const void* pBuffer, const EBML::uint64 ui64BufferSize)
{
	appendOutputChunkData<3>(pBuffer, ui64BufferSize);
}

boolean CSpectralAnalysis::process()
{
	IBoxIO * l_pDynamicContext = getBoxAlgorithmContext()->getDynamicBoxContext();
	uint32 l_ui32InputChunkCount = l_pDynamicContext->getInputChunkCount(0);
	float64 l_float64BandStart, l_float64BandStop;
	char l_sFrequencyBandName [1024];

	for(uint32 i = 0; i < l_ui32InputChunkCount; i++)
    {
		uint64 l_ui64ChunkSize = 0;
		const uint8 * l_pChunkBuffer = NULL;

		if(l_pDynamicContext->getInputChunk(0, i, m_ui64LastChunkStartTime, m_ui64LastChunkEndTime, l_ui64ChunkSize, l_pChunkBuffer))
		{
			m_pSignalReader->processData(l_pChunkBuffer, l_ui64ChunkSize);

			if(m_pInputBuffer == NULL) //dealing with the signal header
			{
				if (!m_bCoefComputed)
				{
					m_ui32FFTSize=1;
					while (m_ui32FFTSize < m_ui32SampleCount)
					{
						m_ui32FFTSize<<=1;
					}
					m_ui32FFTSize>>=1;
					m_bCoefComputed = true;
				}

				m_ui32FrequencyBandCount = m_ui32FFTSize;
				m_pSpectrumOutputWriterHelper->setFrequencyBandCount(m_ui32FrequencyBandCount);
				for (uint32 j=0; j < m_ui32FrequencyBandCount; j++)
				{
					l_float64BandStart = static_cast<float64>(j*((double)(m_ui32SamplingRate/2)/m_ui32FrequencyBandCount));
					l_float64BandStop = static_cast<float64>((j+1)*((double)(m_ui32SamplingRate/2)/m_ui32FrequencyBandCount));
					if (l_float64BandStop <l_float64BandStart )
					{
						l_float64BandStop = l_float64BandStart;
					}

					m_pSpectrumOutputWriterHelper->setFrequencyBandStart(j, l_float64BandStart);
					m_pSpectrumOutputWriterHelper->setFrequencyBandStop(j,  l_float64BandStop);
					sprintf(l_sFrequencyBandName, "%lg-%lg", l_float64BandStart, l_float64BandStop);
					m_pSpectrumOutputWriterHelper->setFrequencyBandName(j, l_sFrequencyBandName);
				}

				if (m_bAmplitudeSpectrum)
				{
					m_pSpectrumOutputWriterHelper->writeHeader(*m_pWriter[0]);
					l_pDynamicContext->markOutputAsReadyToSend(0,m_ui64LastChunkStartTime, m_ui64LastChunkEndTime);
				}
				if (m_bPhaseSpectrum)
				{
					m_pSpectrumOutputWriterHelper->writeHeader(*m_pWriter[1]);
					l_pDynamicContext->markOutputAsReadyToSend(1,m_ui64LastChunkStartTime, m_ui64LastChunkEndTime);
				}
				if (m_bRealPartSpectrum)
				{
					m_pSpectrumOutputWriterHelper->writeHeader(*m_pWriter[2]);
					l_pDynamicContext->markOutputAsReadyToSend(2,m_ui64LastChunkStartTime, m_ui64LastChunkEndTime);
				}
				if (m_bImagPartSpectrum)
				{
					m_pSpectrumOutputWriterHelper->writeHeader(*m_pWriter[3]);
					l_pDynamicContext->markOutputAsReadyToSend(3,m_ui64LastChunkStartTime, m_ui64LastChunkEndTime);
				}

				m_pOutputBuffer = new float64[m_ui32FFTSize*m_ui32ChannelCount];
				m_pSpectrumOutputWriterHelper->setBuffer(m_pOutputBuffer);
			}
			else
			{
				//doing the processing
				vec x(m_ui32SampleCount);
				cvec y(m_ui32SampleCount);
				cvec z(m_ui32ChannelCount*m_ui32FFTSize);

				for (uint64 i=0;  i < m_ui32ChannelCount; i++)
				{
					for(uint64 j=0 ; j<m_ui32SampleCount ; j++)
					{
						x[j] =  (double)m_pInputBuffer[i*m_ui32SampleCount+j];
					}
					y = fft_real(x, m_ui32FFTSize<<1);

					for(uint64 k=0 ; k<m_ui32FFTSize ; k++)
					{
						z[k+i*m_ui32FFTSize] = y[k];
					}
				}

				if (m_bAmplitudeSpectrum)
				{
					for (uint64 i=0;  i < m_ui32ChannelCount*m_ui32FFTSize; i++)
					{
						m_pOutputBuffer[i] =  sqrt(real(z[i])*real(z[i])+ imag(z[i])*imag(z[i]));
					}
					m_pSpectrumOutputWriterHelper->writeBuffer(*m_pWriter[0]);
					l_pDynamicContext->markOutputAsReadyToSend(0,m_ui64LastChunkStartTime, m_ui64LastChunkEndTime);
				}
				if (m_bPhaseSpectrum)
				{
					for (uint64 i=0;  i < m_ui32ChannelCount*m_ui32FFTSize; i++)
					{
						m_pOutputBuffer[i] =  imag(z[i])/real(z[i]);
					}
					m_pSpectrumOutputWriterHelper->writeBuffer(*m_pWriter[1]);
					l_pDynamicContext->markOutputAsReadyToSend(1,m_ui64LastChunkStartTime, m_ui64LastChunkEndTime);
				}
				if (m_bRealPartSpectrum)
				{
					for (uint64 i=0;  i < m_ui32ChannelCount*m_ui32FFTSize; i++)
					{
						m_pOutputBuffer[i] = real(z[i]);
					}
					m_pSpectrumOutputWriterHelper->writeBuffer(*m_pWriter[2]);
					l_pDynamicContext->markOutputAsReadyToSend(2,m_ui64LastChunkStartTime, m_ui64LastChunkEndTime);
				}
				if (m_bImagPartSpectrum)
				{
					for (uint64 i=0;  i < m_ui32ChannelCount*m_ui32FFTSize; i++)
					{
						m_pOutputBuffer[i] = imag(z[i]);
					}
					m_pSpectrumOutputWriterHelper->writeBuffer(*m_pWriter[3]);
					l_pDynamicContext->markOutputAsReadyToSend(3,m_ui64LastChunkStartTime, m_ui64LastChunkEndTime);
				}
			}
			l_pDynamicContext->markInputAsDeprecated(0,i);
		}
	}
	return true;
}
