#ifndef __OpenViBEDesigner_CSettingEditorDialog_H__
#define __OpenViBEDesigner_CSettingEditorDialog_H__

#include "ovdCSettingCollectionHelper.h"

#include <string>
#include <map>

namespace OpenViBEDesigner
{
	class CSettingEditorDialog
	{
	public:

		CSettingEditorDialog(const OpenViBE::Kernel::IKernelContext& rKernelContext, OpenViBE::Kernel::IBox& rBox, OpenViBE::uint32 ui32SettingIndex, const char* sTitle, const char* sGUIFilename, const char* sGUISettingsFilename);
		virtual ~CSettingEditorDialog(void);
		virtual OpenViBE::boolean run(void);

		virtual void typeChangedCB(void);

	protected:

		const OpenViBE::Kernel::IKernelContext& m_rKernelContext;
		OpenViBE::Kernel::IBox& m_rBox;
		OpenViBEDesigner::CSettingCollectionHelper m_oHelper;
		OpenViBE::uint32 m_ui32SettingIndex;
		OpenViBE::CString m_sGUIFilename;
		OpenViBE::CString m_sGUISettingsFilename;
		std::string m_sTitle;
		::GtkWidget* m_pTable;
		::GtkWidget* m_pType;
		::GtkWidget* m_pDefaultValue;
		std::map<std::string, OpenViBE::CIdentifier> m_vSettingTypes;
	};
};

#endif // __OpenViBEDesigner_CSettingEditorDialog_H__
