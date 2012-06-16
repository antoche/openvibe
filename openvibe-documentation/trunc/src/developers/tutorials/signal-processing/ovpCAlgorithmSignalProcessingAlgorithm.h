#ifndef __OpenViBEPlugins_Algorithm_SignalProcessingAlgorithm_H__
#define __OpenViBEPlugins_Algorithm_SignalProcessingAlgorithm_H__

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#define OVP_ClassId_Algorithm_SignalProcessingAlgorithm     OpenViBE::CIdentifier(0x3B7523EC, 0x16C30A39)
#define OVP_ClassId_Algorithm_SignalProcessingAlgorithmDesc OpenViBE::CIdentifier(0x11BE2168, 0x5B494BBB)

#define OVP_Algorithm_SignalProcessingAlgorithm_InputParameterId_Matrix  OpenViBE::CIdentifier(0x56154223, 0x42180588)
#define OVP_Algorithm_SignalProcessingAlgorithm_OutputParameterId_Matrix OpenViBE::CIdentifier(0x023A4450, 0x6DFD17DB)

#define OVP_Algorithm_SignalProcessingAlgorithm_InputTriggerId_Initialize  OpenViBE::CIdentifier(0x01803B07, 0x667A69BC)
#define OVP_Algorithm_SignalProcessingAlgorithm_InputTriggerId_Process     OpenViBE::CIdentifier(0x661A59C0, 0x12FB7F74)
#define OVP_Algorithm_SignalProcessingAlgorithm_InputTriggerId_ProcessDone OpenViBE::CIdentifier(0x33802521, 0x785D51FD)


namespace OpenViBEPlugins
{
	namespace Samples
	{
		class CAlgorithmSignalProcessingAlgorithm : public OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >, OVP_ClassId_Algorithm_SignalProcessingAlgorithm);

		protected:

			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > ip_pMatrix;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > op_pMatrix;
		};

		class CAlgorithmSignalProcessingAlgorithmDesc : public OpenViBE::Plugins::IAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Signal processing algorithm"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("This is a sample signal processing algorithm that does nothing but can be used as a quick start for creating new signal processing algorithms"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Samples"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-execute"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_Algorithm_SignalProcessingAlgorithm; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::Samples::CAlgorithmSignalProcessingAlgorithm; }

			virtual OpenViBE::boolean getAlgorithmPrototype(
				OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				rAlgorithmPrototype.addInputParameter (OVP_Algorithm_SignalProcessingAlgorithm_InputParameterId_Matrix,     "Matrix", OpenViBE::Kernel::ParameterType_Matrix);
				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_SignalProcessingAlgorithm_OutputParameterId_Matrix,    "Matrix", OpenViBE::Kernel::ParameterType_Matrix);

				rAlgorithmPrototype.addInputTrigger   (OVP_Algorithm_SignalProcessingAlgorithm_InputTriggerId_Initialize,   "Initialize");
				rAlgorithmPrototype.addInputTrigger   (OVP_Algorithm_SignalProcessingAlgorithm_InputTriggerId_Process,      "Process");
				rAlgorithmPrototype.addOutputTrigger  (OVP_Algorithm_SignalProcessingAlgorithm_OutputTriggerId_ProcessDone, "Process done");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IAlgorithmDesc, OVP_ClassId_Algorithm_SignalProcessingAlgorithmDesc);
		};
	};
};

#endif // __OpenViBEPlugins_Algorithm_SignalProcessingAlgorithm_H__
