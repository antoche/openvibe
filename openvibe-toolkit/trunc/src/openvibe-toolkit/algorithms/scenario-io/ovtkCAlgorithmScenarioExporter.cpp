#include "ovtkCAlgorithmScenarioExporter.h"
#include <vector>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEToolkit;

namespace OpenViBEToolkit
{
	namespace
	{
		class CAlgorithmScenarioExporterHelper
		{
		public:

			CAlgorithmScenarioExporterHelper(IAlgorithmContext& rKernelContext, CAlgorithmScenarioExporter& rParent);
			boolean exportBox(IMemoryBuffer& rMemoryBuffer, const IBox& rBox);
			boolean exportComment(IMemoryBuffer& rMemoryBuffer, const IComment& rComment);
			boolean exportLink(IMemoryBuffer& rMemoryBuffer, const ILink& rLink);
			boolean exportVisualisationTree(IMemoryBuffer& rMemoryBuffer, const IVisualisationTree& rVisualisationTree);
			boolean exportVisualisationWidget(IMemoryBuffer& rMemoryBuffer, const IVisualisationTree& rVisualisationTree, const IVisualisationWidget& rVisualisationWidget);

		protected:

			IAlgorithmContext& m_rAlgorithmContext;
			CAlgorithmScenarioExporter& m_rParent;
		};
	};
};

