#ifndef __OpenViBEPlugins_BoxAlgorithm_CSVFileWriter_H__
#define __OpenViBEPlugins_BoxAlgorithm_CSVFileWriter_H__

#include "../../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include <cstdlib>
#include <cstdio>

// TODO:
// - please move the identifier definitions in ovp_defines.h
// - please include your desciptor in ovp_main.cpp

//#define OVP_ClassId_BoxAlgorithm_CSVFileWriter     OpenViBE::CIdentifier(0x2C9312F1, 0x2D6613E5)
//#define OVP_ClassId_BoxAlgorithm_CSVFileWriterDesc OpenViBE::CIdentifier(0x65075FF7, 0x2B555E97)


namespace OpenViBEPlugins
{
	namespace FileIO
	{
		class CBoxAlgorithmCSVFileWriter : public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			CBoxAlgorithmCSVFileWriter(void);
			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			OpenViBE::boolean process_streamedMatrix(void);
			OpenViBE::boolean process_stimulation(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_CSVFileWriter);

		protected:

			::FILE* m_pFile;
			OpenViBE::CString m_sSeparator;
			OpenViBE::CIdentifier m_oTypeIdentifier;
			OpenViBE::boolean m_bUseCompression;
			OpenViBE::boolean m_bFirstBuffer;
			OpenViBE::boolean (OpenViBEPlugins::FileIO::CBoxAlgorithmCSVFileWriter::*m_fpRealProcess)(void);

			OpenViBE::IMatrix* m_pMatrix;
			OpenViBE::boolean m_bDeleteMatrix;

			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamDecoder;
			OpenViBE::Kernel::TParameterHandler < const OpenViBE::IMemoryBuffer* > ip_pMemoryBuffer;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > op_pMatrix;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > op_pMinMaxFrequencyBand;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > op_ui64SamplingFrequency;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IStimulationSet* > op_pStimulationSet;
		};

		class CBoxAlgorithmCSVFileWriterListener : public OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >
		{
		public:

			virtual OpenViBE::boolean onInputTypeChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				OpenViBE::CIdentifier l_oTypeIdentifier;
				rBox.getInputType(ui32Index, l_oTypeIdentifier);
				if(this->getTypeManager().isDerivedFromStream(l_oTypeIdentifier, OV_TypeId_StreamedMatrix))
				{
					rBox.setInputName(ui32Index, "Streamed matrix");
				}
				else if(l_oTypeIdentifier==OV_TypeId_Stimulations)
				{
					rBox.setInputName(ui32Index, "Stimulations");
				}
				else
				{
					rBox.setInputName(ui32Index, "Streamed matrix");
					rBox.setInputType(ui32Index, OV_TypeId_Signal);
					return false;
				}
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier);
		};

		class CBoxAlgorithmCSVFileWriterDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("CSV File Writer"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Writes signal in a CSV (text based) file"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("File reading and writing/CSV"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-open"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_CSVFileWriter; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::FileIO::CBoxAlgorithmCSVFileWriter; }
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener(void) const               { return new CBoxAlgorithmCSVFileWriterListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput  ("Streamed matrix",  OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addSetting("Filename",         OV_TypeId_Filename, "record-[$core{date}-$core{time}].csv");
				rBoxAlgorithmPrototype.addSetting("Column separator", OV_TypeId_String, ";");
				rBoxAlgorithmPrototype.addSetting("Use compression",  OV_TypeId_Boolean, "true");
				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyInput);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_CSVFileWriterDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_CSVFileWriter_H__
