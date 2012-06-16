#include "ovpCScenarioExporterXML.h"

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

class CVisualisationTreeExporterVisitor : public IObjectVisitor
{
public:
	CVisualisationTreeExporterVisitor(XML::IWriter& rWriter) :
		m_pObjectVisitorContext(NULL),
		m_rWriter(rWriter),
		m_pVisualisationTree(NULL)
	{}

	virtual boolean processBegin(IObjectVisitorContext& rObjectVisitorContext, IVisualisationTree& rVisualisationTree)
	{
		m_pObjectVisitorContext=&rObjectVisitorContext;

		m_rWriter.openChild("VisualisationTree");

		m_pVisualisationTree = &rVisualisationTree;

		return true;
	}

	virtual boolean processEnd(IObjectVisitorContext& rObjectVisitorContext, IVisualisationTree& rVisualisationTree)
	{
		m_pObjectVisitorContext=&rObjectVisitorContext;

		m_rWriter.closeChild();

		m_pVisualisationTree = NULL;

		return true;
	}

	virtual boolean processBegin(IObjectVisitorContext& rObjectVisitorContext, IVisualisationWidget& rVisualisationWidget)
	{
		char l_sBuffer[1024];

		m_pObjectVisitorContext=&rObjectVisitorContext;

		m_rWriter.openChild("VisualisationWidget");

		m_rWriter.openChild("Identifier");
		m_rWriter.setChildData(rVisualisationWidget.getIdentifier().toString());
		m_rWriter.closeChild();

		//visualisation box name can be retrieved from corresponding IBox, so we can skip it for these
		if(rVisualisationWidget.getType() != EVisualisationWidget_VisualisationBox)
		{
			m_rWriter.openChild("Name");
			m_rWriter.setChildData(rVisualisationWidget.getName());
			m_rWriter.closeChild();
		}

		m_rWriter.openChild("Type");
		sprintf(l_sBuffer, "%d", (int)rVisualisationWidget.getType());
		m_rWriter.setChildData(l_sBuffer);
		m_rWriter.closeChild();

		m_rWriter.openChild("ParentIdentifier");
		m_rWriter.setChildData(rVisualisationWidget.getParentIdentifier().toString());
		m_rWriter.closeChild();

		uint32 l_ui32ChildIndex=0;
		IVisualisationWidget* l_pParentVisualisationWidget = m_pVisualisationTree->getVisualisationWidget(rVisualisationWidget.getParentIdentifier());
		if(l_pParentVisualisationWidget != NULL)
		{
			l_pParentVisualisationWidget->getChildIndex(rVisualisationWidget.getIdentifier(), l_ui32ChildIndex);
		}
		m_rWriter.openChild("Index");
		sprintf(l_sBuffer, "%d", (int)l_ui32ChildIndex);
		m_rWriter.setChildData(l_sBuffer);
		m_rWriter.closeChild();

		m_rWriter.openChild("BoxIdentifier");
		m_rWriter.setChildData(rVisualisationWidget.getBoxIdentifier().toString());
		m_rWriter.closeChild();

		m_rWriter.openChild("NumChildren");
		sprintf(l_sBuffer, "%d", (int)rVisualisationWidget.getNbChildren());
		m_rWriter.setChildData(l_sBuffer);
		m_rWriter.closeChild();

		if(rVisualisationWidget.hasAttributes())
		{
			m_rWriter.openChild("Attributes");
				CIdentifier l_oAttributeIdentifier=rVisualisationWidget.getNextAttributeIdentifier(OV_UndefinedIdentifier);
				while(l_oAttributeIdentifier!=OV_UndefinedIdentifier)
				{
					exportAttribute(l_oAttributeIdentifier, rVisualisationWidget.getAttributeValue(l_oAttributeIdentifier));
					l_oAttributeIdentifier=rVisualisationWidget.getNextAttributeIdentifier(l_oAttributeIdentifier);
				}
			m_rWriter.closeChild();
		}

		return true;
	}

	virtual boolean processEnd(IObjectVisitorContext& rObjectVisitorContext, IVisualisationWidget& rVisualisationWidget)
	{
		m_pObjectVisitorContext=&rObjectVisitorContext;

		m_rWriter.closeChild();

		return true;
	}

	void exportAttribute(const CIdentifier& rAttributeIdentifier, const CString& rAttributeValue)
	{
		m_rWriter.openChild("Attribute");
		 m_rWriter.openChild("Identifier");
			m_rWriter.setChildData(rAttributeIdentifier.toString());
		 m_rWriter.closeChild();
		 m_rWriter.openChild("Value");
			m_rWriter.setChildData(rAttributeValue);
		 m_rWriter.closeChild();
		m_rWriter.closeChild();
	}

	IObjectVisitorContext* m_pObjectVisitorContext;
	XML::IWriter& m_rWriter;
	IVisualisationTree* m_pVisualisationTree;

#undef boolean
	_IsDerivedFromClass_Final_(IObjectVisitor, OV_UndefinedIdentifier);
#define boolean OpenViBE::boolean
};

