#include "ovkCScenario.h"
#include "ovkCScenarioExporterContext.h"
#include "ovkCScenarioImporterContext.h"

#include "ovkCBox.h"
#include "ovkCComment.h"
#include "ovkCLink.h"
#include "ovkCProcessingUnit.h"

#include "../ovkCObjectVisitorContext.h"

#include <iostream>
#include <cstdlib>

using namespace std;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

//___________________________________________________________________//
//                                                                   //

namespace
{
	template <class T>
	struct TTestTrue
	{
		boolean operator()(typename map<CIdentifier, T*>::const_iterator it) const { return true; }
	};

	struct TTestEqSourceBox
	{
		TTestEqSourceBox(const CIdentifier& rId) : m_rId(rId) { }
		boolean operator()(map<CIdentifier, CLink*>::const_iterator it) const { return it->second->getSourceBoxIdentifier()==m_rId; }
		const CIdentifier& m_rId;
	};

	struct TTestEqSourceBoxOutput
	{
		TTestEqSourceBoxOutput(const CIdentifier& rId, uint32 ui32Id) : m_rId(rId), m_ui32Id(ui32Id) { }
		boolean operator()(map<CIdentifier, CLink*>::const_iterator it) const { return it->second->getSourceBoxIdentifier()==m_rId && it->second->getSourceBoxOutputIndex()==m_ui32Id; }
		const CIdentifier& m_rId;
		uint32 m_ui32Id;
	};

	struct TTestEqTargetBox
	{
		TTestEqTargetBox(const CIdentifier& rId) : m_rId(rId) { }
		boolean operator()(map<CIdentifier, CLink*>::const_iterator it) const { return it->second->getTargetBoxIdentifier()==m_rId; }
		const CIdentifier& m_rId;
	};

	struct TTestEqTargetBoxInput
	{
		TTestEqTargetBoxInput(const CIdentifier& rId, uint32 ui32Id) : m_rId(rId), m_ui32Id(ui32Id) { }
		boolean operator()(map<CIdentifier, CLink*>::const_iterator it) const { return it->second->getTargetBoxIdentifier()==m_rId && it->second->getTargetBoxInputIndex()==m_ui32Id; }
		const CIdentifier& m_rId;
		uint32 m_ui32Id;
	};

	struct TTestEqProcessingUnitIdentifier
	{
		TTestEqProcessingUnitIdentifier(const CIdentifier& rId) : m_rId(rId) { }
		boolean operator()(map<CIdentifier, CBox*>::const_iterator it) const { return it->second->getProcessingUnitIdentifier()==m_rId; }
		const CIdentifier& m_rId;
	};

	template <class T, class TTest>
	CIdentifier getNextTIdentifier(
		const map<CIdentifier, T*>& vMap,
		const CIdentifier& rPreviousIdentifier,
		const TTest& rTest)
	{
		typename map<CIdentifier, T*>::const_iterator it;

		if(rPreviousIdentifier==OV_UndefinedIdentifier)
		{
			it=vMap.begin();
		}
		else
		{
			it=vMap.find(rPreviousIdentifier);
			if(it==vMap.end())
			{
				return OV_UndefinedIdentifier;
			}
			it++;
		}

		while(it!=vMap.end())
		{
			if(rTest(it))
			{
				return it->first;
			}
			it++;
		}

		return OV_UndefinedIdentifier;
	}
};

//___________________________________________________________________//
//                                                                   //

CScenario::CScenario(const IKernelContext& rKernelContext, const CIdentifier& rIdentifier)
	:TAttributable < TKernelObject < IScenario > > (rKernelContext)
	,m_oIdentifier(rIdentifier)
{
}

CScenario::~CScenario(void)
{
	this->clear();
}

//___________________________________________________________________//
//                                                                   //

boolean CScenario::clear(void)
{
	this->getLogManager() << LogLevel_Trace << "Clearing scenario\n";

	// Clears boxes
	map<CIdentifier, CBox*>::iterator itBox;
	for(itBox=m_vBox.begin(); itBox!=m_vBox.end(); itBox++)
	{
		delete itBox->second;
	}
	m_vBox.clear();

	// Clears comments
	map<CIdentifier, CComment*>::iterator itComment;
	for(itComment=m_vComment.begin(); itComment!=m_vComment.end(); itComment++)
	{
		delete itComment->second;
	}
	m_vComment.clear();

	// Clears links
	map<CIdentifier, CLink*>::iterator itLink;
	for(itLink=m_vLink.begin(); itLink!=m_vLink.end(); itLink++)
	{
		delete itLink->second;
	}
	m_vLink.clear();

	// Clears processing units
	map<CIdentifier, CProcessingUnit*>::iterator itProcessingUnit;
	for(itProcessingUnit=m_vProcessingUnit.begin(); itProcessingUnit!=m_vProcessingUnit.end(); itProcessingUnit++)
	{
		delete itProcessingUnit->second;
	}
	m_vProcessingUnit.clear();

	// Clears attributes
	this->removeAllAttributes();

	return true;
}

