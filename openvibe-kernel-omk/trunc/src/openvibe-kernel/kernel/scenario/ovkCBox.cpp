#include "ovkCBox.h"
#include "ovkCBoxProto.h"
#include "ovkCBoxListenerContext.h"
#include "ovkCScenario.h"

#include "../ovkCObjectVisitorContext.h"

using namespace std;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

//___________________________________________________________________//
//                                                                   //

CBox::CBox(const IKernelContext& rKernelContext, CScenario& rOwnerScenario)
	:TAttributable < TKernelObject < IBox > >(rKernelContext)
	,m_rOwnerScenario(rOwnerScenario)
	,m_pBoxAlgorithmDescriptor(NULL)
	,m_pBoxListener(NULL)
	,m_bIsNotifyingDescriptor(false)
	,m_bIsNotificationActive(true)
	,m_oIdentifier(OV_UndefinedIdentifier)
	,m_oAlgorithmClassIdentifier(OV_UndefinedIdentifier)
	,m_oProcessingUnitIdentifier(OV_UndefinedIdentifier)
	,m_sName("unnamed")
{
}

CBox::~CBox(void)
{
	if(m_pBoxAlgorithmDescriptor && m_pBoxListener)
	{
		CBoxListenerContext l_oContext(this->getKernelContext(), *this, 0xffffffff);
		m_pBoxListener->uninitialize(l_oContext);
		m_pBoxAlgorithmDescriptor->releaseBoxListener(m_pBoxListener);
	}
}

//___________________________________________________________________//
//                                                                   //

CIdentifier CBox::getIdentifier(void) const
{
	return m_oIdentifier;
}

CString CBox::getName(void) const
{
	return m_sName;
}

CIdentifier CBox::getAlgorithmClassIdentifier(void) const
{
	return m_oAlgorithmClassIdentifier;
}

CIdentifier CBox::getProcessingUnitIdentifier(void) const
{
	return m_oProcessingUnitIdentifier;
}

boolean CBox::setIdentifier(
	const CIdentifier& rIdentifier)
{
	if(m_oIdentifier!=OV_UndefinedIdentifier)
	{
		return false;
	}
	if(rIdentifier==OV_UndefinedIdentifier)
	{
		return false;
	}
	m_oIdentifier=rIdentifier;

	this->notify(BoxModification_IdentifierChanged);

	return true;
}

boolean CBox::setName(
	const CString& sName)
{
	m_sName=sName;

	this->notify(BoxModification_NameChanged);

	return true;
}

boolean CBox::setAlgorithmClassIdentifier(
	const CIdentifier& rAlgorithmClassIdentifier)
{
	m_oAlgorithmClassIdentifier=rAlgorithmClassIdentifier;

	if(!getKernelContext().getPluginManager().canCreatePluginObject(rAlgorithmClassIdentifier))
	{
		this->getLogManager() << LogLevel_Warning << "Box algorithm descriptor not found\n";
		return false;
	}

	if(m_pBoxAlgorithmDescriptor && m_pBoxListener)
	{
		CBoxListenerContext l_oContext(this->getKernelContext(), *this, 0xffffffff);
		m_pBoxListener->uninitialize(l_oContext);
		m_pBoxAlgorithmDescriptor->releaseBoxListener(m_pBoxListener);
	}

	const IPluginObjectDesc* l_pPluginObjectDescriptor=getKernelContext().getPluginManager().getPluginObjectDescCreating(rAlgorithmClassIdentifier);
	m_pBoxAlgorithmDescriptor=dynamic_cast<const IBoxAlgorithmDesc*>(l_pPluginObjectDescriptor);

	if(m_pBoxAlgorithmDescriptor)
	{
		m_pBoxListener=m_pBoxAlgorithmDescriptor->createBoxListener();
		if(m_pBoxListener)
		{
			CBoxListenerContext l_oContext(this->getKernelContext(), *this, 0xffffffff);
			m_pBoxListener->initialize(l_oContext);
		}
	}

	this->notify(BoxModification_AlgorithmClassIdentifierChanged);

	return true;
}

