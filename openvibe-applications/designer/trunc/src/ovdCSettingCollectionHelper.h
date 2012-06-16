#ifndef __OpenViBEDesigner_CSettingCollectionHelper_H__
#define __OpenViBEDesigner_CSettingCollectionHelper_H__

#include "ovd_base.h"

#include <string>

namespace OpenViBEDesigner
{
	class CSettingCollectionHelper
	{
	public:

		CSettingCollectionHelper(const OpenViBE::Kernel::IKernelContext& rKernelContext, const char* sGUIFilename);
		virtual ~CSettingCollectionHelper(void);

		OpenViBE::CString getSettingWidgetName(const OpenViBE::CIdentifier& rTypeIdentifier);

		OpenViBE::CString getValue(const OpenViBE::CIdentifier& rTypeIdentifier, ::GtkWidget* pWidget);
		OpenViBE::CString getValueBoolean(::GtkWidget* pWidget);
		OpenViBE::CString getValueInteger(::GtkWidget* pWidget);
		OpenViBE::CString getValueFloat(::GtkWidget* pWidget);
		OpenViBE::CString getValueString(::GtkWidget* pWidget);
		OpenViBE::CString getValueFilename(::GtkWidget* pWidget);
		OpenViBE::CString getValueScript(::GtkWidget* pWidget);
		OpenViBE::CString getValueColor(::GtkWidget* pWidget);
		OpenViBE::CString getValueColorGradient(::GtkWidget* pWidget);
		OpenViBE::CString getValueEnumeration(const OpenViBE::CIdentifier& rTypeIdentifier, ::GtkWidget* pWidget);
		OpenViBE::CString getValueBitMask(const OpenViBE::CIdentifier& rTypeIdentifier, ::GtkWidget* pWidget);

		void setValue(const OpenViBE::CIdentifier& rTypeIdentifier, ::GtkWidget* pWidget, const OpenViBE::CString& rValue);
		void setValueBoolean(::GtkWidget* pWidget, const OpenViBE::CString& rValue);
		void setValueInteger(::GtkWidget* pWidget, const OpenViBE::CString& rValue);
		void setValueFloat(::GtkWidget* pWidget, const OpenViBE::CString& rValue);
		void setValueString(::GtkWidget* pWidget, const OpenViBE::CString& rValue);
		void setValueFilename(::GtkWidget* pWidget, const OpenViBE::CString& rValue);
		void setValueScript(::GtkWidget* pWidget, const OpenViBE::CString& rValue);
		void setValueColor(::GtkWidget* pWidget, const OpenViBE::CString& rValue);
		void setValueColorGradient(::GtkWidget* pWidget, const OpenViBE::CString& rValue);
		void setValueEnumeration(const OpenViBE::CIdentifier& rTypeIdentifier, ::GtkWidget* pWidget, const OpenViBE::CString& rValue);
		void setValueBitMask(const OpenViBE::CIdentifier& rTypeIdentifier, ::GtkWidget* pWidget, const OpenViBE::CString& rValue);

	public:

		const OpenViBE::Kernel::IKernelContext& m_rKernelContext;
		OpenViBE::CString m_sGUIFilename;

	private:

		CSettingCollectionHelper(void);
	};
};

#endif // __OpenViBEDesigner_CSettingCollectionHelper_H__