boolean CScenario::merge(
	const IScenario& rScenario)
{
	this->getLogManager() << LogLevel_ImportantWarning << "Scenario merging is not yet implemented\n";

#if 0
	// Prepares copy
	map < CIdentifier, CIdentifier > l_vIdMapping;

	// Copies boxes
	CIdentifier l_oBoxIdentifier;
	while((l_oBoxIdentifier=rScenario.getNextBoxIdentifier(l_oBoxIdentifier))!=OV_UndefinedIdentifier)
	{
		CIdentifier l_oNewIdentifier;
		const IBox* l_pBox=rScenario.getBoxDetails(l_oBoxIdentifier);
		this->addBox(
			*l_pBox,
			l_oNewIdentifier);
		l_vIdMapping[l_oBoxIdentifier]=l_oNewIdentifier;

		// Updates visualisation manager
		CIdentifier l_oBoxAlgorithmIdentifier=l_pBox->getAlgorithmClassIdentifier();
		const IPluginObjectDesc* l_pPOD = this->getPluginManager().getPluginObjectDescCreating(l_oBoxAlgorithmIdentifier);

		// If a visualisation box was dropped, add it in window manager
		if(l_pPOD && l_pPOD->hasFunctionality(PluginFunctionality_Visualization))
		{
			CIdentifier l_oVisualisationWidgetIdentifier;

			// Let window manager know about new box
			this->getVisualisationTreeDetails().addVisualisationWidget(
				l_oVisualisationWidgetIdentifier,
				l_pBox->getName(),
				EVisualisationWidget_VisualisationBox,
				OV_UndefinedIdentifier,
				0,
				l_oNewIdentifier,
				0);

			// this->getVisualisationTreeDetails().reloadTree();
		}
	}

	// Copies links
	CIdentifier l_oLinkIdentifier;
	while((l_oLinkIdentifier=rScenario.getNextLinkIdentifier(l_oLinkIdentifier))!=OV_UndefinedIdentifier)
	{
		CIdentifier l_oNewIdentifier;
		const ILink* l_pLink=rScenario.getLinkDetails(l_oLinkIdentifier);
		this->connect(
			l_vIdMapping[l_pLink->getSourceBoxIdentifier()],
			l_pLink->getSourceBoxOutputIndex(),
			l_vIdMapping[l_pLink->getTargetBoxIdentifier()],
			l_pLink->getTargetBoxInputIndex(),
			l_oNewIdentifier);
	}

	return true;
#else
	return false;
#endif
}

//___________________________________________________________________//
//                                                                   //

boolean CScenario::setVisualisationTreeIdentifier(const CIdentifier& rVisualisationTreeIdentifier)
{
	m_oVisualisationTreeIdentifier=rVisualisationTreeIdentifier;
	return true;
}

CIdentifier CScenario::getVisualisationTreeIdentifier(void) const
{
	return m_oVisualisationTreeIdentifier;
}

const IVisualisationTree& CScenario::getVisualisationTreeDetails() const
{
	return getKernelContext().getVisualisationManager().getVisualisationTree(m_oVisualisationTreeIdentifier);
}

IVisualisationTree& CScenario::getVisualisationTreeDetails()
{
	return getKernelContext().getVisualisationManager().getVisualisationTree(m_oVisualisationTreeIdentifier);
}

//___________________________________________________________________//
//                                                                   //

CIdentifier CScenario::getNextBoxIdentifier(
	const CIdentifier& rPreviousIdentifier) const
{
	return getNextTIdentifier<CBox, TTestTrue<CBox> >(m_vBox, rPreviousIdentifier, TTestTrue<CBox>());
}

