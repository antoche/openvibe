#ifndef __OpenViBEPlugins_BoxAlgorithm_ElectrodeLocalisationFileReader_H__
#define __OpenViBEPlugins_BoxAlgorithm_ElectrodeLocalisationFileReader_H__

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

namespace OpenViBEPlugins
{
	namespace FileIO
	{
		class CBoxAlgorithmElectrodeLocalisationFileReader : public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::uint64 getClockFrequency(void);
			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processClock(OpenViBE::CMessageClock& rMessageClock);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_ElectrodeLocalisationFileReader);

		protected:

			OpenViBE::Kernel::IAlgorithmProxy* m_pOVMatrixFileReader;
			OpenViBE::Kernel::IAlgorithmProxy* m_pChannelLocalisationStreamEncoder;

			OpenViBE::CString m_sFilename;
			OpenViBE::boolean m_bHeaderSent;
			OpenViBE::boolean m_bBufferSent;
		};

		class CBoxAlgorithmElectrodeLocalisationFileReaderDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Electrode localisation file reader"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Vincent Delannoy"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("File reading and writing/OpenViBE"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-open"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_ElectrodeLocalisationFileReader; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::FileIO::CBoxAlgorithmElectrodeLocalisationFileReader; }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				// Adds box outputs
				rBoxAlgorithmPrototype.addOutput("Channel localisation", OV_TypeId_ChannelLocalisation);

				// Adds settings
				rBoxAlgorithmPrototype.addSetting("Filename", OV_TypeId_Filename, "");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_ElectrodeLocalisationFileReaderDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_ElectrodeLocalisationFileReader_H__
