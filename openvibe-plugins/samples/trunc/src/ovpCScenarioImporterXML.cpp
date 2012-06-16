#include "ovpCScenarioImporterXML.h"

#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;
using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Samples;
using namespace std;

enum
{
	Status_ParsingNothing,
	//
	Status_ParsingScenario,
	Status_ParsingScenarioAttribute,
	//
	Status_ParsingBox,
	Status_ParsingBoxInput,
	Status_ParsingBoxOutput,
	Status_ParsingBoxSetting,
	Status_ParsingBoxAttribute,
	//
	Status_ParsingLink,
	Status_ParsingLinkSource,
	Status_ParsingLinkTarget,
	Status_ParsingLinkAttribute,
	//
	Status_ParsingVisualisationTree,
	//
	Status_ParsingVisualisationWidget,
	Status_ParsingVisualisationWidgetAttribute,
};

//___________________________________________________________________//
//                                                                   //

class _AutoCaster_
{
public:
	_AutoCaster_(const string& sValue) : m_sValue(sValue) { }
	operator CString (void) { return CString(m_sValue.c_str()); }
	operator CIdentifier (void) { CIdentifier l_oResult; l_oResult.fromString(m_sValue.c_str()); return l_oResult; }
	operator uint32 (void) { return atoi(m_sValue.c_str()); }
	operator EVisualisationWidgetType (void) { return (EVisualisationWidgetType)atoi(m_sValue.c_str()); }
protected:
	const string& m_sValue;
};

//___________________________________________________________________//
//                                                                   //

CScenarioImporterXML::CScenarioImporterXML(void)
	:m_ui32Status(Status_ParsingNothing)
	,m_bIsOpenViBEScenario(false)
{
}

void CScenarioImporterXML::openChild(const char* sName, const char** sAttributeName, const char** sAttributeValue, XML::uint64 ui64AttributeCount)
{
	m_vNodes.push(sName);

	string& l_sTop=m_vNodes.top();
	if(false) { }
	else if(l_sTop=="OpenViBE-Scenario" && m_ui32Status==Status_ParsingNothing)  { m_ui32Status=Status_ParsingScenario;          m_bIsOpenViBEScenario=true; }
	else if(l_sTop=="Attribute"         && m_ui32Status==Status_ParsingScenario) { m_ui32Status=Status_ParsingScenarioAttribute; m_oScenario.m_vAttribute.push_back(SAttribute()); }
	//
	else if(l_sTop=="Box"               && m_ui32Status==Status_ParsingScenario) { m_ui32Status=Status_ParsingBox;               m_oScenario.m_vBox.push_back(SBox()); }
	else if(l_sTop=="Input"             && m_ui32Status==Status_ParsingBox)      { m_ui32Status=Status_ParsingBoxInput;          m_oScenario.m_vBox.back().m_vInput.push_back(SInput());  }
	else if(l_sTop=="Output"            && m_ui32Status==Status_ParsingBox)      { m_ui32Status=Status_ParsingBoxOutput;         m_oScenario.m_vBox.back().m_vOutput.push_back(SOutput()); }
	else if(l_sTop=="Setting"           && m_ui32Status==Status_ParsingBox)      { m_ui32Status=Status_ParsingBoxSetting;        m_oScenario.m_vBox.back().m_vSetting.push_back(SSetting()); }
	else if(l_sTop=="Attribute"         && m_ui32Status==Status_ParsingBox)      { m_ui32Status=Status_ParsingBoxAttribute;      m_oScenario.m_vBox.back().m_vAttribute.push_back(SAttribute()); }
	//
	else if(l_sTop=="Link"              && m_ui32Status==Status_ParsingScenario) { m_ui32Status=Status_ParsingLink;              m_oScenario.m_vLink.push_back(SLink()); }
	else if(l_sTop=="Source"            && m_ui32Status==Status_ParsingLink)     { m_ui32Status=Status_ParsingLinkSource; }
	else if(l_sTop=="Target"            && m_ui32Status==Status_ParsingLink)     { m_ui32Status=Status_ParsingLinkTarget; }
	else if(l_sTop=="Attribute"         && m_ui32Status==Status_ParsingLink)     { m_ui32Status=Status_ParsingLinkAttribute;     m_oScenario.m_vLink.back().m_vAttribute.push_back(SAttribute()); }
	//
	else if(l_sTop=="VisualisationTree" && m_ui32Status==Status_ParsingScenario) { m_ui32Status=Status_ParsingVisualisationTree; }
	//
	else if(l_sTop=="VisualisationWidget" && m_ui32Status==Status_ParsingVisualisationTree)   { m_ui32Status=Status_ParsingVisualisationWidget; m_oScenario.m_vVisualisationWidget.push_back(SVisualisationWidget()); }
	else if(l_sTop=="Attribute"           && m_ui32Status==Status_ParsingVisualisationWidget) { m_ui32Status=Status_ParsingVisualisationWidgetAttribute; m_oScenario.m_vVisualisationWidget.back().m_vAttribute.push_back(SAttribute()); }
	// cout << string(m_vNodes.size(), '\t') << m_ui32Status << endl;
}

