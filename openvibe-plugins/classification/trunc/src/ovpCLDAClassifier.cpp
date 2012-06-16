#include "ovpCLDAClassifier.h"

#include <iostream>
#include <sstream>
#include <string>
using namespace std;

using namespace OpenViBE;
using namespace OpenViBE::Plugins;
using namespace OpenViBE::Kernel;
using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Classification;
using namespace OpenViBEToolkit;

namespace OpenViBEPlugins
{
	namespace Classification
	{

		CLDAClassifier::CLDAClassifier() :
			m_oClassOutputWriterCallbackProxy(
				*this,
				&CLDAClassifier::writeToClassOutput),
			m_oAmplitudeOutputWriterCallbackProxy(
				*this,
				&CLDAClassifier::writeToAmplitudeOutput),
			m_pStreamedMatrixOutputWriterHelper(NULL),
			m_ui64LastChunkStartTime(0),
			m_ui64LastChunkEndTime(0),
			m_bError(false)
		{
			m_pWriter[0] = NULL;
			m_pWriter[1] = NULL;
		}

		OpenViBE::boolean CLDAClassifier::initialize()
		{
			const IBox* l_pBox=getBoxAlgorithmContext()->getStaticBoxContext();

			// Prepares EBML reader
			m_pFeatureVectorInputReaderCallback = createBoxAlgorithmFeatureVectorInputReaderCallback(*this);
			m_pReader=EBML::createReader(*m_pFeatureVectorInputReaderCallback);

			m_pStreamedMatrixOutputWriterHelper=createBoxAlgorithmStreamedMatrixOutputWriter();
			m_pWriter[0]=EBML::createWriter(m_oClassOutputWriterCallbackProxy);
			m_pWriter[1]=EBML::createWriter(m_oAmplitudeOutputWriterCallbackProxy);

			//parses the configuration string
			CString l_oParameters;
			l_pBox->getSettingValue(0, l_oParameters);

			istringstream l_oStreamString((const char*)l_oParameters);
			float64 l_f64CurrentValue;

			while(l_oStreamString>>l_f64CurrentValue)
			{
				m_oCoefficients.push_back(l_f64CurrentValue);
			}

			//Sends the output header
			m_pStreamedMatrixOutputWriterHelper->setDimmensionCount(1);
			m_pStreamedMatrixOutputWriterHelper->setDimmensionSize(0,1);
			m_pStreamedMatrixOutputWriterHelper->setDimmensionLabel(0, 0, "Class");
			m_pStreamedMatrixOutputWriterHelper->writeHeader(*m_pWriter[0]);
			getBoxAlgorithmContext()->getDynamicBoxContext()->markOutputAsReadyToSend(0, 0, 0);

			m_pStreamedMatrixOutputWriterHelper->setDimmensionCount(1);
			m_pStreamedMatrixOutputWriterHelper->setDimmensionSize(0,1);
			m_pStreamedMatrixOutputWriterHelper->setDimmensionLabel(0, 0, "Amplitude");
			m_pStreamedMatrixOutputWriterHelper->writeHeader(*m_pWriter[1]);
			getBoxAlgorithmContext()->getDynamicBoxContext()->markOutputAsReadyToSend(1, 0, 0);

			return true;
		}

		OpenViBE::boolean CLDAClassifier::uninitialize()
		{
			// Cleans up EBML reader
			releaseBoxAlgorithmFeatureVectorInputReaderCallback(m_pFeatureVectorInputReaderCallback);

			m_pReader->release();
			m_pReader=NULL;

			// Cleans up EBML writer
			m_pWriter[0]->release();
			m_pWriter[0]=NULL;
			m_pWriter[1]->release();
			m_pWriter[1]=NULL;

			releaseBoxAlgorithmStreamedMatrixOutputWriter(m_pStreamedMatrixOutputWriterHelper);
			m_pStreamedMatrixOutputWriterHelper=NULL;

			return true;
		}

		OpenViBE::boolean CLDAClassifier::processInput(OpenViBE::uint32 ui32InputIndex)
		{
			if(m_bError)
			{
				return false;
			}

			getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
			return true;
		}

		OpenViBE::boolean CLDAClassifier::process()
		{
			IBoxIO* l_pBoxIO = getBoxAlgorithmContext()->getDynamicBoxContext();

			for(uint32 j=0; j<l_pBoxIO->getInputChunkCount(0); j++)
			{
					uint64 l_ui64ChunkSize;
					const uint8* l_pBuffer;

					l_pBoxIO->getInputChunk(0, j, m_ui64LastChunkStartTime, m_ui64LastChunkEndTime, l_ui64ChunkSize, l_pBuffer);
					l_pBoxIO->markInputAsDeprecated(0, j);
					m_pReader->processData(l_pBuffer, l_ui64ChunkSize);
			}

			return true;
		}

		void CLDAClassifier::setFeatureCount(const OpenViBE::uint32 ui32FeatureCount)
		{
			if(m_oCoefficients.size() < 2)
			{
				getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Warning << "Number of coefficients too low, needs at least 2!\n";
				m_bError=true;
			}

			//check the number of dimmensions
			if(ui32FeatureCount != m_oCoefficients.size()-1)
			{
				getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Warning << "Feature count different from the classifier's number of coefficients!\n";
				m_bError = true;
			}
		}

		void CLDAClassifier::setFeatureName(const OpenViBE::uint32 ui32FeatureIndex, const char* sFeatureName)
		{
			/* No need for this */
		}

		void CLDAClassifier::setFeatureVector(const OpenViBE::float64* pFeatureBuffer)
		{
			if(m_bError)
			{
				return;
			}

			float64 l_f64Amplitude = m_oCoefficients[0];

			for(size_t i=0 ; i<m_oCoefficients.size()-1 ; i++)
			{
				l_f64Amplitude += pFeatureBuffer[i] * m_oCoefficients[i+1];
			}

			float64 l_f64Class = (l_f64Amplitude>0) ? 0 : 1;

			//cout<<"Class "<<l_f64Class<<" - amplitude : "<<l_f64Amplitude<<endl;

			m_pStreamedMatrixOutputWriterHelper->setBuffer(&l_f64Class);
			m_pStreamedMatrixOutputWriterHelper->writeBuffer(*m_pWriter[0]);
			getBoxAlgorithmContext()->getDynamicBoxContext()->markOutputAsReadyToSend(0, m_ui64LastChunkStartTime, m_ui64LastChunkEndTime);

			m_pStreamedMatrixOutputWriterHelper->setBuffer(&l_f64Amplitude);
			m_pStreamedMatrixOutputWriterHelper->writeBuffer(*m_pWriter[1]);
			getBoxAlgorithmContext()->getDynamicBoxContext()->markOutputAsReadyToSend(1, m_ui64LastChunkStartTime, m_ui64LastChunkEndTime);

		}

		void CLDAClassifier::writeToClassOutput(const void* pBuffer, const EBML::uint64 ui64BufferSize)
		{
			appendOutputChunkData<0>(pBuffer, ui64BufferSize);
		}

		void CLDAClassifier::writeToAmplitudeOutput(const void* pBuffer, const EBML::uint64 ui64BufferSize)
		{
			appendOutputChunkData<1>(pBuffer, ui64BufferSize);
		}

	};
};

