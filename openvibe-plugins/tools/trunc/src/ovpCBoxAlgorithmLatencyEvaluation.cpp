#include "ovpCBoxAlgorithmLatencyEvaluation.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Tools;

boolean CBoxAlgorithmLatencyEvaluation::initialize(void)
{
	CString l_sLogLevel;
	getBoxAlgorithmContext()->getStaticBoxContext()->getSettingValue(0, l_sLogLevel);
	m_eLogLevel=static_cast<ELogLevel>(getBoxAlgorithmContext()->getPlayerContext()->getTypeManager().getEnumerationEntryValueFromName(OV_TypeId_LogLevel, l_sLogLevel));

	m_ui64StartTime=System::Time::zgetTime();

	return true;
}

boolean CBoxAlgorithmLatencyEvaluation::uninitialize(void)
{
	return true;
}

boolean CBoxAlgorithmLatencyEvaluation::processInput(uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	return true;
}

boolean CBoxAlgorithmLatencyEvaluation::process(void)
{
	IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

	uint64 l_ui64Time=getPlayerContext().getCurrentTime();

	for(uint32 i=0; i<l_rDynamicBoxContext.getInputChunkCount(0); i++)
	{
		uint64 l_ui64StartTime=l_rDynamicBoxContext.getInputChunkStartTime(0, i);
		uint64 l_ui64EndTime=l_rDynamicBoxContext.getInputChunkEndTime(0, i);

		float64 l_f64StartLatencyMilli=(((((int64)(l_ui64Time-l_ui64StartTime)) >> 22) * 1000) / 1024.0);
		float64 l_f64EndLatencyMilli=(((((int64)(l_ui64Time-l_ui64EndTime)) >> 22) * 1000) / 1024.0);

		// getLogManager() << LogLevel_Debug << "Timing values [start:end:current]=[" << l_ui64StartTime << ":" << l_ui64EndTime << ":" << l_ui64Time << "]\n";
		getLogManager() << m_eLogLevel << "Current latency [start:end]=[" << l_f64StartLatencyMilli << ":" << l_f64EndLatencyMilli << "]\n";

		l_rDynamicBoxContext.markInputAsDeprecated(0, i);
	}

	float64 l_f64InnerLatencyMilli=(((int64(System::Time::zgetTime()-m_ui64StartTime - l_ui64Time) >> 22) * 1000) / 1024.0);

	getLogManager() << m_eLogLevel << "Inner latency : " << l_f64InnerLatencyMilli << "\n";

	return true;
}