CIdentifier CScenario::getNextBoxIdentifierOnProcessingUnit(
	const CIdentifier& rPreviousIdentifier,
	const CIdentifier& rProcessingUnitIdentifier) const
{
	return getNextTIdentifier<CBox, TTestEqProcessingUnitIdentifier >(m_vBox, rPreviousIdentifier, TTestEqProcessingUnitIdentifier(rProcessingUnitIdentifier));
}

const IBox* CScenario::getBoxDetails(
	const CIdentifier& rBoxIdentifier) const
{
	this->getLogManager() << LogLevel_Debug << "Getting const box details from scenario\n";

	map<CIdentifier, CBox*>::const_iterator itBox;
	itBox=m_vBox.find(rBoxIdentifier);
	if(itBox==m_vBox.end())
	{
		this->getLogManager() << LogLevel_Warning << "The box does not exist\n";
		return NULL;
	}
	return itBox->second;
}

boolean CScenario::isBox(
	const CIdentifier& rIdentifier) const
{
	map<CIdentifier, CBox*>::const_iterator itBox;
	itBox=m_vBox.find(rIdentifier);
	return itBox!=m_vBox.end();
}

IBox* CScenario::getBoxDetails(
	const CIdentifier& rBoxIdentifier)
{
	this->getLogManager() << LogLevel_Debug << "Getting box details from scenario\n";

	map<CIdentifier, CBox*>::const_iterator itBox;
	itBox=m_vBox.find(rBoxIdentifier);
	if(itBox==m_vBox.end())
	{
		this->getLogManager() << LogLevel_Warning << "The box does not exist\n";
		return NULL;
	}
	return itBox->second;
}

boolean CScenario::addBox(
	CIdentifier& rBoxIdentifier)
{
	this->getLogManager() << LogLevel_Trace << "Adding new empty box in scenario\n";

	rBoxIdentifier=getUnusedIdentifier();
	CBox* l_pBox=new CBox(this->getKernelContext(), *this);
	l_pBox->setIdentifier(rBoxIdentifier);

	m_vBox[rBoxIdentifier]=l_pBox;
	return true;
}

boolean CScenario::addBox(
	const IBox& rBox,
	CIdentifier& rBoxIdentifier)
{
	this->getLogManager() << LogLevel_Trace << "Adding a new box in the scenario based on an existing one\n";

	if(!addBox(rBoxIdentifier))
	{
		return false;
	}

	IBox* l_pBox=getBoxDetails(rBoxIdentifier);
	if(!l_pBox)
	{
		return false;
	}

	return l_pBox->initializeFromExistingBox(rBox);
}

boolean CScenario::addBox(
	const CIdentifier& rBoxAlgorithmIdentifier,
	CIdentifier& rBoxIdentifier)
{
	this->getLogManager() << LogLevel_Trace << "Adding new box in scenario\n";

	if(!addBox(rBoxIdentifier))
	{
		return false;
	}

	IBox* l_pBox=getBoxDetails(rBoxIdentifier);
	if(!l_pBox)
	{
		return false;
	}

	return l_pBox->initializeFromAlgorithmClassIdentifier(rBoxAlgorithmIdentifier);
}

boolean CScenario::removeBox(
	const CIdentifier& rBoxIdentifier)
{
	this->getLogManager() << LogLevel_Trace << "Removing box from scenario\n";

	// Finds the box according to its identifier
	map<CIdentifier, CBox*>::iterator itBox;
	itBox=m_vBox.find(rBoxIdentifier);
	if(itBox==m_vBox.end())
	{
		// The box does not exist !
		this->getLogManager() << LogLevel_Warning << "The box does not exist\n";
		return false;
	}

	this->getLogManager() << LogLevel_Trace << "Found the box !\n";

	// Found the box,
	// now find all the links that are used by this box
	map<CIdentifier, CLink*>::iterator itLink=m_vLink.begin();
	while(itLink!=m_vLink.end())
	{
		map<CIdentifier, CLink*>::iterator itLinkCurrent=itLink;
		itLink++;

		if(itLinkCurrent->second->getSourceBoxIdentifier()==rBoxIdentifier || itLinkCurrent->second->getTargetBoxIdentifier()==rBoxIdentifier)
		{
			this->getLogManager() << LogLevel_Trace << "Found a link to this box - it will be deleted !\n";

			// Deletes this link
			delete itLinkCurrent->second;

			// Removes link from the link list
			m_vLink.erase(itLinkCurrent);
		}
	}

	// Deletes the box itself
	delete itBox->second;

	// Removes box from the box list
	m_vBox.erase(itBox);

	this->getLogManager() << LogLevel_Trace << "The box has been removed\n";

	return true;
}

