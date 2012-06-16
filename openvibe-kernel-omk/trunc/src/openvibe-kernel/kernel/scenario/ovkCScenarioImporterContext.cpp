#include "ovkCScenarioImporterContext.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;

CScenarioImporterContext::CScenarioImporterContext(
	const IKernelContext& rKernelContext,
	const CString& sFileName,
	IScenario& rScenario)
	:TKernelObject<IScenarioImporterContext>(rKernelContext)
	,m_sFileName(sFileName)
	,m_rScenario(rScenario)
{
}

CString CScenarioImporterContext::getFileName(void) const
{
	return m_sFileName;
}

IScenario& CScenarioImporterContext::getScenario(void) const
{
	return m_rScenario;
}
