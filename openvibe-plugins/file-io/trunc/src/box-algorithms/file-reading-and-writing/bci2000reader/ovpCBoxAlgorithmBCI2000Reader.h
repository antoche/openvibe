#ifndef __OpenViBEPlugins_BoxAlgorithm_BCI2000Reader_H__
#define __OpenViBEPlugins_BoxAlgorithm_BCI2000Reader_H__

#include "ovpCBCI2000ReaderHelper.h"
#include "../../../ovp_defines.h"

#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#define OVP_ClassId_BoxAlgorithm_BCI2000Reader OpenViBE::CIdentifier(0xFF78DAF4, 0xC41544B8)
#define OVP_ClassId_BoxAlgorithm_BCI2000ReaderDesc OpenViBE::CIdentifier(0xFF53D107, 0xC31144B8)

namespace OpenViBEPlugins
{
	namespace FileIO
	{
		/**
		 * \class CBoxAlgorithmBCI2000Reader
		 * \author Olivier Rochel (INRIA)
		 * \date Tue Jun 21 11:11:04 2011
		 * \brief The class CBoxAlgorithmBCI2000Reader describes the box BCI2000 Reader.
		 *
		 */
		class CBoxAlgorithmBCI2000Reader : virtual public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:
			virtual void release(void)
			{
				delete this;
			}

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);

			virtual OpenViBE::boolean processClock(OpenViBE::CMessageClock& rMessageClock);
			virtual OpenViBE::uint64 getClockFrequency(void);

			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_BCI2000Reader);

		protected:

			OpenViBE::boolean m_bHeaderSent;

			OpenViBEToolkit::TSignalEncoder < CBoxAlgorithmBCI2000Reader > m_oSignalEncoder;
			OpenViBEToolkit::TSignalEncoder < CBoxAlgorithmBCI2000Reader > m_oStateEncoder;

			// These 2 were from the time the matrices were built, not given by the encoders.
			// They could be removed, but for now make the code a bit easier to read - that's
			// why they're still there.
			OpenViBE::IMatrix *m_pSignalOutputMatrix;
			OpenViBE::IMatrix *m_pStateOutputMatrix;

			OpenViBE::uint32 m_ui32Rate;
			OpenViBE::uint32 m_ui32ChannelCount;
			OpenViBE::uint32 m_ui32SampleCountPerBuffer;
			OpenViBE::float64 *m_pBuffer; 	// temporary buffer as we'll have to transpose data for signal_out
			OpenViBE::uint32 *m_pStates; 	// state variables, to be converted too;
			OpenViBE::uint64 m_ui32SamplesSent;
			BCI2000::CBCI2000ReaderHelper *m_pB2KReaderHelper;
			// helpers
			void sendHeader(void);

		};

		/**
		 * \class CBoxAlgorithmBCI2000ReaderDesc
		 * \author Olivier Rochel (INRIA)
		 * \date Tue Jun 21 11:11:04 2011
		 * \brief Descriptor of the box BCI2000 Reader.
		 *
		 */
		class CBoxAlgorithmBCI2000ReaderDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("BCI2000 File Reader"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Olivier Rochel"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Reads BCI2000 .dat files."); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("The box reads EEG/States signals from a BCI2000 file (.dat)"); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("File reading and writing/BCI2000"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.3"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-open");}

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_BCI2000Reader; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::FileIO::CBoxAlgorithmBCI2000Reader; }

			virtual OpenViBE::boolean getBoxPrototype(OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addOutput("Signal",OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addOutput("State",OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addSetting("File name",OV_TypeId_Filename,"");
				rBoxAlgorithmPrototype.addSetting("Samples per buffer",OV_TypeId_Integer,"16");
				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_IsUnstable); // meuh non !

				return true;
			}
			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_BCI2000ReaderDesc);
		};
	};
};

#endif