//___________________________________________________________________//
//                                                                   //

CScenarioExporterXML::CScenarioExporterXML(void)
	:m_pWriter(NULL)
{
}

void CScenarioExporterXML::release(void)
{
	delete this;
}

void CScenarioExporterXML::write(const char* sString)
{
	m_oFile << sString;
}

void CScenarioExporterXML::exportAttribute(const CIdentifier& rAttributeIdentifier, const CString& rAttributeValue)
{
	m_pWriter->openChild("Attribute");
	 m_pWriter->openChild("Identifier");
	  m_pWriter->setChildData(rAttributeIdentifier.toString());
	 m_pWriter->closeChild();
	 m_pWriter->openChild("Value");
	  m_pWriter->setChildData(rAttributeValue);
	 m_pWriter->closeChild();
	m_pWriter->closeChild();
}

void CScenarioExporterXML::exportBox(const IBox& rBox)
{
	m_pWriter->openChild("Box");
	 m_pWriter->openChild("Identifier");
	  m_pWriter->setChildData(rBox.getIdentifier().toString());
	 m_pWriter->closeChild();
	 m_pWriter->openChild("Name");
	  m_pWriter->setChildData(rBox.getName());
	 m_pWriter->closeChild();
	 m_pWriter->openChild("AlgorithmClassIdentifier");
	  m_pWriter->setChildData(rBox.getAlgorithmClassIdentifier().toString());
	 m_pWriter->closeChild();

	if(rBox.getInputCount()!=0)
	{
		m_pWriter->openChild("Inputs");
		for(uint32 i=0; i<rBox.getInputCount(); i++)
		{
			CIdentifier l_oInputTypeIdentifier;
			CString l_sInputName;
			rBox.getInputType(i, l_oInputTypeIdentifier);
			rBox.getInputName(i, l_sInputName);
			m_pWriter->openChild("Input");
			 m_pWriter->openChild("TypeIdentifier");
			  m_pWriter->setChildData(l_oInputTypeIdentifier.toString());
			 m_pWriter->closeChild();
			 m_pWriter->openChild("Name");
			  m_pWriter->setChildData(l_sInputName);
			 m_pWriter->closeChild();
			m_pWriter->closeChild();
		}
		m_pWriter->closeChild();
	}

	if(rBox.getOutputCount()!=0)
	{
		m_pWriter->openChild("Outputs");
		for(uint32 i=0; i<rBox.getOutputCount(); i++)
		{
			CIdentifier l_oOutputTypeIdentifier;
			CString l_sOutputName;
			rBox.getOutputType(i, l_oOutputTypeIdentifier);
			rBox.getOutputName(i, l_sOutputName);
			m_pWriter->openChild("Output");
			 m_pWriter->openChild("TypeIdentifier");
			  m_pWriter->setChildData(l_oOutputTypeIdentifier.toString());
			 m_pWriter->closeChild();
			 m_pWriter->openChild("Name");
			  m_pWriter->setChildData(l_sOutputName);
			 m_pWriter->closeChild();
			m_pWriter->closeChild();
		}
		m_pWriter->closeChild();
	}

	if(rBox.getSettingCount()!=0)
	{
		m_pWriter->openChild("Settings");
		for(uint32 i=0; i<rBox.getSettingCount(); i++)
		{
			CIdentifier l_oSettingTypeIdentifier;
			CString l_sSettingName;
			CString l_sDefaultValue;
			CString l_sValue;
			rBox.getSettingType(i, l_oSettingTypeIdentifier);
			rBox.getSettingName(i, l_sSettingName);
			rBox.getSettingDefaultValue(i, l_sDefaultValue);
			rBox.getSettingValue(i, l_sValue);
			m_pWriter->openChild("Setting");
			 m_pWriter->openChild("TypeIdentifier");
			  m_pWriter->setChildData(l_oSettingTypeIdentifier.toString());
			 m_pWriter->closeChild();
			 m_pWriter->openChild("Name");
			  m_pWriter->setChildData(l_sSettingName);
			 m_pWriter->closeChild();
			 m_pWriter->openChild("DefaultValue");
			  m_pWriter->setChildData(l_sDefaultValue);
			 m_pWriter->closeChild();
			 m_pWriter->openChild("Value");
			  m_pWriter->setChildData(l_sValue);
			 m_pWriter->closeChild();
			m_pWriter->closeChild();
		}
		m_pWriter->closeChild();
	}

	if(rBox.hasAttributes())
	{
		m_pWriter->openChild("Attributes");
			CIdentifier l_oAttributeIdentifier=rBox.getNextAttributeIdentifier(OV_UndefinedIdentifier);
			while(l_oAttributeIdentifier!=OV_UndefinedIdentifier)
			{
				exportAttribute(l_oAttributeIdentifier, rBox.getAttributeValue(l_oAttributeIdentifier));
				l_oAttributeIdentifier=rBox.getNextAttributeIdentifier(l_oAttributeIdentifier);
			}
		m_pWriter->closeChild();
	}

	m_pWriter->closeChild();
}

