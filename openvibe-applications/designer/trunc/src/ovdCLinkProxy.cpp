#include "ovdCLinkProxy.h"
#include "ovdTAttributeHandler.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBEDesigner;

CLinkProxy::CLinkProxy(const ILink& rLink)
	:m_pConstLink(&rLink)
	,m_pLink(NULL)
	,m_iXSource(0)
	,m_iYSource(0)
	,m_iXTarget(0)
	,m_iYTarget(0)
{
	if(m_pConstLink)
	{
		TAttributeHandler l_oAttributeHandler(*m_pConstLink);
		m_iXSource=l_oAttributeHandler.getAttributeValue<int>(OV_AttributeId_Link_XSourcePosition);
		m_iYSource=l_oAttributeHandler.getAttributeValue<int>(OV_AttributeId_Link_YSourcePosition);
		m_iXTarget=l_oAttributeHandler.getAttributeValue<int>(OV_AttributeId_Link_XTargetPosition);
		m_iYTarget=l_oAttributeHandler.getAttributeValue<int>(OV_AttributeId_Link_YTargetPosition);
	}
}

CLinkProxy::CLinkProxy(IScenario& rScenario, const CIdentifier& rLinkIdentifier)
	:m_pConstLink(rScenario.getLinkDetails(rLinkIdentifier))
	,m_pLink(rScenario.getLinkDetails(rLinkIdentifier))
	,m_iXSource(0)
	,m_iYSource(0)
	,m_iXTarget(0)
	,m_iYTarget(0)
{
	if(m_pConstLink)
	{
		TAttributeHandler l_oAttributeHandler(*m_pConstLink);
		m_iXSource=l_oAttributeHandler.getAttributeValue<int>(OV_AttributeId_Link_XSourcePosition);
		m_iYSource=l_oAttributeHandler.getAttributeValue<int>(OV_AttributeId_Link_YSourcePosition);
		m_iXTarget=l_oAttributeHandler.getAttributeValue<int>(OV_AttributeId_Link_XTargetPosition);
		m_iYTarget=l_oAttributeHandler.getAttributeValue<int>(OV_AttributeId_Link_YTargetPosition);
	}
}

CLinkProxy::~CLinkProxy(void)
{
	if(m_pLink)
	{
		TAttributeHandler l_oAttributeHandler(*m_pLink);

		if(l_oAttributeHandler.hasAttribute(OV_AttributeId_Link_XSourcePosition))
			l_oAttributeHandler.setAttributeValue<int>(OV_AttributeId_Link_XSourcePosition, m_iXSource);
		else
			l_oAttributeHandler.addAttribute<int>(OV_AttributeId_Link_XSourcePosition, m_iXSource);

		if(l_oAttributeHandler.hasAttribute(OV_AttributeId_Link_YSourcePosition))
			l_oAttributeHandler.setAttributeValue<int>(OV_AttributeId_Link_YSourcePosition, m_iYSource);
		else
			l_oAttributeHandler.addAttribute<int>(OV_AttributeId_Link_YSourcePosition, m_iYSource);

		if(l_oAttributeHandler.hasAttribute(OV_AttributeId_Link_XTargetPosition))
			l_oAttributeHandler.setAttributeValue<int>(OV_AttributeId_Link_XTargetPosition, m_iXTarget);
		else
			l_oAttributeHandler.addAttribute<int>(OV_AttributeId_Link_XTargetPosition, m_iXTarget);

		if(l_oAttributeHandler.hasAttribute(OV_AttributeId_Link_YTargetPosition))
			l_oAttributeHandler.setAttributeValue<int>(OV_AttributeId_Link_YTargetPosition, m_iYTarget);
		else
			l_oAttributeHandler.addAttribute<int>(OV_AttributeId_Link_YTargetPosition, m_iYTarget);
	}
}

CLinkProxy::operator ILink* (void)
{
	return m_pLink;
}

CLinkProxy::operator const ILink* (void)
{
	return m_pConstLink;
}

int32 CLinkProxy::getXSource(void)
{
	return m_iXSource;
}

int32 CLinkProxy::getYSource(void)
{
	return m_iYSource;
}

int32 CLinkProxy::getXTarget(void)
{
	return m_iXTarget;
}

int32 CLinkProxy::getYTarget(void)
{
	return m_iYTarget;
}

void CLinkProxy::setSource(int32 i32XSource, int32 i32YSource)
{
	m_iXSource=i32XSource;
	m_iYSource=i32YSource;
}

void CLinkProxy::setTarget(int32 i32XTarget, int32 i32YTarget)
{
	m_iXTarget=i32XTarget;
	m_iYTarget=i32YTarget;
}
