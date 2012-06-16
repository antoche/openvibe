#include <cstdio>
#include <cstring>
#include "ovpCAlgorithmXMLScenarioExporter.h"

//___________________________________________________________________//
//                                                                   //

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;
using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::FileIO;
using namespace std;

//___________________________________________________________________//
//                                                                   //

CAlgorithmXMLScenarioExporter::CAlgorithmXMLScenarioExporter(void)
	:m_pWriter(NULL)
	,m_pMemoryBuffer(NULL)
{
	m_pWriter=XML::createWriter(*this);
}

CAlgorithmXMLScenarioExporter::~CAlgorithmXMLScenarioExporter(void)
{
	m_pWriter->release();
}

void CAlgorithmXMLScenarioExporter::write(const char* sString)
{
	m_pMemoryBuffer->append(reinterpret_cast<const uint8*>(sString), ::strlen(sString));
}

boolean CAlgorithmXMLScenarioExporter::exportStart(IMemoryBuffer& rMemoryBuffer, const CIdentifier& rIdentifier)
{
	m_pMemoryBuffer=&rMemoryBuffer;

	CString l_sNodeName;

	if(false) { }

	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_OpenViBEScenario                         ) l_sNodeName="OpenViBE-Scenario";

	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_Boxes                                    ) l_sNodeName="Boxes";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_Box                                      ) l_sNodeName="Box";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_Box_Identifier                           ) l_sNodeName="Identifier";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_Box_Name                                 ) l_sNodeName="Name";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_Box_AlgorithmClassIdentifier             ) l_sNodeName="AlgorithmClassIdentifier";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_Box_Inputs                               ) l_sNodeName="Inputs";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_Box_Input                                ) l_sNodeName="Input";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_Box_Input_TypeIdentifier                 ) l_sNodeName="TypeIdentifier";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_Box_Input_Name                           ) l_sNodeName="Name";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_Box_Outputs                              ) l_sNodeName="Outputs";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_Box_Output                               ) l_sNodeName="Output";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_Box_Output_TypeIdentifier                ) l_sNodeName="TypeIdentifier";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_Box_Output_Name                          ) l_sNodeName="Name";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_Box_Settings                             ) l_sNodeName="Settings";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting                              ) l_sNodeName="Setting";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_TypeIdentifier               ) l_sNodeName="TypeIdentifier";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_Name                         ) l_sNodeName="Name";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_DefaultValue                 ) l_sNodeName="DefaultValue";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_Value                        ) l_sNodeName="Value";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_Box_Attributes                           ) l_sNodeName="Attributes";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_Box_Attribute                            ) l_sNodeName="Attribute";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_Box_Attribute_Identifier                 ) l_sNodeName="Identifier";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_Box_Attribute_Value                      ) l_sNodeName="Value";

	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_Comments                                 ) l_sNodeName="Comments";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_Comment                                  ) l_sNodeName="Comment";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_Comment_Identifier                       ) l_sNodeName="Identifier";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_Comment_Text                             ) l_sNodeName="Text";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_Comment_Attributes                       ) l_sNodeName="Attributes";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_Comment_Attribute                        ) l_sNodeName="Attribute";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_Comment_Attribute_Identifier             ) l_sNodeName="Identifier";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_Comment_Attribute_Value                  ) l_sNodeName="Value";

	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_Links                                    ) l_sNodeName="Links";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_Link                                     ) l_sNodeName="Link";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_Link_Identifier                          ) l_sNodeName="Identifier";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_Link_Source                              ) l_sNodeName="Source";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_Link_Source_BoxIdentifier                ) l_sNodeName="BoxIdentifier";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_Link_Source_BoxOutputIndex               ) l_sNodeName="BoxOutputIndex";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_Link_Target                              ) l_sNodeName="Target";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_Link_Target_BoxIdentifier                ) l_sNodeName="BoxIdentifier";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_Link_Target_BoxInputIndex                ) l_sNodeName="BoxInputIndex";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_Link_Attributes                          ) l_sNodeName="Attributes";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_Link_Attribute                           ) l_sNodeName="Attribute";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_Link_Attribute_Identifier                ) l_sNodeName="Identifier";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_Link_Attribute_Value                     ) l_sNodeName="Value";

	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Attributes                      ) l_sNodeName="Attributes";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Attribute                       ) l_sNodeName="Attribute";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Attribute_Identifier            ) l_sNodeName="Identifier";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Attribute_Value                 ) l_sNodeName="Value";

	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_VisualisationTree                        ) l_sNodeName="VisualisationTree";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_VisualisationWidget                      ) l_sNodeName="VisualisationWidget";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_VisualisationWidget_Identifier           ) l_sNodeName="Identifier";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_VisualisationWidget_Name                 ) l_sNodeName="Name";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_VisualisationWidget_Type                 ) l_sNodeName="Type";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_VisualisationWidget_ParentIdentifier     ) l_sNodeName="ParentIdentifier";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_VisualisationWidget_Index                ) l_sNodeName="Index";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_VisualisationWidget_BoxIdentifier        ) l_sNodeName="BoxIdentifier";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_VisualisationWidget_NumChildren          ) l_sNodeName="NumChildren";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_VisualisationWidget_Attributes           ) l_sNodeName="Attributes";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_VisualisationWidget_Attribute            ) l_sNodeName="Attribute";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_VisualisationWidget_Attribute_Identifier ) l_sNodeName="Identifier";
	else if(rIdentifier==OVTK_Algorithm_ScenarioExporter_NodeId_VisualisationWidget_Attribute_Value      ) l_sNodeName="Value";

	else return false;

	m_pWriter->openChild(l_sNodeName.toASCIIString());

	return true;
}

boolean CAlgorithmXMLScenarioExporter::exportIdentifier(IMemoryBuffer& rMemoryBuffer, const CIdentifier& rIdentifier, const CIdentifier& rValue)
{
	m_pMemoryBuffer=&rMemoryBuffer;

	this->exportStart(rMemoryBuffer, rIdentifier);
	m_pWriter->setChildData(rValue.toString().toASCIIString());
	this->exportStop(rMemoryBuffer);

	return true;
}

boolean CAlgorithmXMLScenarioExporter::exportString(IMemoryBuffer& rMemoryBuffer, const CIdentifier& rIdentifier, const CString& rValue)
{
	m_pMemoryBuffer=&rMemoryBuffer;

	this->exportStart(rMemoryBuffer, rIdentifier);
	m_pWriter->setChildData(rValue.toASCIIString());
	this->exportStop(rMemoryBuffer);

	return true;
}

boolean CAlgorithmXMLScenarioExporter::exportUInteger(IMemoryBuffer& rMemoryBuffer, const CIdentifier& rIdentifier, const uint64 ui64Value)
{
	char l_sValue[1024];
	sprintf(l_sValue, "%llu", ui64Value);

	m_pMemoryBuffer=&rMemoryBuffer;

	this->exportStart(rMemoryBuffer, rIdentifier);
	m_pWriter->setChildData(l_sValue);
	this->exportStop(rMemoryBuffer);

	return true;
}

boolean CAlgorithmXMLScenarioExporter::exportStop(IMemoryBuffer& rMemoryBuffer)
{
	m_pMemoryBuffer=&rMemoryBuffer;

	m_pWriter->closeChild();

	return true;
}
