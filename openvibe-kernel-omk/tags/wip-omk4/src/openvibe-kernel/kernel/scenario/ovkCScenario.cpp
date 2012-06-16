#include "ovkCScenario.h"
#include "ovkCScenarioExporterContext.h"
#include "ovkCScenarioImporterContext.h"

#include "../ovkCObjectVisitorContext.h"

#include <iostream>
#include <map>
#include <vector>

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
		boolean operator()(map<CIdentifier, ILink*>::const_iterator it) const { return it->second->getSourceBoxIdentifier()==m_rId; }
		const CIdentifier& m_rId;
	};

	struct TTestEqSourceBoxOutput
	{
		TTestEqSourceBoxOutput(const CIdentifier& rId, uint32 ui32Id) : m_rId(rId), m_ui32Id(ui32Id) { }
		boolean operator()(map<CIdentifier, ILink*>::const_iterator it) const { return it->second->getSourceBoxIdentifier()==m_rId && it->second->getSourceBoxOutputIndex()==m_ui32Id; }
		const CIdentifier& m_rId;
		uint32 m_ui32Id;
	};

	struct TTestEqTargetBox
	{
		TTestEqTargetBox(const CIdentifier& rId) : m_rId(rId) { }
		boolean operator()(map<CIdentifier, ILink*>::const_iterator it) const { return it->second->getTargetBoxIdentifier()==m_rId; }
		const CIdentifier& m_rId;
	};

	struct TTestEqTargetBoxInput
	{
		TTestEqTargetBoxInput(const CIdentifier& rId, uint32 ui32Id) : m_rId(rId), m_ui32Id(ui32Id) { }
		boolean operator()(map<CIdentifier, ILink*>::const_iterator it) const { return it->second->getTargetBoxIdentifier()==m_rId && it->second->getTargetBoxInputIndex()==m_ui32Id; }
		const CIdentifier& m_rId;
		uint32 m_ui32Id;
	};

	struct TTestEqProcessingUnitIdentifier
	{
		TTestEqProcessingUnitIdentifier(const CIdentifier& rId) : m_rId(rId) { }
		boolean operator()(map<CIdentifier, IBox*>::const_iterator it) const { return it->second->getProcessingUnitIdentifier()==m_rId; }
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

CScenario::CScenario(const IKernelContext& rKernelContext)
	:TKernelObject<TAttributable<IScenario> >(rKernelContext)
{
}

//___________________________________________________________________//
//                                                                   //

boolean CScenario::clear(void)
{
	log() << LogLevel_Trace << "Clearing scenario\n";

	// Clears boxes
	map<CIdentifier, IBox*>::iterator itBox;
	for(itBox=m_vBox.begin(); itBox!=m_vBox.end(); itBox++)
	{
		OpenViBE::Tools::CKernelObjectFactoryHelper(getKernelContext().getKernelObjectFactory()).releaseObject(itBox->second);
	}
	m_vBox.clear();

	// Clears links
	map<CIdentifier, ILink*>::iterator itLink;
	for(itLink=m_vLink.begin(); itLink!=m_vLink.end(); itLink++)
	{
		OpenViBE::Tools::CKernelObjectFactoryHelper(getKernelContext().getKernelObjectFactory()).releaseObject(itLink->second);
	}
	m_vLink.clear();

	// Clears attributes
	this->removeAllAttributes();

	return true;
}

boolean CScenario::load(
	const CString& sFileName)
{
	// TODO
	log() << LogLevel_Debug << "CScenario::load - Not implemented yet\n";

	return false;
}

boolean CScenario::load(
	const CString& sFileName,
	const CIdentifier& rLoaderIdentifier)
{
	log() << LogLevel_Info << "Loading scenario with specific importer\n";

	IScenarioImporter* l_pScenarioImporter=dynamic_cast<IScenarioImporter*>(getKernelContext().getPluginManager().createPluginObject(rLoaderIdentifier));
	if(!l_pScenarioImporter)
	{
		log() << LogLevel_Warning << "Importer not found\n";
		return false;
	}
	CScenarioImporterContext l_oScenarioImporterContext(getKernelContext(), sFileName, *this);
	boolean l_bResult=l_pScenarioImporter->doImport(l_oScenarioImporterContext);
	getKernelContext().getPluginManager().releasePluginObject(l_pScenarioImporter);

	if(!l_bResult)
	{
		log() << LogLevel_Warning << "Import failed...\n";
		return false;
	}
	return true;
}

boolean CScenario::save(
	const CString& sFileName)
{
	// TODO
	log() << LogLevel_Debug << "CScenario::save - Not implemented yet\n";

	return false;
}

boolean CScenario::save(
	const CString& sFileName,
	const CIdentifier& rSaverIdentifier)
{
	log() << LogLevel_Info << "Saving scenario with specific exporter\n";

	IScenarioExporter* l_pScenarioExporter=dynamic_cast<IScenarioExporter*>(getKernelContext().getPluginManager().createPluginObject(rSaverIdentifier));
	if(!l_pScenarioExporter)
	{
		log() << LogLevel_Warning << "Exporter not found\n";
		return false;
	}
	CScenarioExporterContext l_oScenarioExporterContext(getKernelContext(), sFileName, *this);
	boolean l_bResult=l_pScenarioExporter->doExport(l_oScenarioExporterContext);
	getKernelContext().getPluginManager().releasePluginObject(l_pScenarioExporter);

	if(!l_bResult)
	{
		log() << LogLevel_Warning << "Export failed...\n";
		return false;
	}
	return true;
}

boolean CScenario::merge(const IScenario& rScenario)
{
	log() << LogLevel_Info << "Merging scenario with specific scenario\n";

	map<CIdentifier, CIdentifier> l_vBoxBox;

	// adds boxes
	CIdentifier l_oBoxIdentifier=getNextBoxIdentifier(OV_UndefinedIdentifier);
	while(l_oBoxIdentifier!=OV_UndefinedIdentifier)
	{
		const IBox* l_pBox=rScenario.getBoxDetails(l_oBoxIdentifier);
		if(l_pBox)
		{
			CIdentifier l_oMergedBox;
			this->addBox(*l_pBox, l_oMergedBox);
			l_vBoxBox[l_oBoxIdentifier]=l_oMergedBox;
		}
		l_oBoxIdentifier=getNextBoxIdentifier(l_oBoxIdentifier);
	}

	// adds links
	CIdentifier l_oLinkIdentifier=getNextLinkIdentifier(OV_UndefinedIdentifier);
	while(l_oLinkIdentifier!=OV_UndefinedIdentifier)
	{
		const ILink* l_pLink=rScenario.getLinkDetails(l_oLinkIdentifier);
		if(l_pLink)
		{
			CIdentifier l_oMergedLink;
			this->connect(
				l_vBoxBox[l_pLink->getSourceBoxIdentifier()],
				l_pLink->getSourceBoxOutputIndex(),
				l_vBoxBox[l_pLink->getTargetBoxIdentifier()],
				l_pLink->getTargetBoxInputIndex(),
				l_oMergedLink);
		}
		l_oLinkIdentifier=getNextLinkIdentifier(l_oLinkIdentifier);
	}

	return true;
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

#if 0
boolean CScenario::enumerateBoxes(
	IScenario::IBoxEnum& rCallback) const
{
	// log() << LogLevel_Info << "Enumerating scenario boxes\n";

	rCallback.preCallback();
	map<CIdentifier, IBox*>::const_iterator itBox=m_vBox.begin();
	while(itBox!=m_vBox.end())
	{
		if(!rCallback.callback(*this, *(itBox->second)))
		{
			rCallback.postCallback();
			return true;
		}
		itBox++;
	}
	rCallback.postCallback();
	return true;
}
#endif

CIdentifier CScenario::getNextBoxIdentifier(
	const CIdentifier& rPreviousIdentifier) const
{
	return getNextTIdentifier<IBox, TTestTrue<IBox> >(m_vBox, rPreviousIdentifier, TTestTrue<IBox>());
}

CIdentifier CScenario::getNextBoxIdentifierOnProcessingUnit(
	const CIdentifier& rPreviousIdentifier,
	const CIdentifier& rProcessingUnitIdentifier) const
{
	return getNextTIdentifier<IBox, TTestEqProcessingUnitIdentifier >(m_vBox, rPreviousIdentifier, TTestEqProcessingUnitIdentifier(rProcessingUnitIdentifier));
}

const IBox* CScenario::getBoxDetails(
	const CIdentifier& rBoxIdentifier) const
{
	log() << LogLevel_Debug << "Getting const box details from scenario\n";

	map<CIdentifier, IBox*>::const_iterator itBox;
	itBox=m_vBox.find(rBoxIdentifier);
	if(itBox==m_vBox.end())
	{
		log() << LogLevel_Warning << "The box does not exist\n";
		return NULL;
	}
	return itBox->second;
}

boolean CScenario::isBox(
	const CIdentifier& rIdentifier) const
{
	map<CIdentifier, IBox*>::const_iterator itBox;
	itBox=m_vBox.find(rIdentifier);
	return itBox!=m_vBox.end();
}

IBox* CScenario::getBoxDetails(
	const CIdentifier& rBoxIdentifier)
{
	log() << LogLevel_Debug << "Getting box details from scenario\n";

	map<CIdentifier, IBox*>::const_iterator itBox;
	itBox=m_vBox.find(rBoxIdentifier);
	if(itBox==m_vBox.end())
	{
		log() << LogLevel_Warning << "The box does not exist\n";
		return NULL;
	}
	return itBox->second;
}

boolean CScenario::addBox(
	CIdentifier& rBoxIdentifier)
{
	log() << LogLevel_Trace << "Adding new empty box in scenario\n";

	IBox* l_pBox=OpenViBE::Tools::CKernelObjectFactoryHelper(getKernelContext().getKernelObjectFactory()).createObject<IBox*>(OV_ClassId_Kernel_Scenario_Box);
	rBoxIdentifier=getUnusedIdentifier();
	l_pBox->setIdentifier(rBoxIdentifier);

	m_vBox[rBoxIdentifier]=l_pBox;
	return true;
}

boolean CScenario::addBox(
	const IBox& rBox,
	CIdentifier& rBoxIdentifier)
{
	log () << LogLevel_Trace << "Adding a new box in the scenario based on an existing one\n";

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
	log() << LogLevel_Trace << "Adding new box in scenario\n";

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
	log() << LogLevel_Trace << "Removing box from scenario\n";

	// Finds the box according to its identifier
	map<CIdentifier, IBox*>::iterator itBox;
	itBox=m_vBox.find(rBoxIdentifier);
	if(itBox==m_vBox.end())
	{
		// The box does not exist !
		log() << LogLevel_Warning << "The box does not exist\n";
		return false;
	}

	log() << LogLevel_Trace << "Found the box !\n";

	// Found the box,
	// now find all the links that are used by this box
	map<CIdentifier, ILink*>::iterator itLink;
	for(itLink=m_vLink.begin(); itLink!=m_vLink.end(); )
	{
		map<CIdentifier, ILink*>::iterator itLinkCurrent=itLink;
		itLink++;

		if(itLinkCurrent->second->getSourceBoxIdentifier()==rBoxIdentifier || itLinkCurrent->second->getTargetBoxIdentifier()==rBoxIdentifier)
		{
			log() << LogLevel_Trace << "Found a link to this box - it will be deleted !\n";

			// Deletes this link
			OpenViBE::Tools::CKernelObjectFactoryHelper(getKernelContext().getKernelObjectFactory()).releaseObject(itLinkCurrent->second);

			// Removes link from the link list
			m_vLink.erase(itLinkCurrent);
		}
	}

	// Deletes the box itself
	OpenViBE::Tools::CKernelObjectFactoryHelper(getKernelContext().getKernelObjectFactory()).releaseObject(itBox->second);

	// Removes box from the box list
	m_vBox.erase(itBox);

	log() << LogLevel_Trace << "The box has been removed\n";

	return true;
}

//___________________________________________________________________//
//                                                                   //

#if 0
boolean CScenario::enumerateLinks(
	IScenario::ILinkEnum& rCallback) const
{
	log() << LogLevel_Debug << "Enumerating scenario links\n";

	rCallback.preCallback();
	map<CIdentifier, ILink*>::const_iterator itLink=m_vLink.begin();
	while(itLink!=m_vLink.end())
	{
		if(!rCallback.callback(*this, *(itLink->second)))
		{
			rCallback.postCallback();
			return true;
		}
		itLink++;
	}
	rCallback.postCallback();
	return true;
}

boolean CScenario::enumerateLinksFromBox(
	IScenario::ILinkEnum& rCallback,
	const CIdentifier& rBoxIdentifier) const
{
	log() << LogLevel_Debug << "Enumerating scenario links from specific box\n";

	rCallback.preCallback();
	map<CIdentifier, ILink*>::const_iterator itLink;
	for(itLink=m_vLink.begin(); itLink!=m_vLink.end(); itLink++)
	{
		if(itLink->second->getSourceBoxIdentifier()==rBoxIdentifier)
		{
			if(!rCallback.callback(*this, *(itLink->second)))
			{
				rCallback.postCallback();
				return true;
			}
		}
	}
	rCallback.postCallback();
	return true;
}

boolean CScenario::enumerateLinksFromBoxOutput(
	IScenario::ILinkEnum& rCallback,
	const CIdentifier& rBoxIdentifier,
	const uint32 ui32OutputIndex) const
{
	log() << LogLevel_Debug << "Enumerating scenario links from specific box output\n";

	rCallback.preCallback();
	map<CIdentifier, ILink*>::const_iterator itLink;
	for(itLink=m_vLink.begin(); itLink!=m_vLink.end(); itLink++)
	{
		if(itLink->second->getSourceBoxIdentifier()==rBoxIdentifier)
		{
			if(itLink->second->getSourceBoxOutputIndex()==ui32OutputIndex)
			{
				if(!rCallback.callback(*this, *(itLink->second)))
				{
					rCallback.postCallback();
					return true;
				}
			}
		}
	}
	rCallback.postCallback();
	return true;
}

boolean CScenario::enumerateLinksToBox(
	IScenario::ILinkEnum& rCallback,
	const CIdentifier& rBoxIdentifier) const
{
	log() << LogLevel_Debug << "Enumerating scenario links to specific box\n";

	rCallback.preCallback();
	map<CIdentifier, ILink*>::const_iterator itLink;
	for(itLink=m_vLink.begin(); itLink!=m_vLink.end(); itLink++)
	{
		if(itLink->second->getTargetBoxIdentifier()==rBoxIdentifier)
		{
			if(!rCallback.callback(*this, *(itLink->second)))
			{
				rCallback.postCallback();
				return true;
			}
		}
	}
	rCallback.postCallback();
	return true;
}
#endif

CIdentifier CScenario::getNextLinkIdentifier(
	const CIdentifier& rPreviousIdentifier) const
{
	return getNextTIdentifier<ILink, TTestTrue<ILink> >(m_vLink, rPreviousIdentifier, TTestTrue<ILink>());
}

CIdentifier CScenario::getNextLinkIdentifierFromBox(
	const CIdentifier& rPreviousIdentifier,
	const CIdentifier& rBoxIdentifier) const
{
	return getNextTIdentifier<ILink, TTestEqSourceBox>(m_vLink, rPreviousIdentifier, TTestEqSourceBox(rBoxIdentifier));
}

CIdentifier CScenario::getNextLinkIdentifierFromBoxOutput(
	const CIdentifier& rPreviousIdentifier,
	const CIdentifier& rBoxIdentifier,
	const uint32 ui32OutputIndex) const
{
	return getNextTIdentifier<ILink, TTestEqSourceBoxOutput>(m_vLink, rPreviousIdentifier, TTestEqSourceBoxOutput(rBoxIdentifier, ui32OutputIndex));
}

CIdentifier CScenario::getNextLinkIdentifierToBox(
	const CIdentifier& rPreviousIdentifier,
	const CIdentifier& rBoxIdentifier) const
{
	return getNextTIdentifier<ILink, TTestEqTargetBox>(m_vLink, rPreviousIdentifier, TTestEqTargetBox(rBoxIdentifier));
}

CIdentifier CScenario::getNextLinkIdentifierToBoxInput(
	const CIdentifier& rPreviousIdentifier,
	const CIdentifier& rBoxIdentifier,
	const uint32 ui32InputInex) const
{

	return getNextTIdentifier<ILink, TTestEqTargetBoxInput>(m_vLink, rPreviousIdentifier, TTestEqTargetBoxInput(rBoxIdentifier, ui32InputInex));
}

boolean CScenario::isLink(
	const CIdentifier& rIdentifier) const
{
	map<CIdentifier, ILink*>::const_iterator itLink;
	itLink=m_vLink.find(rIdentifier);
	return itLink!=m_vLink.end();
}

const ILink* CScenario::getLinkDetails(
	const CIdentifier& rLinkIdentifier) const
{
	log() << LogLevel_Debug << "Retrieving const link details from scenario\n";

	map<CIdentifier, ILink*>::const_iterator itLink;
	itLink=m_vLink.find(rLinkIdentifier);
	if(itLink==m_vLink.end())
	{
		log() << LogLevel_Warning << "The link does not exist\n";
		return NULL;
	}
	return itLink->second;
}

ILink* CScenario::getLinkDetails(
	const CIdentifier& rLinkIdentifier)
{
	log() << LogLevel_Debug << "Retrieving link details from scenario\n";

	map<CIdentifier, ILink*>::const_iterator itLink;
	itLink=m_vLink.find(rLinkIdentifier);
	if(itLink==m_vLink.end())
	{
		log() << LogLevel_Warning << "The link does not exist\n";
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
	log() << LogLevel_Trace << "Connecting boxes\n";

	map<CIdentifier, IBox*>::const_iterator itBox1;
	map<CIdentifier, IBox*>::const_iterator itBox2;
	itBox1=m_vBox.find(rSourceBoxIdentifier);
	itBox2=m_vBox.find(rTargetBoxIdentifier);
	if(itBox1==m_vBox.end() || itBox2==m_vBox.end())
	{
		log() << LogLevel_Warning << "At least one of the boxes does not exist\n";
		return false;
	}
	IBox* l_pSourceBox=itBox1->second;
	IBox* l_pTargetBox=itBox2->second;
	if(ui32SourceBoxOutputIndex >= l_pSourceBox->getOutputCount())
	{
		log() << LogLevel_Warning << "Wrong output index\n";
		return false;
	}
	if(ui32TargetBoxInputIndex >= l_pTargetBox->getInputCount())
	{
		log() << LogLevel_Warning << "Wrong input index\n";
		return false;
	}

	rLinkIdentifier=getUnusedIdentifier();

	ILink* l_pLink=OpenViBE::Tools::CKernelObjectFactoryHelper(getKernelContext().getKernelObjectFactory()).createObject<ILink*>(OV_ClassId_Kernel_Scenario_Link);
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
	// TODO
	log() << LogLevel_Debug << "CScenario::disconnect (identifier based) - Not implemented yet\n";

	return false;
}

boolean CScenario::disconnect(
	const CIdentifier& rLinkIdentifier)
{
	log() << LogLevel_Trace << "Disconnecting boxes\n";

	// Finds the link according to its identifier
	map<CIdentifier, ILink*>::iterator itLink;
	itLink=m_vLink.find(rLinkIdentifier);
	if(itLink==m_vLink.end())
	{
		// The link does not exist !
		log() << LogLevel_Warning << "The link does not exist\n";
		return false;
	}

	log() << LogLevel_Trace << "Found the link !\n";

	// Deletes the link itself
	OpenViBE::Tools::CKernelObjectFactoryHelper(getKernelContext().getKernelObjectFactory()).releaseObject(itLink->second);

	// Removes link from the link list
	m_vLink.erase(itLink);

	log() << LogLevel_Trace << "Link removed\n";

	return true;
}

//___________________________________________________________________//
//                                                                   //

CIdentifier CScenario::getNextProcessingUnitIdentifier(
	const CIdentifier& rPreviousIdentifier) const
{
	return getNextTIdentifier<IProcessingUnit, TTestTrue<IProcessingUnit> >(m_vProcessingUnit, rPreviousIdentifier, TTestTrue<IProcessingUnit>());
}

boolean CScenario::isProcessingUnit(
	const CIdentifier& rIdentifier) const
{
	map<CIdentifier, IProcessingUnit*>::const_iterator itProcessingUnit=m_vProcessingUnit.find(rIdentifier);
	return itProcessingUnit!=m_vProcessingUnit.end()?true:false;
}

const IProcessingUnit* CScenario::getProcessingUnitDetails(
	const CIdentifier& rProcessingUnitIdentifier) const
{
	log() << LogLevel_Debug << "Getting const processing unit details from scenario\n";

	map<CIdentifier, IProcessingUnit*>::const_iterator itProcessingUnit;
	itProcessingUnit=m_vProcessingUnit.find(rProcessingUnitIdentifier);
	if(itProcessingUnit==m_vProcessingUnit.end())
	{
		log() << LogLevel_Warning << "The processing unit does not exist\n";
		return NULL;
	}
	return itProcessingUnit->second;
}

IProcessingUnit* CScenario::getProcessingUnitDetails(
	const CIdentifier& rProcessingUnitIdentifier)
{
	log() << LogLevel_Debug << "Getting processing unit details from scenario\n";

	map<CIdentifier, IProcessingUnit*>::const_iterator itProcessingUnit;
	itProcessingUnit=m_vProcessingUnit.find(rProcessingUnitIdentifier);
	if(itProcessingUnit==m_vProcessingUnit.end())
	{
		log() << LogLevel_Warning << "The processing unit does not exist\n";
		return NULL;
	}
	return itProcessingUnit->second;
}

boolean CScenario::addProcessingUnit(
	CIdentifier& rProcessingUnitIdentifier)
{
	log() << LogLevel_Trace << "Adding new processing unit in scenario\n";

	IProcessingUnit* l_pProcessingUnit=OpenViBE::Tools::CKernelObjectFactoryHelper(getKernelContext().getKernelObjectFactory()).createObject<IProcessingUnit*>(OV_ClassId_Kernel_Scenario_ProcessingUnit);
	rProcessingUnitIdentifier=getUnusedIdentifier();
	l_pProcessingUnit->setIdentifier(rProcessingUnitIdentifier);

	m_vProcessingUnit[rProcessingUnitIdentifier]=l_pProcessingUnit;
	return true;
}

boolean CScenario::removeProcessingUnit(
	const CIdentifier& rProcessingUnitIdentifier)
{
	log() << LogLevel_Trace << "Removing processing unit from scenario\n";

	// Finds the processing unit according to its identifier
	map<CIdentifier, IProcessingUnit*>::iterator itProcessingUnit;
	itProcessingUnit=m_vProcessingUnit.find(rProcessingUnitIdentifier);
	if(itProcessingUnit==m_vProcessingUnit.end())
	{
		// The processing unit does not exist !
		log() << LogLevel_Warning << "The processing unit does not exist\n";
		return false;
	}

	// Found the processing unit,
	// now unaffect all the boxes that are using this processing unit
	map<CIdentifier, IBox*>::const_iterator itBox;
	for(itBox=m_vBox.begin(); itBox!=m_vBox.end(); )
	{
		if(itBox->second->getProcessingUnitIdentifier() == rProcessingUnitIdentifier)
		{
			log() << LogLevel_Trace << "The box " << itBox->second->getIdentifier() << " is affected to this processing unit - it will now be affected to undefined processing unit\n";

			// Affects to unknown identifier
			itBox->second->setProcessingUnitIdentifier(OV_UndefinedIdentifier);
		}
		itBox++;
	}

	// Deletes the processing unit itself
	OpenViBE::Tools::CKernelObjectFactoryHelper(getKernelContext().getKernelObjectFactory()).releaseObject(itProcessingUnit->second);

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

	map<CIdentifier, IBox*>::iterator i;
	for(i=m_vBox.begin(); i!=m_vBox.end(); i++)
	{
		if(!i->second->acceptVisitor(rObjectVisitor))
		{
			return false;
		}
	}

	map<CIdentifier, ILink*>::iterator j;
	for(j=m_vLink.begin(); j!=m_vLink.end(); j++)
	{
		if(!j->second->acceptVisitor(rObjectVisitor))
		{
			return false;
		}
	}

	map<CIdentifier, IProcessingUnit*>::iterator k;
	for(k=m_vProcessingUnit.begin(); k!=m_vProcessingUnit.end(); k++)
	{
		if(!k->second->acceptVisitor(rObjectVisitor))
		{
			return false;
		}
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
	map<CIdentifier, IBox*>::const_iterator i;
	map<CIdentifier, ILink*>::const_iterator j;
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
