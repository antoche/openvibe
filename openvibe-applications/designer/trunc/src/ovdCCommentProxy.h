#ifndef __OpenViBEDesigner_CCommentProxy_H__
#define __OpenViBEDesigner_CCommentProxy_H__

#include "ovd_base.h"

#include <string>

namespace OpenViBEDesigner
{
	class CCommentProxy
	{
	public:

		CCommentProxy(
			const OpenViBE::Kernel::IKernelContext& rKernelContext,
			const OpenViBE::Kernel::IComment& rComment);
		CCommentProxy(
			const OpenViBE::Kernel::IKernelContext& rKernelContext,
			OpenViBE::Kernel::IScenario& rScenario,
			const OpenViBE::CIdentifier& rCommentIdentifier);
		virtual ~CCommentProxy(void);

		operator OpenViBE::Kernel::IComment* (void);
		operator const OpenViBE::Kernel::IComment* (void);

		OpenViBE::int32 getWidth(
			::GtkWidget* pWidget) const;
		OpenViBE::int32 getHeight(
			::GtkWidget* pWidget) const;

		OpenViBE::int32 getXCenter(void) const;
		OpenViBE::int32 getYCenter(void) const;

		void setCenter(
			OpenViBE::int32 i32XCenter,
			OpenViBE::int32 i32YCenter);

		void apply(void);

		virtual const char* getLabel(void) const;

	protected:

		virtual void updateSize(
			::GtkWidget* pWidget,
			const char* sText,
			int* pXSize,
			int* pYSize) const;

	protected:

		const OpenViBE::Kernel::IKernelContext& m_rKernelContext;
		const OpenViBE::Kernel::IComment* m_pConstComment;
		OpenViBE::Kernel::IComment* m_pComment;
		OpenViBE::boolean m_bApplied;
		int m_iXCenter;
		int m_iYCenter;
		mutable std::string m_sLabel;
	};
};

#endif // __OpenViBEDesigner_CCommentProxy_H__
