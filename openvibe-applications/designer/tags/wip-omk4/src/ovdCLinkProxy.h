#ifndef __OpenViBEDesigner_CLinkProxy_H__
#define __OpenViBEDesigner_CLinkProxy_H__

#include "ovd_base.h"

namespace OpenViBEDesigner
{
	class CLinkProxy
	{
	public:

		CLinkProxy(const OpenViBE::Kernel::ILink& rLink);
		CLinkProxy(OpenViBE::Kernel::IScenario& rScenario, const OpenViBE::CIdentifier& rLinkIdentifier);
		virtual ~CLinkProxy(void);

		operator OpenViBE::Kernel::ILink* (void);
		operator const OpenViBE::Kernel::ILink* (void);

		OpenViBE::int32 getXSource(void);
		OpenViBE::int32 getYSource(void);
		OpenViBE::int32 getXTarget(void);
		OpenViBE::int32 getYTarget(void);

		void setSource(OpenViBE::int32 i32XSource, OpenViBE::int32 i32YSource);
		void setTarget(OpenViBE::int32 i32XTarget, OpenViBE::int32 i32YTarget);

	protected:

		const OpenViBE::Kernel::ILink* m_pConstLink;
		OpenViBE::Kernel::ILink* m_pLink;
		int m_iXSource;
		int m_iYSource;
		int m_iXTarget;
		int m_iYTarget;
	};
};

#endif // __OpenViBEDesigner_CLinkProxy_H__
