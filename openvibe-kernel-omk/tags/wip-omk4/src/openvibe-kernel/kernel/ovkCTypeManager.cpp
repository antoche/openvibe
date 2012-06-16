#include "ovkCTypeManager.h"

#include "../ovk_tools.h"

#include <stdio.h>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace std;

CTypeManager::CTypeManager(const IKernelContext& rKernelContext)
	:TKernelObject<ITypeManager>(rKernelContext)
{
	m_vName[OV_UndefinedIdentifier]="undefined";
}


CIdentifier CTypeManager::getNextTypeIdentifier(
	const CIdentifier& rPreviousIdentifier) const
{
	return getNextIdentifier< CString >(m_vName, rPreviousIdentifier);
}

boolean CTypeManager::registerType(
	const CIdentifier& rTypeIdentifier,
	const CString& sTypeName)
{
	if(isRegistered(rTypeIdentifier))
	{
		return false;
	}
	m_vName[rTypeIdentifier]=sTypeName;
	log() << LogLevel_Trace << "Registered type id " << rTypeIdentifier << " - " << sTypeName << "\n";
	return true;
}

boolean CTypeManager::registerStreamType(
	const CIdentifier& rTypeIdentifier,
	const CString& sTypeName,
	const CIdentifier& rParentTypeIdentifier)
{
	if(isRegistered(rTypeIdentifier))
	{
		return false;
	}
	if(rParentTypeIdentifier!=OV_UndefinedIdentifier && !isStream(rParentTypeIdentifier))
	{
		return false;
	}
	m_vName[rTypeIdentifier]=sTypeName;
	m_vStream[rTypeIdentifier]=rParentTypeIdentifier;
	log() << LogLevel_Trace << "Registered stream type id " << rTypeIdentifier << "::" << rParentTypeIdentifier << " - " << sTypeName << "\n";
	return true;
}

boolean CTypeManager::registerEnumerationType(
	const CIdentifier& rTypeIdentifier,
	const CString& sTypeName)
{
	if(isRegistered(rTypeIdentifier))
	{
		return false;
	}
	m_vName[rTypeIdentifier]=sTypeName;
	m_vEnumeration[rTypeIdentifier];
	log() << LogLevel_Trace << "Registered enumeration type id " << rTypeIdentifier << " - " << sTypeName << "\n";
	return true;
}

boolean CTypeManager::registerEnumerationEntry(
	const CIdentifier& rTypeIdentifier,
	const CString& sEntryName,
	const uint64 ui64EntryValue)
{
	map<CIdentifier, map<uint64, CString> >::iterator itEnumeration=m_vEnumeration.find(rTypeIdentifier);
	if(itEnumeration==m_vEnumeration.end())
	{
		return false;
	}
	map<uint64, CString>::iterator itEnumerationEntry=itEnumeration->second.find(ui64EntryValue);
	if(itEnumerationEntry!=itEnumeration->second.end())
	{
		return false;
	}
	itEnumeration->second[ui64EntryValue]=sEntryName;
	return true;
}

boolean CTypeManager::registerBitMaskType(
	const CIdentifier& rTypeIdentifier,
	const CString& sTypeName)
{
	if(isRegistered(rTypeIdentifier))
	{
		return false;
	}
	m_vName[rTypeIdentifier]=sTypeName;
	m_vBitMask[rTypeIdentifier];
	log() << LogLevel_Trace << "Registered bitmask type id " << rTypeIdentifier << " - " << sTypeName << "\n";
	return true;
}

boolean CTypeManager::registerBitMaskEntry(
	const CIdentifier& rTypeIdentifier,
	const CString& sEntryName,
	const uint64 ui64EntryValue)
{
	map<CIdentifier, map<uint64, CString> >::iterator itBitMask=m_vBitMask.find(rTypeIdentifier);
	if(itBitMask==m_vBitMask.end())
	{
		return false;
	}
	map<uint64, CString>::iterator itBitMaskEntry=itBitMask->second.find(ui64EntryValue);
	if(itBitMaskEntry!=itBitMask->second.end())
	{
		return false;
	}
	itBitMask->second[ui64EntryValue]=sEntryName;
	return true;
}

boolean CTypeManager::isRegistered(
	const CIdentifier& rTypeIdentifier)
{
	return m_vName.find(rTypeIdentifier)!=m_vName.end()?true:false;
}

boolean CTypeManager::isStream(
	const CIdentifier& rTypeIdentifier)
{
	return m_vStream.find(rTypeIdentifier)!=m_vStream.end()?true:false;
}

boolean CTypeManager::isEnumeration(
	const CIdentifier& rTypeIdentifier)
{
	return m_vEnumeration.find(rTypeIdentifier)!=m_vEnumeration.end()?true:false;
}

boolean CTypeManager::isBitMask(
	const CIdentifier& rTypeIdentifier)
{
	return m_vBitMask.find(rTypeIdentifier)!=m_vBitMask.end()?true:false;
}

CString CTypeManager::getTypeName(
	const CIdentifier& rTypeIdentifier)
{
	if(!isRegistered(rTypeIdentifier))
	{
		return CString("");
	}
	return m_vName[rTypeIdentifier];
}

CIdentifier CTypeManager::getStreamParentType(
	const CIdentifier& rTypeIdentifier)
{
	if(!isStream(rTypeIdentifier))
	{
		return OV_UndefinedIdentifier;
	}
	return m_vStream[rTypeIdentifier];
}

