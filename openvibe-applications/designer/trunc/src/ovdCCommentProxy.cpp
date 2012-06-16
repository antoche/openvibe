#include "ovdCCommentProxy.h"
#include "ovdTAttributeHandler.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBEDesigner;
using namespace std;

CCommentProxy::CCommentProxy(const IKernelContext& rKernelContext, const IComment& rComment)
	:m_rKernelContext(rKernelContext)
	,m_pConstComment(&rComment)
	,m_pComment(NULL)
	,m_bApplied(false)
	,m_iXCenter(0)
	,m_iYCenter(0)
{
	if(m_pConstComment)
	{
		TAttributeHandler l_oAttributeHandler(*m_pConstComment);
		m_iXCenter=l_oAttributeHandler.getAttributeValue<int>(OV_AttributeId_Comment_XCenterPosition);
		m_iYCenter=l_oAttributeHandler.getAttributeValue<int>(OV_AttributeId_Comment_YCenterPosition);
	}
}

CCommentProxy::CCommentProxy(const IKernelContext& rKernelContext, IScenario& rScenario, const CIdentifier& rCommentIdentifier)
	:m_rKernelContext(rKernelContext)
	,m_pConstComment(rScenario.getCommentDetails(rCommentIdentifier))
	,m_pComment(rScenario.getCommentDetails(rCommentIdentifier))
	,m_iXCenter(0)
	,m_iYCenter(0)
{
	if(m_pConstComment)
	{
		TAttributeHandler l_oAttributeHandler(*m_pConstComment);
		m_iXCenter=l_oAttributeHandler.getAttributeValue<int>(OV_AttributeId_Comment_XCenterPosition);
		m_iYCenter=l_oAttributeHandler.getAttributeValue<int>(OV_AttributeId_Comment_YCenterPosition);
	}
}

CCommentProxy::~CCommentProxy(void)
{
	if(!m_bApplied)
	{
		this->apply();
	}
}

CCommentProxy::operator IComment* (void)
{
	return m_pComment;
}

CCommentProxy::operator const IComment* (void)
{
	return m_pConstComment;
}

int32 CCommentProxy::getWidth(::GtkWidget* pWidget) const
{
	int x, y;
	updateSize(pWidget, getLabel(), &x, &y);
	return x;
}

int32 CCommentProxy::getHeight(::GtkWidget* pWidget) const
{
	int x, y;
	updateSize(pWidget, getLabel(), &x, &y);
	return y;
}

int32 CCommentProxy::getXCenter(void) const
{
	return m_iXCenter;
}

int32 CCommentProxy::getYCenter(void) const
{
	return m_iYCenter;
}

void CCommentProxy::setCenter(int32 i32XCenter, int32 i32YCenter)
{
	m_iXCenter=i32XCenter;
	m_iYCenter=i32YCenter;
	m_bApplied=false;
}

void CCommentProxy::apply(void)
{
	if(m_pComment)
	{
		TAttributeHandler l_oAttributeHandler(*m_pComment);

		if(l_oAttributeHandler.hasAttribute(OV_AttributeId_Comment_XCenterPosition))
			l_oAttributeHandler.setAttributeValue<int>(OV_AttributeId_Comment_XCenterPosition, m_iXCenter);
		else
			l_oAttributeHandler.addAttribute<int>(OV_AttributeId_Comment_XCenterPosition, m_iXCenter);

		if(l_oAttributeHandler.hasAttribute(OV_AttributeId_Comment_YCenterPosition))
			l_oAttributeHandler.setAttributeValue<int>(OV_AttributeId_Comment_YCenterPosition, m_iYCenter);
		else
			l_oAttributeHandler.addAttribute<int>(OV_AttributeId_Comment_YCenterPosition, m_iYCenter);
		m_bApplied=true;
	}
}

const char* CCommentProxy::getLabel(void) const
{
	m_sLabel=m_pConstComment->getText().toASCIIString();
	return m_sLabel.c_str();
}

void CCommentProxy::updateSize(::GtkWidget* pWidget, const char* sText, int* pXSize, int* pYSize) const
{
	::PangoContext* l_pPangoContext=NULL;
	::PangoLayout* l_pPangoLayout=NULL;
	::PangoRectangle l_oPangoRectangle;
	l_pPangoContext=gtk_widget_create_pango_context(pWidget);
	l_pPangoLayout=pango_layout_new(l_pPangoContext);
	pango_layout_set_alignment(l_pPangoLayout, PANGO_ALIGN_CENTER);
	if(pango_parse_markup(sText, -1, 0, NULL, NULL, NULL, NULL))
	{
		pango_layout_set_markup(l_pPangoLayout, sText, -1);
	}
	else
	{
		pango_layout_set_text(l_pPangoLayout, sText, -1);
	}
	pango_layout_get_pixel_extents(l_pPangoLayout, NULL, &l_oPangoRectangle);
	*pXSize=l_oPangoRectangle.width;
	*pYSize=l_oPangoRectangle.height;
	g_object_unref(l_pPangoLayout);
	g_object_unref(l_pPangoContext);
}
