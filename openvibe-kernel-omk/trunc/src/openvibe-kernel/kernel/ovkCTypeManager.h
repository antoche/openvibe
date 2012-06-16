#ifndef __OpenViBEKernel_Kernel_ITypeManager_H__
#define __OpenViBEKernel_Kernel_ITypeManager_H__

#include "ovkTKernelObject.h"

#include <map>

namespace OpenViBE
{
	namespace Kernel
	{
		class CTypeManager : public OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::ITypeManager>
		{
		public:

			CTypeManager(const OpenViBE::Kernel::IKernelContext& rKernelContext);

			virtual OpenViBE::CIdentifier getNextTypeIdentifier(
				const OpenViBE::CIdentifier& rPreviousIdentifier) const;

			virtual OpenViBE::boolean registerType(
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const OpenViBE::CString& sTypeName);

			virtual OpenViBE::boolean registerStreamType(
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const OpenViBE::CString& sTypeName,
				const OpenViBE::CIdentifier& rParentTypeIdentifier);


			virtual OpenViBE::boolean registerEnumerationType(
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const OpenViBE::CString& sTypeName);
			virtual OpenViBE::boolean registerEnumerationEntry(
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const OpenViBE::CString& sEntryName,
				const OpenViBE::uint64 ui64EntryValue);

			virtual OpenViBE::boolean registerBitMaskType(
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const OpenViBE::CString& sTypeName);
			virtual OpenViBE::boolean registerBitMaskEntry(
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const OpenViBE::CString& sEntryName,
				const OpenViBE::uint64 ui64EntryValue);

			virtual OpenViBE::boolean isRegistered(
				const OpenViBE::CIdentifier& rTypeIdentifier) const;
			virtual OpenViBE::boolean isStream(
				const OpenViBE::CIdentifier& rTypeIdentifier) const;
			virtual OpenViBE::boolean isDerivedFromStream(
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const OpenViBE::CIdentifier& rParentTypeIdentifier) const;
			virtual OpenViBE::boolean isEnumeration(
				const OpenViBE::CIdentifier& rTypeIdentifier) const;
			virtual OpenViBE::boolean isBitMask(
				const OpenViBE::CIdentifier& rTypeIdentifier) const;

			virtual OpenViBE::CString getTypeName(
				const OpenViBE::CIdentifier& rTypeIdentifier) const;
			virtual OpenViBE::CIdentifier getStreamParentType(
				const OpenViBE::CIdentifier& rTypeIdentifier) const;

			virtual OpenViBE::uint64 getEnumerationEntryCount(
				const OpenViBE::CIdentifier& rTypeIdentifier) const;
			virtual OpenViBE::boolean getEnumerationEntry(
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const OpenViBE::uint64 ui64EntryIndex,
				OpenViBE::CString& sEntryName,
				OpenViBE::uint64& rEntryValue) const;
			virtual OpenViBE::CString getEnumerationEntryNameFromValue(
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const OpenViBE::uint64 ui64EntryValue) const;
			virtual OpenViBE::uint64 getEnumerationEntryValueFromName(
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const OpenViBE::CString& rEntryName) const;

			virtual OpenViBE::uint64 getBitMaskEntryCount(
				const OpenViBE::CIdentifier& rTypeIdentifier) const;
			virtual OpenViBE::boolean getBitMaskEntry(
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const OpenViBE::uint64 ui64EntryIndex,
				OpenViBE::CString& sEntryName,
				OpenViBE::uint64& rEntryValue) const;
			virtual OpenViBE::CString getBitMaskEntryNameFromValue(
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const OpenViBE::uint64 ui64EntryValue) const;
			virtual OpenViBE::uint64 getBitMaskEntryValueFromName(
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const OpenViBE::CString& rEntryName) const;
			virtual OpenViBE::CString getBitMaskEntryCompositionNameFromValue(
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const OpenViBE::uint64 ui64EntryCompositionValue) const;
			virtual OpenViBE::uint64 getBitMaskEntryCompositionValueFromName(
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const OpenViBE::CString& rEntryCompositionName) const;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::ITypeManager>, OVK_ClassId_Kernel_TypeManager);

		protected:

			std::map<OpenViBE::CIdentifier, OpenViBE::CString> m_vName;
			std::map<OpenViBE::CIdentifier, std::map<OpenViBE::uint64, OpenViBE::CString> > m_vEnumeration;
			std::map<OpenViBE::CIdentifier, std::map<OpenViBE::uint64, OpenViBE::CString> > m_vBitMask;
			std::map<OpenViBE::CIdentifier, OpenViBE::CIdentifier> m_vStream;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_ITypeManager_H__
