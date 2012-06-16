#ifndef __OpenViBEPlugins_BoxAlgorithm_GenericStreamWriter_H__
#define __OpenViBEPlugins_BoxAlgorithm_GenericStreamWriter_H__

#include "../../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include <ebml/CWriter.h>
#include <ebml/CWriterHelper.h>
#include <ebml/CReader.h>

#include <cstdio>

#include <fstream>

// TODO:
// - please move the identifier definitions in ovp_defines.h
// - please include your desciptor in ovp_main.cpp

#define OVP_ClassId_BoxAlgorithm_GenericStreamWriter     OpenViBE::CIdentifier(0x09C92218, 0x7C1216F8)
#define OVP_ClassId_BoxAlgorithm_GenericStreamWriterDesc OpenViBE::CIdentifier(0x50AB506A, 0x54804437)

namespace OpenViBEPlugins
{
	namespace FileIO
	{
		class CBoxAlgorithmGenericStreamWriter : public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, public EBML::IWriterCallback
		{
		public:

			CBoxAlgorithmGenericStreamWriter(void);
			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_GenericStreamWriter);

		protected:

			OpenViBE::boolean m_bUseCompression;
			OpenViBE::CString m_sFilename;
			EBML::CWriter m_oWriter;
			EBML::CWriterHelper m_oWriterHelper;

		private:

			virtual void write(const void* pBuffer, const EBML::uint64 ui64BufferSize);

		private:

			OpenViBE::CMemoryBuffer m_oSwap;
			std::ofstream m_oFile;
		};

		class CBoxAlgorithmGenericStreamWriterListener : public OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >
		{
		public:

			OpenViBE::boolean check(OpenViBE::Kernel::IBox& rBox)
			{
				char l_sName[1024];
				OpenViBE::uint32 i;
				for(i=0; i<rBox.getInputCount(); i++)
				{
					sprintf(l_sName, "Input stream %u", i+1);
					rBox.setInputName(i, l_sName);
				}
				return true;
			}

			virtual OpenViBE::boolean onInputAdded(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				rBox.setInputType(ui32Index, OV_TypeId_EBMLStream);
				this->check(rBox);
				return true;
			}

			virtual OpenViBE::boolean onInputRemoved(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				this->check(rBox);
				return true;
			}

			virtual OpenViBE::boolean onInputTypeChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				this->check(rBox);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)
		};

		class CBoxAlgorithmGenericStreamWriterDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Generic stream writer"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Writes any number of stream into a file"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("File reading and writing/OpenViBE"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-save"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_GenericStreamWriter; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::FileIO::CBoxAlgorithmGenericStreamWriter; }
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener(void) const               { return new CBoxAlgorithmGenericStreamWriterListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput  ("Input stream 1",  OV_TypeId_EBMLStream);
				rBoxAlgorithmPrototype.addSetting("Filename",        OV_TypeId_Filename, "record-[$core{date}-$core{time}].ov");
				rBoxAlgorithmPrototype.addSetting("Use compression", OV_TypeId_Boolean, "true");
				rBoxAlgorithmPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_CanAddInput);
				rBoxAlgorithmPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_CanModifyInput);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_GenericStreamWriterDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_GenericStreamWriter_H__
