#ifndef __OpenViBEPlugins_BoxAlgorithm_SignalProcessingBoxAlgorithm_H__
#define __OpenViBEPlugins_BoxAlgorithm_SignalProcessingBoxAlgorithm_H__

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#define OVP_ClassId_BoxAlgorithm_SignalProcessingBoxAlgorithm OpenViBE::CIdentifier(0x330E3A87, 0x31565BA6)
#define OVP_ClassId_BoxAlgorithm_SignalProcessingBoxAlgorithmDesc OpenViBE::CIdentifier(0x376A4712, 0x1AA65567)

namespace OpenViBEPlugins
{
	namespace Samples
	{
		class CBoxAlgorithmSignalProcessingBoxAlgorithm : virtual public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_SignalProcessingBoxAlgorithm);

		protected:

			OpenViBE::boolean m_bActive;

			OpenViBE::Kernel::IAlgorithmProxy* m_pSignalDecoder;
			OpenViBE::Kernel::TParameterHandler < const OpenViBE::IMemoryBuffer* > ip_pMemoryBufferToDecode;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > op_ui64SamplingRate;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > op_pDecodedMatrix;

			OpenViBE::Kernel::IAlgorithmProxy* m_pSignalProcessingAlgorithm;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > ip_pSignalProcessingAlgorithmMatrix;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > op_pSignalProcessingAlgorithmMatrix;

			OpenViBE::Kernel::IAlgorithmProxy* m_pSignalEncoder;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > ip_ui64SamplingRate;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > ip_pMatrixToEncode;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMemoryBuffer* > op_pEncodedMemoryBuffer;
		};

		class CBoxAlgorithmSignalProcessingBoxAlgorithmDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Signal processing box algorithm"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("This is a sample signal processing box algorithm that uses the sample signal processing algorithm in order to demonstrate how to build a signal processing box algorithm"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Samples"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-execute"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_SignalProcessingBoxAlgorithm; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::Samples::CBoxAlgorithmSignalProcessingBoxAlgorithm; }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput  ("Input signal",  OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addOutput ("Output signal", OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addSetting("Active", OV_TypeId_Boolean, "true");
				rBoxAlgorithmPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_IsUnstable);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_SignalProcessingBoxAlgorithmDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_SignalProcessingBoxAlgorithm_H__
