#ifndef __OpenViBEKernel_Kernel_Configuration_CConfigurationManager_H__
#define __OpenViBEKernel_Kernel_Configuration_CConfigurationManager_H__

#include "../ovkTKernelObject.h"

#include <map>
#include <string>

namespace OpenViBE
{
	namespace Kernel
	{
		typedef struct
		{
			OpenViBE::CString m_sConfigurationName;
			OpenViBE::CString m_sConfigurationValue;
		} SConfigurationToken;

		class CConfigurationManager : public OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IConfigurationManager >
		{
		public:

			CConfigurationManager(const OpenViBE::Kernel::IKernelContext& rKernelContext, OpenViBE::Kernel::IConfigurationManager* pParentConfigurationManager=NULL);

			virtual void clear(void);

			virtual OpenViBE::boolean addConfigurationFromFile(
				const OpenViBE::CString& rFileNameWildCard);

			virtual OpenViBE::CIdentifier createConfigurationToken(
				const OpenViBE::CString& rConfigurationTokenName,
				const OpenViBE::CString& rConfigurationTokenValue);
			virtual OpenViBE::boolean releaseConfigurationToken(
				const OpenViBE::CIdentifier& rConfigurationTokenIdentifier);
			virtual OpenViBE::CIdentifier getNextConfigurationTokenIdentifier(
				const OpenViBE::CIdentifier& rPreviousConfigurationTokenIdentifier) const;

			virtual OpenViBE::CString getConfigurationTokenName(
				const OpenViBE::CIdentifier& rConfigurationTokenIdentifier) const;
			virtual OpenViBE::CString getConfigurationTokenValue(
				const OpenViBE::CIdentifier& rConfigurationTokenIdentifier) const;

			virtual OpenViBE::boolean setConfigurationTokenName(
				const OpenViBE::CIdentifier& rConfigurationTokenIdentifier,
				const OpenViBE::CString& rConfigurationTokenName);
			virtual OpenViBE::boolean setConfigurationTokenValue(
				const OpenViBE::CIdentifier& rConfigurationTokenIdentifier,
				const OpenViBE::CString& rConfigurationTokenValue);

			virtual OpenViBE::CIdentifier lookUpConfigurationTokenIdentifier(
				const OpenViBE::CString& rConfigurationTokenName,
				const OpenViBE::boolean bRecursive) const;
			virtual OpenViBE::CString lookUpConfigurationTokenValue(
				const OpenViBE::CString& rConfigurationTokenName) const;

			virtual OpenViBE::CString expand(
				const OpenViBE::CString& rExpression) const;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IConfigurationManager >, OVK_ClassId_Kernel_Configuration_ConfigurationManager);

			virtual OpenViBE::float64 expandAsFloat(
				const OpenViBE::CString& rExpression,
				const OpenViBE::float64 f64FallbackValue) const;
			virtual OpenViBE::int64 expandAsInteger(
				const OpenViBE::CString& rExpression,
				const OpenViBE::int64 i64FallbackValue) const;
			virtual OpenViBE::uint64 expandAsUInteger(
				const OpenViBE::CString& rExpression,
				const OpenViBE::uint64 ui64FallbackValue) const;
			virtual OpenViBE::boolean expandAsBoolean(
				const OpenViBE::CString& rExpression,
				const OpenViBE::boolean bFallbackValue) const;
			virtual OpenViBE::uint64 expandAsEnumerationEntryValue(
				const OpenViBE::CString& rExpression,
				const OpenViBE::CIdentifier& rEnumerationTypeIdentifier,
				const OpenViBE::uint64 ui64FallbackValue) const;

		protected:

			virtual OpenViBE::CIdentifier getUnusedIdentifier(void) const;

			OpenViBE::boolean internalExpand(const std::string& sValue, std::string& sResult) const;
			OpenViBE::boolean internalGetConfigurationTokenValueFromName(const std::string& sTokenName, std::string& sTokenValue) const;

		protected:

			OpenViBE::Kernel::IConfigurationManager* m_pParentConfigurationManager;

		protected:

			mutable OpenViBE::uint32 m_ui32Index;
			mutable OpenViBE::uint32 m_ui32StartTime;

			OpenViBE::uint32 getRandom(void) const;
			OpenViBE::uint32 getIndex(void) const;
			OpenViBE::CString getTime(void) const;
			OpenViBE::CString getDate(void) const;
			OpenViBE::uint32 getRealTime(void) const;
			OpenViBE::uint32 getProcessId(void) const;

		protected:

			std::map < OpenViBE::CIdentifier, OpenViBE::Kernel::SConfigurationToken > m_vConfigurationToken;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_Configuration_CConfigurationManager_H__
