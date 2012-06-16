#include "ovdCBoxProxy.h"
#include "ovdTAttributeHandler.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBEDesigner;
using namespace std;

CBoxProxy::CBoxProxy(const IBox& rBox)
	:m_pConstBox(&rBox)
	,m_pBox(NULL)
	,m_iXCenter(0)
	,m_iYCenter(0)
{
	if(m_pConstBox)
	{
		TAttributeHandler l_oAttributeHandler(*m_pConstBox);
		m_iXCenter=l_oAttributeHandler.getAttributeValue<int>(OVD_AttributeId_XBoxCenterPosition);
		m_iYCenter=l_oAttributeHandler.getAttributeValue<int>(OVD_AttributeId_YBoxCenterPosition);
	}
}

CBoxProxy::CBoxProxy(IScenario& rScenario, const CIdentifier& rBoxIdentifier)
	:m_pConstBox(rScenario.getBoxDetails(rBoxIdentifier))
	,m_pBox(rScenario.getBoxDetails(rBoxIdentifier))
	,m_iXCenter(0)
	,m_iYCenter(0)
{
	if(m_pConstBox)
	{
		TAttributeHandler l_oAttributeHandler(*m_pConstBox);
		m_iXCenter=l_oAttributeHandler.getAttributeValue<int>(OVD_AttributeId_XBoxCenterPosition);
		m_iYCenter=l_oAttributeHandler.getAttributeValue<int>(OVD_AttributeId_YBoxCenterPosition);
	}
}

CBoxProxy::~CBoxProxy(void)
{
	if(m_pBox)
	{
		TAttributeHandler l_oAttributeHandler(*m_pBox);

		if(l_oAttributeHandler.hasAttribute(OVD_AttributeId_XBoxCenterPosition))
			l_oAttributeHandler.setAttributeValue<int>(OVD_AttributeId_XBoxCenterPosition, m_iXCenter);
		else
			l_oAttributeHandler.addAttribute<int>(OVD_AttributeId_XBoxCenterPosition, m_iXCenter);

		if(l_oAttributeHandler.hasAttribute(OVD_AttributeId_YBoxCenterPosition))
			l_oAttributeHandler.setAttributeValue<int>(OVD_AttributeId_YBoxCenterPosition, m_iYCenter);
		else
			l_oAttributeHandler.addAttribute<int>(OVD_AttributeId_YBoxCenterPosition, m_iYCenter);
	}
}

CBoxProxy::operator IBox* (void)
{
	return m_pBox;
}

CBoxProxy::operator const IBox* (void)
{
	return m_pConstBox;
}

int32 CBoxProxy::getWidth(::GtkWidget* pWidget) const
{
	int x, y;
	updateSize(pWidget, getLabel(), &x, &y);
	return x;
}

int32 CBoxProxy::getHeight(::GtkWidget* pWidget) const
{
	int x, y;
	updateSize(pWidget, getLabel(), &x, &y);
	return y;
}

int32 CBoxProxy::getXCenter(void) const
{
	return m_iXCenter;
}

int32 CBoxProxy::getYCenter(void) const
{
	return m_iYCenter;
}

void CBoxProxy::setCenter(int32 i32XCenter, int32 i32YCenter)
{
	m_iXCenter=i32XCenter;
	m_iYCenter=i32YCenter;
}

const char* CBoxProxy::getLabel(void) const
{
	string l_sBoxHasSettings(m_pConstBox->getSettingCount()!=0?"~":"-");
	string l_sBoxName(m_pConstBox->getName());
	string l_sBoxIden(m_pConstBox->getIdentifier().toString());
	m_sLabel=l_sBoxHasSettings+" "+l_sBoxName+" "+l_sBoxHasSettings; // +"\n"+l_sBoxIden;
	return m_sLabel.c_str();
}

void CBoxProxy::updateSize(::GtkWidget* pWidget, const char* sText, int* pXSize, int* pYSize) const
{
	::PangoContext* l_pPangoContext=NULL;
	::PangoLayout* l_pPangoLayout=NULL;
	::PangoRectangle l_oPangoRectangle;
	l_pPangoContext=gtk_widget_get_pango_context(pWidget);
	l_pPangoLayout=pango_layout_new(l_pPangoContext);
	pango_layout_set_alignment(l_pPangoLayout, PANGO_ALIGN_CENTER);
	pango_layout_set_text(l_pPangoLayout, sText, -1);
	pango_layout_get_pixel_extents(l_pPangoLayout, NULL, &l_oPangoRectangle);
	*pXSize=l_oPangoRectangle.width;
	*pYSize=l_oPangoRectangle.height;
	g_object_unref(l_pPangoContext);
}
