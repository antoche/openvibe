#ifndef __OpenViBEPlugins_Acquisition_CGenericNetworkAcquisition_H__
#define __OpenViBEPlugins_Acquisition_CGenericNetworkAcquisition_H__

#include "ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include <ebml/TReaderCallbackProxy.h>
#include <ebml/TWriterCallbackProxy.h>

#include <socket/IConnectionClient.h>

#include <ebml/IReader.h>
#include <ebml/IReaderHelper.h>
#include <ebml/IWriter.h>
#include <ebml/IWriterHelper.h>

#include <string>
#include <vector>

#define GenericNetworkAcquisition_ExperimentInfoOutput 0
#define GenericNetworkAcquisition_SignalOutput         1
#define GenericNetworkAcquisition_StimulationOutput    2

namespace OpenViBEPlugins
{
	namespace Acquisition
	{

		class CGenericNetworkAcquisition : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			class CExperimentInfoHeader
			{
				public:
					EBML::uint64 m_ui64ExperimentId;
					EBML::uint64 m_ui64ExperimentDate;

					EBML::uint64 m_ui64SubjectId;
					std::string m_pSubjectName;
					EBML::uint64 m_ui64SubjectAge;
					EBML::uint64 m_ui64SubjectSex;

					EBML::uint64 m_ui64LaboratoryId;
					std::string m_pLaboratoryName;
					EBML::uint64 m_ui64TechnicianId;
					std::string m_pTechnicianName;

					bool m_bReadyToSend;
			};

			// Used to store information about the signal stream
			class CSignalDescription
			{
				public:
					CSignalDescription() : m_ui32StreamVersion(1), m_ui32SamplingRate(0), m_ui32ChannelCount(0), m_ui32SampleCount(0), m_ui32CurrentChannel(0), m_bReadyToSend(false)
					{
					}

				public:
					EBML::uint32 m_ui32StreamVersion;
					EBML::uint32 m_ui32SamplingRate;
					EBML::uint32 m_ui32ChannelCount;
					EBML::uint32 m_ui32SampleCount;
					std::vector<std::string> m_pChannelName;
					EBML::uint32 m_ui32CurrentChannel;

					bool m_bReadyToSend;
			};

		public:

			CGenericNetworkAcquisition(void);

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize();

			virtual OpenViBE::boolean uninitialize();

			virtual OpenViBE::uint64 getClockFrequency(void) { return (100LL<<32); }

			virtual OpenViBE::boolean processClock(OpenViBE::CMessageClock& rMessageClock);

			virtual OpenViBE::boolean process();

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithm, OVP_ClassId_GenericNetworkAcquisition)

		public:

			virtual EBML::boolean readerIsMasterChild(const EBML::CIdentifier& rIdentifier);
			virtual void readerOpenChild(const EBML::CIdentifier& rIdentifier);
			virtual void readerProcessChildData(const void* pBuffer, const EBML::uint64 ui64BufferSize);
			virtual void readerCloseChild();

			virtual void writeExperimentOutput(const void* pBuffer, const EBML::uint64 ui64BufferSize);
			virtual void writeSignalOutput(const void* pBuffer, const EBML::uint64 ui64BufferSize);
			virtual void writeStimulationOutput(const void* pBuffer, const EBML::uint64 ui64BufferSize);
		public:

			//Network
			OpenViBE::CString m_sServerHostName;
			OpenViBE::uint32 m_ui32ServerHostPort;
			Socket::IConnectionClient* m_pConnectionClient;

			//Input etc
			EBML::TReaderCallbackProxy1<OpenViBEPlugins::Acquisition::CGenericNetworkAcquisition> m_oInputReaderCallbackProxy;

			EBML::CIdentifier m_oCurrentIdentifier;
			EBML::IReader* m_pReader;
			EBML::IReaderHelper* m_pReaderHelper;
			OpenViBE::uint32 m_ui32CurrentChannel;

			//Output Writers etc
			EBML::IWriter* m_pWriter[3];

			OpenViBEToolkit::IBoxAlgorithmSignalOutputWriter * m_pSignalOutputWriterHelper;
			OpenViBEToolkit::IBoxAlgorithmExperimentInformationOutputWriter * m_pExperimentInformationOutputWriterHelper;
//			OpenViBEToolkit::IBoxAlgorithmStimulationOutputWriter * m_pStimulationOutputWriterHelper;

			EBML::TWriterCallbackProxy1<OpenViBEPlugins::Acquisition::CGenericNetworkAcquisition> * m_pOutputWriterCallbackProxy[3];

			OpenViBE::uint64 m_ui64CurrentBufferSize;

			CExperimentInfoHeader * m_pExperimentInfoHeader;
			bool m_bHeaderSent;

			CSignalDescription * m_pSignalDescription;
			bool m_bSignalDescriptionSent;

			EBML::float64 * m_pMatrixBuffer;
			OpenViBE::uint64 m_ui64MatrixBufferSize;
			bool m_bMatrixReadyToSend;

			OpenViBE::uint32 m_ui32SentSampleCount;

			OpenViBE::uint64 m_ui64CurrentDate;
		};

		class CGenericNetworkAcquisitionDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Generic network acquisition"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard / Bruno Renier"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("A generic network based acquisition algorithm"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("This algorithm waits for EEG data from the network and distributes it into the scenario"); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Acquisition"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("0.2"); }
			virtual void release(void)                                   { }
			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_GenericNetworkAcquisition; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::Acquisition::CGenericNetworkAcquisition(); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-connect"); }
			virtual OpenViBE::boolean getBoxPrototype(OpenViBE::Kernel::IBoxProto& rPrototype) const;

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_GenericNetworkAcquisitionDesc)
		};
	};
};

#endif // __OpenViBEPlugins_Acquisition_CGenericNetworkAcquisition_H__
