#ifndef __OpenViBEPlugins_BoxAlgorithm_GenericStreamReader_H__
#define __OpenViBEPlugins_BoxAlgorithm_GenericStreamReader_H__

#include "../../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include <ebml/CReader.h>
#include <ebml/CReaderHelper.h>

#include <stack>
#include <map>

#include <cstdio>
#include <cstdlib>

// TODO:
// - please move the identifier definitions in ovp_defines.h
// - please include your desciptor in ovp_main.cpp

#define OVP_ClassId_BoxAlgorithm_GenericStreamReader     OpenViBE::CIdentifier(0x6468099F, 0x0370095A)
#define OVP_ClassId_BoxAlgorithm_GenericStreamReaderDesc OpenViBE::CIdentifier(0x1F1E3A53, 0x6CA07237)

namespace OpenViBEPlugins
{
	namespace FileIO
	{
		class CBoxAlgorithmGenericStreamReader : public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, public EBML::IReaderCallback
		{
		public:

			CBoxAlgorithmGenericStreamReader(void);

			virtual void release(void) { delete this; }

			virtual OpenViBE::uint64 getClockFrequency(void);
			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processClock(OpenViBE::CMessageClock& rMessageClock);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_GenericStreamReader);

		protected:

			OpenViBE::CString m_sFilename;

			EBML::CReader m_oReader;
			EBML::CReaderHelper m_oReaderHelper;

			OpenViBE::CMemoryBuffer m_oSwap;
			OpenViBE::CMemoryBuffer m_oPendingChunk;
			OpenViBE::uint64 m_ui64StartTime;
			OpenViBE::uint64 m_ui64EndTime;
			OpenViBE::uint32 m_ui32OutputIndex;
			OpenViBE::boolean m_bPending;
			OpenViBE::boolean m_bUseCompression;

			::FILE* m_pFile;
			std::stack < EBML::CIdentifier > m_vNodes;
			std::map < OpenViBE::uint32, OpenViBE::uint32 > m_vStreamIndexToOutputIndex;
			std::map < OpenViBE::uint32, OpenViBE::CIdentifier > m_vStreamIndexToTypeIdentifier;

		private:

			virtual EBML::boolean isMasterChild(const EBML::CIdentifier& rIdentifier);
			virtual void openChild(const EBML::CIdentifier& rIdentifier);
			virtual void processChildData(const void* pBuffer, const EBML::uint64 ui64BufferSize);
			virtual void closeChild(void);
		};

		class CBoxAlgorithmGenericStreamReaderListener : public OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >
		{
		public:

			OpenViBE::boolean check(OpenViBE::Kernel::IBox& rBox)
			{
				char l_sName[1024];
				OpenViBE::uint32 i;
				for(i=0; i<rBox.getOutputCount(); i++)
				{
					sprintf(l_sName, "Output stream %u", i+1);
					rBox.setOutputName(i, l_sName);
				}
				return true;
			}

			virtual OpenViBE::boolean onOutputAdded(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				rBox.setOutputType(ui32Index, OV_TypeId_EBMLStream);
				this->check(rBox);
				return true;
			}

			virtual OpenViBE::boolean onOutputRemoved(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				this->check(rBox);
				return true;
			}

			virtual OpenViBE::boolean onOutputTypeChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				this->check(rBox);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)
		};

		class CBoxAlgorithmGenericStreamReaderDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Generic stream reader"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("File reading and writing/OpenViBE"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-open"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_GenericStreamReader; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::FileIO::CBoxAlgorithmGenericStreamReader; }
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener(void) const               { return new CBoxAlgorithmGenericStreamReaderListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addOutput ("Output stream 1", OV_TypeId_EBMLStream);
				rBoxAlgorithmPrototype.addSetting("Filename", OV_TypeId_Filename, "");
				rBoxAlgorithmPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_CanAddOutput);
				rBoxAlgorithmPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_CanModifyOutput);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_GenericStreamReaderDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_GenericStreamReader_H__