boolean CBox::setProcessingUnitIdentifier(
	const CIdentifier& rProcessingUnitIdentifier)
{
	m_oProcessingUnitIdentifier=rProcessingUnitIdentifier;

	this->notify(BoxModification_ProcessingUnitChanged);

	return true;
}

//___________________________________________________________________//
//                                                                   //

boolean CBox::initializeFromAlgorithmClassIdentifier(
	const CIdentifier& rAlgorithmClassIdentifier)
{
	this->disableNotification();

	const IBoxAlgorithmDesc* l_pBoxAlgorithmDesc=dynamic_cast<const IBoxAlgorithmDesc*>(getKernelContext().getPluginManager().getPluginObjectDescCreating(rAlgorithmClassIdentifier));
	if(!l_pBoxAlgorithmDesc)
	{
		this->getLogManager() << LogLevel_Warning << "Algorithm descriptor not found\n";

		this->enableNotification();

		return false;
	}

	clear();
	setName(l_pBoxAlgorithmDesc->getName());
	setAlgorithmClassIdentifier(rAlgorithmClassIdentifier);

	CBoxProto l_oBoxProto(getKernelContext(), *this);
	l_pBoxAlgorithmDesc->getBoxPrototype(l_oBoxProto);

	if(this->hasAttribute(OV_AttributeId_Box_InitialPrototypeHashValue))
	{
		this->setAttributeValue(OV_AttributeId_Box_InitialPrototypeHashValue, this->getPluginManager().getPluginObjectHashValue(rAlgorithmClassIdentifier).toString());
	}
	else
	{
		this->addAttribute(OV_AttributeId_Box_InitialPrototypeHashValue, this->getPluginManager().getPluginObjectHashValue(rAlgorithmClassIdentifier).toString());
	}

	this->enableNotification();

	this->notify(BoxModification_Initialized);

	return true;
}

boolean CBox::initializeFromExistingBox(
	const IBox& rExisitingBox)
{
	uint32 i;

	this->disableNotification();

	clear();
	setName(rExisitingBox.getName());
	setAlgorithmClassIdentifier(rExisitingBox.getAlgorithmClassIdentifier());

	for(i=0; i<rExisitingBox.getInputCount(); i++)
	{
		CIdentifier l_oType;
		CString l_sName;
		rExisitingBox.getInputType(i, l_oType);
		rExisitingBox.getInputName(i, l_sName);
		addInput(l_sName, l_oType);
	}

	for(i=0; i<rExisitingBox.getOutputCount(); i++)
	{
		CIdentifier l_oType;
		CString l_sName;
		rExisitingBox.getOutputType(i, l_oType);
		rExisitingBox.getOutputName(i, l_sName);
		addOutput(l_sName, l_oType);
	}

	for(i=0; i<rExisitingBox.getSettingCount(); i++)
	{
		CIdentifier l_oType;
		CString l_sName;
		CString l_sValue;
		CString l_sDefaultValue;
		rExisitingBox.getSettingType(i, l_oType);
		rExisitingBox.getSettingName(i, l_sName);
		rExisitingBox.getSettingValue(i, l_sValue);
		rExisitingBox.getSettingDefaultValue(i, l_sDefaultValue);
		addSetting(l_sName, l_oType, l_sDefaultValue);
		setSettingValue(i, l_sValue);
	}

	CIdentifier l_oIdentifier=rExisitingBox.getNextAttributeIdentifier(OV_UndefinedIdentifier);
	while(l_oIdentifier!=OV_UndefinedIdentifier)
	{
		addAttribute(l_oIdentifier, rExisitingBox.getAttributeValue(l_oIdentifier));
		l_oIdentifier=rExisitingBox.getNextAttributeIdentifier(l_oIdentifier);
	}

	this->enableNotification();

	this->notify(BoxModification_Initialized);

	return true;
}

//___________________________________________________________________//
//                                                                   //