//___________________________________________________________________//
//                                                                   //

CIdentifier CScenario::getNextCommentIdentifier(
	const CIdentifier& rPreviousIdentifier) const
{
	return getNextTIdentifier<CComment, TTestTrue<CComment> >(m_vComment, rPreviousIdentifier, TTestTrue<CComment>());
}

const IComment* CScenario::getCommentDetails(
	const CIdentifier& rCommentIdentifier) const
{
	this->getLogManager() << LogLevel_Debug << "Getting const comment details from scenario\n";

	map<CIdentifier, CComment*>::const_iterator itComment;
	itComment=m_vComment.find(rCommentIdentifier);
	if(itComment==m_vComment.end())
	{
		this->getLogManager() << LogLevel_Warning << "The comment does not exist\n";
		return NULL;
	}
	return itComment->second;
}

boolean CScenario::isComment(
	const CIdentifier& rIdentifier) const
{
	map<CIdentifier, CComment*>::const_iterator itComment;
	itComment=m_vComment.find(rIdentifier);
	return itComment!=m_vComment.end();
}

IComment* CScenario::getCommentDetails(
	const CIdentifier& rCommentIdentifier)
{
	this->getLogManager() << LogLevel_Debug << "Getting comment details from scenario\n";

	map<CIdentifier, CComment*>::const_iterator itComment;
	itComment=m_vComment.find(rCommentIdentifier);
	if(itComment==m_vComment.end())
	{
		this->getLogManager() << LogLevel_Warning << "The comment does not exist\n";
		return NULL;
	}
	return itComment->second;
}

boolean CScenario::addComment(
	CIdentifier& rCommentIdentifier)
{
	this->getLogManager() << LogLevel_Trace << "Adding new empty comment in scenario\n";

	rCommentIdentifier=getUnusedIdentifier();
	CComment* l_pComment=new CComment(this->getKernelContext(), *this);
	l_pComment->setIdentifier(rCommentIdentifier);

	m_vComment[rCommentIdentifier]=l_pComment;
	return true;
}

boolean CScenario::addComment(
	const IComment& rComment,
	CIdentifier& rCommentIdentifier)
{
	this->getLogManager() << LogLevel_Trace << "Adding a new comment in the scenario based on an existing one\n";

	if(!addComment(rCommentIdentifier))
	{
		return false;
	}

	IComment* l_pComment=getCommentDetails(rCommentIdentifier);
	if(!l_pComment)
	{
		return false;
	}

	return l_pComment->initializeFromExistingComment(rComment);
}

boolean CScenario::removeComment(
	const CIdentifier& rCommentIdentifier)
{
	this->getLogManager() << LogLevel_Trace << "Removing comment from scenario\n";

	// Finds the comment according to its identifier
	map<CIdentifier, CComment*>::iterator itComment;
	itComment=m_vComment.find(rCommentIdentifier);
	if(itComment==m_vComment.end())
	{
		// The comment does not exist !
		this->getLogManager() << LogLevel_Warning << "The comment does not exist\n";
		return false;
	}

	this->getLogManager() << LogLevel_Trace << "Found the comment !\n";

	// Deletes the comment itself
	delete itComment->second;

	// Removes comment from the comment list
	m_vComment.erase(itComment);

	this->getLogManager() << LogLevel_Trace << "The comment has been removed\n";

	return true;
}

//___________________________________________________________________//
//                                                                   //

CIdentifier CScenario::getNextLinkIdentifier(
	const CIdentifier& rPreviousIdentifier) const
{
	return getNextTIdentifier<CLink, TTestTrue<CLink> >(m_vLink, rPreviousIdentifier, TTestTrue<CLink>());
}

CIdentifier CScenario::getNextLinkIdentifierFromBox(
	const CIdentifier& rPreviousIdentifier,
	const CIdentifier& rBoxIdentifier) const
{
	return getNextTIdentifier<CLink, TTestEqSourceBox>(m_vLink, rPreviousIdentifier, TTestEqSourceBox(rBoxIdentifier));
}

CIdentifier CScenario::getNextLinkIdentifierFromBoxOutput(
	const CIdentifier& rPreviousIdentifier,
	const CIdentifier& rBoxIdentifier,
	const uint32 ui32OutputIndex) const
{
	return getNextTIdentifier<CLink, TTestEqSourceBoxOutput>(m_vLink, rPreviousIdentifier, TTestEqSourceBoxOutput(rBoxIdentifier, ui32OutputIndex));
}

