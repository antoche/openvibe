#ifndef __OpenViBEDesigner_CAboutPluginDialog_H__
#define __OpenViBEDesigner_CAboutPluginDialog_H__

#include "ovd_base.h"

namespace OpenViBEDesigner
{
	class CAboutPluginDialog
	{
	public:

		CAboutPluginDialog(const OpenViBE::Kernel::IKernelContext& rKernelContext, const OpenViBE::CIdentifier& rPluginClassIdentifier, const char* sGUIFilename);
		virtual ~CAboutPluginDialog(void);

		OpenViBE::boolean run(void);

	protected:

		const OpenViBE::Kernel::IKernelContext& m_rKernelContext;
		OpenViBE::CIdentifier m_oPluginClassIdentifier;
		OpenViBE::CString m_sGUIFilename;

	private:

		CAboutPluginDialog(void);
	};
};

#endif // __OpenViBEDesigner_CAboutPluginDialog_H__