uint64 CTypeManager::getEnumerationEntryCount(
	const CIdentifier& rTypeIdentifier)
{
	map<CIdentifier, map<uint64, CString> >::iterator itEnumeration=m_vEnumeration.find(rTypeIdentifier);
	if(itEnumeration==m_vEnumeration.end())
	{
		return 0;
	}
	return itEnumeration->second.size();
}

boolean CTypeManager::getEnumerationEntry(
	const CIdentifier& rTypeIdentifier,
	const uint64 ui64EntryIndex,
	CString& sEntryName,
	uint64& rEntryValue)
{
	map<CIdentifier, map<uint64, CString> >::iterator itEnumeration=m_vEnumeration.find(rTypeIdentifier);
	if(itEnumeration==m_vEnumeration.end())
	{
		return false;
	}

	if(ui64EntryIndex>=itEnumeration->second.size())
	{
		return false;
	}

	map<uint64, CString>::iterator itEnumerationEntry=itEnumeration->second.begin();
	for(uint64 i=0; i<ui64EntryIndex && itEnumerationEntry!=itEnumeration->second.end(); i++, itEnumerationEntry++)
	{
	}

	rEntryValue=itEnumerationEntry->first;
	sEntryName=itEnumerationEntry->second;
	return true;

}

CString CTypeManager::getEnumerationEntryNameFromValue(
	const CIdentifier& rTypeIdentifier,
	const uint64 ui64EntryValue)
{
	map<CIdentifier, map<uint64, CString> >::iterator itEnumeration=m_vEnumeration.find(rTypeIdentifier);
	if(itEnumeration==m_vEnumeration.end())
	{
		return "";
	}
	map<uint64, CString>::iterator itEnumerationEntry=itEnumeration->second.find(ui64EntryValue);
	if(itEnumerationEntry==itEnumeration->second.end())
	{
		return "";
	}
	return itEnumeration->second[ui64EntryValue];
}

uint64 CTypeManager::getEnumerationEntryValueFromName(
	const CIdentifier& rTypeIdentifier,
	const CString& rEntryName)
{
	map<CIdentifier, map<uint64, CString> >::iterator itEnumeration=m_vEnumeration.find(rTypeIdentifier);
	if(itEnumeration==m_vEnumeration.end())
	{
		return 0xffffffffffffffffll;
	}

	uint64 l_ui64Value;
	if(sscanf((const char*)rEntryName, "%lli", &l_ui64Value)==1)
	{
		return l_ui64Value;
	}

	map<uint64, CString>::iterator itEnumerationEntry;
	for(itEnumerationEntry=itEnumeration->second.begin(); itEnumerationEntry!=itEnumeration->second.end(); itEnumerationEntry++)
	{
		if(itEnumerationEntry->second==rEntryName)
		{
			return itEnumerationEntry->first;
		}
	}
	return 0xffffffffffffffffll;
}

uint64 CTypeManager::getBitMaskEntryCount(
	const CIdentifier& rTypeIdentifier)
{
	map<CIdentifier, map<uint64, CString> >::iterator itBitMask=m_vBitMask.find(rTypeIdentifier);
	if(itBitMask==m_vBitMask.end())
	{
		return 0;
	}
	return itBitMask->second.size();
}

boolean CTypeManager::getBitMaskEntry(
	const CIdentifier& rTypeIdentifier,
	const uint64 ui64EntryIndex,
	CString& sEntryName,
	uint64& rEntryValue)
{
	map<CIdentifier, map<uint64, CString> >::iterator itBitMask=m_vBitMask.find(rTypeIdentifier);
	if(itBitMask==m_vBitMask.end())
	{
		return false;
	}

	if(ui64EntryIndex>=itBitMask->second.size())
	{
		return false;
	}

	map<uint64, CString>::iterator itBitMaskEntry=itBitMask->second.begin();
	for(uint64 i=0; i<ui64EntryIndex && itBitMaskEntry!=itBitMask->second.end(); i++, itBitMaskEntry++)
	{
	}

	rEntryValue=itBitMaskEntry->first;
	sEntryName=itBitMaskEntry->second;
	return true;

}

CString CTypeManager::getBitMaskEntryNameFromValue(
	const CIdentifier& rTypeIdentifier,
	const uint64 ui64EntryValue)
{
	map<CIdentifier, map<uint64, CString> >::iterator itBitMask=m_vBitMask.find(rTypeIdentifier);
	if(itBitMask==m_vBitMask.end())
	{
		return "";
	}
	map<uint64, CString>::iterator itBitMaskEntry=itBitMask->second.find(ui64EntryValue);
	if(itBitMaskEntry==itBitMask->second.end())
	{
		return "";
	}
	return itBitMask->second[ui64EntryValue];
}

uint64 CTypeManager::getBitMaskEntryValueFromName(
	const CIdentifier& rTypeIdentifier,
	const CString& rEntryName)
{
	map<CIdentifier, map<uint64, CString> >::iterator itBitMask=m_vBitMask.find(rTypeIdentifier);
	if(itBitMask==m_vBitMask.end())
	{
		return 0xffffffffffffffffll;
	}
	map<uint64, CString>::iterator itBitMaskEntry;
	for(itBitMaskEntry=itBitMask->second.begin(); itBitMaskEntry!=itBitMask->second.end(); itBitMaskEntry++)
	{
		if(itBitMaskEntry->second==rEntryName)
		{
			return itBitMaskEntry->first;
		}
	}
	return 0xffffffffffffffffll;
}