CIdentifier CScenario::getNextLinkIdentifierToBox(
	const CIdentifier& rPreviousIdentifier,
	const CIdentifier& rBoxIdentifier) const
{
	return getNextTIdentifier<CLink, TTestEqTargetBox>(m_vLink, rPreviousIdentifier, TTestEqTargetBox(rBoxIdentifier));
}

CIdentifier CScenario::getNextLinkIdentifierToBoxInput(
	const CIdentifier& rPreviousIdentifier,
	const CIdentifier& rBoxIdentifier,
	const uint32 ui32InputInex) const
{

	return getNextTIdentifier<CLink, TTestEqTargetBoxInput>(m_vLink, rPreviousIdentifier, TTestEqTargetBoxInput(rBoxIdentifier, ui32InputInex));
}

boolean CScenario::isLink(
	const CIdentifier& rIdentifier) const
{
	map<CIdentifier, CLink*>::const_iterator itLink;
	itLink=m_vLink.find(rIdentifier);
	return itLink!=m_vLink.end();
}

const ILink* CScenario::getLinkDetails(
	const CIdentifier& rLinkIdentifier) const
{
	this->getLogManager() << LogLevel_Debug << "Retrieving const link details from scenario\n";

	map<CIdentifier, CLink*>::const_iterator itLink;
	itLink=m_vLink.find(rLinkIdentifier);
	if(itLink==m_vLink.end())
	{
		this->getLogManager() << LogLevel_Warning << "The link does not exist\n";
		return NULL;
	}
	return itLink->second;
}

ILink* CScenario::getLinkDetails(
	const CIdentifier& rLinkIdentifier)
{
	this->getLogManager() << LogLevel_Debug << "Retrieving link details from scenario\n";

	map<CIdentifier, CLink*>::const_iterator itLink;
	itLink=m_vLink.find(rLinkIdentifier);
	if(itLink==m_vLink.end())
	{
		this->getLogManager() << LogLevel_Warning << "The link does not exist\n";
		return NULL;
	}
	return itLink->second;
}

boolean CScenario::connect(
	const CIdentifier& rSourceBoxIdentifier,
	const uint32 ui32SourceBoxOutputIndex,
	const CIdentifier& rTargetBoxIdentifier,
	const uint32 ui32TargetBoxInputIndex,
	CIdentifier& rLinkIdentifier)
{
	this->getLogManager() << LogLevel_Trace << "Connecting boxes\n";

	map<CIdentifier, CBox*>::const_iterator itBox1;
	map<CIdentifier, CBox*>::const_iterator itBox2;
	itBox1=m_vBox.find(rSourceBoxIdentifier);
	itBox2=m_vBox.find(rTargetBoxIdentifier);
	if(itBox1==m_vBox.end() || itBox2==m_vBox.end())
	{
		this->getLogManager() << LogLevel_Warning << "At least one of the boxes does not exist\n";
		return false;
	}
	CBox* l_pSourceBox=itBox1->second;
	CBox* l_pTargetBox=itBox2->second;
	if(ui32SourceBoxOutputIndex >= l_pSourceBox->getOutputCount())
	{
		this->getLogManager() << LogLevel_Warning << "Wrong output index\n";
		return false;
	}
	if(ui32TargetBoxInputIndex >= l_pTargetBox->getInputCount())
	{
		this->getLogManager() << LogLevel_Warning << "Wrong input index\n";
		return false;
	}

	// Looks for any connected link to this box input and removes it
	map<CIdentifier, CLink*>::iterator itLink=m_vLink.begin();
	while(itLink!=m_vLink.end())
	{
		map<CIdentifier, CLink*>::iterator itLinkCurrent=itLink;
		itLink++;

		CLink* l_pLink=itLinkCurrent->second;
		if(l_pLink)
		{
			if(l_pLink->getTargetBoxIdentifier()==rTargetBoxIdentifier && l_pLink->getTargetBoxInputIndex()==ui32TargetBoxInputIndex)
			{
				delete l_pLink;
				m_vLink.erase(itLinkCurrent);
			}
		}
	}

	rLinkIdentifier=getUnusedIdentifier();

	CLink* l_pLink=new CLink(this->getKernelContext(), *this);
	l_pLink->setIdentifier(rLinkIdentifier);
	l_pLink->setSource(rSourceBoxIdentifier, ui32SourceBoxOutputIndex);
	l_pLink->setTarget(rTargetBoxIdentifier, ui32TargetBoxInputIndex);

	m_vLink[l_pLink->getIdentifier()]=l_pLink;

	return true;
}