boolean CBox::addInput(
	const CString& sName,
	const CIdentifier& rTypeIdentifier)
{
	if(!this->getTypeManager().isStream(rTypeIdentifier))
	{
		this->getLogManager() << LogLevel_Warning << "While adding input to box, unknown stream type identifier " << rTypeIdentifier << "\n";
	}

	CInput i;
	i.m_sName=sName;
	i.m_oTypeIdentifier=rTypeIdentifier;
	m_vInput.push_back(i);

	this->notify(BoxModification_InputAdded, m_vInput.size()-1);

	return true;
}

boolean CBox::removeInput(
	const uint32 ui32InputIndex)
{
	CIdentifier l_oIdentifier;
	size_t i;

	if(ui32InputIndex >= m_vInput.size())
	{
		return false;
	}

	while((l_oIdentifier=m_rOwnerScenario.getNextLinkIdentifierToBoxInput(l_oIdentifier, m_oIdentifier, ui32InputIndex))!=OV_UndefinedIdentifier)
	{
		m_rOwnerScenario.disconnect(l_oIdentifier);
	}

	// $$$
	// The way the links are removed here
	// is not correct because they are all
	// collected and then all removed. In case
	// the box listener callback on box removal,
	// the nextcoming links would potentially be
	// invalid
	vector < CIdentifier > l_vLinksToRemove;
	vector < pair < pair < uint64, uint32 >, pair < uint64, uint32 > > > l_vLink;
	while((l_oIdentifier=m_rOwnerScenario.getNextLinkIdentifierToBox(l_oIdentifier, m_oIdentifier))!=OV_UndefinedIdentifier)
	{
		ILink* l_pLink=m_rOwnerScenario.getLinkDetails(l_oIdentifier);
		if(l_pLink->getTargetBoxInputIndex()>ui32InputIndex)
		{
			pair < pair < uint64, uint32 >, pair < uint64, uint32 > > l;
			l.first.first=l_pLink->getSourceBoxIdentifier().toUInteger();
			l.first.second=l_pLink->getSourceBoxOutputIndex();
			l.second.first=l_pLink->getTargetBoxIdentifier().toUInteger();
			l.second.second=l_pLink->getTargetBoxInputIndex();
			l_vLink.push_back(l);
			l_vLinksToRemove.push_back(l_oIdentifier);
		}
	}

	for(i=0; i<l_vLinksToRemove.size(); i++)
	{
		m_rOwnerScenario.disconnect(l_vLinksToRemove[i]);
	}

	m_vInput.erase(m_vInput.begin()+ui32InputIndex);

	for(i=0; i<l_vLink.size(); i++)
	{
		m_rOwnerScenario.connect(
			l_vLink[i].first.first,
			l_vLink[i].first.second,
			l_vLink[i].second.first,
			l_vLink[i].second.second-1,
			l_oIdentifier);
	}

	this->notify(BoxModification_InputRemoved, ui32InputIndex);

	return true;
}

uint32 CBox::getInputCount(void) const
{
	return m_vInput.size();
}

boolean CBox::getInputType(
	const uint32 ui32InputIndex,
	CIdentifier& rTypeIdentifier) const
{
	if(ui32InputIndex>=m_vInput.size())
	{
		return false;
	}
	rTypeIdentifier=m_vInput[ui32InputIndex].m_oTypeIdentifier;
	return true;
}

boolean CBox::getInputName(
	const uint32 ui32InputIndex,
	CString& rName) const
{
	if(ui32InputIndex>=m_vInput.size())
	{
		return false;
	}
	rName=m_vInput[ui32InputIndex].m_sName;
	return true;
}

boolean CBox::setInputType(
	const uint32 ui32InputIndex,
	const CIdentifier& rTypeIdentifier)
{
	if(!this->getTypeManager().isStream(rTypeIdentifier))
	{
		this->getLogManager() << LogLevel_Warning << "While changing box input type, unknown stream type identifier " << rTypeIdentifier << "\n";
	}

	if(ui32InputIndex>=m_vInput.size())
	{
		return false;
	}
	m_vInput[ui32InputIndex].m_oTypeIdentifier=rTypeIdentifier;

	this->notify(BoxModification_InputTypeChanged, ui32InputIndex);

	return true;
}

