#ifndef __OpenViBEToolkit_Algorithm_ScenarioExporter_H__
#define __OpenViBEToolkit_Algorithm_ScenarioExporter_H__

#include "../ovtkTAlgorithm.h"
#include "ovtk_scenario_io.h"

#define OVTK_ClassId_Algorithm_ScenarioExporter     OpenViBE::CIdentifier(0x7C281FEA, 0x40B66277)
#define OVTK_ClassId_Algorithm_ScenarioExporterDesc OpenViBE::CIdentifier(0x49A9778E, 0x7BB377F9)

#define OVTK_Algorithm_ScenarioExporter_InputParameterId_Scenario      OpenViBE::CIdentifier(0x5B9C0D54, 0x04BA2957)
#define OVTK_Algorithm_ScenarioExporter_OutputParameterId_MemoryBuffer OpenViBE::CIdentifier(0x64030633, 0x419E3A33)

namespace OpenViBEToolkit
{
	class OVTK_API CAlgorithmScenarioExporter : public OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >
	{
	public:

		virtual void release(void) { delete this; }

		virtual OpenViBE::boolean process(void);

		virtual OpenViBE::boolean exportStart(OpenViBE::IMemoryBuffer& rMemoryBuffer, const OpenViBE::CIdentifier& rIdentifier)=0;
		virtual OpenViBE::boolean exportIdentifier(OpenViBE::IMemoryBuffer& rMemoryBuffer, const OpenViBE::CIdentifier& rIdentifier, const OpenViBE::CIdentifier& rValue)=0;
		virtual OpenViBE::boolean exportString(OpenViBE::IMemoryBuffer& rMemoryBuffer, const OpenViBE::CIdentifier& rIdentifier, const OpenViBE::CString& rValue)=0;
		virtual OpenViBE::boolean exportUInteger(OpenViBE::IMemoryBuffer& rMemoryBuffer, const OpenViBE::CIdentifier& rIdentifier, const OpenViBE::uint64 ui64Value)=0;
		virtual OpenViBE::boolean exportStop(OpenViBE::IMemoryBuffer& rMemoryBuffer)=0;

		_IsDerivedFromClass_Final_(OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >, OVTK_ClassId_Algorithm_ScenarioExporter);
	};

	class OVTK_API CAlgorithmScenarioExporterDesc : public OpenViBE::Plugins::IAlgorithmDesc
	{
	public:

		virtual OpenViBE::boolean getAlgorithmPrototype(
			OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
		{
			rAlgorithmPrototype.addInputParameter (OVTK_Algorithm_ScenarioExporter_InputParameterId_Scenario,      "Scenario",      OpenViBE::Kernel::ParameterType_Object);
			rAlgorithmPrototype.addOutputParameter(OVTK_Algorithm_ScenarioExporter_OutputParameterId_MemoryBuffer, "Memory buffer", OpenViBE::Kernel::ParameterType_MemoryBuffer);
			return true;
		}

		_IsDerivedFromClass_(OpenViBE::Plugins::IAlgorithmDesc, OVTK_ClassId_Algorithm_ScenarioExporterDesc);
	};
};

#endif // __OpenViBEToolkit_Algorithm_ScenarioExporter_H__