boolean CScenario::disconnect(
	const CIdentifier& rSourceBoxIdentifier,
	const uint32 ui32SourceBoxOutputIndex,
	const CIdentifier& rTargetBoxIdentifier,
	const uint32 ui32TargetBoxInputIndex)
{
	// Looks for any link with the same signature
	map<CIdentifier, CLink*>::iterator itLink;
	for(itLink=m_vLink.begin(); itLink!=m_vLink.end(); itLink++)
	{
		CLink* l_pLink=itLink->second;
		if(l_pLink)
		{
			if(l_pLink->getTargetBoxIdentifier()==rTargetBoxIdentifier && l_pLink->getTargetBoxInputIndex()==ui32TargetBoxInputIndex)
			{
				if(l_pLink->getSourceBoxIdentifier()==rSourceBoxIdentifier && l_pLink->getSourceBoxOutputIndex()==ui32SourceBoxOutputIndex)
				{
					// Found a link, so removes it
					delete l_pLink;
					m_vLink.erase(itLink);

					this->getLogManager() << LogLevel_Trace << "Link removed\n";
					return true;
				}
			}
		}
	}

	this->getLogManager() << LogLevel_Warning << "The link does not exist\n";
	return false;
}

boolean CScenario::disconnect(
	const CIdentifier& rLinkIdentifier)
{
	this->getLogManager() << LogLevel_Trace << "Disconnecting boxes\n";

	// Finds the link according to its identifier
	map<CIdentifier, CLink*>::iterator itLink;
	itLink=m_vLink.find(rLinkIdentifier);
	if(itLink==m_vLink.end())
	{
		// The link does not exist !
		this->getLogManager() << LogLevel_Warning << "The link does not exist\n";
		return false;
	}

	this->getLogManager() << LogLevel_Trace << "Found the link !\n";

	// Deletes the link itself
	delete itLink->second;

	// Removes link from the link list
	m_vLink.erase(itLink);

	this->getLogManager() << LogLevel_Trace << "Link removed\n";

	return true;
}

//___________________________________________________________________//
//                                                                   //

CIdentifier CScenario::getNextProcessingUnitIdentifier(
	const CIdentifier& rPreviousIdentifier) const
{
	return getNextTIdentifier<CProcessingUnit, TTestTrue<CProcessingUnit> >(m_vProcessingUnit, rPreviousIdentifier, TTestTrue<CProcessingUnit>());
}

boolean CScenario::isProcessingUnit(
	const CIdentifier& rIdentifier) const
{
	map<CIdentifier, CProcessingUnit*>::const_iterator itProcessingUnit=m_vProcessingUnit.find(rIdentifier);
	return itProcessingUnit!=m_vProcessingUnit.end()?true:false;
}

const IProcessingUnit* CScenario::getProcessingUnitDetails(
	const CIdentifier& rProcessingUnitIdentifier) const
{
	this->getLogManager() << LogLevel_Debug << "Getting const processing unit details from scenario\n";

	map<CIdentifier, CProcessingUnit*>::const_iterator itProcessingUnit;
	itProcessingUnit=m_vProcessingUnit.find(rProcessingUnitIdentifier);
	if(itProcessingUnit==m_vProcessingUnit.end())
	{
		this->getLogManager() << LogLevel_Warning << "The processing unit does not exist\n";
		return NULL;
	}
	return itProcessingUnit->second;
}

IProcessingUnit* CScenario::getProcessingUnitDetails(
	const CIdentifier& rProcessingUnitIdentifier)
{
	this->getLogManager() << LogLevel_Debug << "Getting processing unit details from scenario\n";

	map<CIdentifier, CProcessingUnit*>::const_iterator itProcessingUnit;
	itProcessingUnit=m_vProcessingUnit.find(rProcessingUnitIdentifier);
	if(itProcessingUnit==m_vProcessingUnit.end())
	{
		this->getLogManager() << LogLevel_Warning << "The processing unit does not exist\n";
		return NULL;
	}
	return itProcessingUnit->second;
}

