#ifndef __OpenViBEPlugins_Algorithm_BrainampFileReader_H__
#define __OpenViBEPlugins_Algorithm_BrainampFileReader_H__

#include "../../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include <fstream>

namespace OpenViBEPlugins
{
	namespace FileIO
	{
		class CAlgorithmBrainampFileReader : public OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >, OVP_ClassId_Algorithm_BrainampFileReader);

		protected:

			enum EStatus
			{
				Status_Nothing,
				Status_CommonInfos,
				Status_BinrayInfos,
				Status_ChannelInfos,
				Status_MarkerInfos,
				Status_Comment,
			};

			enum EBinaryFormat
			{
				BinaryFormat_Integer16,
				BinaryFormat_UnsignedInteger16,
				BinaryFormat_Float32,
			};

			enum EEndianness
			{
				Endianness_LittleEndian,
				Endianness_BigEndian,
			};

			typedef struct
			{
				OpenViBE::uint64 m_ui64Identifier;
				OpenViBE::uint64 m_ui64StartIndex;
				OpenViBE::uint64 m_ui64Duration;
				std::string m_sName;
			} SStimulation;

			OpenViBE::Kernel::TParameterHandler < OpenViBE::CString* > ip_sFilename;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::float64 > ip_f64EpochDuration;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > ip_ui64SeekTime;

			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > op_ui64CurrentStartTime;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > op_ui64CurrentEndTime;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > op_ui64SamplingRate;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > op_pSignalMatrix;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IStimulationSet* > op_pStimulations;

			OpenViBE::CString m_sFilename;

			OpenViBE::uint32 m_ui32BinaryFormat;
			OpenViBE::uint32 m_ui32Endianness;
			OpenViBE::uint32 m_ui32ChannelCount;
			OpenViBE::uint64 m_ui64SamplingInterval;
			OpenViBE::uint64 m_ui64StartSampleIndex;
			OpenViBE::uint64 m_ui64EndSampleIndex;
			OpenViBE::uint64 m_ui64SampleCountPerBuffer;

			OpenViBE::uint8* m_pBuffer;
			std::vector < OpenViBE::float64 > m_vChannelScale;
			std::vector < OpenViBEPlugins::FileIO::CAlgorithmBrainampFileReader::SStimulation > m_vStimulation;

			std::ifstream m_oHeaderFile;
			std::ifstream m_oDataFile;
			std::ifstream m_oMarkerFile;
		};

		class CAlgorithmBrainampFileReaderDesc : public OpenViBE::Plugins::IAlgorithmDesc
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

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_Algorithm_BrainampFileReader; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::FileIO::CAlgorithmBrainampFileReader; }

			virtual OpenViBE::boolean getAlgorithmPrototype(
				OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				rAlgorithmPrototype.addInputParameter (OVP_Algorithm_BrainampFileReader_InputParameterId_Filename,          "Filename",           OpenViBE::Kernel::ParameterType_String);
				rAlgorithmPrototype.addInputParameter (OVP_Algorithm_BrainampFileReader_InputParameterId_EpochDuration,     "Epoch duration",     OpenViBE::Kernel::ParameterType_Float);
				rAlgorithmPrototype.addInputParameter (OVP_Algorithm_BrainampFileReader_InputParameterId_SeekTime,          "Seek time",          OpenViBE::Kernel::ParameterType_Integer);
				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_BrainampFileReader_OutputParameterId_CurrentStartTime, "Current start time", OpenViBE::Kernel::ParameterType_Integer);
				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_BrainampFileReader_OutputParameterId_CurrentEndTime,   "Current end time",   OpenViBE::Kernel::ParameterType_Integer);
				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_BrainampFileReader_OutputParameterId_SamplingRate,     "Sampling rate",      OpenViBE::Kernel::ParameterType_Integer);
				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_BrainampFileReader_OutputParameterId_SignalMatrix,     "Signal samples",     OpenViBE::Kernel::ParameterType_Matrix);
				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_BrainampFileReader_OutputParameterId_Stimulations,     "Stimulations",       OpenViBE::Kernel::ParameterType_StimulationSet);
				rAlgorithmPrototype.addInputTrigger   (OVP_Algorithm_BrainampFileReader_InputTriggerId_Open,                "Open");
				rAlgorithmPrototype.addInputTrigger   (OVP_Algorithm_BrainampFileReader_InputTriggerId_Seek,                "Seek");
				rAlgorithmPrototype.addInputTrigger   (OVP_Algorithm_BrainampFileReader_InputTriggerId_Next,                "Next");
				rAlgorithmPrototype.addInputTrigger   (OVP_Algorithm_BrainampFileReader_InputTriggerId_Close,               "Close");
				rAlgorithmPrototype.addOutputTrigger  (OVP_Algorithm_BrainampFileReader_OutputTriggerId_Error,              "Error");
				rAlgorithmPrototype.addOutputTrigger  (OVP_Algorithm_BrainampFileReader_OutputTriggerId_DataProduced,       "Data produced");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IAlgorithmDesc, OVP_ClassId_Algorithm_BrainampFileReaderDesc);
		};
	};
};

#endif // __OpenViBEPlugins_Algorithm_BrainampFileReader_H__
