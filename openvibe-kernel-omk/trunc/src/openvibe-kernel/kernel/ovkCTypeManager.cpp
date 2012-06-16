#include "ovkCTypeManager.h"

#include "../ovk_tools.h"

#include <string>
#include <algorithm>
#include <functional>
#include <cctype>
#include <cstdio>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;

namespace
{
	// because std::tolower has multiple signatures,
	// it can not be easily used in std::transform
	// this workaround is taken from http://www.gcek.net/ref/books/sw/cpp/ticppv2/
	template <class charT>
	charT to_lower(charT c)
	{
		return std::tolower(c);
	}
};

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
	this->getLogManager() << LogLevel_Trace << "Registered type id " << rTypeIdentifier << " - " << sTypeName << "\n";
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
	this->getLogManager() << LogLevel_Trace << "Registered stream type id " << rTypeIdentifier << "::" << rParentTypeIdentifier << " - " << sTypeName << "\n";
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
	this->getLogManager() << LogLevel_Trace << "Registered enumeration type id " << rTypeIdentifier << " - " << sTypeName << "\n";
	return true;
}

boolean CTypeManager::registerEnumerationEntry(
	const CIdentifier& rTypeIdentifier,
	const CString& sEntryName,
	const uint64 ui64EntryValue)
{
	std::map<CIdentifier, std::map<uint64, CString> >::iterator itEnumeration=m_vEnumeration.find(rTypeIdentifier);
	if(itEnumeration==m_vEnumeration.end())
	{
		return false;
	}
	std::map<uint64, CString>::iterator itEnumerationEntry=itEnumeration->second.find(ui64EntryValue);
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
	this->getLogManager() << LogLevel_Trace << "Registered bitmask type id " << rTypeIdentifier << " - " << sTypeName << "\n";
	return true;
}

boolean CTypeManager::registerBitMaskEntry(
	const CIdentifier& rTypeIdentifier,
	const CString& sEntryName,
	const uint64 ui64EntryValue)
{
	std::map<CIdentifier, std::map<uint64, CString> >::iterator itBitMask=m_vBitMask.find(rTypeIdentifier);
	if(itBitMask==m_vBitMask.end())
	{
		return false;
	}
	std::map<uint64, CString>::iterator itBitMaskEntry=itBitMask->second.find(ui64EntryValue);
	if(itBitMaskEntry!=itBitMask->second.end())
	{
		return false;
	}
	for(uint32 l_ui32BitCount=0, i=0; i<64; i++)
	{
		if(ui64EntryValue&(1LL<<i))
		{
			l_ui32BitCount++;
			if(l_ui32BitCount>1)
			{
				this->getLogManager() << LogLevel_ImportantWarning << "Discarded bitmask entry (" << m_vName[rTypeIdentifier] << ":" << sEntryName << ") because value " << ui64EntryValue << " contains more than one bit\n";
				return false;
			}
		}
	}
	itBitMask->second[ui64EntryValue]=sEntryName;
	return true;
}

boolean CTypeManager::isRegistered(
	const CIdentifier& rTypeIdentifier) const
{
	return m_vName.find(rTypeIdentifier)!=m_vName.end()?true:false;
}

boolean CTypeManager::isStream(
	const CIdentifier& rTypeIdentifier) const
{
	return m_vStream.find(rTypeIdentifier)!=m_vStream.end()?true:false;
}

boolean CTypeManager::isDerivedFromStream(
	const CIdentifier& rTypeIdentifier,
	const CIdentifier& rParentTypeIdentifier) const
{
	std::map < CIdentifier, CIdentifier >::const_iterator it=m_vStream.find(rTypeIdentifier);
	std::map < CIdentifier, CIdentifier >::const_iterator itParent=m_vStream.find(rParentTypeIdentifier);
	if(it==m_vStream.end()) return false;
	if(itParent==m_vStream.end()) return false;
	while(it!=m_vStream.end())
	{
		if(it->first==rParentTypeIdentifier)
		{
			return true;
		}
		it=m_vStream.find(it->second);
	}
	return false;
}

boolean CTypeManager::isEnumeration(
	const CIdentifier& rTypeIdentifier) const
{
	return m_vEnumeration.find(rTypeIdentifier)!=m_vEnumeration.end()?true:false;
}

boolean CTypeManager::isBitMask(
	const CIdentifier& rTypeIdentifier) const
{
	return m_vBitMask.find(rTypeIdentifier)!=m_vBitMask.end()?true:false;
}

CString CTypeManager::getTypeName(
	const CIdentifier& rTypeIdentifier) const
{
	if(!isRegistered(rTypeIdentifier))
	{
		return CString("");
	}
	return m_vName.find(rTypeIdentifier)->second;
}

CIdentifier CTypeManager::getStreamParentType(
	const CIdentifier& rTypeIdentifier) const
{
	if(!isStream(rTypeIdentifier))
	{
		return OV_UndefinedIdentifier;
	}
	return m_vStream.find(rTypeIdentifier)->second;
}

