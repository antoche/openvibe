#ifndef __OpenViBEDesigner_CAboutScenarioDialog_H__
#define __OpenViBEDesigner_CAboutScenarioDialog_H__

#include "ovd_base.h"

namespace OpenViBEDesigner
{
	class CAboutScenarioDialog
	{
	public:

		CAboutScenarioDialog(const OpenViBE::Kernel::IKernelContext& rKernelContext, OpenViBE::Kernel::IScenario& rScenario, const char* sGUIFilename);
		virtual ~CAboutScenarioDialog(void);

		OpenViBE::boolean run(void);

	protected:

		const OpenViBE::Kernel::IKernelContext& m_rKernelContext;
		OpenViBE::Kernel::IScenario& m_rScenario;
		OpenViBE::CString m_sGUIFilename;

	private:

		CAboutScenarioDialog(void);
	};
};

#endif // __OpenViBEDesigner_CAboutScenarioDialog_H__
