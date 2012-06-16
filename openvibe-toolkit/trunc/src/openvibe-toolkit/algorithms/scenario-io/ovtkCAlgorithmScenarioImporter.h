#ifndef __OpenViBEToolkit_Algorithm_ScenarioImporter_H__
#define __OpenViBEToolkit_Algorithm_ScenarioImporter_H__

#include "../ovtkTAlgorithm.h"
#include "ovtk_scenario_io.h"

#define OVP_ClassId_Algorithm_ScenarioImporter     OpenViBE::CIdentifier(0x1EE72169, 0x2BF146C1)
#define OVP_ClassId_Algorithm_ScenarioImporterDesc OpenViBE::CIdentifier(0x503C0DDE, 0x5D4C6CB2)

#define OVTK_Algorithm_ScenarioImporter_OutputParameterId_Scenario    OpenViBE::CIdentifier(0x29574C87, 0x7BA77780)
#define OVTK_Algorithm_ScenarioImporter_InputParameterId_MemoryBuffer OpenViBE::CIdentifier(0x600463A3, 0x474B7F66)

namespace OpenViBEToolkit
{
	class OVTK_API IAlgorithmScenarioImporterContext : public OpenViBEToolkit::IObject
	{
	public:

		virtual OpenViBE::boolean processStart(const OpenViBE::CIdentifier& rIdentifier)=0;
		virtual OpenViBE::boolean processIdentifier(const OpenViBE::CIdentifier& rIdentifier, const OpenViBE::CIdentifier& rValue)=0;
		virtual OpenViBE::boolean processString(const OpenViBE::CIdentifier& rIdentifier, const OpenViBE::CString& rValue)=0;
		virtual OpenViBE::boolean processUInteger(const OpenViBE::CIdentifier& rIdentifier, const OpenViBE::uint64 ui64Value)=0;
		virtual OpenViBE::boolean processStop(void)=0;

		_IsDerivedFromClass_(OpenViBEToolkit::IObject, OV_UndefinedIdentifier);
	};

	class OVTK_API CAlgorithmScenarioImporter : public OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >
	{
	public:

		virtual void release(void) { delete this; }

		virtual OpenViBE::boolean process(void);

		virtual OpenViBE::boolean import(OpenViBEToolkit::IAlgorithmScenarioImporterContext& rContext, const OpenViBE::IMemoryBuffer& rMemoryBuffer)=0;

		_IsDerivedFromClass_(OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >, OVP_ClassId_Algorithm_ScenarioImporter);
	};

	class OVTK_API CAlgorithmScenarioImporterDesc : public OpenViBE::Plugins::IAlgorithmDesc
	{
	public:

		virtual OpenViBE::boolean getAlgorithmPrototype(
			OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
		{
			rAlgorithmPrototype.addOutputParameter(OVTK_Algorithm_ScenarioImporter_OutputParameterId_Scenario,    "Scenario",      OpenViBE::Kernel::ParameterType_Object);
			rAlgorithmPrototype.addInputParameter (OVTK_Algorithm_ScenarioImporter_InputParameterId_MemoryBuffer, "Memory buffer", OpenViBE::Kernel::ParameterType_MemoryBuffer);
			return true;
		}

		_IsDerivedFromClass_Final_(OpenViBE::Plugins::IAlgorithmDesc, OVP_ClassId_Algorithm_ScenarioImporterDesc);
	};
};

#endif // __OpenViBEToolkit_Algorithm_ScenarioImporter_H__