boolean CScenario::addProcessingUnit(
	CIdentifier& rProcessingUnitIdentifier)
{
	this->getLogManager() << LogLevel_Trace << "Adding new processing unit in scenario\n";

	CProcessingUnit* l_pProcessingUnit=new CProcessingUnit(getKernelContext());
	rProcessingUnitIdentifier=getUnusedIdentifier();
	l_pProcessingUnit->setIdentifier(rProcessingUnitIdentifier);

	m_vProcessingUnit[rProcessingUnitIdentifier]=l_pProcessingUnit;
	return true;
}

boolean CScenario::removeProcessingUnit(
	const CIdentifier& rProcessingUnitIdentifier)
{
	this->getLogManager() << LogLevel_Trace << "Removing processing unit from scenario\n";

	// Finds the processing unit according to its identifier
	map<CIdentifier, CProcessingUnit*>::iterator itProcessingUnit;
	itProcessingUnit=m_vProcessingUnit.find(rProcessingUnitIdentifier);
	if(itProcessingUnit==m_vProcessingUnit.end())
	{
		// The processing unit does not exist !
		this->getLogManager() << LogLevel_Warning << "The processing unit does not exist\n";
		return false;
	}

	// Found the processing unit,
	// now unaffect all the boxes that are using this processing unit
	map<CIdentifier, CBox*>::const_iterator itBox;
	for(itBox=m_vBox.begin(); itBox!=m_vBox.end(); )
	{
		if(itBox->second->getProcessingUnitIdentifier() == rProcessingUnitIdentifier)
		{
			this->getLogManager() << LogLevel_Trace << "The box " << itBox->second->getIdentifier() << " is affected to this processing unit - it will now be affected to undefined processing unit\n";

			// Affects to unknown identifier
			itBox->second->setProcessingUnitIdentifier(OV_UndefinedIdentifier);
		}
		itBox++;
	}

	// Deletes the processing unit itself
	delete itProcessingUnit->second;

	// Removes processing unit from the processing unit list
	m_vProcessingUnit.erase(itProcessingUnit);
	return true;
}

//___________________________________________________________________//
//                                                                   //

boolean CScenario::acceptVisitor(
	IObjectVisitor& rObjectVisitor)
{
	CObjectVisitorContext l_oObjectVisitorContext(getKernelContext());

	if(!rObjectVisitor.processBegin(l_oObjectVisitorContext, *this))
	{
		return false;
	}

	map<CIdentifier, CBox*>::iterator i;
	for(i=m_vBox.begin(); i!=m_vBox.end(); i++)
	{
		if(!i->second->acceptVisitor(rObjectVisitor))
		{
			return false;
		}
	}

	map<CIdentifier, CComment*>::iterator j;
	for(j=m_vComment.begin(); j!=m_vComment.end(); j++)
	{
		if(!j->second->acceptVisitor(rObjectVisitor))
		{
			return false;
		}
	}

	map<CIdentifier, CLink*>::iterator k;
	for(k=m_vLink.begin(); k!=m_vLink.end(); k++)
	{
		if(!k->second->acceptVisitor(rObjectVisitor))
		{
			return false;
		}
	}

	map<CIdentifier, CProcessingUnit*>::iterator l;
	for(l=m_vProcessingUnit.begin(); l!=m_vProcessingUnit.end(); l++)
	{
		if(!l->second->acceptVisitor(rObjectVisitor))
		{
			return false;
		}
	}

	if(!getKernelContext().getVisualisationManager().getVisualisationTree(m_oVisualisationTreeIdentifier).acceptVisitor(rObjectVisitor))
	{
		return false;
	}

	if(!rObjectVisitor.processEnd(l_oObjectVisitorContext, *this))
	{
		return false;
	}

	return true;
}

//___________________________________________________________________//
//                                                                   //

CIdentifier CScenario::getUnusedIdentifier(void) const
{
	uint64 l_ui64Identifier=(((uint64)rand())<<32)+((uint64)rand());
	CIdentifier l_oResult;
	map<CIdentifier, CBox*>::const_iterator i;
	map<CIdentifier, CLink*>::const_iterator j;
	do
	{
		l_ui64Identifier++;
		l_oResult=CIdentifier(l_ui64Identifier);
		i=m_vBox.find(l_oResult);
		j=m_vLink.find(l_oResult);
	}
	while(i!=m_vBox.end() || j!= m_vLink.end() || l_oResult==OV_UndefinedIdentifier);
	return l_oResult;
}
