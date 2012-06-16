#ifndef __OpenViBEKernel_Kernel_Scenario_CBox_H__
#define __OpenViBEKernel_Kernel_Scenario_CBox_H__

#include "../ovkTKernelObject.h"

#include "ovkTAttributable.h"

#include <vector>
#include <string>
#include <iostream>

namespace OpenViBE
{
	namespace Kernel
	{
		class CBox : public OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::TAttributable<OpenViBE::Kernel::IBox> >
		{
		public:

			CBox(const OpenViBE::Kernel::IKernelContext& rKernelContext);

			virtual OpenViBE::CIdentifier getIdentifier(void) const;
			virtual OpenViBE::CString getName(void) const;
			virtual OpenViBE::CIdentifier getAlgorithmClassIdentifier(void) const;
			virtual OpenViBE::CIdentifier getProcessingUnitIdentifier(void) const;
			virtual OpenViBE::boolean setIdentifier(
				const OpenViBE::CIdentifier& rIdentifier);
			virtual OpenViBE::boolean setName(
				const OpenViBE::CString& sName);
			virtual OpenViBE::boolean setAlgorithmClassIdentifier(
				const OpenViBE::CIdentifier& rAlgorithmClassIdentifier);
			virtual OpenViBE::boolean setProcessingUnitIdentifier(
				const OpenViBE::CIdentifier& rProcessingUnitIdentifier);

			virtual OpenViBE::boolean initializeFromAlgorithmClassIdentifier(
				const OpenViBE::CIdentifier& rAlgorithmClassIdentifier);
			virtual OpenViBE::boolean initializeFromExistingBox(
				const OpenViBE::Kernel::IBox& rExisitingBox);

			virtual OpenViBE::boolean addInput(
				const OpenViBE::CString& sName,
				const OpenViBE::CIdentifier& rTypeIdentifier);
			virtual OpenViBE::boolean removeInput(
				const OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::uint32 getInputCount(void) const;
			virtual OpenViBE::boolean getInputType(
				const OpenViBE::uint32 ui32InputIndex,
				OpenViBE::CIdentifier& rTypeIdentifier) const;
			virtual OpenViBE::boolean getInputName(
				const OpenViBE::uint32 ui32InputIndex,
				OpenViBE::CString& rName) const;
			virtual OpenViBE::boolean setInputType(
				const OpenViBE::uint32 ui32InputIndex,
				const OpenViBE::CIdentifier& rTypeIdentifier);
			virtual OpenViBE::boolean setInputName(
				const OpenViBE::uint32 ui32InputIndex,
				const OpenViBE::CString& rName);

			virtual OpenViBE::boolean addOutput(
				const OpenViBE::CString& sName,
				const OpenViBE::CIdentifier& rTypeIdentifier);
			virtual OpenViBE::boolean removeOutput(
				const OpenViBE::uint32 ui32OutputIndex);
			virtual OpenViBE::uint32 getOutputCount(void) const;
			virtual OpenViBE::boolean getOutputType(
				const OpenViBE::uint32 ui32OutputIndex,
				OpenViBE::CIdentifier& rTypeIdentifier) const;
			virtual OpenViBE::boolean getOutputName(
				const OpenViBE::uint32 ui32OutputIndex,
				OpenViBE::CString& rName) const;
			virtual OpenViBE::boolean setOutputType(
				const OpenViBE::uint32 ui32OutputIndex,
				const OpenViBE::CIdentifier& rTypeIdentifier);
			virtual OpenViBE::boolean setOutputName(
				const OpenViBE::uint32 ui32OutputIndex,
				const OpenViBE::CString& rName);

			virtual OpenViBE::boolean addSetting(
				const OpenViBE::CString& sName,
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const OpenViBE::CString& sDefaultValue);
			virtual OpenViBE::boolean removeSetting(
				const OpenViBE::uint32 ui32Index);
			virtual OpenViBE::uint32 getSettingCount(void) const;
			virtual OpenViBE::boolean getSettingType(
				const OpenViBE::uint32 ui32SettingIndex,
				OpenViBE::CIdentifier& rTypeIdentifier) const;
			virtual OpenViBE::boolean getSettingName(
				const OpenViBE::uint32 ui32SettingIndex,
				OpenViBE::CString& rName) const;
			virtual OpenViBE::boolean getSettingDefaultValue(
				const OpenViBE::uint32 ui32SettingIndex,
				OpenViBE::CString& rDefaultValue) const;
			virtual OpenViBE::boolean getSettingValue(
				const OpenViBE::uint32 ui32SettingIndex,
				OpenViBE::CString& rValue) const;
			virtual OpenViBE::boolean setSettingName(
				const OpenViBE::uint32 ui32SettingIndex,
				const OpenViBE::CString& rName);
			virtual OpenViBE::boolean setSettingType(
				const OpenViBE::uint32 ui32SettingIndex,
				const OpenViBE::CIdentifier& rTypeIdentifier);
			virtual OpenViBE::boolean setSettingDefaultValue(
				const OpenViBE::uint32 ui32SettingIndex,
				const OpenViBE::CString& rDefaultValue);
			virtual OpenViBE::boolean setSettingValue(
				const OpenViBE::uint32 ui32SettingIndex,
				const OpenViBE::CString& rValue);

			virtual OpenViBE::boolean acceptVisitor(
				OpenViBE::IObjectVisitor& rObjectVisitor);

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::TAttributable<OpenViBE::Kernel::IBox> >, OVK_ClassId_Kernel_Scenario_Box)

		protected:

			virtual void clear(void);

			virtual void activateModificationCallback(void);
			virtual void deactivateModificationCallback(void);
			virtual void callModificationCallback(
				const OpenViBE::Kernel::EBoxModification eBoxModificationType);

		protected:

			class CInput
			{
			public:
				CInput(void) { }
				CInput(const CInput& i)
					:m_sName(i.m_sName)
					,m_oTypeIdentifier(i.m_oTypeIdentifier) { }
				OpenViBE::CString m_sName;
				OpenViBE::CIdentifier m_oTypeIdentifier;
			};

			class COutput
			{
			public:
				COutput(void) { }
				COutput(const COutput& o)
					:m_sName(o.m_sName)
					,m_oTypeIdentifier(o.m_oTypeIdentifier) { }
				OpenViBE::CString m_sName;
				OpenViBE::CIdentifier m_oTypeIdentifier;
			};

			class CSetting
			{
			public:
				CSetting(void) { }
				CSetting(const CSetting& s)
					:m_sName(s.m_sName)
					,m_oTypeIdentifier(s.m_oTypeIdentifier)
					,m_sDefaultValue(s.m_sDefaultValue)
					,m_sValue(s.m_sValue) { }
				OpenViBE::CString m_sName;
				OpenViBE::CIdentifier m_oTypeIdentifier;
				OpenViBE::CString m_sDefaultValue;
				OpenViBE::CString m_sValue;
			};

		protected:

			const OpenViBE::Plugins::IBoxAlgorithmDesc* m_pBoxAlgorithmDescriptor;
			OpenViBE::boolean m_bIsNotifyingDescriptor;
			OpenViBE::boolean m_bIsModificationCallbackActive;

			OpenViBE::CIdentifier m_oIdentifier;
			OpenViBE::CIdentifier m_oAlgorithmClassIdentifier;
			OpenViBE::CIdentifier m_oProcessingUnitIdentifier;
			OpenViBE::CString m_sName;

			std::vector<CInput> m_vInput;
			std::vector<COutput> m_vOutput;
			std::vector<CSetting> m_vSetting;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_Scenario_CBox_H__
