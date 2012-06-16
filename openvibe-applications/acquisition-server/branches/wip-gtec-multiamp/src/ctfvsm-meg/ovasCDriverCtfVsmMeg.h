#ifndef __OpenViBE_AcquisitionServer_CDriverCtfVsmMeg_H__
#define __OpenViBE_AcquisitionServer_CDriverCtfVsmMeg_H__

#include "../ovasIDriver.h"
#include "../ovasCHeader.h"

#include <openvibe/ov_all.h>

#include <socket/IConnectionClient.h>

#define NB_CHAN_RECORDED_MAX 410
#define NB_SAMP_ACQ_Packet   28160

namespace OpenViBEAcquisitionServer
{
	class CDriverCtfVsmMeg : public OpenViBEAcquisitionServer::IDriver
	{
	public:

		typedef char str32[32];
		typedef char str100[100];

		CDriverCtfVsmMeg(OpenViBEAcquisitionServer::IDriverContext& rDriverContext);
		virtual ~CDriverCtfVsmMeg(void);
		virtual const char* getName(void);

		virtual OpenViBE::boolean isFlagSet(
			const OpenViBEAcquisitionServer::EDriverFlag eFlag) const
		{
			return eFlag==DriverFlag_IsUnstable;
		}

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

		Socket::IConnectionClient* m_pConnectionClient;
		OpenViBE::CString m_sServerHostName;
		OpenViBE::uint32 m_ui32ServerHostPort;

		struct
		{
			int nbCharExperimentId;
			str100 experimentId;
			int nbCharExperimentDate;
			str32 experimentDate;

			int nbCharSubjectName;
			str32 subjectName;
			int subjectAge;
			char subjectGender[1]; /**F: female or M: Male*/

			int labId;
			str32 labName;
			int technicianId;
			str32 technicianName;

			float samplingRate;
			int numberOfChannels;
			str32 channelLabel[NB_CHAN_RECORDED_MAX];
			int channelTypeIndex[NB_CHAN_RECORDED_MAX];

			float properGain[NB_CHAN_RECORDED_MAX];
			float qGain[NB_CHAN_RECORDED_MAX];
			float ioGain[NB_CHAN_RECORDED_MAX];

			int numberOfCoils[NB_CHAN_RECORDED_MAX];
			int gradOrderNum[NB_CHAN_RECORDED_MAX];
		} m_structHeader;

		char* m_pStructHeader;

		struct
		{
			int sampleNumber;
			int nbSamplesPerChanPerBlock;
			int nbSamplesTotPerBlock;
			signed int data[NB_SAMP_ACQ_Packet];
		} m_structBuffData;

		char* m_pStructBuffData;

	protected:

		OpenViBEAcquisitionServer::IDriverCallback* m_pCallback;
		OpenViBEAcquisitionServer::CHeader m_oHeader;

		OpenViBE::uint32 m_ui32SampleCountPerSentBlock;
		OpenViBE::float32* m_pSample;

		OpenViBE::uint32 m_ui32IndexIn;
		OpenViBE::uint32 m_ui32IndexOut;
		OpenViBE::uint32 m_ui32SocketFlag;
		OpenViBE::uint32 m_ui32BuffDataIndex;
	};
};

#endif // __OpenViBE_AcquisitionServer_CDriverCtfVsmMeg_H__
