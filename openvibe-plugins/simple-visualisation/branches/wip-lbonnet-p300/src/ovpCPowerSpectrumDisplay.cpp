#include "ovpCPowerSpectrumDisplay.h"

#include <cstdlib>

using namespace OpenViBE;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SimpleVisualisation;
using namespace OpenViBEToolkit;

namespace OpenViBEPlugins
{
	namespace SimpleVisualisation
	{

		CPowerSpectrumDisplay::CPowerSpectrumDisplay() :
			m_pReader(NULL),
			m_pSpectrumReaderCallBack(NULL),
			m_pPowerSpectrumDisplayView(NULL),
			m_pPowerSpectrumDisplayDatabase(NULL)
		{
		}

		OpenViBE::boolean CPowerSpectrumDisplay::initialize()
		{
			//initializes the ebml input
			m_pSpectrumReaderCallBack = createBoxAlgorithmSpectrumInputReaderCallback(*this);
			m_pReader=EBML::createReader(*m_pSpectrumReaderCallBack);

			m_pPowerSpectrumDisplayDatabase = new CPowerSpectrumDatabase(*this);

			//retrieve displayed frequency range settings
			CString l_sMinDisplayedFrequencySettingValue;
			CString l_sMaxDisplayedFrequencySettingValue;
			getStaticBoxContext().getSettingValue(0, l_sMinDisplayedFrequencySettingValue);
			getStaticBoxContext().getSettingValue(1, l_sMaxDisplayedFrequencySettingValue);

			m_pPowerSpectrumDisplayView = new CPowerSpectrumDisplayView(*m_pPowerSpectrumDisplayDatabase,
				atof(l_sMinDisplayedFrequencySettingValue), atof(l_sMaxDisplayedFrequencySettingValue));

			m_pPowerSpectrumDisplayDatabase->setDrawable(m_pPowerSpectrumDisplayView);

			//parent visualisation box in visualisation tree
			::GtkWidget* l_pWidget=NULL;
			::GtkWidget* l_pToolbarWidget=NULL;
			dynamic_cast<CPowerSpectrumDisplayView*>(m_pPowerSpectrumDisplayView)->getWidgets(l_pWidget, l_pToolbarWidget);
			getBoxAlgorithmContext()->getVisualisationContext()->setWidget(l_pWidget);
			if(l_pToolbarWidget != NULL)
			{
				getBoxAlgorithmContext()->getVisualisationContext()->setToolbar(l_pToolbarWidget);
			}
			return true;
		}

		OpenViBE::boolean CPowerSpectrumDisplay::uninitialize()
		{
			//release the ebml reader
			releaseBoxAlgorithmSpectrumInputReaderCallback(m_pSpectrumReaderCallBack);

			m_pReader->release();
			m_pReader=NULL;

			delete m_pPowerSpectrumDisplayView;
			delete m_pPowerSpectrumDisplayDatabase;

			return true;
		}

		OpenViBE::boolean CPowerSpectrumDisplay::processInput(OpenViBE::uint32 ui32InputIndex)
		{
			getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
			return true;
		}

		OpenViBE::boolean CPowerSpectrumDisplay::process()
		{
			IDynamicBoxContext* l_pDynamicBoxContext=getBoxAlgorithmContext()->getDynamicBoxContext();

			for(uint32 i=0; i<l_pDynamicBoxContext->getInputChunkCount(0); i++)
			{
				uint64 l_ui64ChunkSize;
				const uint8* l_pChunkBuffer=NULL;

				if(l_pDynamicBoxContext->getInputChunk(0, i, m_ui64StartTime, m_ui64EndTime, l_ui64ChunkSize, l_pChunkBuffer))
				{
					m_pReader->processData(l_pChunkBuffer, l_ui64ChunkSize);
					l_pDynamicBoxContext->markInputAsDeprecated(0, i);
				}
			}

			return true;
		}

		void CPowerSpectrumDisplay::setChannelCount(const OpenViBE::uint32 ui32ChannelCount)
		{
			m_pPowerSpectrumDisplayDatabase->setChannelCount(ui32ChannelCount);
		}

		void CPowerSpectrumDisplay::setChannelName(const OpenViBE::uint32 ui32ChannelIndex, const char* sChannelName)
		{
			m_pPowerSpectrumDisplayDatabase->setChannelName(ui32ChannelIndex, sChannelName);
		}

		void CPowerSpectrumDisplay::setFrequencyBandCount(const OpenViBE::uint32 ui32FrequencyBandCount)
		{
			m_pPowerSpectrumDisplayDatabase->setFrequencyBandCount(ui32FrequencyBandCount);
		}

		void CPowerSpectrumDisplay::setFrequencyBandName(const OpenViBE::uint32 ui32FrequencyBandIndex, const char* sFrequencyBandName)
		{
			m_pPowerSpectrumDisplayDatabase->setFrequencyBandName(ui32FrequencyBandIndex, sFrequencyBandName);
		}

		void CPowerSpectrumDisplay::setFrequencyBandStart(const OpenViBE::uint32 ui32FrequencyBandIndex, const OpenViBE::float64 f64FrequencyBandStart)
		{
			m_pPowerSpectrumDisplayDatabase->setFrequencyBandStart(ui32FrequencyBandIndex, f64FrequencyBandStart);
		}

		void CPowerSpectrumDisplay::setFrequencyBandStop(const OpenViBE::uint32 ui32FrequencyBandIndex, const OpenViBE::float64 f64FrequencyBandStop)
		{
			m_pPowerSpectrumDisplayDatabase->setFrequencyBandStop(ui32FrequencyBandIndex, f64FrequencyBandStop);
		}

		void CPowerSpectrumDisplay::setBuffer(const OpenViBE::float64* pBuffer)
		{
			m_pPowerSpectrumDisplayDatabase->setBuffer(pBuffer);
		}
	};
};
