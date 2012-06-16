#include "ovpCScenarioExporterSVG.h"

//___________________________________________________________________//
//                                                                   //

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;
using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Samples;
using namespace std;

//___________________________________________________________________//
//                                                                   //

static std::string colorFromIdentifier(const CIdentifier& rIdentifier)
{
	char l_sString[1024];
	unsigned int l_ui32Value1=0;
	unsigned int l_ui32Value2=0;
	uint64 l_ui64Result=0;

	sscanf(rIdentifier.toString(), "(0x%08X, 0x%08X)", &l_ui32Value1, &l_ui32Value2);
	l_ui64Result+=l_ui32Value1;
	l_ui64Result<<=32;
	l_ui64Result+=l_ui32Value2;

	sprintf(l_sString, "#%02x%02x%02x",
		((uint16)(((l_ui64Result    )&0xffff)|0x8000))>>8,
		((uint16)(((l_ui64Result>>16)&0xffff)|0x8000))>>8,
		((uint16)(((l_ui64Result>>32)&0xffff)|0x8000))>>8);

	return l_sString;
}

//___________________________________________________________________//
//                                                                   //

CScenarioExporterSVG::CScenarioExporterSVG(void)
	:m_pWriter(NULL)
{
}

void CScenarioExporterSVG::release(void)
{
	delete this;
}

void CScenarioExporterSVG::write(const char* sString)
{
	m_oFile << sString;
}

void CScenarioExporterSVG::exportBox(const IBox& rBox)
{
	int w, h;
	int x, y;
	sscanf(rBox.getAttributeValue(OV_AttributeId_Box_XCenterPosition).toASCIIString(), "%i", &x);
	sscanf(rBox.getAttributeValue(OV_AttributeId_Box_YCenterPosition).toASCIIString(), "%i", &y);
	sscanf(rBox.getAttributeValue(OV_AttributeId_Box_XSize).toASCIIString(), "%i", &w);
	sscanf(rBox.getAttributeValue(OV_AttributeId_Box_YSize).toASCIIString(), "%i", &h);

	char l_sDimmensions[8][1024];
	sprintf(l_sDimmensions[0], "%i", x-w/2);
	sprintf(l_sDimmensions[1], "%i", y-h/2);
	sprintf(l_sDimmensions[2], "%i", x+w/2);
	sprintf(l_sDimmensions[3], "%i", y+h/2);
	sprintf(l_sDimmensions[4], "%i", w);
	sprintf(l_sDimmensions[5], "%i", h);
	sprintf(l_sDimmensions[6], "%i", x);
	sprintf(l_sDimmensions[7], "%i", y+4);

	m_pWriter->openChild("rect");
	 m_pWriter->setAttribute("width", l_sDimmensions[4]);
	 m_pWriter->setAttribute("height", l_sDimmensions[5]);
	 m_pWriter->setAttribute("x", l_sDimmensions[0]);
	 m_pWriter->setAttribute("y", l_sDimmensions[1]);
	 m_pWriter->setAttribute("rx", "8");
	 m_pWriter->setAttribute("ry", "8");
	 m_pWriter->setAttribute("id", rBox.getIdentifier().toString());
	 m_pWriter->setAttribute("fill", "#f7f7f7");
	m_pWriter->closeChild();
	m_pWriter->openChild("text");
	 m_pWriter->setAttribute("font-family", "Verdana");
	 m_pWriter->setAttribute("font-size", "12");
	 m_pWriter->setAttribute("fill", "black");
	 m_pWriter->setAttribute("stroke", "none");
	 m_pWriter->setAttribute("text-anchor", "middle");
	 m_pWriter->setAttribute("x", l_sDimmensions[6]);
	 m_pWriter->setAttribute("y", l_sDimmensions[7]);
	 if(rBox.getSettingCount()==0)
	 {
	  m_pWriter->setChildData("- " + rBox.getName() + " -");
	 }
	 else
	 {
	  m_pWriter->setChildData("~ " + rBox.getName() + " ~");
	 }
	m_pWriter->closeChild();

	if(rBox.getInputCount()!=0)
	{
		for(uint32 i=0; i<rBox.getInputCount(); i++)
		{
			CIdentifier l_oInputTypeIdentifier;
			CString l_sInputName;
			rBox.getInputType(i, l_oInputTypeIdentifier);
			rBox.getInputName(i, l_sInputName);

			int l_iXOrigin=(int)((x    )+i*15-(rBox.getInputCount()-1)*7.5+0.5);
			int l_iYOrigin=(int)((y-h/2));

			char l_sTriangle[1024];
			sprintf(l_sTriangle, "%i,%i %i,%i %i,%i", l_iXOrigin-4, l_iYOrigin-5, l_iXOrigin+4, l_iYOrigin-5, l_iXOrigin, l_iYOrigin+7);
			m_pWriter->openChild("polygon");
			m_pWriter->setAttribute("points", l_sTriangle);
	 		m_pWriter->setAttribute("fill", colorFromIdentifier(l_oInputTypeIdentifier).c_str());
			m_pWriter->closeChild();
		}
	}

	if(rBox.getOutputCount()!=0)
	{
		for(uint32 i=0; i<rBox.getOutputCount(); i++)
		{
			CIdentifier l_oOutputTypeIdentifier;
			CString l_sOutputName;
			rBox.getOutputType(i, l_oOutputTypeIdentifier);
			rBox.getOutputName(i, l_sOutputName);

			int l_iXOrigin=(int)((x    )+i*15-(rBox.getOutputCount()-1)*7.5+0.5);
			int l_iYOrigin=(int)((y+h/2));

			char l_sTriangle[1024];
			sprintf(l_sTriangle, "%i,%i %i,%i %i,%i", l_iXOrigin-4, l_iYOrigin-5, l_iXOrigin+4, l_iYOrigin-5, l_iXOrigin, l_iYOrigin+7);
			m_pWriter->openChild("polygon");
			m_pWriter->setAttribute("points", l_sTriangle);
	 		m_pWriter->setAttribute("fill", colorFromIdentifier(l_oOutputTypeIdentifier).c_str());
			m_pWriter->closeChild();
		}
	}
}

