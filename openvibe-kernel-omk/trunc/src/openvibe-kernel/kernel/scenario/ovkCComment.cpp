#include "ovkCComment.h"
#include "ovkCScenario.h"

#include "../ovkCObjectVisitorContext.h"

using namespace std;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

//___________________________________________________________________//
//                                                                   //

CComment::CComment(const IKernelContext& rKernelContext, CScenario& rOwnerScenario)
	:TAttributable < TKernelObject < IComment > >(rKernelContext)
	,m_rOwnerScenario(rOwnerScenario)
	,m_oIdentifier(OV_UndefinedIdentifier)
	,m_sText("")
{
}

CComment::~CComment(void)
{
}

//___________________________________________________________________//
//                                                                   //

CIdentifier CComment::getIdentifier(void) const
{
	return m_oIdentifier;
}

CString CComment::getText(void) const
{
	return m_sText;
}

boolean CComment::setIdentifier(
	const CIdentifier& rIdentifier)
{
	if(m_oIdentifier!=OV_UndefinedIdentifier)
	{
		return false;
	}
	if(rIdentifier==OV_UndefinedIdentifier)
	{
		return false;
	}
	m_oIdentifier=rIdentifier;

	return true;
}

boolean CComment::setText(
	const CString& sText)
{
	m_sText=sText;

	return true;
}

//___________________________________________________________________//
//                                                                   //

boolean CComment::initializeFromExistingComment(
	const IComment& rExisitingComment)
{
	m_sText=rExisitingComment.getText();

	CIdentifier l_oIdentifier=rExisitingComment.getNextAttributeIdentifier(OV_UndefinedIdentifier);
	while(l_oIdentifier!=OV_UndefinedIdentifier)
	{
		addAttribute(l_oIdentifier, rExisitingComment.getAttributeValue(l_oIdentifier));
		l_oIdentifier=rExisitingComment.getNextAttributeIdentifier(l_oIdentifier);
	}

	return true;
}

//___________________________________________________________________//
//                                                                   //

boolean CComment::acceptVisitor(
	IObjectVisitor& rObjectVisitor)
{
	CObjectVisitorContext l_oObjectVisitorContext(getKernelContext());
	return rObjectVisitor.processBegin(l_oObjectVisitorContext, *this) && rObjectVisitor.processEnd(l_oObjectVisitorContext, *this);
}