#define __export_attributes__(rAttributable, rParent, rMemoryBuffer, ID_NAME) \
	if((rAttributable).getNextAttributeIdentifier(OV_UndefinedIdentifier)!=OV_UndefinedIdentifier) \
	{ \
		CIdentifier l_oIdentifer; \
		(rParent).exportStart((rMemoryBuffer), OVTK_Algorithm_ScenarioExporter_NodeId_##ID_NAME##_Attributes); \
		while((l_oIdentifer=(rAttributable).getNextAttributeIdentifier(l_oIdentifer))!=OV_UndefinedIdentifier) \
		{ \
			(rParent).exportStart((rMemoryBuffer), OVTK_Algorithm_ScenarioExporter_NodeId_##ID_NAME##_Attribute); \
			 (rParent).exportIdentifier((rMemoryBuffer), OVTK_Algorithm_ScenarioExporter_NodeId_##ID_NAME##_Attribute_Identifier, l_oIdentifer); \
			 (rParent).exportString((rMemoryBuffer), OVTK_Algorithm_ScenarioExporter_NodeId_##ID_NAME##_Attribute_Value, (rAttributable).getAttributeValue(l_oIdentifer)); \
			(rParent).exportStop((rMemoryBuffer)); \
		} \
		(rParent).exportStop((rMemoryBuffer)); \
	}

boolean CAlgorithmScenarioExporter::process(void)
{
	CAlgorithmScenarioExporterHelper l_oHelper(this->getAlgorithmContext(), *this);
	CMemoryBuffer l_oTemporaryMemoryBuffer;
	CIdentifier l_oBoxIdentifer;
	CIdentifier l_oCommentIdentifier;
	CIdentifier l_oLinkIdentifier;
	CIdentifier l_oVisualisationWidgetIdentifier;

	// preallocates 1 Mbytes
	l_oTemporaryMemoryBuffer.reserve(1024*1024);

	TParameterHandler < IScenario* > ip_pScenario(this->getInputParameter(OVTK_Algorithm_ScenarioExporter_InputParameterId_Scenario));
	IScenario* l_pScenario=ip_pScenario;
	if(!l_pScenario)
	{
		this->getLogManager() << LogLevel_Warning << "No scenario to export\n";
		return false;
	}

	TParameterHandler < IMemoryBuffer* > op_pMemoryBuffer(this->getOutputParameter(OVTK_Algorithm_ScenarioExporter_OutputParameterId_MemoryBuffer));
	IMemoryBuffer* l_pMemoryBuffer=op_pMemoryBuffer;
	if(!l_pMemoryBuffer)
	{
		this->getLogManager() << LogLevel_Warning << "No memory buffer to export scenario to\n";
		return false;
	}

	this->exportStart(l_oTemporaryMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_OpenViBEScenario);

	 this->exportStart(l_oTemporaryMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Boxes);
	  while((l_oBoxIdentifer=l_pScenario->getNextBoxIdentifier(l_oBoxIdentifer))!=OV_UndefinedIdentifier)
	   l_oHelper.exportBox(l_oTemporaryMemoryBuffer, *l_pScenario->getBoxDetails(l_oBoxIdentifer));
	 this->exportStop(l_oTemporaryMemoryBuffer);

	 this->exportStart(l_oTemporaryMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Links);
	  while((l_oLinkIdentifier=l_pScenario->getNextLinkIdentifier(l_oLinkIdentifier))!=OV_UndefinedIdentifier)
	   l_oHelper.exportLink(l_oTemporaryMemoryBuffer, *l_pScenario->getLinkDetails(l_oLinkIdentifier));
	 this->exportStop(l_oTemporaryMemoryBuffer);

	 this->exportStart(l_oTemporaryMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Comments);
	  while((l_oCommentIdentifier=l_pScenario->getNextCommentIdentifier(l_oCommentIdentifier))!=OV_UndefinedIdentifier)
	   l_oHelper.exportComment(l_oTemporaryMemoryBuffer, *l_pScenario->getCommentDetails(l_oCommentIdentifier));
	 this->exportStop(l_oTemporaryMemoryBuffer);

	 this->exportStart(l_oTemporaryMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_VisualisationTree);
	  l_oHelper.exportVisualisationTree(l_oTemporaryMemoryBuffer, l_pScenario->getVisualisationTreeDetails());
	 this->exportStop(l_oTemporaryMemoryBuffer);

	 __export_attributes__(*l_pScenario, *this, l_oTemporaryMemoryBuffer, Scenario);

	this->exportStop(l_oTemporaryMemoryBuffer);

	l_pMemoryBuffer->setSize(0, true);
	l_pMemoryBuffer->append(l_oTemporaryMemoryBuffer);

	return true;
}

CAlgorithmScenarioExporterHelper::CAlgorithmScenarioExporterHelper(IAlgorithmContext& rAlgorithmContext, CAlgorithmScenarioExporter& rParent)
	:m_rAlgorithmContext(rAlgorithmContext)
	,m_rParent(rParent)
{
}

boolean CAlgorithmScenarioExporterHelper::exportBox(IMemoryBuffer& rMemoryBuffer, const IBox& rBox)
{
	m_rParent.exportStart(rMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box);
	 m_rParent.exportIdentifier(rMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Identifier, rBox.getIdentifier());
	 m_rParent.exportString(rMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Name, rBox.getName());
	 m_rParent.exportIdentifier(rMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_AlgorithmClassIdentifier, rBox.getAlgorithmClassIdentifier());

	if(rBox.getInputCount()!=0)
	{
		m_rParent.exportStart(rMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Inputs);
		for(uint32 i=0; i<rBox.getInputCount(); i++)
		{
			CIdentifier l_oInputTypeIdentifier;
			CString l_sInputName;
			rBox.getInputType(i, l_oInputTypeIdentifier);
			rBox.getInputName(i, l_sInputName);
			m_rParent.exportStart(rMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Input);
			 m_rParent.exportIdentifier(rMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Input_TypeIdentifier, l_oInputTypeIdentifier);
			 m_rParent.exportString(rMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Input_Name, l_sInputName);
			m_rParent.exportStop(rMemoryBuffer);
		}
		m_rParent.exportStop(rMemoryBuffer);
	}

	if(rBox.getOutputCount()!=0)
	{
		m_rParent.exportStart(rMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Outputs);
		for(uint32 i=0; i<rBox.getOutputCount(); i++)
		{
			CIdentifier l_oOutputTypeIdentifier;
			CString l_sOutputName;
			rBox.getOutputType(i, l_oOutputTypeIdentifier);
			rBox.getOutputName(i, l_sOutputName);
			m_rParent.exportStart(rMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Output);
			 m_rParent.exportIdentifier(rMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Output_TypeIdentifier, l_oOutputTypeIdentifier);
			 m_rParent.exportString(rMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Output_Name, l_sOutputName);
			m_rParent.exportStop(rMemoryBuffer);
		}
		m_rParent.exportStop(rMemoryBuffer);
	}

	if(rBox.getSettingCount()!=0)
	{
		m_rParent.exportStart(rMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Settings);
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
			m_rParent.exportStart(rMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting);
			 m_rParent.exportIdentifier(rMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_TypeIdentifier, l_oSettingTypeIdentifier);
			 m_rParent.exportString(rMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_Name, l_sSettingName);
			 m_rParent.exportString(rMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_DefaultValue, l_sDefaultValue);
			 m_rParent.exportString(rMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_Value, l_sValue);
			m_rParent.exportStop(rMemoryBuffer);
		}
		m_rParent.exportStop(rMemoryBuffer);
	}

	__export_attributes__(rBox, m_rParent, rMemoryBuffer, Box);

	m_rParent.exportStop(rMemoryBuffer);

	return true;
}

boolean CAlgorithmScenarioExporterHelper::exportComment(IMemoryBuffer& rMemoryBuffer, const IComment& rComment)
{
	m_rParent.exportStart(rMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Comment);
	 m_rParent.exportIdentifier(rMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Comment_Identifier, rComment.getIdentifier());
	 m_rParent.exportString(rMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Comment_Text, rComment.getText());

	__export_attributes__(rComment, m_rParent, rMemoryBuffer, Comment);

	m_rParent.exportStop(rMemoryBuffer);

	return true;
}

boolean CAlgorithmScenarioExporterHelper::exportLink(IMemoryBuffer& rMemoryBuffer, const ILink& rLink)
{
	CIdentifier l_oSourceBoxIdentifier;
	CIdentifier l_oTargetBoxIdentifier;
	uint32 l_ui32SourceBoxOutputIndex=0xffffffff;
	uint32 l_ui32TargetBoxInputIndex=0xffffffff;

	rLink.getSource(l_oSourceBoxIdentifier, l_ui32SourceBoxOutputIndex);
	rLink.getTarget(l_oTargetBoxIdentifier, l_ui32TargetBoxInputIndex);

	m_rParent.exportStart(rMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Link);
	 m_rParent.exportIdentifier(rMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Link_Identifier, rLink.getIdentifier());
	 m_rParent.exportStart(rMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Link_Source);
	  m_rParent.exportIdentifier(rMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Link_Source_BoxIdentifier, l_oSourceBoxIdentifier);
	  m_rParent.exportUInteger(rMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Link_Source_BoxOutputIndex, l_ui32SourceBoxOutputIndex);
	 m_rParent.exportStop(rMemoryBuffer);
	 m_rParent.exportStart(rMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Link_Target);
	  m_rParent.exportIdentifier(rMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Link_Target_BoxIdentifier, l_oTargetBoxIdentifier);
	  m_rParent.exportUInteger(rMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Link_Target_BoxInputIndex, l_ui32TargetBoxInputIndex);
	 m_rParent.exportStop(rMemoryBuffer);

	__export_attributes__(rLink, m_rParent, rMemoryBuffer, Link);

	m_rParent.exportStop(rMemoryBuffer);

	return true;
}

boolean CAlgorithmScenarioExporterHelper::exportVisualisationTree(IMemoryBuffer& rMemoryBuffer, const IVisualisationTree& rVisualisationTree)
{
	CIdentifier l_oIdentifier;

	std::vector < CIdentifier > l_vWidget;

	while(rVisualisationTree.getNextVisualisationWidgetIdentifier(l_oIdentifier))
	{
		IVisualisationWidget* l_pWidget=rVisualisationTree.getVisualisationWidget(l_oIdentifier);
		if(l_pWidget->getType()==EVisualisationWidget_VisualisationWindow
		|| l_pWidget->getParentIdentifier()==OV_UndefinedIdentifier)
		{
			l_vWidget.push_back(l_oIdentifier);
		}
	}

	for(std::vector < CIdentifier >::size_type i=0; i<l_vWidget.size(); i++)
	{
		IVisualisationWidget* l_pWidget=rVisualisationTree.getVisualisationWidget(l_vWidget[i]);

		this->exportVisualisationWidget(rMemoryBuffer, rVisualisationTree, *l_pWidget);

		for(uint32 i=0; i<l_pWidget->getNbChildren(); i++)
		{
			if(l_pWidget->getChildIdentifier(i, l_oIdentifier))
			{
				l_vWidget.push_back(l_oIdentifier);
			}
		}
	}

	return true;
}

boolean CAlgorithmScenarioExporterHelper::exportVisualisationWidget(IMemoryBuffer& rMemoryBuffer, const IVisualisationTree& rVisualisationTree, const IVisualisationWidget& rVisualisationWidget)
{
	m_rParent.exportStart(rMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_VisualisationWidget);

	m_rParent.exportIdentifier(rMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_VisualisationWidget_Identifier, rVisualisationWidget.getIdentifier());

	// visualisation box name can be retrieved from corresponding IBox, so we can skip it for these
	if(rVisualisationWidget.getType() != EVisualisationWidget_VisualisationBox)
	{
		m_rParent.exportString(rMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_VisualisationWidget_Name, rVisualisationWidget.getName());
	}

	m_rParent.exportUInteger(rMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_VisualisationWidget_Type, (uint64)rVisualisationWidget.getType());
	m_rParent.exportIdentifier(rMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_VisualisationWidget_ParentIdentifier, rVisualisationWidget.getParentIdentifier());

	// visualisation widget index
	IVisualisationWidget* l_pParentVisualisationWidget=rVisualisationTree.getVisualisationWidget(rVisualisationWidget.getParentIdentifier());
	if(l_pParentVisualisationWidget != NULL)
	{
		uint32 l_ui32ChildIndex=0;
		l_pParentVisualisationWidget->getChildIndex(rVisualisationWidget.getIdentifier(), l_ui32ChildIndex);
		m_rParent.exportUInteger(rMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_VisualisationWidget_Index, l_ui32ChildIndex);
	}

	m_rParent.exportIdentifier(rMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_VisualisationWidget_BoxIdentifier, rVisualisationWidget.getBoxIdentifier());
	m_rParent.exportUInteger(rMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_VisualisationWidget_NumChildren, rVisualisationWidget.getNbChildren());

	 __export_attributes__(rVisualisationWidget, m_rParent, rMemoryBuffer, VisualisationWidget);

	m_rParent.exportStop(rMemoryBuffer);

	return true;
}
