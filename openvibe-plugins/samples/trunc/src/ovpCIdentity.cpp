#include "ovpCIdentity.h"

#include <iostream>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;
using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Samples;
using namespace OpenViBEToolkit;
using namespace std;

void CIdentity::release(void)
{
	delete this;
}

boolean CIdentity::processInput(uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CIdentity::process(void)
{
	IBox* l_pStaticBoxContext=getBoxAlgorithmContext()->getStaticBoxContext();
	IBoxIO* l_pDynamicBoxContext=getBoxAlgorithmContext()->getDynamicBoxContext();

	uint64 l_ui64StartTime=0;
	uint64 l_ui64EndTime=0;
	uint64 l_ui64ChunkSize=0;
	const uint8* l_pChunkBuffer=NULL;

	for(uint32 i=0; i<l_pStaticBoxContext->getInputCount(); i++)
	{
		for(uint32 j=0; j<l_pDynamicBoxContext->getInputChunkCount(i); j++)
		{
			l_pDynamicBoxContext->getInputChunk(i, j, l_ui64StartTime, l_ui64EndTime, l_ui64ChunkSize, l_pChunkBuffer);
			l_pDynamicBoxContext->appendOutputChunkData(i, l_pChunkBuffer, l_ui64ChunkSize);
			l_pDynamicBoxContext->markOutputAsReadyToSend(i, l_ui64StartTime, l_ui64EndTime);
			l_pDynamicBoxContext->markInputAsDeprecated(i, j);
		}
	}

	return true;
}
