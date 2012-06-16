#include "ovkCAlgorithmProto.h"
#include "ovkCAlgorithm.h"

using namespace OpenViBE;
using namespace Kernel;

CAlgorithmProto::CAlgorithmProto(const IKernelContext& rKernelContext, CAlgorithm& rAlgorithm)
	:TKernelObject < IAlgorithmProto >(rKernelContext)
	,m_rAlgorithm(rAlgorithm)
{
}

boolean CAlgorithmProto::addInputParameter(
	const CIdentifier& rInputParameterIdentifier,
	const CString& sInputName,
	const EParameterType eParameterType,
	const CIdentifier& rSubTypeIdentifier)
{
	return m_rAlgorithm.addInputParameter(rInputParameterIdentifier, sInputName, eParameterType, rSubTypeIdentifier);
}

boolean CAlgorithmProto::addOutputParameter(
	const CIdentifier& rOutputParameterIdentifier,
	const CString& sOutputName,
	const EParameterType eParameterType,
	const CIdentifier& rSubTypeIdentifier)
{
	return m_rAlgorithm.addOutputParameter(rOutputParameterIdentifier, sOutputName, eParameterType, rSubTypeIdentifier);
}

boolean CAlgorithmProto::addInputTrigger(
	const CIdentifier& rInputTriggerIdentifier,
	const CString& rInputTriggerName)
{
	return m_rAlgorithm.addInputTrigger(rInputTriggerIdentifier, rInputTriggerName);
}

boolean CAlgorithmProto::addOutputTrigger(
	const CIdentifier& rOutputTriggerIdentifier,
	const CString& rOutputTriggerName)
{
	return m_rAlgorithm.addOutputTrigger(rOutputTriggerIdentifier, rOutputTriggerName);
}
