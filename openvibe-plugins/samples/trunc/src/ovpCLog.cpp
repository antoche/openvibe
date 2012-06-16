#include "ovpCLog.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;
using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Samples;
using namespace OpenViBEToolkit;

void CLog::release(void)
{
	delete this;
}

boolean CLog::initialize(void)
{
	getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Info << "initialize\n";
	return true;
}

boolean CLog::uninitialize(void)
{
	getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Info << "uninitialize\n";
	return true;
}

boolean CLog::processClock(IMessageClock& rMessageClock)
{
	getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Info << "processClock\n";
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CLog::processInput(uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Info << "processInput\n";
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CLog::process(void)
{
	getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Info << "process\n";

	IBox* l_pStaticBoxContext=getBoxAlgorithmContext()->getStaticBoxContext();
	IBoxIO* l_pDynamicBoxContext=getBoxAlgorithmContext()->getDynamicBoxContext();

	for(uint32 i=0; i<l_pStaticBoxContext->getInputCount(); i++)
	{
		for(uint32 j=0; j<l_pDynamicBoxContext->getInputChunkCount(i); j++)
		{
			l_pDynamicBoxContext->markInputAsDeprecated(i, j);
		}
	}

	return true;
}