void CScenarioImporterXML::processChildData(const char* sData)
{
	string& l_sTop=m_vNodes.top();
	switch(m_ui32Status)
	{
		case Status_ParsingBox:
			if(l_sTop=="Identifier")               m_oScenario.m_vBox.back().m_sIdentifier+=sData;
			if(l_sTop=="AlgorithmClassIdentifier") m_oScenario.m_vBox.back().m_sAlgorithmClassIdentifier+=sData;
			if(l_sTop=="Name")                     m_oScenario.m_vBox.back().m_sName+=sData;
			break;
		case Status_ParsingBoxInput:
			if(l_sTop=="TypeIdentifier")           m_oScenario.m_vBox.back().m_vInput.back().m_sTypeIdentifier+=sData;
			if(l_sTop=="Name")                     m_oScenario.m_vBox.back().m_vInput.back().m_sName+=sData;
			break;
		case Status_ParsingBoxOutput:
			if(l_sTop=="TypeIdentifier")           m_oScenario.m_vBox.back().m_vOutput.back().m_sTypeIdentifier+=sData;
			if(l_sTop=="Name")                     m_oScenario.m_vBox.back().m_vOutput.back().m_sName+=sData;
			break;
		case Status_ParsingBoxSetting:
			if(l_sTop=="TypeIdentifier")           m_oScenario.m_vBox.back().m_vSetting.back().m_sTypeIdentifier+=sData;
			if(l_sTop=="Name")                     m_oScenario.m_vBox.back().m_vSetting.back().m_sName+=sData;
			if(l_sTop=="DefaultValue")             m_oScenario.m_vBox.back().m_vSetting.back().m_sDefaultValue+=sData;
			if(l_sTop=="Value")                    m_oScenario.m_vBox.back().m_vSetting.back().m_sValue+=sData;
			break;
		case Status_ParsingBoxAttribute:
			if(l_sTop=="Identifier")               m_oScenario.m_vBox.back().m_vAttribute.back().m_sIdentifier+=sData;
			if(l_sTop=="Value")                    m_oScenario.m_vBox.back().m_vAttribute.back().m_sValue+=sData;
			break;

		case Status_ParsingLink:
			if(l_sTop=="Identifier")               m_oScenario.m_vLink.back().m_sIdentifier+=sData;
			break;
		case Status_ParsingLinkSource:
			if(l_sTop=="BoxIdentifier")           m_oScenario.m_vLink.back().m_oLinkSource.m_sBoxIdentifier+=sData;
			if(l_sTop=="BoxOutputIndex")          m_oScenario.m_vLink.back().m_oLinkSource.m_sBoxOutputIndex+=sData;
			break;
		case Status_ParsingLinkTarget:
			if(l_sTop=="BoxIdentifier")           m_oScenario.m_vLink.back().m_oLinkTarget.m_sBoxIdentifier+=sData;
			if(l_sTop=="BoxInputIndex")           m_oScenario.m_vLink.back().m_oLinkTarget.m_sBoxInputIndex+=sData;
			break;
		case Status_ParsingLinkAttribute:
			if(l_sTop=="Identifier")               m_oScenario.m_vLink.back().m_vAttribute.back().m_sIdentifier+=sData;
			if(l_sTop=="Value")                    m_oScenario.m_vLink.back().m_vAttribute.back().m_sValue+=sData;
			break;

		case Status_ParsingVisualisationWidget:
			if(l_sTop=="Identifier")               m_oScenario.m_vVisualisationWidget.back().m_sIdentifier+=sData;
			if(l_sTop=="Name")                     m_oScenario.m_vVisualisationWidget.back().m_sName+=sData;
			if(l_sTop=="Type")                     m_oScenario.m_vVisualisationWidget.back().m_sType+=sData;
			if(l_sTop=="ParentIdentifier")         m_oScenario.m_vVisualisationWidget.back().m_sParentIdentifier+=sData;
			if(l_sTop=="Index")                    m_oScenario.m_vVisualisationWidget.back().m_sIndex+=sData;
			if(l_sTop=="BoxIdentifier")            m_oScenario.m_vVisualisationWidget.back().m_sBoxIdentifier+=sData;
			if(l_sTop=="NumChildren")              m_oScenario.m_vVisualisationWidget.back().m_sNbChildren+=sData;
			break;
		case Status_ParsingVisualisationWidgetAttribute:
			if(l_sTop=="Identifier")               m_oScenario.m_vVisualisationWidget.back().m_vAttribute.back().m_sIdentifier+=sData;
			if(l_sTop=="Value")                    m_oScenario.m_vVisualisationWidget.back().m_vAttribute.back().m_sValue+=sData;
			break;

		case Status_ParsingScenarioAttribute:
			if(l_sTop=="Identifier")               m_oScenario.m_vAttribute.back().m_sIdentifier+=sData;
			if(l_sTop=="Value")                    m_oScenario.m_vAttribute.back().m_sValue+=sData;
			break;
	}
}