boolean CBox::setInputName(
	const uint32 ui32InputIndex,
	const CString& rName)
{
	if(ui32InputIndex>=m_vInput.size())
	{
		return false;
	}
	m_vInput[ui32InputIndex].m_sName=rName;

	this->notify(BoxModification_InputNameChanged, ui32InputIndex);

	return true;
}

//___________________________________________________________________//
//                                                                   //

boolean CBox::addOutput(
	const CString& sName,
	const CIdentifier& rTypeIdentifier)
{
	if(!this->getTypeManager().isStream(rTypeIdentifier))
	{
		this->getLogManager() << LogLevel_Warning << "While adding output to box, unknown stream type identifier " << rTypeIdentifier << "\n";
	}

	COutput o;
	o.m_sName=sName;
	o.m_oTypeIdentifier=rTypeIdentifier;
	m_vOutput.push_back(o);

	this->notify(BoxModification_OutputAdded, m_vOutput.size()-1);

	return true;
}

boolean CBox::removeOutput(
	const uint32 ui32OutputIndex)
{
	CIdentifier l_oIdentifier;
	size_t i;

	if(ui32OutputIndex >= m_vOutput.size())
	{
		return false;
	}

	while((l_oIdentifier=m_rOwnerScenario.getNextLinkIdentifierFromBoxOutput(l_oIdentifier, m_oIdentifier, ui32OutputIndex))!=OV_UndefinedIdentifier)
	{
		m_rOwnerScenario.disconnect(l_oIdentifier);
	}

	// $$$
	// The way the links are removed here
	// is not correct because they are all
	// collected and then all removed. In case
	// the box listener callback on box removal,
	// the nextcoming links would potentially be
	// invalid
	vector < CIdentifier > l_vLinksToRemove;
	vector < pair < pair < uint64, uint32 >, pair < uint64, uint32 > > > l_vLink;
	while((l_oIdentifier=m_rOwnerScenario.getNextLinkIdentifierFromBox(l_oIdentifier, m_oIdentifier))!=OV_UndefinedIdentifier)
	{
		ILink* l_pLink=m_rOwnerScenario.getLinkDetails(l_oIdentifier);
		if(l_pLink->getSourceBoxOutputIndex()>ui32OutputIndex)
		{
			pair < pair < uint64, uint32 >, pair < uint64, uint32 > > l;
			l.first.first=l_pLink->getSourceBoxIdentifier().toUInteger();
			l.first.second=l_pLink->getSourceBoxOutputIndex();
			l.second.first=l_pLink->getTargetBoxIdentifier().toUInteger();
			l.second.second=l_pLink->getTargetBoxInputIndex();
			l_vLink.push_back(l);
			l_vLinksToRemove.push_back(l_oIdentifier);
		}
	}

	for(i=0; i<l_vLinksToRemove.size(); i++)
	{
		m_rOwnerScenario.disconnect(l_vLinksToRemove[i]);
	}

	m_vOutput.erase(m_vOutput.begin()+ui32OutputIndex);

	for(i=0; i<l_vLink.size(); i++)
	{
		m_rOwnerScenario.connect(
			l_vLink[i].first.first,
			l_vLink[i].first.second-1,
			l_vLink[i].second.first,
			l_vLink[i].second.second,
			l_oIdentifier);
	}

	this->notify(BoxModification_OutputRemoved, ui32OutputIndex);

	return true;
}

uint32 CBox::getOutputCount(void) const
{
	return m_vOutput.size();
}

boolean CBox::getOutputType(
	const uint32 ui32OutputIndex,
	CIdentifier& rTypeIdentifier) const
{
	if(ui32OutputIndex>=m_vOutput.size())
	{
		return false;
	}
	rTypeIdentifier=m_vOutput[ui32OutputIndex].m_oTypeIdentifier;
	return true;
}

