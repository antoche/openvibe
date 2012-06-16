#include "ovkCAlgorithmProxy.h"
#include "ovkCAlgorithm.h"

using namespace OpenViBE;
using namespace Kernel;

CAlgorithmProxy::CAlgorithmProxy(const IKernelContext& rKernelContext, CAlgorithm& rAlgorithm)
	:TKernelObject < IAlgorithmProxy >(rKernelContext)
	,m_rAlgorithm(rAlgorithm)
	,m_bIsInitialized(false)
{
}

CIdentifier CAlgorithmProxy::getNextInputParameterIdentifier(
	const CIdentifier& rPreviousInputParameterIdentifier) const
{
	if(!m_bIsInitialized) getLogManager() << LogLevel_ImportantWarning << "Calling accessor on uninitialized algorithm\n";
	return m_rAlgorithm.getNextInputParameterIdentifier(rPreviousInputParameterIdentifier);
}

CString CAlgorithmProxy::getInputParameterName(
	const CIdentifier& rInputParameterIdentifier) const
{
	if(!m_bIsInitialized) getLogManager() << LogLevel_ImportantWarning << "Calling accessor on uninitialized algorithm\n";
	return m_rAlgorithm.getInputParameterName(rInputParameterIdentifier);
}

IParameter* CAlgorithmProxy::getInputParameter(
	const CIdentifier& rInputParameterIdentifier)
{
	if(!m_bIsInitialized) getLogManager() << LogLevel_ImportantWarning << "Calling accessor on uninitialized algorithm\n";
	return m_rAlgorithm.getInputParameter(rInputParameterIdentifier);
}

CIdentifier CAlgorithmProxy::getNextOutputParameterIdentifier(
	const CIdentifier& rPreviousOutputParameterIdentifier) const
{
	if(!m_bIsInitialized) getLogManager() << LogLevel_ImportantWarning << "Calling accessor on uninitialized algorithm\n";
	return m_rAlgorithm.getNextOutputParameterIdentifier(rPreviousOutputParameterIdentifier);
}

CString CAlgorithmProxy::getOutputParameterName(
	const CIdentifier& rOutputParameterIdentifier) const
{
	if(!m_bIsInitialized) getLogManager() << LogLevel_ImportantWarning << "Calling accessor on uninitialized algorithm\n";
	return m_rAlgorithm.getOutputParameterName(rOutputParameterIdentifier);
}

IParameter* CAlgorithmProxy::getOutputParameter(
	const CIdentifier& rOutputParameterIdentifier)
{
	if(!m_bIsInitialized) getLogManager() << LogLevel_ImportantWarning << "Calling accessor on uninitialized algorithm\n";
	return m_rAlgorithm.getOutputParameter(rOutputParameterIdentifier);
}

CIdentifier CAlgorithmProxy::getNextOutputTriggerIdentifier(
	const CIdentifier& rPreviousOutputTriggerIdentifier) const
{
	if(!m_bIsInitialized) getLogManager() << LogLevel_ImportantWarning << "Calling accessor on uninitialized algorithm\n";
	return m_rAlgorithm.getNextOutputTriggerIdentifier(rPreviousOutputTriggerIdentifier);
}

CString CAlgorithmProxy::getOutputTriggerName(
	const CIdentifier& rOutputTriggerIdentifier) const
{
	if(!m_bIsInitialized) getLogManager() << LogLevel_ImportantWarning << "Calling accessor on uninitialized algorithm\n";
	return m_rAlgorithm.getOutputTriggerName(rOutputTriggerIdentifier);
}

boolean CAlgorithmProxy::isOutputTriggerActive(
	const CIdentifier& rOutputTriggerIdentifier) const
{
	if(!m_bIsInitialized) getLogManager() << LogLevel_ImportantWarning << "Calling state accessor on uninitialized algorithm\n";
	return m_rAlgorithm.isOutputTriggerActive(rOutputTriggerIdentifier);
}

CIdentifier CAlgorithmProxy::getNextInputTriggerIdentifier(
	const CIdentifier& rPreviousInputTriggerIdentifier) const
{
	if(!m_bIsInitialized) getLogManager() << LogLevel_ImportantWarning << "Calling accessor on uninitialized algorithm\n";
	return m_rAlgorithm.getNextInputTriggerIdentifier(rPreviousInputTriggerIdentifier);
}

CString CAlgorithmProxy::getInputTriggerName(
	const CIdentifier& rInputTriggerIdentifier) const
{
	if(!m_bIsInitialized) getLogManager() << LogLevel_ImportantWarning << "Calling accessor on uninitialized algorithm\n";
	return m_rAlgorithm.getInputTriggerName(rInputTriggerIdentifier);
}

boolean CAlgorithmProxy::activateInputTrigger(
	const CIdentifier& rInputTriggerIdentifier,
	const boolean bTriggerState)
{
	if(!m_bIsInitialized) getLogManager() << LogLevel_ImportantWarning << "Calling state accessor on uninitialized algorithm\n";
	return m_rAlgorithm.activateInputTrigger(rInputTriggerIdentifier, bTriggerState);
}

boolean CAlgorithmProxy::initialize(void)
{
	if(m_bIsInitialized) getLogManager() << LogLevel_ImportantWarning << "Can not initialize already initialized algorithm\n";
	return (m_bIsInitialized=m_rAlgorithm.initialize());
}

boolean CAlgorithmProxy::uninitialize(void)
{
	if(!m_bIsInitialized) getLogManager() << LogLevel_ImportantWarning << "Can not uninitialize uninitialized algorithm\n";
	return (m_bIsInitialized=!m_rAlgorithm.uninitialize());
}

boolean CAlgorithmProxy::process(void)
{
	if(!m_bIsInitialized) getLogManager() << LogLevel_ImportantWarning << "Calling accessor on uninitialized algorithm\n";
	return m_rAlgorithm.process();
}

boolean CAlgorithmProxy::process(
	const CIdentifier& rTriggerIdentifier)
{
	if(!m_bIsInitialized) getLogManager() << LogLevel_ImportantWarning << "Calling accessor on uninitialized algorithm\n";
	return m_rAlgorithm.process(rTriggerIdentifier);
}
