#include "ovdCBoxProxy.h"
#include "ovdTAttributeHandler.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;
using namespace OpenViBEDesigner;
using namespace std;

CBoxProxy::CBoxProxy(const IKernelContext& rKernelContext, const IBox& rBox)
	:m_rKernelContext(rKernelContext)
	,m_pConstBox(&rBox)
	,m_pBox(NULL)
	,m_bApplied(false)
	,m_iXCenter(0)
	,m_iYCenter(0)
{
	if(m_pConstBox)
	{
		TAttributeHandler l_oAttributeHandler(*m_pConstBox);
		m_iXCenter=l_oAttributeHandler.getAttributeValue<int>(OV_AttributeId_Box_XCenterPosition);
		m_iYCenter=l_oAttributeHandler.getAttributeValue<int>(OV_AttributeId_Box_YCenterPosition);
	}
	m_bShowOriginalNameWhenModified=m_rKernelContext.getConfigurationManager().expandAsBoolean("${Designer_ShowOriginalBoxName}", true);
}

CBoxProxy::CBoxProxy(const IKernelContext& rKernelContext, IScenario& rScenario, const CIdentifier& rBoxIdentifier)
	:m_rKernelContext(rKernelContext)
	,m_pConstBox(rScenario.getBoxDetails(rBoxIdentifier))
	,m_pBox(rScenario.getBoxDetails(rBoxIdentifier))
	,m_bApplied(false)
	,m_iXCenter(0)
	,m_iYCenter(0)
{
	if(m_pConstBox)
	{
		TAttributeHandler l_oAttributeHandler(*m_pConstBox);
		m_iXCenter=l_oAttributeHandler.getAttributeValue<int>(OV_AttributeId_Box_XCenterPosition);
		m_iYCenter=l_oAttributeHandler.getAttributeValue<int>(OV_AttributeId_Box_YCenterPosition);
	}
	m_bShowOriginalNameWhenModified=m_rKernelContext.getConfigurationManager().expandAsBoolean("${Designer_ShowOriginalBoxName}", true);
}

