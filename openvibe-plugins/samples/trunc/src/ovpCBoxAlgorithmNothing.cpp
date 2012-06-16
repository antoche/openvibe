#include "ovpCBoxAlgorithmNothing.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Samples;

/*
uint64 CBoxAlgorithmNothing::getClockFrequency(void)
{
	return 0; // the box clock frequency
}
*/

boolean CBoxAlgorithmNothing::initialize(void)
{
	// CString l_sSettingValue;
	// getStaticBoxContext().getSettingValue(0, l_sSettingValue);
	// ...

	return true;
}

boolean CBoxAlgorithmNothing::uninitialize(void)
{
	// ...

	return true;
}

/*
boolean CBoxAlgorithmNothing::processEvent(IMessageEvent& rMessageEvent)
{
	// ...

	// getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	return true;
}
*/

/*
boolean CBoxAlgorithmNothing::processSignal(IMessageSignal& rMessageSignal)
{
	// ...

	// getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	return true;
}
*/

/*
boolean CBoxAlgorithmNothing::processClock(IMessageClock& rMessageClock)
{
	// ...

	// getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	return true;
}
*/

/*
boolean CBoxAlgorithmNothing::processInput(uint32 ui32InputIndex)
{
	// ...

	// getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	return true;
}
*/

boolean CBoxAlgorithmNothing::process(void)
{
	// IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	// IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

	// ...

	// l_rStaticBoxContext.getInputCount();
	// l_rStaticBoxContext.getOutputCount();
	// l_rStaticBoxContext.getSettingCount();

	// l_rDynamicBoxContext.getInputChunkCount()
	// l_rDynamicBoxContext.getInputChunk(i, )
	// l_rDynamicBoxContext.getOutputChunk(i, )

	return true;
}