void CScenarioExporterXML::exportLink(const ILink& rLink)
{
	CIdentifier l_oSourceBoxIdentifier;
	CIdentifier l_oTargetBoxIdentifier;
	uint32 l_ui32SourceBoxOutputIndex=0xffffffff;
	uint32 l_ui32TargetBoxInputIndex=0xffffffff;

	rLink.getSource(l_oSourceBoxIdentifier, l_ui32SourceBoxOutputIndex);
	rLink.getTarget(l_oTargetBoxIdentifier, l_ui32TargetBoxInputIndex);

	char l_sSourceBoxOutputIndex[1024];
	char l_sTargetBoxInputIndex[1024];
	sprintf(l_sSourceBoxOutputIndex, "%i", (int)l_ui32SourceBoxOutputIndex);
	sprintf(l_sTargetBoxInputIndex, "%i", (int)l_ui32TargetBoxInputIndex);

	m_pWriter->openChild("Link");
	 m_pWriter->openChild("Identifier");
	  m_pWriter->setChildData(rLink.getIdentifier().toString());
	 m_pWriter->closeChild();
	 m_pWriter->openChild("Source");
	  m_pWriter->openChild("BoxIdentifier");
	   m_pWriter->setChildData(l_oSourceBoxIdentifier.toString());
	  m_pWriter->closeChild();
	  m_pWriter->openChild("BoxOutputIndex");
	   m_pWriter->setChildData(l_sSourceBoxOutputIndex);
	  m_pWriter->closeChild();
	 m_pWriter->closeChild();
	 m_pWriter->openChild("Target");
	  m_pWriter->openChild("BoxIdentifier");
	   m_pWriter->setChildData(l_oTargetBoxIdentifier.toString());
	  m_pWriter->closeChild();
	  m_pWriter->openChild("BoxInputIndex");
	   m_pWriter->setChildData(l_sTargetBoxInputIndex);
	  m_pWriter->closeChild();
	 m_pWriter->closeChild();

	if(rLink.hasAttributes())
	{
		m_pWriter->openChild("Attributes");
			CIdentifier l_oAttributeIdentifier=rLink.getNextAttributeIdentifier(OV_UndefinedIdentifier);
			while(l_oAttributeIdentifier!=OV_UndefinedIdentifier)
			{
				exportAttribute(l_oAttributeIdentifier, rLink.getAttributeValue(l_oAttributeIdentifier));
				l_oAttributeIdentifier=rLink.getNextAttributeIdentifier(l_oAttributeIdentifier);
			}
		m_pWriter->closeChild();
	}

	m_pWriter->closeChild();
}

boolean CScenarioExporterXML::doExport(IScenarioExporterContext& rScenarioExporterContext)
{
	m_pWriter=XML::createWriter(*this);
	if(!m_pWriter)
	{
		return false;
	}

	const IScenario& l_rScenario=rScenarioExporterContext.getScenario();

	m_oFile.open(rScenarioExporterContext.getFileName());

	m_pWriter->openChild("OpenViBE-Scenario");
	 m_pWriter->openChild("Boxes");
		CIdentifier l_oBoxIdentifier=l_rScenario.getNextBoxIdentifier(OV_UndefinedIdentifier);
		while(l_oBoxIdentifier!=OV_UndefinedIdentifier)
		{
			exportBox(*l_rScenario.getBoxDetails(l_oBoxIdentifier));
			l_oBoxIdentifier=l_rScenario.getNextBoxIdentifier(l_oBoxIdentifier);
		}
	 m_pWriter->closeChild();
	 m_pWriter->openChild("Links");
		CIdentifier l_oLinkIdentifier=l_rScenario.getNextLinkIdentifier(OV_UndefinedIdentifier);
		while(l_oLinkIdentifier!=OV_UndefinedIdentifier)
		{
			exportLink(*l_rScenario.getLinkDetails(l_oLinkIdentifier));
			l_oLinkIdentifier=l_rScenario.getNextLinkIdentifier(l_oLinkIdentifier);
		}
	 m_pWriter->closeChild();

	if(l_rScenario.hasAttributes())
	{
		m_pWriter->openChild("Attributes");
			CIdentifier l_oAttributeIdentifier=l_rScenario.getNextAttributeIdentifier(OV_UndefinedIdentifier);
			while(l_oAttributeIdentifier!=OV_UndefinedIdentifier)
			{
				exportAttribute(l_oAttributeIdentifier, l_rScenario.getAttributeValue(l_oAttributeIdentifier));
				l_oAttributeIdentifier=l_rScenario.getNextAttributeIdentifier(l_oAttributeIdentifier);
			}
		m_pWriter->closeChild();
	}

	CVisualisationTreeExporterVisitor	l_oVisualisationTreeExporterVisitor(*m_pWriter);
	const_cast<IScenario&>(rScenarioExporterContext.getScenario()).acceptVisitor(l_oVisualisationTreeExporterVisitor);

	m_pWriter->closeChild();
	m_pWriter->release();
	m_pWriter=NULL;

	m_oFile.close();
	return true;
}
