#ifndef __OpenViBEPlugins_BoxAlgorithms_CEncoderAlgorithmTest_H__
#define __OpenViBEPlugins_BoxAlgorithms_CEncoderAlgorithmTest_H__

#include "../ovp_defines.h"

#include <openvibe-toolkit/ovtk_all.h>

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CEncoderAlgorithmTest : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::uint64 getClockFrequency(void) { return 1LL<<32; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uininitialize(void);

			virtual OpenViBE::boolean processClock(OpenViBE::Kernel::IMessageClock& rMessageClock);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_StreamEncoderAlgorithmTest)

		protected:

			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamEncoder[7];
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMemoryBuffer* > op_pMemoryBuffer[7];

			OpenViBE::boolean m_bHasSentHeader;
			OpenViBE::uint64 m_ui64StartTime;
			OpenViBE::uint64 m_ui64EndTime;

			OpenViBE::CMatrix* m_pMatrix1;
			OpenViBE::CMatrix* m_pMatrix2;
			OpenViBE::CMatrix* m_pMatrix3;
			OpenViBE::CStimulationSet* m_pStimulationSet;
		};

		class CEncoderAlgorithmTestDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }
			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Encoder algorithm test"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Unit test/Algorithms"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-about"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_StreamEncoderAlgorithmTest; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::StreamCodecs::CEncoderAlgorithmTest(); }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rPrototype) const
			{
				rPrototype.addOutput("Experimentation information", OV_TypeId_ExperimentationInformation);
				rPrototype.addOutput("Feature vector", OV_TypeId_FeatureVector);
				rPrototype.addOutput("Signal", OV_TypeId_Signal);
				rPrototype.addOutput("Spectrum", OV_TypeId_Spectrum);
				rPrototype.addOutput("Stimulation", OV_TypeId_Stimulations);
				rPrototype.addOutput("Streamed matrix", OV_TypeId_StreamedMatrix);
				rPrototype.addOutput("Channel localisation", OV_TypeId_ChannelLocalisation);
				rPrototype.addFlag (OpenViBE::Kernel::BoxFlag_IsUnstable);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_StreamEncoderAlgorithmTestDesc)
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithms_CEncoderAlgorithmTest_H__
