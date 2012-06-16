#include "ovpCSignalDisplay.h"

#include <cmath>
#include <iostream>
#include <cstdlib>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SimpleVisualisation;

using namespace OpenViBEToolkit;

using namespace std;

namespace OpenViBEPlugins
{
	namespace SimpleVisualisation
	{
		/**
		* Constructor
		*/
		CSignalDisplay::CSignalDisplay(void)
			:m_pStreamedMatrixReader(NULL)
			,m_pStimulationReader(NULL)
			,m_pStreamedMatrixReaderCallBack(NULL)
			,m_pStimulationReaderCallBack(NULL)
			,m_pSignalDisplayView(NULL)
			,m_pBufferDatabase(NULL)
		{
		}

		boolean CSignalDisplay::initialize()
		{
			//initializes the ebml input
			m_pStreamedMatrixReaderCallBack = createBoxAlgorithmStreamedMatrixInputReaderCallback(*this);
			m_pStreamedMatrixReader=EBML::createReader(*m_pStreamedMatrixReaderCallBack);
			m_pStimulationReaderCallBack = createBoxAlgorithmStimulationInputReaderCallback(*this);
			m_pStimulationReader=EBML::createReader(*m_pStimulationReaderCallBack);

			m_pBufferDatabase = new CBufferDatabase(*this);

			//retrieve settings
			CString l_sTimeScaleSettingValue=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
			CString l_sDisplayModeSettingValue=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);
			CString l_sManualVerticalScaleSettingValue="false";
			CString l_sVerticalScaleSettingValue="100.";
			if(this->getStaticBoxContext().getSettingCount() > 2) l_sManualVerticalScaleSettingValue=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2);
			if(this->getStaticBoxContext().getSettingCount() > 3) l_sVerticalScaleSettingValue=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 3);

			this->getLogManager() << LogLevel_Debug << "l_sManualVerticalScaleSettingValue=" << l_sManualVerticalScaleSettingValue << "\n";
			this->getLogManager() << LogLevel_Debug << "l_sVerticalScaleSettingValue=" << l_sVerticalScaleSettingValue << "\n";

			//create GUI
			m_pSignalDisplayView = new CSignalDisplayView(
				*m_pBufferDatabase,
				::atof(l_sTimeScaleSettingValue),
				CIdentifier(getTypeManager().getEnumerationEntryValueFromName(OVP_TypeId_SignalDisplayMode, l_sDisplayModeSettingValue)),
				!this->getConfigurationManager().expandAsBoolean(l_sManualVerticalScaleSettingValue),
				::atof(l_sVerticalScaleSettingValue));

			m_pBufferDatabase->setDrawable(m_pSignalDisplayView);

			//parent visualisation box in visualisation tree
			::GtkWidget* l_pWidget=NULL;
			::GtkWidget* l_pToolbarWidget=NULL;
			dynamic_cast<CSignalDisplayView*>(m_pSignalDisplayView)->getWidgets(l_pWidget, l_pToolbarWidget);
			getBoxAlgorithmContext()->getVisualisationContext()->setWidget(l_pWidget);
			if(l_pToolbarWidget != NULL)
			{
				getBoxAlgorithmContext()->getVisualisationContext()->setToolbar(l_pToolbarWidget);
			}

			return true;
		}

		boolean CSignalDisplay::uninitialize()
		{
			//release the ebml reader
			releaseBoxAlgorithmStreamedMatrixInputReaderCallback(m_pStreamedMatrixReaderCallBack);
			releaseBoxAlgorithmStimulationInputReaderCallback(m_pStimulationReaderCallBack);
			m_pStreamedMatrixReaderCallBack=NULL;
			m_pStimulationReaderCallBack=NULL;

			m_pStreamedMatrixReader->release();
			m_pStimulationReader->release();
			m_pStreamedMatrixReader=NULL;
			m_pStimulationReader=NULL;

			delete m_pSignalDisplayView;
			delete m_pBufferDatabase;
			m_pSignalDisplayView=NULL;
			m_pBufferDatabase=NULL;

			return true;
		}

		boolean CSignalDisplay::processInput(uint32 ui32InputIndex)
		{
			getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
			return true;
		}

		boolean CSignalDisplay::process()
		{
			IDynamicBoxContext* l_pDynamicBoxContext=getBoxAlgorithmContext()->getDynamicBoxContext();
			uint32 i;

			for(i=0; i<l_pDynamicBoxContext->getInputChunkCount(1); i++)
			{
				uint64 l_ui64ChunkSize=0;
				const uint8* l_pChunkBuffer=NULL;
				uint64 l_ui64StartTime=0;
				uint64 l_ui64EndTime=0;

				if(l_pDynamicBoxContext->getInputChunk(1, i, l_ui64StartTime, l_ui64EndTime, l_ui64ChunkSize, l_pChunkBuffer))
				{
					m_pStimulationReader->processData(l_pChunkBuffer, l_ui64ChunkSize);
					l_pDynamicBoxContext->markInputAsDeprecated(1, i);
				}
			}

			for(i=0; i<l_pDynamicBoxContext->getInputChunkCount(0); i++)
			{
				uint64 l_ui64ChunkSize=0;
				const uint8* l_pChunkBuffer=NULL;

				if(l_pDynamicBoxContext->getInputChunk(0, i, m_ui64StartTime, m_ui64EndTime, l_ui64ChunkSize, l_pChunkBuffer))
				{
					m_pStreamedMatrixReader->processData(l_pChunkBuffer, l_ui64ChunkSize);
					l_pDynamicBoxContext->markInputAsDeprecated(0, i);
				}
			}

			return true;
		}

		void CSignalDisplay::setMatrixDimmensionCount(const uint32 ui32DimmensionCount)
		{
			m_pBufferDatabase->setMatrixDimmensionCount(ui32DimmensionCount);
		}

		void CSignalDisplay::setMatrixDimmensionSize(const uint32 ui32DimmensionIndex, const uint32 ui32DimmensionSize)
		{
			m_pBufferDatabase->setMatrixDimmensionSize(ui32DimmensionIndex, ui32DimmensionSize);
		}

		void CSignalDisplay::setMatrixDimmensionLabel(const uint32 ui32DimmensionIndex, const uint32 ui32DimmensionEntryIndex, const char* sDimmensionLabel)
		{
			m_pBufferDatabase->setMatrixDimmensionLabel(ui32DimmensionIndex, ui32DimmensionEntryIndex, sDimmensionLabel);
		}

		void CSignalDisplay::setMatrixBuffer(const float64* pBuffer)
		{
			m_pBufferDatabase->setMatrixBuffer(pBuffer, m_ui64StartTime, m_ui64EndTime);
		}

		void CSignalDisplay::setStimulationCount(const uint32 ui32StimulationCount)
		{
			m_pBufferDatabase->setStimulationCount(ui32StimulationCount);
		}

		void CSignalDisplay::setStimulation(const uint32 ui32StimulationIndex, const uint64 ui64StimulationIdentifier, const uint64 ui64StimulationDate)
		{
			CString l_oStimulationName = getTypeManager().getEnumerationEntryNameFromValue(OV_TypeId_Stimulation, ui64StimulationIdentifier);
			((CSignalDisplayView*)m_pSignalDisplayView)->onStimulationReceivedCB(ui64StimulationIdentifier, l_oStimulationName);
			m_pBufferDatabase->setStimulation(ui32StimulationIndex, ui64StimulationIdentifier, ui64StimulationDate);
		}
	};
};