CBoxProxy::~CBoxProxy(void)
{
	if(!m_bApplied)
	{
		this->apply();
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
	m_bApplied=false;
}

void CBoxProxy::apply(void)
{
	if(m_pBox)
	{
		TAttributeHandler l_oAttributeHandler(*m_pBox);

		if(l_oAttributeHandler.hasAttribute(OV_AttributeId_Box_XCenterPosition))
			l_oAttributeHandler.setAttributeValue<int>(OV_AttributeId_Box_XCenterPosition, m_iXCenter);
		else
			l_oAttributeHandler.addAttribute<int>(OV_AttributeId_Box_XCenterPosition, m_iXCenter);

		if(l_oAttributeHandler.hasAttribute(OV_AttributeId_Box_YCenterPosition))
			l_oAttributeHandler.setAttributeValue<int>(OV_AttributeId_Box_YCenterPosition, m_iYCenter);
		else
			l_oAttributeHandler.addAttribute<int>(OV_AttributeId_Box_YCenterPosition, m_iYCenter);

		m_bApplied=true;
	}
}

const char* CBoxProxy::getLabel(void) const
{
	boolean l_bBoxCanChangeInput  (m_pConstBox->hasAttribute(OV_AttributeId_Box_FlagCanModifyInput)  ||m_pConstBox->hasAttribute(OV_AttributeId_Box_FlagCanAddInput));
	boolean l_bBoxCanChangeOutput (m_pConstBox->hasAttribute(OV_AttributeId_Box_FlagCanModifyOutput) ||m_pConstBox->hasAttribute(OV_AttributeId_Box_FlagCanAddOutput));
	boolean l_bBoxCanChangeSetting(m_pConstBox->hasAttribute(OV_AttributeId_Box_FlagCanModifySetting)||m_pConstBox->hasAttribute(OV_AttributeId_Box_FlagCanAddSetting));
	boolean l_bBoxIsUpToDate      (this->isBoxAlgorithmPluginPresent()  ? this->isUpToDate() : true);
	boolean l_bBoxIsDeprecated    (this->isBoxAlgorithmPluginPresent() && this->isDeprecated());
	boolean l_bBoxIsUnstable      (this->isBoxAlgorithmPluginPresent() && this->isUnstable());

	const IPluginObjectDesc* l_pDesc=m_rKernelContext.getPluginManager().getPluginObjectDescCreating(m_pConstBox->getAlgorithmClassIdentifier());

	string l_sBoxName(m_pConstBox->getName());
	string l_sBoxIden(m_pConstBox->getIdentifier().toString());

	string l_sRed("#602020");
	string l_sGreen("#206020");
	string l_sBlue("#202060");
	string l_sGrey("#404040");

	m_sLabel=l_sBoxName;

	if(m_pConstBox->getSettingCount()!=0)
	{
		m_sLabel="<span weight=\"bold\">"+m_sLabel+"</span>";
	}

	if(m_bShowOriginalNameWhenModified)
	{
		string l_sBoxOriginalName(l_pDesc?string(l_pDesc->getName()):l_sBoxName);
		if(l_sBoxOriginalName!=l_sBoxName)
		{
			m_sLabel="<small><i><span foreground=\""+l_sGrey+"\">"+l_sBoxOriginalName+"</span></i></small>\n"+m_sLabel;
		}
	}

	if(l_bBoxCanChangeInput || l_bBoxCanChangeOutput || l_bBoxCanChangeSetting)
	{
		m_sLabel+="\n";
		m_sLabel+="<span size=\"smaller\">";
		m_sLabel+="<span foreground=\""+(l_bBoxCanChangeInput?l_sGreen:l_sRed)+"\">In</span>";
		m_sLabel+="|";
		m_sLabel+="<span foreground=\""+(l_bBoxCanChangeOutput?l_sGreen:l_sRed)+"\">Out</span>";
		m_sLabel+="|";
		m_sLabel+="<span foreground=\""+(l_bBoxCanChangeSetting?l_sGreen:l_sRed)+"\">Set</span>";
		m_sLabel+="</span>";
	}

	if(l_bBoxIsDeprecated || l_bBoxIsUnstable || !l_bBoxIsUpToDate)
	{
		m_sLabel+="\n";
		m_sLabel+="<span size=\"smaller\" foreground=\""+l_sBlue+"\">";
		if(l_bBoxIsDeprecated) m_sLabel+=" <span style=\"italic\">deprecated</span>";
		if(l_bBoxIsUnstable)   m_sLabel+=" <span style=\"italic\">unstable</span>";
		if(!l_bBoxIsUpToDate)  m_sLabel+=" <span style=\"italic\">update</span>";
		m_sLabel+=" </span>";
	}
	return m_sLabel.c_str();
}

boolean CBoxProxy::isBoxAlgorithmPluginPresent(void) const
{
	return m_rKernelContext.getPluginManager().canCreatePluginObject(m_pConstBox->getAlgorithmClassIdentifier());
}

boolean CBoxProxy::isUpToDate(void) const
{
	CIdentifier l_oBoxHashCode1;
	CIdentifier l_oBoxHashCode2;
	l_oBoxHashCode1=m_rKernelContext.getPluginManager().getPluginObjectHashValue(m_pConstBox->getAlgorithmClassIdentifier());
	l_oBoxHashCode2.fromString(m_pConstBox->getAttributeValue(OV_AttributeId_Box_InitialPrototypeHashValue));
	return l_oBoxHashCode1==OV_UndefinedIdentifier || (l_oBoxHashCode1!=OV_UndefinedIdentifier && l_oBoxHashCode1==l_oBoxHashCode2);
}

boolean CBoxProxy::isDeprecated(void) const
{
	return m_rKernelContext.getPluginManager().isPluginObjectFlaggedAsDeprecated(m_pConstBox->getAlgorithmClassIdentifier());
}

boolean CBoxProxy::isUnstable(void) const
{
	return m_rKernelContext.getPluginManager().isPluginObjectFlaggedAsUnstable  (m_pConstBox->getAlgorithmClassIdentifier());
}

void CBoxProxy::updateSize(::GtkWidget* pWidget, const char* sText, int* pXSize, int* pYSize) const
{
	::PangoContext* l_pPangoContext=NULL;
	::PangoLayout* l_pPangoLayout=NULL;
	::PangoRectangle l_oPangoRectangle;
	l_pPangoContext=gtk_widget_create_pango_context(pWidget);
	l_pPangoLayout=pango_layout_new(l_pPangoContext);
	pango_layout_set_alignment(l_pPangoLayout, PANGO_ALIGN_CENTER);
	pango_layout_set_markup(l_pPangoLayout, sText, -1);
	pango_layout_get_pixel_extents(l_pPangoLayout, NULL, &l_oPangoRectangle);
	*pXSize=l_oPangoRectangle.width;
	*pYSize=l_oPangoRectangle.height;
	g_object_unref(l_pPangoLayout);
	g_object_unref(l_pPangoContext);
}