void CScenarioImporterXML::closeChild(void)
{
	string& l_sTop=m_vNodes.top();
	if(false) { }
	else if(l_sTop=="OpenViBE-Scenario" && m_ui32Status==Status_ParsingScenario)          m_ui32Status=Status_ParsingNothing;
	//
	else if(l_sTop=="Box"               && m_ui32Status==Status_ParsingBox)               m_ui32Status=Status_ParsingScenario;
	else if(l_sTop=="Input"             && m_ui32Status==Status_ParsingBoxInput)          m_ui32Status=Status_ParsingBox;
	else if(l_sTop=="Output"            && m_ui32Status==Status_ParsingBoxOutput)         m_ui32Status=Status_ParsingBox;
	else if(l_sTop=="Setting"           && m_ui32Status==Status_ParsingBoxSetting)        m_ui32Status=Status_ParsingBox;
	else if(l_sTop=="Attribute"         && m_ui32Status==Status_ParsingBoxAttribute)      m_ui32Status=Status_ParsingBox;
	//
	else if(l_sTop=="Link"              && m_ui32Status==Status_ParsingLink)              m_ui32Status=Status_ParsingScenario;
	else if(l_sTop=="Source"            && m_ui32Status==Status_ParsingLinkSource)        m_ui32Status=Status_ParsingLink;
	else if(l_sTop=="Target"            && m_ui32Status==Status_ParsingLinkTarget)        m_ui32Status=Status_ParsingLink;
	else if(l_sTop=="Attribute"         && m_ui32Status==Status_ParsingLinkAttribute)     m_ui32Status=Status_ParsingLink;
	//
	else if(l_sTop=="VisualisationWidget" && m_ui32Status==Status_ParsingVisualisationWidget) m_ui32Status=Status_ParsingVisualisationTree;
	else if(l_sTop=="Attribute"         && m_ui32Status==Status_ParsingVisualisationWidgetAttribute) m_ui32Status=Status_ParsingVisualisationWidget;
	//
	else if(l_sTop=="Attribute"         && m_ui32Status==Status_ParsingScenarioAttribute) m_ui32Status=Status_ParsingScenario;

	m_vNodes.pop();
}

