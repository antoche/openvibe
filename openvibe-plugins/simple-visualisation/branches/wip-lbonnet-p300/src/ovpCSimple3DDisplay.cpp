#include "ovpCSimple3DDisplay.h"
#include <cstdlib>
#include <cmath>
#include <memory.h>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SimpleVisualisation;

namespace OpenViBEPlugins
{
namespace SimpleVisualisation
{

CSimple3DDisplay::CSimple3DDisplay(void) :
	m_pSimple3DDatabase(NULL),
	m_o3DWidgetIdentifier(OV_UndefinedIdentifier)
{
}

uint64 CSimple3DDisplay::getClockFrequency(void)
{
	return ((uint64)1LL)<<36;
}

boolean CSimple3DDisplay::initialize(void)
{
	m_pSimple3DDatabase = new CSimple3DDatabase(*this);

	m_pSimple3DView = new CSimple3DView(*m_pSimple3DDatabase);
	m_pSimple3DDatabase->setDrawable(m_pSimple3DView);
	m_pSimple3DDatabase->setRedrawOnNewData(false);

	//send widget pointers to visualisation context for parenting
	::GtkWidget* l_pWidget=NULL;
	m_o3DWidgetIdentifier = getBoxAlgorithmContext()->getVisualisationContext()->create3DWidget(l_pWidget);
	if(l_pWidget != NULL)
	{
		getBoxAlgorithmContext()->getVisualisationContext()->setWidget(l_pWidget);
	}

	::GtkWidget* l_pToolbarWidget=NULL;
	dynamic_cast<CSimple3DView*>(m_pSimple3DView)->getToolbar(l_pToolbarWidget);
	if(l_pToolbarWidget != NULL)
	{
		getBoxAlgorithmContext()->getVisualisationContext()->setToolbar(l_pToolbarWidget);
	}

	m_pSimple3DDatabase->set3DWidgetIdentifier(m_o3DWidgetIdentifier);

	/*
	//initialise plugin-specific resources
	m_oResourceGroupIdentifier = getBoxAlgorithmContext()->getVisualisationContext()->createResourceGroup(m_sName);

	//add resources
	//char buf[MAX_PATH];
	//sprintf(buf, "./media/%s", m_sName.toCString());
	//getBoxAlgorithmContext()->getVisualisationContext()->addResourceLocation(m_oResourceGroupIdentifier, buf, EResourceLocationType::RESOURCE_LOCATION_FILE, true);
	getBoxAlgorithmContext()->getVisualisationContext()->addResourceLocation(m_oResourceGroupIdentifier, "./media/Plugin0", OpenViBE::Kernel::RESOURCE_LOCATION_FILE, true);
	//getBoxAlgorithmContext()->getVisualisationContext()->addResourceLocation(m_oResourceGroupIdentifier, "./media/Plugin0/tempo", EResourceLocationType::RESOURCE_LOCATION_FILE, true);

	//initialize them
	getBoxAlgorithmContext()->getVisualisationContext()->initializeResourceGroup(m_oResourceGroupIdentifier);
	*/

	return true;
}

boolean CSimple3DDisplay::uninitialize(void)
{
	delete m_pSimple3DView;
	m_pSimple3DView = NULL;
	delete m_pSimple3DDatabase;
	m_pSimple3DDatabase = NULL;

	return true;
}

OpenViBE::boolean CSimple3DDisplay::processInput(OpenViBE::uint32 ui32InputIndex)
{
	if(!getBoxAlgorithmContext()->getVisualisationContext()->is3DWidgetRealized(m_o3DWidgetIdentifier))
	{
		return true;
	}
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CSimple3DDisplay::processClock(IMessageClock& rMessageClock)
{
	if(!getBoxAlgorithmContext()->getVisualisationContext()->is3DWidgetRealized(m_o3DWidgetIdentifier))
	{
		return true;
	}
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CSimple3DDisplay::process(void)
{
	m_pSimple3DDatabase->process3D();
	getBoxAlgorithmContext()->getVisualisationContext()->update3DWidget(m_o3DWidgetIdentifier);

	return true;
}

};
};
