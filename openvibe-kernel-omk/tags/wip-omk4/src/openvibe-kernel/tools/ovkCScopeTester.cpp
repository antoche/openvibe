#include "ovkCScopeTester.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Tools;

CScopeTester::CScopeTester(const IKernelContext& rKernelContext, const CString& sPrefix)
	:m_sPrefix(sPrefix)
	,m_rKernelContext(rKernelContext)
{
	m_rKernelContext.getLogManager() << LogLevel_Debug << "## CScopeTester [" << m_sPrefix << "] enter\n";
}

CScopeTester::~CScopeTester(void)
{
	m_rKernelContext.getLogManager() << LogLevel_Debug << "## CScopeTester [" << m_sPrefix << "] leave\n";
}
