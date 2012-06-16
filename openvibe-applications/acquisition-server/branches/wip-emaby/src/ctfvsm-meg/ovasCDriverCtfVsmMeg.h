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

		typedef char Str32[32];
		typedef char Str60[60];

		CDriverCtfVsmMeg(OpenViBEAcquisitionServer::IDriverContext& rDriverContext);
		virtual ~CDriverCtfVsmMeg(void);
		virtual const char* getName(void);

		virtual OpenViBE::boolean isFlagSet(
			const OpenViBEAcquisitionServer::EDriverFlag eFlag) const
		{
			return eFlag==DriverFlag_IsUnstable;
		}

		virtual OpenViBE::boolean initialize(
			const OpenViBE::uint32 ui32RequestedSampleCountPerSentBlock,
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
	
	protected:		
				OpenViBEAcquisitionServer::IDriverCallback* m_pCallback;
		OpenViBEAcquisitionServer::CHeader m_oHeader;

		OpenViBE::uint32 m_ui32SampleCountPerSentBlock;
		OpenViBE::float32* m_pSample;

		OpenViBE::uint32 m_ui32IndexIn;
		OpenViBE::uint32 m_ui32IndexOut;
		OpenViBE::uint32 m_ui32SocketFlag;
		OpenViBE::uint32 m_ui32BuffDataIndex;
	  


		OpenViBE::uint32 m_ui32NbStimChannels;
		OpenViBE::uint32 *m_pStimChannelIndex;
		OpenViBE::uint32 *m_pPrevStimValue;
		
		enum
		{
			CTF_HEADER=0,
			CTF_DATA=1,
			CTF_STOP=2
		};

		struct
		{
			OpenViBE::uint32 m_ui32Command; /* may be 0 : header, 1 : data, 2 : stop . */
			OpenViBE::uint32 m_ui32Size;
		} m_structCommandHeader;
		
		struct
		{
			char *m_strExperimentId; /* Str60 */
			char * m_strExperimenDate; /* Str32 */

			char *m_strSubjectName; /* Str32 */
			OpenViBE::int32 *m_i32SubjectAge;
			char *m_strSubjectSex; /* 4 char */ /**F: female or M: Male*/

			OpenViBE::int32 *m_i32LabId;
			char *m_strLabName; /* Str32 */
			OpenViBE::int32 *m_i32TechnicianId;
			char *m_strTechnicianName; /* Str32 */

			OpenViBE::float32 *m_f32SamplingRate;
			OpenViBE::int32 *m_i32NumberOfChannels;

			OpenViBE::float64 *m_f64PosRefNaLeReDew; /* 9 values */
			
			OpenViBE::uint32 *m_ui32NumSamples;
		} m_structFixedHeader;
		char* m_pStructFixedHeaderBuffer;

		struct
		{
			Str32 *m_strChannelLabel;
			OpenViBE::int32 *m_i32ChannelTypeIndex;

			OpenViBE::float32 *m_f32ProperGain;
			OpenViBE::float32 *m_f32QGain;
			OpenViBE::float32 *m_f32IOGain;
			
			OpenViBE::int32 *m_i32GradOrderNum;
		} m_structVariableHeader;
		char* m_pStructVariableHeaderBuffer;
		
		struct
		{
			OpenViBE::int32 *m_i32SampleNumber;
			OpenViBE::int32 *m_pDataBuffer;
		} m_structData;
		char* m_pStructDataBuffer;
		
		OpenViBE::uint32 m_ui32FixedHeaderLength;
		OpenViBE::uint32 m_ui32VariableHeaderLength;
		OpenViBE::uint32 m_ui32DataBufferLength;

		void bufferToFixedHeaderStruct(void);
		void bufferToVariableHeaderStruct(void);
		void readCTFCommandHeader(void);
	};
};

#endif // __OpenViBE_AcquisitionServer_CDriverCtfVsmMeg_H__