void CScenarioExporterSVG::exportLink(const ILink& rLink)
{
	m_pWriter->openChild("line");
	 m_pWriter->setAttribute("x1", rLink.getAttributeValue(OV_AttributeId_Link_XSourcePosition));
	 m_pWriter->setAttribute("y1", rLink.getAttributeValue(OV_AttributeId_Link_YSourcePosition));
	 m_pWriter->setAttribute("x2", rLink.getAttributeValue(OV_AttributeId_Link_XTargetPosition));
	 m_pWriter->setAttribute("y2", rLink.getAttributeValue(OV_AttributeId_Link_YTargetPosition));
	 m_pWriter->setAttribute("id", rLink.getIdentifier().toString());
	 // m_pWriter->setAttribute("fill", "none");
	m_pWriter->closeChild();
}

boolean CScenarioExporterSVG::doExport(IScenarioExporterContext& rScenarioExporterContext)
{
	m_pWriter=XML::createWriter(*this);
	if(!m_pWriter)
	{
		return false;
	}

	const IScenario& l_rScenario=rScenarioExporterContext.getScenario();

	m_oFile.open(rScenarioExporterContext.getFileName());

	m_pWriter->openChild("svg");
	 m_pWriter->setAttribute("xmlns", "http://www.w3.org/2000/svg");
	 m_pWriter->openChild("g");
	  m_pWriter->setAttribute("id", "boxes");
	  m_pWriter->setAttribute("stroke", "black");
		CIdentifier l_oBoxIdentifier=l_rScenario.getNextBoxIdentifier(OV_UndefinedIdentifier);
		while(l_oBoxIdentifier!=OV_UndefinedIdentifier)
		{
			exportBox(*l_rScenario.getBoxDetails(l_oBoxIdentifier));
			l_oBoxIdentifier=l_rScenario.getNextBoxIdentifier(l_oBoxIdentifier);
		}
	 m_pWriter->closeChild();
	 m_pWriter->openChild("g");
	  m_pWriter->setAttribute("id", "links");
	  m_pWriter->setAttribute("stroke", "black");
		CIdentifier l_oLinkIdentifier=l_rScenario.getNextLinkIdentifier(OV_UndefinedIdentifier);
		while(l_oLinkIdentifier!=OV_UndefinedIdentifier)
		{
			exportLink(*l_rScenario.getLinkDetails(l_oLinkIdentifier));
			l_oLinkIdentifier=l_rScenario.getNextLinkIdentifier(l_oLinkIdentifier);
		}
	 m_pWriter->closeChild();

	m_pWriter->closeChild();
	m_pWriter->release();
	m_pWriter=NULL;

	m_oFile.close();
	return true;
}