uint64 CTypeManager::getEnumerationEntryCount(
	const CIdentifier& rTypeIdentifier) const
{
	std::map<CIdentifier, std::map<uint64, CString> >::const_iterator itEnumeration=m_vEnumeration.find(rTypeIdentifier);
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
	uint64& rEntryValue) const
{
	std::map<CIdentifier, std::map<uint64, CString> >::const_iterator itEnumeration=m_vEnumeration.find(rTypeIdentifier);
	if(itEnumeration==m_vEnumeration.end())
	{
		return false;
	}

	if(ui64EntryIndex>=itEnumeration->second.size())
	{
		return false;
	}

	std::map<uint64, CString>::const_iterator itEnumerationEntry=itEnumeration->second.begin();
	for(uint64 i=0; i<ui64EntryIndex && itEnumerationEntry!=itEnumeration->second.end(); i++, itEnumerationEntry++)
	{
	}

	rEntryValue=itEnumerationEntry->first;
	sEntryName=itEnumerationEntry->second;
	return true;

}

CString CTypeManager::getEnumerationEntryNameFromValue(
	const CIdentifier& rTypeIdentifier,
	const uint64 ui64EntryValue) const
{
	std::map<CIdentifier, std::map<uint64, CString> >::const_iterator itEnumeration=m_vEnumeration.find(rTypeIdentifier);
	if(itEnumeration==m_vEnumeration.end())
	{
		return "";
	}
	std::map<uint64, CString>::const_iterator itEnumerationEntry=itEnumeration->second.find(ui64EntryValue);
	if(itEnumerationEntry==itEnumeration->second.end())
	{
		return "";
	}
	return itEnumeration->second.find(ui64EntryValue)->second;
}

uint64 CTypeManager::getEnumerationEntryValueFromName(
	const CIdentifier& rTypeIdentifier,
	const CString& rEntryName) const
{
	std::map<CIdentifier, std::map<uint64, CString> >::const_iterator itEnumeration=m_vEnumeration.find(rTypeIdentifier);
	std::map<uint64, CString>::const_iterator itEnumerationEntry;
	if(itEnumeration==m_vEnumeration.end())
	{
		return 0xffffffffffffffffll;
	}

	// first looks at the exact std::string match
	for(itEnumerationEntry=itEnumeration->second.begin(); itEnumerationEntry!=itEnumeration->second.end(); itEnumerationEntry++)
	{
		if(itEnumerationEntry->second==rEntryName)
		{
			return itEnumerationEntry->first;
		}
	}

	// then looks at the caseless std::string match
	std::string l_sEntryNameLower=rEntryName.toASCIIString();
	std::transform(l_sEntryNameLower.begin(), l_sEntryNameLower.end(), l_sEntryNameLower.begin(), ::to_lower<std::string::value_type>);
	for(itEnumerationEntry=itEnumeration->second.begin(); itEnumerationEntry!=itEnumeration->second.end(); itEnumerationEntry++)
	{
		std::string l_sItEntryNameLower=itEnumerationEntry->second.toASCIIString();
		std::transform(l_sItEntryNameLower.begin(), l_sItEntryNameLower.end(), l_sItEntryNameLower.begin(), ::to_lower<std::string::value_type>);
		if(l_sItEntryNameLower==l_sEntryNameLower)
		{
			return itEnumerationEntry->first;
		}
	}

	// then looks at the std::string being the value itself
	uint64 l_ui64Value;
	if(::sscanf((const char*)rEntryName, "%lli", &l_ui64Value)==1)
	{
		if(itEnumeration->second.find(l_ui64Value)!=itEnumeration->second.end() || this->getConfigurationManager().expandAsBoolean("Kernel_AllowUnregisteredNumericalStimulationIdentifiers"))
		{
			return l_ui64Value;
		}
	}

	return 0xffffffffffffffffll;
}

uint64 CTypeManager::getBitMaskEntryCount(
	const CIdentifier& rTypeIdentifier) const
{
	std::map<CIdentifier, std::map<uint64, CString> >::const_iterator itBitMask=m_vBitMask.find(rTypeIdentifier);
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
	uint64& rEntryValue) const
{
	std::map<CIdentifier, std::map<uint64, CString> >::const_iterator itBitMask=m_vBitMask.find(rTypeIdentifier);
	if(itBitMask==m_vBitMask.end())
	{
		return false;
	}

	if(ui64EntryIndex>=itBitMask->second.size())
	{
		return false;
	}

	std::map<uint64, CString>::const_iterator itBitMaskEntry=itBitMask->second.begin();
	for(uint64 i=0; i<ui64EntryIndex && itBitMaskEntry!=itBitMask->second.end(); i++, itBitMaskEntry++)
	{
	}

	rEntryValue=itBitMaskEntry->first;
	sEntryName=itBitMaskEntry->second;
	return true;

}

