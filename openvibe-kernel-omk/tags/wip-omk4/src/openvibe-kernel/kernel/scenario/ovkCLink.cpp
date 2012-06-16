#include "ovkCLink.h"

#include "../ovkCObjectVisitorContext.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;

//___________________________________________________________________//
//                                                                   //

CLink::CLink(const IKernelContext& rKernelContext)
	:TKernelObject<TAttributable<ILink> >(rKernelContext)
	,m_oIdentifier(OV_UndefinedIdentifier)
	,m_oSourceBoxIdentifier(OV_UndefinedIdentifier)
	,m_oTargetBoxIdentifier(OV_UndefinedIdentifier)
	,m_ui32SourceOutputIndex(0)
	,m_ui32TargetInputIndex(0)
{
}

//___________________________________________________________________//
//                                                                   //

boolean CLink::setIdentifier(
	const CIdentifier& rIdentifier)
{
	m_oIdentifier=rIdentifier;
	return true;
}

CIdentifier CLink::getIdentifier(void) const
{
	return m_oIdentifier;
}

//___________________________________________________________________//
//                                                                   //

boolean CLink::setSource(
	const CIdentifier& rBoxIdentifier,
	const uint32 ui32BoxOutputIndex)
{
	m_oSourceBoxIdentifier=rBoxIdentifier;
	m_ui32SourceOutputIndex=ui32BoxOutputIndex;
	return true;
}

boolean CLink::setTarget(
	const CIdentifier& rBoxIdentifier,
	const uint32 ui32BoxInputIndex)
{
	m_oTargetBoxIdentifier=rBoxIdentifier;
	m_ui32TargetInputIndex=ui32BoxInputIndex;
	return true;
}

boolean CLink::getSource(
	CIdentifier& rBoxIdentifier,
	uint32& ui32BoxOutputIndex) const
{
	rBoxIdentifier=m_oSourceBoxIdentifier;
	ui32BoxOutputIndex=m_ui32SourceOutputIndex;
	return true;
}

CIdentifier CLink::getSourceBoxIdentifier(void) const
{
	return m_oSourceBoxIdentifier;
}

uint32 CLink::getSourceBoxOutputIndex(void) const
{
	return m_ui32SourceOutputIndex;
}

boolean CLink::getTarget(
	CIdentifier& rTargetBoxIdentifier,
	uint32& ui32BoxInputIndex) const
{
	rTargetBoxIdentifier=m_oTargetBoxIdentifier;
	ui32BoxInputIndex=m_ui32TargetInputIndex;
	return true;
}

CIdentifier CLink::getTargetBoxIdentifier(void) const
{
	return m_oTargetBoxIdentifier;
}

uint32 CLink::getTargetBoxInputIndex(void) const
{
	return m_ui32TargetInputIndex;
}

//___________________________________________________________________//
//                                                                   //

boolean CLink::acceptVisitor(
	IObjectVisitor& rObjectVisitor)
{
	CObjectVisitorContext l_oObjectVisitorContext(getKernelContext());
	return rObjectVisitor.processBegin(l_oObjectVisitorContext, *this) && rObjectVisitor.processEnd(l_oObjectVisitorContext, *this);
}
