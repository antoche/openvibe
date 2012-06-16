#include "ovpCTimeFrequencyMapDisplay.h"
#include "ovp_defines.h"

#include <cstdlib>

using namespace OpenViBE;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SimpleVisualisation;
using namespace OpenViBEToolkit;

CTimeFrequencyMapDisplay::CTimeFrequencyMapDisplay() :
	m_pSpectrumDatabase(NULL),
	m_pTimeFrequencyMapDisplayView(NULL)
{
}

OpenViBE::boolean CTimeFrequencyMapDisplay::initialize()
{
	//create spectrum database
	m_pSpectrumDatabase = new CSpectrumDatabase(*this);

	m_pSpectrumDatabase->initialize();

	//retrieve default settings
	CString l_sTimeScaleSettingValue;
	getStaticBoxContext().getSettingValue(0, l_sTimeScaleSettingValue);
	CString l_sMinDisplayedFrequencySettingValue;
	CString l_sMaxDisplayedFrequencySettingValue;
	getStaticBoxContext().getSettingValue(1, l_sMinDisplayedFrequencySettingValue);
	getStaticBoxContext().getSettingValue(2, l_sMaxDisplayedFrequencySettingValue);

	//create view
	m_pTimeFrequencyMapDisplayView = new CTimeFrequencyMapDisplayView(
		*m_pSpectrumDatabase,
		atof(l_sMinDisplayedFrequencySettingValue),
		atof(l_sMaxDisplayedFrequencySettingValue),
		atof(l_sTimeScaleSettingValue));

	//have database notify view when data is received
	m_pSpectrumDatabase->setDrawable(m_pTimeFrequencyMapDisplayView);

	//parent visualisation box in visualisation tree
	::GtkWidget* l_pWidget=NULL;
	::GtkWidget* l_pToolbarWidget=NULL;
	dynamic_cast<CTimeFrequencyMapDisplayView*>(m_pTimeFrequencyMapDisplayView)->getWidgets(l_pWidget, l_pToolbarWidget);

	getBoxAlgorithmContext()->getVisualisationContext()->setWidget(l_pWidget);
	if(l_pToolbarWidget != NULL)
	{
		getBoxAlgorithmContext()->getVisualisationContext()->setToolbar(l_pToolbarWidget);
	}

	return true;
}

OpenViBE::boolean CTimeFrequencyMapDisplay::uninitialize()
{
	delete m_pTimeFrequencyMapDisplayView;
	delete m_pSpectrumDatabase;
	return true;
}

OpenViBE::boolean CTimeFrequencyMapDisplay::processInput(OpenViBE::uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

OpenViBE::boolean CTimeFrequencyMapDisplay::process()
{
	IDynamicBoxContext* l_pDynamicBoxContext=getBoxAlgorithmContext()->getDynamicBoxContext();

	for(uint32 i=0; i<l_pDynamicBoxContext->getInputChunkCount(0); i++)
	{
		m_pSpectrumDatabase->decodeMemoryBuffer(
			l_pDynamicBoxContext->getInputChunk(0, i),
			l_pDynamicBoxContext->getInputChunkStartTime(0, i),
			l_pDynamicBoxContext->getInputChunkEndTime(0, i));

		l_pDynamicBoxContext->markInputAsDeprecated(0, i);
	}

	return true;
}