CString CTypeManager::getBitMaskEntryNameFromValue(
	const CIdentifier& rTypeIdentifier,
	const uint64 ui64EntryValue) const
{
	std::map<CIdentifier, std::map<uint64, CString> >::const_iterator itBitMask=m_vBitMask.find(rTypeIdentifier);
	if(itBitMask==m_vBitMask.end())
	{
		return "";
	}
	std::map<uint64, CString>::const_iterator itBitMaskEntry=itBitMask->second.find(ui64EntryValue);
	if(itBitMaskEntry==itBitMask->second.end())
	{
		return "";
	}
	return itBitMask->second.find(ui64EntryValue)->second;
}

uint64 CTypeManager::getBitMaskEntryValueFromName(
	const CIdentifier& rTypeIdentifier,
	const CString& rEntryName) const
{
	std::map<CIdentifier, std::map<uint64, CString> >::const_iterator itBitMask=m_vBitMask.find(rTypeIdentifier);
	std::map<uint64, CString>::const_iterator itBitMaskEntry;
	if(itBitMask==m_vBitMask.end())
	{
		return 0xffffffffffffffffll;
	}

	// first looks at the exact std::string match
	for(itBitMaskEntry=itBitMask->second.begin(); itBitMaskEntry!=itBitMask->second.end(); itBitMaskEntry++)
	{
		if(itBitMaskEntry->second==rEntryName)
		{
			return itBitMaskEntry->first;
		}
	}

	// then looks at the caseless std::string match
	std::string l_sEntryNameLower=rEntryName.toASCIIString();
	std::transform(l_sEntryNameLower.begin(), l_sEntryNameLower.end(), l_sEntryNameLower.begin(), ::to_lower<std::string::value_type>);
	for(itBitMaskEntry=itBitMask->second.begin(); itBitMaskEntry!=itBitMask->second.end(); itBitMaskEntry++)
	{
		std::string l_sItEntryNameLower=itBitMaskEntry->second.toASCIIString();
		std::transform(l_sItEntryNameLower.begin(), l_sItEntryNameLower.end(), l_sItEntryNameLower.begin(), ::to_lower<std::string::value_type>);
		if(l_sItEntryNameLower==l_sEntryNameLower)
		{
			return itBitMaskEntry->first;
		}
	}

	// then looks at the std::string being the value itself
	uint64 l_ui64Value;
	if(sscanf((const char*)rEntryName, "%lli", &l_ui64Value)==1)
	{
		if(itBitMask->second.find(l_ui64Value)!=itBitMask->second.end())
		{
			return l_ui64Value;
		}
	}

	return 0xffffffffffffffffll;
}

CString CTypeManager::getBitMaskEntryCompositionNameFromValue(
	const CIdentifier& rTypeIdentifier,
	const uint64 ui64EntryCompositionValue) const
{
	std::map<CIdentifier, std::map<uint64, CString> >::const_iterator itBitMask=m_vBitMask.find(rTypeIdentifier);
	if(itBitMask==m_vBitMask.end())
	{
		return "";
	}

	std::string l_sResult;
	for(uint32 i=0; i<64; i++)
	{
		if(ui64EntryCompositionValue&(1LL<<i))
		{
			std::map<uint64, CString>::const_iterator itBitMaskEntry=itBitMask->second.find(ui64EntryCompositionValue&(1LL<<i));
			if(itBitMaskEntry==itBitMask->second.end())
			{
				return "";
			}
			if(l_sResult=="")
			{
				l_sResult=itBitMaskEntry->second.toASCIIString();
			}
			else
			{
				;
				l_sResult+=std::string(1, OV_Value_EnumeratedStringSeparator);
				l_sResult+=itBitMaskEntry->second.toASCIIString();
			}
		}
	}
	return CString(l_sResult.c_str());
}

uint64 CTypeManager::getBitMaskEntryCompositionValueFromName(
	const CIdentifier& rTypeIdentifier,
	const CString& rEntryCompositionName) const
{
	std::map<CIdentifier, std::map<uint64, CString> >::const_iterator itBitMask=m_vBitMask.find(rTypeIdentifier);
	if(itBitMask==m_vBitMask.end())
	{
		return 0;
	}

	uint64 l_ui64Result=0;
	std::string l_sEntryCompositionName=rEntryCompositionName.toASCIIString();
	std::string::size_type i=0;
	std::string::size_type j=0;
	do
	{
		i=l_sEntryCompositionName.find(OV_Value_EnumeratedStringSeparator, i);
		if(i==std::string::npos)
		{
			i=l_sEntryCompositionName.length();
		}

		if(i!=j)
		{
			std::string l_sEntryName;
			l_sEntryName.assign(l_sEntryCompositionName, j, i-j);

			boolean l_bFound=false;
			std::map<uint64, CString>::const_iterator itBitMaskEntry;
			for(itBitMaskEntry=itBitMask->second.begin(); itBitMaskEntry!=itBitMask->second.end(); itBitMaskEntry++)
			{
				if(itBitMaskEntry->second==CString(l_sEntryName.c_str()))
				{
					l_ui64Result|=itBitMaskEntry->first;
					l_bFound=true;
				}
			}

			if(!l_bFound)
			{
				return 0;
			}
		}

		i++;
		j=i;
	}
	while(i<l_sEntryCompositionName.length());

	return l_ui64Result;
}
