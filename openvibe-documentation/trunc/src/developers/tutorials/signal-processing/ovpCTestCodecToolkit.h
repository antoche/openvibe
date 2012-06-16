#ifndef __OpenViBEPlugins_TestCodecToolkit_H__
#define __OpenViBEPlugins_TestCodecToolkit_H__

#include "ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include <vector>

#define OVP_ClassId_TestCodecToolkit     OpenViBE::CIdentifier(0x330E3A87, 0x31565BA6)
#define OVP_ClassId_TestCodecToolkitDesc OpenViBE::CIdentifier(0x376A4712, 0x1AA65567)

namespace OpenViBEPlugins
{
	namespace Samples
	{
		class CTestCodecToolkit : public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_TestCodecToolkit);

		protected:

			OpenViBEToolkit::TStreamedMatrixDecoder < CTestCodecToolkit > m_oStreamedMatrixDecoder;
			OpenViBEToolkit::TStreamedMatrixEncoder < CTestCodecToolkit > m_oStreamedMatrixEncoder;

			OpenViBEToolkit::TChannelLocalisationDecoder < CTestCodecToolkit > m_oChannelLocalisationDecoder;
			OpenViBEToolkit::TChannelLocalisationEncoder < CTestCodecToolkit > m_oChannelLocalisationEncoder;

			OpenViBEToolkit::TFeatureVectorDecoder < CTestCodecToolkit > m_oFeatureVectorDecoder;
			OpenViBEToolkit::TFeatureVectorEncoder < CTestCodecToolkit > m_oFeatureVectorEncoder;

			OpenViBEToolkit::TSpectrumDecoder < CTestCodecToolkit > m_oSpectrumDecoder;
			OpenViBEToolkit::TSpectrumEncoder < CTestCodecToolkit > m_oSpectrumEncoder;

			OpenViBEToolkit::TSignalDecoder < CTestCodecToolkit > m_oSignalDecoder;
			OpenViBEToolkit::TSignalEncoder < CTestCodecToolkit > m_oSignalEncoder;

			OpenViBEToolkit::TStimulationDecoder < CTestCodecToolkit > m_oStimDecoder;
			OpenViBEToolkit::TStimulationEncoder < CTestCodecToolkit > m_oStimEncoder;

			OpenViBEToolkit::TExperimentInformationDecoder < CTestCodecToolkit > m_oExperimentInformationDecoder;
			OpenViBEToolkit::TExperimentInformationEncoder < CTestCodecToolkit > m_oExperimentInformationEncoder;

			/* One decoder per input. This vector makes easy the decoding in one iteration over the inputs. */
			std::vector < OpenViBEToolkit::TDecoder < CTestCodecToolkit >*> m_vDecoders;

			/* One encoder per output This vector makes easy the encoding in one iteration over the outputs. */
			std::vector < OpenViBEToolkit::TEncoder < CTestCodecToolkit >*> m_vEncoders;
		};

		class CTestCodecToolkitDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Codec Toolkit testbox"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Laurent Bonnet"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Sample box to test the codec toolkit. Identity (input = output)."); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Samples"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-execute"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_TestCodecToolkit; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::Samples::CTestCodecToolkit; }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput  ("Streamed Matrix",  OV_TypeId_StreamedMatrix);
				rBoxAlgorithmPrototype.addOutput ("Streamed Matrix", OV_TypeId_StreamedMatrix);

				rBoxAlgorithmPrototype.addInput  ("Channel Localisation",  OV_TypeId_ChannelLocalisation);
				rBoxAlgorithmPrototype.addOutput ("Channel Localisation", OV_TypeId_ChannelLocalisation);

				rBoxAlgorithmPrototype.addInput  ("Feature Vector",  OV_TypeId_FeatureVector);
				rBoxAlgorithmPrototype.addOutput ("Feature Vector", OV_TypeId_FeatureVector);

				rBoxAlgorithmPrototype.addInput  ("Spectrum",  OV_TypeId_Spectrum);
				rBoxAlgorithmPrototype.addOutput ("Spectrum", OV_TypeId_Spectrum);

				rBoxAlgorithmPrototype.addInput  ("Signal",  OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addOutput ("Signal", OV_TypeId_Signal);

				rBoxAlgorithmPrototype.addInput  ("Stimulations",  OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addOutput ("Stimulations", OV_TypeId_Stimulations);

				rBoxAlgorithmPrototype.addInput  ("XP info",  OV_TypeId_ExperimentationInformation);
				rBoxAlgorithmPrototype.addOutput ("XP info", OV_TypeId_ExperimentationInformation);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_TestCodecToolkitDesc);
		};
	};
};

#endif // __OpenViBEPlugins_TestCodecToolkit_H__
