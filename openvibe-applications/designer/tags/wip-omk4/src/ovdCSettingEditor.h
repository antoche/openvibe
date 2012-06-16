#ifndef __OpenViBEDesigner_CSettingEditor_H__
#define __OpenViBEDesigner_CSettingEditor_H__

#include "ovd_base.h"

#include <string>

namespace OpenViBEDesigner
{
	class CSettingEditor
	{
	public:

		CSettingEditor(OpenViBE::Kernel::IKernel& rKernel, OpenViBE::Kernel::IBox& rBox, const char* sGUIFilename);
		virtual ~CSettingEditor(void);
		virtual void run(void);

	protected:

		std::string getSettingWidgetName(const OpenViBE::CIdentifier& rTypeIdentifier);

		OpenViBE::CString getValue(const OpenViBE::CIdentifier& rTypeIdentifier, ::GtkWidget* pWidget);
		OpenViBE::CString getValueBoolean(::GtkWidget* pWidget);
		OpenViBE::CString getValueInteger(::GtkWidget* pWidget);
		OpenViBE::CString getValueFloat(::GtkWidget* pWidget);
		OpenViBE::CString getValueString(::GtkWidget* pWidget);
		OpenViBE::CString getValueFilename(::GtkWidget* pWidget);
		OpenViBE::CString getValueEnumeration(const OpenViBE::CIdentifier& rTypeIdentifier, ::GtkWidget* pWidget);
		OpenViBE::CString getValueBitMask(const OpenViBE::CIdentifier& rTypeIdentifier, ::GtkWidget* pWidget);

		void setValue(const OpenViBE::CIdentifier& rTypeIdentifier, ::GtkWidget* pWidget, const OpenViBE::CString& rValue);
		void setValueBoolean(::GtkWidget* pWidget, const OpenViBE::CString& rValue);
		void setValueInteger(::GtkWidget* pWidget, const OpenViBE::CString& rValue);
		void setValueFloat(::GtkWidget* pWidget, const OpenViBE::CString& rValue);
		void setValueString(::GtkWidget* pWidget, const OpenViBE::CString& rValue);
		void setValueFilename(::GtkWidget* pWidget, const OpenViBE::CString& rValue);
		void setValueEnumeration(const OpenViBE::CIdentifier& rTypeIdentifier, ::GtkWidget* pWidget, const OpenViBE::CString& rValue);
		void setValueBitMask(const OpenViBE::CIdentifier& rTypeIdentifier, ::GtkWidget* pWidget, const OpenViBE::CString& rValue);

	protected:

		OpenViBE::Kernel::IKernel& m_rKernel;
		OpenViBE::Kernel::IBox& m_rBox;
		const std::string m_sGUIFilename;
	};
};

#endif // __OpenViBEDesigner_CSettingEditor_H__