boolean CBox::getOutputName(
	const uint32 ui32OutputIndex,
	CString& rName) const
{
	if(ui32OutputIndex>=m_vOutput.size())
	{
		return false;
	}
	rName=m_vOutput[ui32OutputIndex].m_sName;
	return true;
}

boolean CBox::setOutputType(
	const uint32 ui32OutputIndex,
	const CIdentifier& rTypeIdentifier)
{
	if(!this->getTypeManager().isStream(rTypeIdentifier))
	{
		this->getLogManager() << LogLevel_Warning << "While changing box output type, unknown stream type identifier " << rTypeIdentifier << "\n";
	}

	if(ui32OutputIndex>=m_vOutput.size())
	{
		return false;
	}
	m_vOutput[ui32OutputIndex].m_oTypeIdentifier=rTypeIdentifier;

	this->notify(BoxModification_OutputTypeChanged, ui32OutputIndex);

	return true;
}

boolean CBox::setOutputName(
	const uint32 ui32OutputIndex,
	const CString& rName)
{
	if(ui32OutputIndex>=m_vOutput.size())
	{
		return false;
	}
	m_vOutput[ui32OutputIndex].m_sName=rName;

	this->notify(BoxModification_OutputNameChanged, ui32OutputIndex);

	return true;
}

//___________________________________________________________________//
//                                                                   //

boolean CBox::addSetting(
	const CString& sName,
	const CIdentifier& rTypeIdentifier,
	const CString& sDefaultValue)
{
	CString l_sValue(sDefaultValue);
	if(this->getTypeManager().isEnumeration(rTypeIdentifier))
	{
		if(this->getTypeManager().getEnumerationEntryValueFromName(rTypeIdentifier, sDefaultValue)==OV_UndefinedIdentifier)
		{
			if(this-getTypeManager().getEnumerationEntryCount(rTypeIdentifier)!=0)
			{
				// get value to the first enum entry
				// and eventually correct this after
				uint64 l_ui64Value=0;
				this->getTypeManager().getEnumerationEntry(rTypeIdentifier, 0, l_sValue, l_ui64Value);

				// Find if the default value string actually is an identifier
				CIdentifier l_oIdentifier;
				l_oIdentifier.fromString(sDefaultValue);

				// Finally, if it is an identifier, then a name should be found
				// from the type manager !
				CString l_sCandidateValue=this->getTypeManager().getEnumerationEntryNameFromValue(rTypeIdentifier, l_oIdentifier.toUInteger());
				if(l_sCandidateValue!=CString(""))
				{
					l_sValue=l_sCandidateValue;
				}
			}
		}
	}

	CSetting s;
	s.m_sName=sName;
	s.m_oTypeIdentifier=rTypeIdentifier;
	s.m_sDefaultValue=l_sValue;
	s.m_sValue=l_sValue;

	m_vSetting.push_back(s);

	this->notify(BoxModification_SettingAdded, m_vSetting.size()-1);

	return true;
}

boolean CBox::removeSetting(
	const uint32 ui32SettingIndex)
{
	uint32 i=0;
	vector<CSetting>::iterator it=m_vSetting.begin();
	for(i=0; i<ui32SettingIndex && it!=m_vSetting.end(); i++)
	{
		it++;
	}
	if(it==m_vSetting.end())
	{
		return false;
	}
	it=m_vSetting.erase(it);

	this->notify(BoxModification_SettingRemoved, ui32SettingIndex);

	return true;
}

uint32 CBox::getSettingCount(void) const
{
	return m_vSetting.size();
}

boolean CBox::getSettingType(
	const uint32 ui32SettingIndex,
	CIdentifier& rTypeIdentifier) const
{
	if(ui32SettingIndex>=m_vSetting.size())
	{
		return false;
	}
	rTypeIdentifier=m_vSetting[ui32SettingIndex].m_oTypeIdentifier;
	return true;
}

