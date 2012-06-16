#include "ovkCProcessingUnit.h"

#include "../ovkCObjectVisitorContext.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace std;

//___________________________________________________________________//
//                                                                   //

CProcessingUnit::CProcessingUnit(const IKernelContext& rKernelContext)
	:TKernelObject<TAttributable<IProcessingUnit> >(rKernelContext)
{
}

//___________________________________________________________________//
//                                                                   //

boolean CProcessingUnit::setIdentifier(
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

CIdentifier CProcessingUnit::getIdentifier(void) const
{
	return m_oIdentifier;
}

boolean CProcessingUnit::setName(
	const CString& sName)
{
	m_sName=sName;
	return true;
}

CString CProcessingUnit::getName(void) const
{
	return m_sName;
}

//___________________________________________________________________//
//                                                                   //

boolean CProcessingUnit::acceptVisitor(
	IObjectVisitor& rObjectVisitor)
{
	CObjectVisitorContext l_oObjectVisitorContext(getKernelContext());
	return rObjectVisitor.processBegin(l_oObjectVisitorContext, *this) && rObjectVisitor.processEnd(l_oObjectVisitorContext, *this);
}
