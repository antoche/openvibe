#include "ovkCBoxProto.h"

#include <cstring>
#include <cstdlib>
#include <cstdio>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

CBoxProto::CBoxProto(const IKernelContext& rKernelContext, IBox& rBox)
	:TKernelObject<IBoxProto>(rKernelContext)
	,m_rBox(rBox)
{
}

uint32 CBoxProto::addInput(
	const CString& sName,
	const CIdentifier& rTypeIdentifier)
{
	if(!m_rBox.addInput(sName, rTypeIdentifier))
	{
		return false;
	}

	char l_sBuffer[1024];
	::sprintf(l_sBuffer, "%d", m_rBox.getInputCount());
	if(m_rBox.hasAttribute(OV_AttributeId_Box_InitialInputCount))
	{
		m_rBox.setAttributeValue(OV_AttributeId_Box_InitialInputCount, l_sBuffer);
	}
	else
	{
		m_rBox.addAttribute(OV_AttributeId_Box_InitialInputCount, l_sBuffer);
	}

	return true;
}

uint32 CBoxProto::addOutput(
	const CString& sName,
	const CIdentifier& rTypeIdentifier)
{
	if(!m_rBox.addOutput(sName, rTypeIdentifier))
	{
		return false;
	}

	char l_sBuffer[1024];
	::sprintf(l_sBuffer, "%d", m_rBox.getOutputCount());
	if(m_rBox.hasAttribute(OV_AttributeId_Box_InitialOutputCount))
	{
		m_rBox.setAttributeValue(OV_AttributeId_Box_InitialOutputCount, l_sBuffer);
	}
	else
	{
		m_rBox.addAttribute(OV_AttributeId_Box_InitialOutputCount, l_sBuffer);
	}

	return true;
}

uint32 CBoxProto::addSetting(
	const CString& sName,
	const CIdentifier& rTypeIdentifier,
	const CString& sDefaultValue)
{
	if(!m_rBox.addSetting(sName, rTypeIdentifier, sDefaultValue))
	{
		return false;
	}

	char l_sBuffer[1024];
	::sprintf(l_sBuffer, "%d", m_rBox.getSettingCount());
	if(m_rBox.hasAttribute(OV_AttributeId_Box_InitialSettingCount))
	{
		m_rBox.setAttributeValue(OV_AttributeId_Box_InitialSettingCount, l_sBuffer);
	}
	else
	{
		m_rBox.addAttribute(OV_AttributeId_Box_InitialSettingCount, l_sBuffer);
	}

	return true;
}

boolean CBoxProto::addFlag(
	const EBoxFlag eBoxFlag)
{
	switch (eBoxFlag)
	{
		case BoxFlag_CanAddInput:      m_rBox.addAttribute(OV_AttributeId_Box_FlagCanAddInput,      ""); break;
		case BoxFlag_CanModifyInput:   m_rBox.addAttribute(OV_AttributeId_Box_FlagCanModifyInput,   ""); break;
		case BoxFlag_CanAddOutput:     m_rBox.addAttribute(OV_AttributeId_Box_FlagCanAddOutput,     ""); break;
		case BoxFlag_CanModifyOutput:  m_rBox.addAttribute(OV_AttributeId_Box_FlagCanModifyOutput,  ""); break;
		case BoxFlag_CanAddSetting:    m_rBox.addAttribute(OV_AttributeId_Box_FlagCanAddSetting,    ""); break;
		case BoxFlag_CanModifySetting: m_rBox.addAttribute(OV_AttributeId_Box_FlagCanModifySetting, ""); break;
		case BoxFlag_IsDeprecated:
		case BoxFlag_IsUnstable:
			break;
		default:
			return false;
	}
	return true;
}