boolean CBox::getSettingName(
	const uint32 ui32SettingIndex,
	CString& rName) const
{
	if(ui32SettingIndex>=m_vSetting.size())
	{
		return false;
	}
	rName=m_vSetting[ui32SettingIndex].m_sName;
	return true;
}

boolean CBox::getSettingDefaultValue(
	const uint32 ui32SettingIndex,
	CString& rDefaultValue) const
{
	if(ui32SettingIndex>=m_vSetting.size())
	{
		return false;
	}
	rDefaultValue=m_vSetting[ui32SettingIndex].m_sDefaultValue;
	return true;
}

boolean CBox::getSettingValue(
	const uint32 ui32SettingIndex,
	CString& rValue) const
{
	if(ui32SettingIndex>=m_vSetting.size())
	{
		return false;
	}
	rValue=m_vSetting[ui32SettingIndex].m_sValue;
	return true;
}

boolean CBox::setSettingType(
	const uint32 ui32SettingIndex,
	const CIdentifier& rTypeIdentifier)
{
	if(ui32SettingIndex>=m_vSetting.size())
	{
		return false;
	}
	m_vSetting[ui32SettingIndex].m_oTypeIdentifier=rTypeIdentifier;

	this->notify(BoxModification_SettingTypeChanged, ui32SettingIndex);

	return true;
}

boolean CBox::setSettingName(
	const uint32 ui32SettingIndex,
	const CString& rName)
{
	if(ui32SettingIndex>=m_vSetting.size())
	{
		return false;
	}
	m_vSetting[ui32SettingIndex].m_sName=rName;

	this->notify(BoxModification_SettingNameChanged, ui32SettingIndex);

	return true;
}

boolean CBox::setSettingDefaultValue(
	const uint32 ui32SettingIndex,
	const CString& rDefaultValue)
{
	if(ui32SettingIndex>=m_vSetting.size())
	{
		return false;
	}
	m_vSetting[ui32SettingIndex].m_sDefaultValue=rDefaultValue;

	this->notify(BoxModification_SettingDefaultValueChanged, ui32SettingIndex);

	return true;
}

boolean CBox::setSettingValue(
	const uint32 ui32SettingIndex,
	const CString& rValue)
{
	if(ui32SettingIndex>=m_vSetting.size())
	{
		return false;
	}
	m_vSetting[ui32SettingIndex].m_sValue=rValue;

	this->notify(BoxModification_SettingValueChanged, ui32SettingIndex);

	return true;
}

//___________________________________________________________________//
//                                                                   //

void CBox::clear(void)
{
	m_pBoxAlgorithmDescriptor=NULL;
	m_oAlgorithmClassIdentifier=OV_UndefinedIdentifier;
	m_sName="";
	m_vInput.clear();
	m_vOutput.clear();
	m_vSetting.clear();
	this->removeAllAttributes();
}

void CBox::enableNotification(void)
{
	m_bIsNotificationActive=true;
}

void CBox::disableNotification(void)
{
	m_bIsNotificationActive=false;
}

void CBox::notify(
	const EBoxModification eBoxModificationType,
	const uint32 ui32Index)
{
	if(m_pBoxListener && !m_bIsNotifyingDescriptor && m_bIsNotificationActive)
	{
		CBoxListenerContext l_oContext(this->getKernelContext(), *this, ui32Index);
		m_bIsNotifyingDescriptor=true;
		m_pBoxListener->process(l_oContext, eBoxModificationType);
		m_bIsNotifyingDescriptor=false;
	}
}

//___________________________________________________________________//
//                                                                   //

boolean CBox::acceptVisitor(
	IObjectVisitor& rObjectVisitor)
{
	CObjectVisitorContext l_oObjectVisitorContext(getKernelContext());
	return rObjectVisitor.processBegin(l_oObjectVisitorContext, *this) && rObjectVisitor.processEnd(l_oObjectVisitorContext, *this);
}
