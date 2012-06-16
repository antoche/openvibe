#include "ovkCScenarioExporterContext.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;

CScenarioExporterContext::CScenarioExporterContext(
	const IKernelContext& rKernelContext,
	const CString& sFileName,
	const IScenario& rScenario)
	:TKernelObject<IScenarioExporterContext>(rKernelContext)
	,m_sFileName(sFileName)
	,m_rScenario(rScenario)
{
}

const CString CScenarioExporterContext::getFileName(void) const
{
	return m_sFileName;
}

const IScenario& CScenarioExporterContext::getScenario(void) const
{
	return m_rScenario;
}
