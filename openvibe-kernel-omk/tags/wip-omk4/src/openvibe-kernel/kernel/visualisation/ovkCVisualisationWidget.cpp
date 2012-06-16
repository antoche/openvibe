#include "ovkCVisualisationWidget.h"

#include <vector>

using namespace std;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;

CVisualisationWidget::CVisualisationWidget(const IKernelContext& rKernelContext) :
	TKernelObject<TAttributable<IVisualisationWidget> >(rKernelContext),
	m_oIdentifier(OV_UndefinedIdentifier),
	m_oType(EVisualisationWidget_Undefined),
	m_oParentIdentifier(OV_UndefinedIdentifier),
	m_oBoxIdentifier(OV_UndefinedIdentifier),
	m_pParentWidget(NULL)
{
}

CVisualisationWidget::~CVisualisationWidget(void)
{
}

boolean CVisualisationWidget::initialize(const CIdentifier& rIdentifier, const CString& rName, EVisualisationWidgetType oType,
	const CIdentifier& rParentIdentifier, const CIdentifier& rBoxIdentifier, uint32 ui32NbChildren)
{
	m_oIdentifier = rIdentifier;
	m_oName = rName;
	m_oType = oType;
	m_oParentIdentifier = rParentIdentifier;
	m_oBoxIdentifier = rBoxIdentifier;
	m_vChildren.resize(ui32NbChildren, OV_UndefinedIdentifier);
	return true;
}

CIdentifier CVisualisationWidget::getIdentifier(void) const
{
	return m_oIdentifier;
}

const CString& CVisualisationWidget::getName(void) const
{
	return m_oName;
}

void CVisualisationWidget::setName(const CString& rName)
{
	m_oName = rName;
}

EVisualisationWidgetType CVisualisationWidget::getType(void) const
{
	return m_oType;
}

CIdentifier CVisualisationWidget::getParentIdentifier(void) const
{
	return m_oParentIdentifier;
}

void CVisualisationWidget::setParentIdentifier(const CIdentifier& rParentIdentifier)
{
	m_oParentIdentifier = rParentIdentifier;
}

CIdentifier CVisualisationWidget::getBoxIdentifier(void) const
{
	return m_oBoxIdentifier;
}

uint32 CVisualisationWidget::getNbChildren(void) const
{
	return m_vChildren.size();
}

boolean CVisualisationWidget::getChildIndex(const CIdentifier& rIdentifier, uint32& ui32Index) const
{
	for(ui32Index=0; ui32Index<m_vChildren.size(); ui32Index++)
	{
		if(m_vChildren[ui32Index] == rIdentifier)
		{
			return true;
		}
	}
	return false;
}

boolean CVisualisationWidget::addChild(const CIdentifier& rChildIdentifier)
{
	m_vChildren.push_back(rChildIdentifier);
	return true;
}

boolean CVisualisationWidget::removeChild(const CIdentifier& rIdentifier)
{
	for(unsigned int i=0; i<m_vChildren.size(); i++)
	{
		if(m_vChildren[i] == rIdentifier)
		{
			//remove tab from a window (variable number of children)
			if(m_oType == EVisualisationWidget_VisualisationWindow)
			{
				m_vChildren.erase(m_vChildren.begin() + i);
			}
			else //clear identifier if ith child for a regular widget (fixed number of children)
			{
				m_vChildren[i] = OV_UndefinedIdentifier;
			}
			return true;
		}
	}

	return false;
}

boolean CVisualisationWidget::getChildIdentifier(uint32 ui32ChildIndex, CIdentifier& rIdentifier) const
{
	if(ui32ChildIndex >= m_vChildren.size())
	{
		rIdentifier = OV_UndefinedIdentifier;
		return false;
	}
	else
	{
		rIdentifier = m_vChildren[ui32ChildIndex];
		return true;
	}
}

boolean CVisualisationWidget::setChildIdentifier(uint32 ui32ChildIndex, const CIdentifier& rIdentifier)
{
	if(ui32ChildIndex >= m_vChildren.size())
	{
		return false;
	}
	else
	{
		m_vChildren[ui32ChildIndex] = rIdentifier;
		return true;
	}
}
