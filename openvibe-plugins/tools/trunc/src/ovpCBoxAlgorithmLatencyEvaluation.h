#ifndef __OpenViBEPlugins_BoxAlgorithm_LatencyEvaluation_H__
#define __OpenViBEPlugins_BoxAlgorithm_LatencyEvaluation_H__

#include "ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include <system/Time.h>

namespace OpenViBEPlugins
{
	namespace Tools
	{
		class CBoxAlgorithmLatencyEvaluation : public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_LatencyEvaluation);

			OpenViBE::uint64 m_ui64StartTime;
			OpenViBE::Kernel::ELogLevel m_eLogLevel;
		};

		class CBoxAlgorithmLatencyEvaluationDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Latency evaluation"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Evaluates i/o jittering and outputs values to log manager"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("This box evaluates i/o jittering comparing input chunk' start and end time against current clock time"); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Tools"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-info"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_LatencyEvaluation; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::Tools::CBoxAlgorithmLatencyEvaluation; }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput("input", OV_UndefinedIdentifier);
				rBoxAlgorithmPrototype.addSetting("Log level to use", OV_TypeId_LogLevel, "Trace");
				rBoxAlgorithmPrototype.addFlag (OpenViBE::Kernel::BoxFlag_CanModifyInput);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_LatencyEvaluationDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_LatencyEvaluation_H__
