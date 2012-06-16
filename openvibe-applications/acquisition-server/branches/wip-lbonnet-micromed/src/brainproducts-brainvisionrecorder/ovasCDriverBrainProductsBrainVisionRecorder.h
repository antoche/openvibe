#ifndef __OpenViBE_AcquisitionServer_CDriverBrainProductsBrainVisionRecorder_H__
#define __OpenViBE_AcquisitionServer_CDriverBrainProductsBrainVisionRecorder_H__

#include "../ovasIDriver.h"
#include "../ovasCHeader.h"

#include <openvibe/ov_all.h>

#include <vector>

#include <socket/IConnectionClient.h>

#ifndef DEFINE_GUID
	#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
		GUID name = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }
#endif

#ifndef COMPARE_GUID
	#define COMPARE_GUID(name1, name2) \
		(	name1.Data1 == name2.Data1 && name1.Data2 == name2.Data2 && \
			name1.Data3 == name2.Data3 && name1.Data4[0] == name2.Data4[0] && \
			name1.Data4[1] == name2.Data4[1] && name1.Data4[2] == name2.Data4[2] &&\
			name1.Data4[3] == name2.Data4[3] && name1.Data4[4] == name2.Data4[4] &&\
			name1.Data4[5] == name2.Data4[5] && name1.Data4[6] == name2.Data4[6] &&\
			name1.Data4[7] == name2.Data4[7] \
		)
#endif

namespace OpenViBEAcquisitionServer
{
	class CDriverBrainProductsBrainVisionRecorder : public OpenViBEAcquisitionServer::IDriver
	{
	public:

		CDriverBrainProductsBrainVisionRecorder(OpenViBEAcquisitionServer::IDriverContext& rDriverContext);
		virtual ~CDriverBrainProductsBrainVisionRecorder(void);
		virtual const char* getName(void);

		virtual OpenViBE::boolean initialize(
			const OpenViBE::uint32 ui32SampleCountPerSentBlock,
			OpenViBEAcquisitionServer::IDriverCallback& rCallback);
		virtual OpenViBE::boolean uninitialize(void);

		virtual OpenViBE::boolean start(void);
		virtual OpenViBE::boolean stop(void);
		virtual OpenViBE::boolean loop(void);

		virtual OpenViBE::boolean isConfigurable(void);
		virtual OpenViBE::boolean configure(void);

		virtual const OpenViBEAcquisitionServer::IHeader* getHeader(void) { return &m_oHeader; }

	protected:

		OpenViBEAcquisitionServer::IDriverCallback* m_pCallback;
		Socket::IConnectionClient* m_pConnectionClient;
		OpenViBE::CString m_sServerHostName;
		OpenViBE::uint32 m_ui32ServerHostPort;

		OpenViBEAcquisitionServer::CHeader m_oHeader;

		OpenViBE::uint32 m_ui32SampleCountPerSentBlock;
		OpenViBE::float32* m_pSample;

		OpenViBE::uint32 m_ui32IndexIn;
		OpenViBE::uint32 m_ui32IndexOut;
		OpenViBE::uint32 m_ui32BuffDataIndex;

		OpenViBE::uint32 m_ui32MarkerCount;
		OpenViBE::uint32 m_ui32NumberOfMarkers;

		std::vector<OpenViBE::uint32> m_vStimulationIdentifier;
		std::vector<OpenViBE::uint64> m_vStimulationDate;
		std::vector<OpenViBE::uint64> m_vStimulationSample;

		#pragma pack(push)
		#pragma pack(1)

		typedef struct
		{
			OpenViBE::uint32  Data1;
			unsigned short Data2;
			unsigned short Data3;
			unsigned char  Data4[8];
		} GUID;

		struct RDA_Marker
		//; A single marker in the marker array of RDA_MessageData
		{
			OpenViBE::uint32 nSize;              // Size of this marker.
			OpenViBE::uint32 nPosition;          // Relative position in the data block.
			OpenViBE::uint32 nPoints;            // Number of points of this marker
			OpenViBE::int32  nChannel;           // Associated channel number (-1 = all channels).
			char             sTypeDesc[1];       // Type, description in ASCII delimited by '\0'.
		};

		struct RDA_MessageHeader
		//; Message header
		{
			GUID             guid;               // Always GUID_RDAHeader
			OpenViBE::uint32 nSize;              // Size of the message block in bytes including this header
			OpenViBE::uint32 nType;              // Message type.
		};

		// **** Messages sent by the RDA server to the clients. ****
		struct RDA_MessageStart : OpenViBEAcquisitionServer::CDriverBrainProductsBrainVisionRecorder::RDA_MessageHeader
		//; Setup / Start infos, Header -> nType = 1
		{
			OpenViBE::uint32  nChannels;          // Number of channels
			OpenViBE::float64 dSamplingInterval;  // Sampling interval in microseconds
			OpenViBE::float64 dResolutions[1];    // Array of channel resolutions -> double dResolutions[nChannels] coded in microvolts. i.e. RealValue = resolution * A/D value
			char              sChannelNames[1];   // Channel names delimited by '\0'. The real size is larger than 1.
		};

		struct RDA_MessageStop : OpenViBEAcquisitionServer::CDriverBrainProductsBrainVisionRecorder::RDA_MessageHeader
		//; Data acquisition has been stopped. // Header -> nType = 3
		{
		};

		struct RDA_MessageData32 : OpenViBEAcquisitionServer::CDriverBrainProductsBrainVisionRecorder::RDA_MessageHeader
		//; Block of 32-bit floating point data, Header -> nType = 4, sent only from port 51244
		{
			OpenViBE::uint32  nBlock;             // Block number, i.e. acquired blocks since acquisition started.
			OpenViBE::uint32  nPoints;            // Number of data points in this block
			OpenViBE::uint32  nMarkers;           // Number of markers in this data block
			OpenViBE::float32 fData[1];           // Data array -> float fData[nChannels * nPoints], multiplexed
			RDA_Marker        Markers[1];         // Array of markers -> RDA_Marker Markers[nMarkers]
		};
		#pragma pack(pop)

		OpenViBEAcquisitionServer::CDriverBrainProductsBrainVisionRecorder::RDA_MessageHeader* m_pStructRDA_MessageHeader;
		char* m_pcharStructRDA_MessageHeader;

		OpenViBEAcquisitionServer::CDriverBrainProductsBrainVisionRecorder::RDA_MessageStart* m_pStructRDA_MessageStart;
		OpenViBEAcquisitionServer::CDriverBrainProductsBrainVisionRecorder::RDA_MessageStop* m_pStructRDA_MessageStop;
		OpenViBEAcquisitionServer::CDriverBrainProductsBrainVisionRecorder::RDA_MessageData32* m_pStructRDA_MessageData32;
		OpenViBEAcquisitionServer::CDriverBrainProductsBrainVisionRecorder::RDA_Marker* m_pStructRDA_Marker;
	};
};

#endif // __OpenViBE_AcquisitionServer_CDriverBrainProductsBrainVisionRecorder_H__
