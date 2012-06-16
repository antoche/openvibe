#ifndef __OpenViBEDesigner_CBoxProxy_H__
#define __OpenViBEDesigner_CBoxProxy_H__

#include "ovd_base.h"

#include <string>

namespace OpenViBEDesigner
{
	class CBoxProxy
	{
	public:

		CBoxProxy(
			const OpenViBE::Kernel::IBox& rBox);
		CBoxProxy(
			OpenViBE::Kernel::IScenario& rScenario,
			const OpenViBE::CIdentifier& rBoxIdentifier);
		virtual ~CBoxProxy(void);

		operator OpenViBE::Kernel::IBox* (void);
		operator const OpenViBE::Kernel::IBox* (void);

		OpenViBE::int32 getWidth(
			::GtkWidget* pWidget) const;
		OpenViBE::int32 getHeight(
			::GtkWidget* pWidget) const;

		OpenViBE::int32 getXCenter(void) const;
		OpenViBE::int32 getYCenter(void) const;

		void setCenter(
			OpenViBE::int32 i32XCenter,
			OpenViBE::int32 i32YCenter);

		virtual const char* getLabel(void) const;

	protected:

		virtual void updateSize(
			::GtkWidget* pWidget,
			const char* sText,
			int* pXSize,
			int* pYSize) const;

	protected:

		const OpenViBE::Kernel::IBox* m_pConstBox;
		OpenViBE::Kernel::IBox* m_pBox;
		int m_iXCenter;
		int m_iYCenter;
		mutable std::string m_sLabel;
	};
};

#endif // __OpenViBEDesigner_CBoxProxy_H__
