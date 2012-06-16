#ifndef __OpenViBEPlugins_BoxAlgorithms_CDecoderAlgorithmTest_H__
#define __OpenViBEPlugins_BoxAlgorithms_CDecoderAlgorithmTest_H__

#include "../ovp_defines.h"

#include <openvibe-toolkit/ovtk_all.h>

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CDecoderAlgorithmTest : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:

			CDecoderAlgorithmTest(void);
			virtual ~CDecoderAlgorithmTest(void);

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uininitialize(void);

			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_StreamDecoderAlgorithmTest)

		protected:

			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamDecoder[7];
			OpenViBE::Kernel::TParameterHandler < const OpenViBE::IMemoryBuffer* > ip_pMemoryBuffer[7];
		};

		class CDecoderAlgorithmTestDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }
			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Decoder algorithm test"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Unit test/Algorithms"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-about"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_StreamDecoderAlgorithmTest; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::StreamCodecs::CDecoderAlgorithmTest(); }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rPrototype) const
			{
				rPrototype.addInput("Experimentation information", OV_TypeId_ExperimentationInformation);
				rPrototype.addInput("Feature vector", OV_TypeId_FeatureVector);
				rPrototype.addInput("Signal", OV_TypeId_Signal);
				rPrototype.addInput("Spectrum", OV_TypeId_Spectrum);
				rPrototype.addInput("Stimulation", OV_TypeId_Stimulations);
				rPrototype.addInput("Streamed matrix", OV_TypeId_StreamedMatrix);
				rPrototype.addInput("Channel localisation", OV_TypeId_ChannelLocalisation);
				rPrototype.addFlag (OpenViBE::Kernel::BoxFlag_IsUnstable);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_StreamDecoderAlgorithmTestDesc)
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithms_CDecoderAlgorithmTest_H__
