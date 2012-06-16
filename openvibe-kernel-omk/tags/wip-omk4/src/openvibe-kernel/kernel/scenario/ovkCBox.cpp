#include "ovkCBox.h"
#include "ovkCBoxProto.h"

#include "../ovkCObjectVisitorContext.h"

using namespace std;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

//___________________________________________________________________//
//                                                                   //

CBox::CBox(const IKernelContext& rKernelContext)
	:TKernelObject<TAttributable<IBox> >(rKernelContext)
	,m_pBoxAlgorithmDescriptor(NULL)
	,m_bIsNotifyingDescriptor(false)
	,m_bIsModificationCallbackActive(true)
	,m_oIdentifier(OV_UndefinedIdentifier)
	,m_oAlgorithmClassIdentifier(OV_UndefinedIdentifier)
	,m_oProcessingUnitIdentifier(OV_UndefinedIdentifier)
	,m_sName("unnamed")
{
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
	return true;
}

boolean CBox::setName(
	const CString& sName)
{
	m_sName=sName;
	return true;
}

boolean CBox::setAlgorithmClassIdentifier(
	const CIdentifier& rAlgorithmClassIdentifier)
{
	m_oAlgorithmClassIdentifier=rAlgorithmClassIdentifier;

	if(!getKernelContext().getPluginManager().canCreatePluginObject(rAlgorithmClassIdentifier))
	{
		log() << LogLevel_Warning << "Algorithm descriptor not found\n";

		return false;
	}

	const IPluginObjectDesc* l_pPluginObjectDescriptor=getKernelContext().getPluginManager().getPluginObjectDescCreating(rAlgorithmClassIdentifier);
	m_pBoxAlgorithmDescriptor=dynamic_cast<const IBoxAlgorithmDesc*>(l_pPluginObjectDescriptor);

	return true;
}

boolean CBox::setProcessingUnitIdentifier(
	const CIdentifier& rProcessingUnitIdentifier)
{
	m_oProcessingUnitIdentifier=rProcessingUnitIdentifier;
	return true;
}

//___________________________________________________________________//
//                                                                   //

boolean CBox::initializeFromAlgorithmClassIdentifier(
	const CIdentifier& rAlgorithmClassIdentifier)
{
	this->deactivateModificationCallback();

	const IBoxAlgorithmDesc* l_pBoxAlgorithmDesc=dynamic_cast<const IBoxAlgorithmDesc*>(getKernelContext().getPluginManager().getPluginObjectDescCreating(rAlgorithmClassIdentifier));
	if(!l_pBoxAlgorithmDesc)
	{
		log() << LogLevel_Warning << "Algorithm descriptor not found\n";

		this->activateModificationCallback();

		return false;
	}


	clear();
	setName(l_pBoxAlgorithmDesc->getName());
	setAlgorithmClassIdentifier(rAlgorithmClassIdentifier);

	CBoxProto l_oBoxProto(getKernelContext(), *this);
	l_pBoxAlgorithmDesc->getBoxPrototype(l_oBoxProto);

	return true;
}

boolean CBox::initializeFromExistingBox(
	const IBox& rExisitingBox)
{
	uint32 i;

	this->deactivateModificationCallback();

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

	this->activateModificationCallback();

	return true;
}

//___________________________________________________________________//
//                                                                   //

boolean CBox::addInput(
	const CString& sName,
	const CIdentifier& rTypeIdentifier)
{
	CInput i;
	i.m_sName=sName;
	i.m_oTypeIdentifier=rTypeIdentifier;
	m_vInput.push_back(i);

	this->callModificationCallback(BoxModification_InputAdded);

	return true;
}

boolean CBox::removeInput(
	const uint32 ui32InputIndex)
{
	uint32 i=0;
	vector<CInput>::iterator it=m_vInput.begin();
	for(i=0; i<ui32InputIndex && it!=m_vInput.end(); i++)
	{
		it++;
	}
	if(it==m_vInput.end())
	{
		return false;
	}
	it=m_vInput.erase(it);

	this->callModificationCallback(BoxModification_InputRemoved);

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
	if(ui32InputIndex>=m_vInput.size())
	{
		return false;
	}
	m_vInput[ui32InputIndex].m_oTypeIdentifier=rTypeIdentifier;

	this->callModificationCallback(BoxModification_InputTypeChanged);

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
	return true;
}

//___________________________________________________________________//
//                                                                   //

boolean CBox::addOutput(
	const CString& sName,
	const CIdentifier& rTypeIdentifier)
{
	COutput o;
	o.m_sName=sName;
	o.m_oTypeIdentifier=rTypeIdentifier;
	m_vOutput.push_back(o);

	this->callModificationCallback(BoxModification_OutputAdded);

	return true;
}

boolean CBox::removeOutput(
	const uint32 ui32OutputIndex)
{
	uint32 i=0;
	vector<COutput>::iterator it=m_vOutput.begin();
	for(i=0; i<ui32OutputIndex && it!=m_vOutput.end(); i++)
	{
		it++;
	}
	if(it==m_vOutput.end())
	{
		return false;
	}
	it=m_vOutput.erase(it);

	this->callModificationCallback(BoxModification_OutputRemoved);

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
	if(ui32OutputIndex>=m_vOutput.size())
	{
		return false;
	}
	m_vOutput[ui32OutputIndex].m_oTypeIdentifier=rTypeIdentifier;

	this->callModificationCallback(BoxModification_OutputTypeChanged);

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
	return true;
}

//___________________________________________________________________//
//                                                                   //

boolean CBox::addSetting(
	const CString& sName,
	const CIdentifier& rTypeIdentifier,
	const CString& sDefaultValue)
{
	CSetting s;
	s.m_sName=sName;
	s.m_oTypeIdentifier=rTypeIdentifier;
	s.m_sDefaultValue=sDefaultValue;
	s.m_sValue=sDefaultValue;
	m_vSetting.push_back(s);

	this->callModificationCallback(BoxModification_SettingAdded);

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

	this->callModificationCallback(BoxModification_SettingRemoved);

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

	this->callModificationCallback(BoxModification_SettingTypeChanged);

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

	this->callModificationCallback(BoxModification_SettingDefaultValueChanged);

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

	this->callModificationCallback(BoxModification_SettingValueChanged);

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

void CBox::activateModificationCallback(void)
{
	m_bIsModificationCallbackActive=true;
}

void CBox::deactivateModificationCallback(void)
{
	m_bIsModificationCallbackActive=false;
}

void CBox::callModificationCallback(
	const EBoxModification eBoxModificationType)
{
	if(m_pBoxAlgorithmDescriptor && !m_bIsNotifyingDescriptor && m_bIsModificationCallbackActive)
	{
		m_bIsNotifyingDescriptor=true;
		m_pBoxAlgorithmDescriptor->boxModificationCallback(
			*((IBoxAlgorithmContext*)NULL),
			eBoxModificationType);
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