void CScenarioImporterXML::release(void)
{
	delete this;
}

boolean CScenarioImporterXML::doImport(IScenarioImporterContext& rScenarioImporterContext)
{
	boolean l_bStatusOk=true;
	XML::IReader* l_pReader=XML::createReader(*this);
	if(!l_pReader)
	{
		return false;
	}

	m_bIsOpenViBEScenario=false;
	IScenario& l_rScenario=rScenarioImporterContext.getScenario();

	// 1. Open scenario file (binary because read would conflict with tellg for text files)
	// 2. Loop until end of file, reading it
	//    and sending what is read to the XML parser
	// 3. Close the scenario file
	ifstream l_oFile(rScenarioImporterContext.getFileName(), ios::binary);
	if(l_oFile.is_open())
	{
		char l_sBuffer[1024];
		size_t l_iBufferLen=0;
		size_t l_iFileLen;
		l_oFile.seekg(0, ios::end);
		l_iFileLen=l_oFile.tellg();
		l_oFile.seekg(0, ios::beg);
		while(l_iFileLen && l_bStatusOk)
		{
			l_iBufferLen=(l_iFileLen>sizeof(l_sBuffer)?sizeof(l_sBuffer):l_iFileLen);
			l_oFile.read(l_sBuffer, l_iBufferLen);
			l_iFileLen-=l_iBufferLen;
			l_bStatusOk=l_pReader->processData(l_sBuffer, l_iBufferLen);
		}
		l_oFile.close();
	}

	// Checks parsin status
	if(!l_bStatusOk || !m_bIsOpenViBEScenario)
	{
		return false;
	}

	// Now build the scenario according to what has been loaded
	vector<SBox>::iterator b;
	vector<SInput>::iterator i;
	vector<SOutput>::iterator o;
	vector<SSetting>::iterator s;
	vector<SLink>::iterator l;
	vector<SVisualisationWidget>::iterator v;
	vector<SAttribute>::iterator a;
	for(b=m_oScenario.m_vBox.begin(); b!=m_oScenario.m_vBox.end(); b++)
	{
		IBox* l_pBox=NULL;
		CIdentifier l_oNewBoxIdentifier;
		CIdentifier l_oBoxIdentifier;
		l_oBoxIdentifier.fromString(_AutoCaster_(b->m_sIdentifier));

		l_rScenario.addBox(l_oNewBoxIdentifier);
		l_pBox=l_rScenario.getBoxDetails(l_oNewBoxIdentifier);
		l_pBox->setName(_AutoCaster_(b->m_sName));

		for(i=b->m_vInput.begin(); i!=b->m_vInput.end(); i++)
		{
			l_pBox->addInput(
				_AutoCaster_(i->m_sName),
				_AutoCaster_(i->m_sTypeIdentifier));
		}
		for(o=b->m_vOutput.begin(); o!=b->m_vOutput.end(); o++)
		{
			l_pBox->addOutput(
				_AutoCaster_(o->m_sName),
				_AutoCaster_(o->m_sTypeIdentifier));
		}
		for(s=b->m_vSetting.begin(); s!=b->m_vSetting.end(); s++)
		{
			l_pBox->addSetting(
				_AutoCaster_(s->m_sName),
				_AutoCaster_(s->m_sTypeIdentifier),
				_AutoCaster_(s->m_sDefaultValue));
			l_pBox->setSettingValue(
				l_pBox->getSettingCount()-1,
				_AutoCaster_(s->m_sValue));
		}
		for(a=b->m_vAttribute.begin(); a!=b->m_vAttribute.end(); a++)
		{
			l_pBox->addAttribute(
				_AutoCaster_(a->m_sIdentifier),
				_AutoCaster_(a->m_sValue));
		}

		// it is important to set box algorithm at
		// last so the box listener is never called
		l_pBox->setAlgorithmClassIdentifier(_AutoCaster_(b->m_sAlgorithmClassIdentifier));
		m_vBoxIdMapping[l_oBoxIdentifier]=l_oNewBoxIdentifier;
	}
	for(l=m_oScenario.m_vLink.begin(); l!=m_oScenario.m_vLink.end(); l++)
	{
		ILink* l_pLink=NULL;
		CIdentifier l_oNewLinkIdentifier;
		CIdentifier l_oLinkIdentifier;
		l_oLinkIdentifier.fromString(_AutoCaster_(l->m_sIdentifier));

		l_rScenario.connect(
			m_vBoxIdMapping[_AutoCaster_(l->m_oLinkSource.m_sBoxIdentifier)],
			_AutoCaster_(l->m_oLinkSource.m_sBoxOutputIndex),
			m_vBoxIdMapping[_AutoCaster_(l->m_oLinkTarget.m_sBoxIdentifier)],
			_AutoCaster_(l->m_oLinkTarget.m_sBoxInputIndex),
			l_oNewLinkIdentifier);
		l_pLink=l_rScenario.getLinkDetails(l_oNewLinkIdentifier);
		for(a=l->m_vAttribute.begin(); a!=l->m_vAttribute.end(); a++)
		{
			l_pLink->addAttribute(
				_AutoCaster_(a->m_sIdentifier),
				_AutoCaster_(a->m_sValue));
		}
		m_vLinkIdMapping[l_oLinkIdentifier]=l_oNewLinkIdentifier;
	}
	for(v=m_oScenario.m_vVisualisationWidget.begin(); v!=m_oScenario.m_vVisualisationWidget.end(); v++)
	{
		IVisualisationWidget* l_pVisualisationWidget=NULL;
		CIdentifier l_oNewVisualisationWidgetIdentifier;
		CIdentifier l_oVisualisationWidgetIdentifier;
		l_oVisualisationWidgetIdentifier.fromString(_AutoCaster_(v->m_sIdentifier));
		CString l_sWidgetName = _AutoCaster_(v->m_sName);
		if(_AutoCaster_(v->m_sType) == EVisualisationWidget_VisualisationBox)
		{
			l_sWidgetName = l_rScenario.getBoxDetails(m_vBoxIdMapping[_AutoCaster_(v->m_sBoxIdentifier)])->getName();
		}

		l_rScenario.getVisualisationTreeDetails().addVisualisationWidget(
			l_oNewVisualisationWidgetIdentifier,
			l_sWidgetName,
			_AutoCaster_(v->m_sType),
			m_vVisualisationWidgetIdMapping[_AutoCaster_(v->m_sParentIdentifier)],
			_AutoCaster_(v->m_sIndex),
			m_vBoxIdMapping[_AutoCaster_(v->m_sBoxIdentifier)],
			_AutoCaster_(v->m_sNbChildren));
		l_pVisualisationWidget=l_rScenario.getVisualisationTreeDetails().getVisualisationWidget(l_oNewVisualisationWidgetIdentifier);
		for(a=v->m_vAttribute.begin(); a!=v->m_vAttribute.end(); a++)
		{
			l_pVisualisationWidget->addAttribute(
				_AutoCaster_(a->m_sIdentifier),
				_AutoCaster_(a->m_sValue));
		}
		m_vVisualisationWidgetIdMapping[l_oVisualisationWidgetIdentifier]=l_oNewVisualisationWidgetIdentifier;
	}
	for(a=m_oScenario.m_vAttribute.begin(); a!=m_oScenario.m_vAttribute.end(); a++)
	{
		l_rScenario.addAttribute(
			_AutoCaster_(a->m_sIdentifier),
			_AutoCaster_(a->m_sValue));
	}

	return true;
}
