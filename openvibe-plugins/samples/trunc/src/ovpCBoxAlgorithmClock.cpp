#include "ovpCBoxAlgorithmClock.h"

#include <cstdio>
#include <cstdlib>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Samples;

uint64 CBoxAlgorithmClock::getClockFrequency(void)
{
	getLogManager() << m_eLogLevel << "Clock frequency requested at time " << time64(getPlayerContext().getCurrentTime()) << "\n";

	return m_ui64ClockFrequency<<32;
}

boolean CBoxAlgorithmClock::initialize(void)
{
	CString l_sSettingValue;

	getStaticBoxContext().getSettingValue(1, l_sSettingValue);
	m_eLogLevel=static_cast<ELogLevel>(getBoxAlgorithmContext()->getPlayerContext()->getTypeManager().getEnumerationEntryValueFromName(OV_TypeId_LogLevel, l_sSettingValue));

	getStaticBoxContext().getSettingValue(0, l_sSettingValue);
	m_ui64ClockFrequency=::atoi(l_sSettingValue.toASCIIString());

	getLogManager() << m_eLogLevel << "Clock frequency tuned to " << m_ui64ClockFrequency << "\n";

	return true;
}

boolean CBoxAlgorithmClock::uninitialize(void)
{
	return true;
}

boolean CBoxAlgorithmClock::processClock(CMessageClock& rMessageClock)
{
	getLogManager() << m_eLogLevel << "Received clock message at time " << time64(rMessageClock.getTime()) << "\n";

	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	return true;
}

boolean CBoxAlgorithmClock::process(void)
{
	getLogManager() << m_eLogLevel << "Process function activated at " << getPlayerContext().getCurrentTime() << "\n";

	return true;
}
