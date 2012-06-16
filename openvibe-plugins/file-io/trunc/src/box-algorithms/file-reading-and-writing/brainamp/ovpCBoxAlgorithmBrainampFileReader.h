#ifndef __OpenViBEPlugins_BoxAlgorithm_BrainampFileReader_H__
#define __OpenViBEPlugins_BoxAlgorithm_BrainampFileReader_H__

#include "../../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

namespace OpenViBEPlugins
{
	namespace FileIO
	{
		class CBoxAlgorithmBrainampFileReader : public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::uint64 getClockFrequency(void);
			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processClock(OpenViBE::CMessageClock& rMessageClock);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_BrainampFileReader);

		protected:

			OpenViBE::Kernel::IAlgorithmProxy* m_pBrainampFileReader;
			OpenViBE::Kernel::IAlgorithmProxy* m_pExperimentInformationStreamEncoder;
			OpenViBE::Kernel::IAlgorithmProxy* m_pSignalStreamEncoder;
			OpenViBE::Kernel::IAlgorithmProxy* m_pStimulationStreamEncoder;

			OpenViBE::boolean m_bHeaderSent;
		};

		class CBoxAlgorithmBrainampFileReaderDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Brainamp file reader"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("File reading and writing/Brainamp"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-open"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_BrainampFileReader; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::FileIO::CBoxAlgorithmBrainampFileReader; }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				// Adds box outputs
				rBoxAlgorithmPrototype.addOutput("Experiment information", OV_TypeId_ExperimentationInformation);
				rBoxAlgorithmPrototype.addOutput("EEG stream", OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addOutput("Stimulations", OV_TypeId_Stimulations);

				// Adds settings
				rBoxAlgorithmPrototype.addSetting("Filename (header)", OV_TypeId_Filename, "");
				rBoxAlgorithmPrototype.addSetting("Epoch size (in sec)", OV_TypeId_Float, "0.0625");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_BrainampFileReaderDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_BrainampFileReader_H__
