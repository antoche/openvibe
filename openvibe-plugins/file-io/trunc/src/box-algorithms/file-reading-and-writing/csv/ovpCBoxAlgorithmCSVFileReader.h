#ifndef __OpenViBEPlugins_BoxAlgorithm_CSVFileReader_H__
#define __OpenViBEPlugins_BoxAlgorithm_CSVFileReader_H__

#include "../../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <ebml/TWriterCallbackProxy.h>

namespace OpenViBEPlugins
{
	namespace FileIO
	{
		class CBoxAlgorithmCSVFileReader : public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			CBoxAlgorithmCSVFileReader(void);

			virtual void release(void) { delete this; }

			virtual OpenViBE::uint64 getClockFrequency(void);
			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processClock(OpenViBE::CMessageClock& rMessageClock);
			virtual OpenViBE::boolean process(void);

			OpenViBE::boolean process_streamedMatrix(void);
			OpenViBE::boolean process_stimulation(void);
			OpenViBE::boolean process_signal(void);
			OpenViBE::boolean process_channelLocalisation(void);
			OpenViBE::boolean process_featureVector(void);
			OpenViBE::boolean process_spectrum(void);
			void convertVectorDataToMatrix(OpenViBE::IMatrix* matrix);
			//void split(OpenViBE::CString line, OpenViBE::CString delim, std::vector < std::string *>* split);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_CSVFileReader);

		protected:

			::FILE* m_pFile;
			std::string m_sSeparator;
			OpenViBE::boolean m_bNotUseTimer;
			OpenViBE::CString m_sFilename;
			OpenViBE::boolean m_bUseCompression;

			OpenViBE::CIdentifier m_oTypeIdentifier;
			OpenViBE::uint32 m_ui32NbColumn;
			OpenViBE::uint64 m_ui64SamplingRate;
			OpenViBE::uint32 m_ui32SamplesPerBuffer;
			OpenViBE::uint32 m_ui32ChannelNumberPerBuffer;

			OpenViBE::boolean (OpenViBEPlugins::FileIO::CBoxAlgorithmCSVFileReader::*m_fpRealProcess)(void);

			OpenViBE::Kernel::IAlgorithmProxy* m_pAlgorithmEncoder;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMemoryBuffer* > op_pMemoryBuffer;

			OpenViBE::Kernel::TParameterHandler <OpenViBE::IMatrix*> ip_pMatrix;
			OpenViBE::Kernel::TParameterHandler <OpenViBE::boolean> ip_pDynamic; //channelLocalisation stream
			OpenViBE::Kernel::TParameterHandler <OpenViBE::IMatrix*> ip_pMinMaxFrequencyBands; //spectrum stream
			OpenViBE::Kernel::TParameterHandler <OpenViBE::uint64> ip_pSamplingRate; //signals stream
			OpenViBE::Kernel::TParameterHandler <OpenViBE::IStimulationSet*> ip_pStimulationSet; // stimulation stream

			OpenViBE::boolean m_bHeaderSent;
			std::vector<std::string> m_vLastLineSplit;
			std::vector<std::string> m_vHeaderFile;
			std::vector<std::vector<std::string> > m_vDataMatrix;

			OpenViBE::float64 m_f64NextTime;
		};

		class CBoxAlgorithmCSVFileReaderListener : public OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >
		{
		public:

			virtual OpenViBE::boolean onOutputTypeChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				OpenViBE::CIdentifier l_oTypeIdentifier;
				rBox.getOutputType(ui32Index, l_oTypeIdentifier);
				if(this->getTypeManager().isDerivedFromStream(l_oTypeIdentifier, OV_TypeId_Spectrum))
				{
					rBox.setOutputName(ui32Index, "Streamed matrix");
					rBox.setSettingName(3,"");
					rBox.setSettingValue(3,"0");
				}
				else if(this->getTypeManager().isDerivedFromStream(l_oTypeIdentifier,OV_TypeId_ChannelLocalisation))
				{
					rBox.setOutputName(ui32Index, "Streamed matrix");
					rBox.setSettingName(3,"Channels number");
					rBox.setSettingValue(3,"32");
				}
				else if(this->getTypeManager().isDerivedFromStream(l_oTypeIdentifier, OV_TypeId_StreamedMatrix))
				{
					rBox.setOutputName(ui32Index, "Streamed matrix");
					rBox.setSettingName(3,"Samples per buffer");
					rBox.setSettingValue(3,"32");
				}
				else if(l_oTypeIdentifier==OV_TypeId_Stimulations)
				{
					rBox.setOutputName(ui32Index, "Stimulations");
					rBox.setSettingName(3,"");
					rBox.setSettingValue(3,"0");
				}
				else
				{
					rBox.setOutputName(ui32Index, "Streamed matrix");
					rBox.setOutputType(ui32Index, OV_TypeId_Signal);
					rBox.setSettingName(3,"Samples per buffer");
					rBox.setSettingValue(3,"32");
					return false;
				}
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)
		};

		class CBoxAlgorithmCSVFileReaderDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("CSV File Reader"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Baptiste Payan"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Read signal in a CSV (text based) file"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("File reading and writing/CSV"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-open"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_CSVFileReader; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::FileIO::CBoxAlgorithmCSVFileReader; }
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener(void) const               { return new CBoxAlgorithmCSVFileReaderListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addOutput ("Streamed matrix", OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addSetting("Filename", OV_TypeId_Filename, "");
				rBoxAlgorithmPrototype.addSetting("Column separator", OV_TypeId_String, ";");
				rBoxAlgorithmPrototype.addSetting("don't use the file time",OV_TypeId_Boolean, "false");
				rBoxAlgorithmPrototype.addSetting("Samples per buffer", OV_TypeId_Integer,"32");

				rBoxAlgorithmPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_CanModifyOutput);

				rBoxAlgorithmPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_IsUnstable);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_CSVFileReaderDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_CSVFileReader_H__
