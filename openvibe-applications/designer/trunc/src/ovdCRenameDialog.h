#ifndef __OpenViBEDesigner_CRenameDialog_H__
#define __OpenViBEDesigner_CRenameDialog_H__

#include "ovd_base.h"

namespace OpenViBEDesigner
{
	class CRenameDialog
	{
	public:

		CRenameDialog(const OpenViBE::Kernel::IKernelContext& rKernelContext, const OpenViBE::CString& rInitialName, const OpenViBE::CString& rDefaultName, const char* sGUIFilename);
		virtual ~CRenameDialog(void);

		OpenViBE::boolean run(void);
		OpenViBE::CString getResult(void);

	protected:

		const OpenViBE::Kernel::IKernelContext& m_rKernelContext;
		OpenViBE::CString m_sInitialName;
		OpenViBE::CString m_sDefaultName;
		OpenViBE::CString m_sResult;
		OpenViBE::CString m_sGUIFilename;

	private:

		CRenameDialog(void);
	};
};

#endif // __OpenViBEDesigner_CRenameDialog_H__
