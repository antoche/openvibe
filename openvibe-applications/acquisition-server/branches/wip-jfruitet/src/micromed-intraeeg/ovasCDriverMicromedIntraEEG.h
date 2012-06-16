#ifndef __OpenViBE_AcquisitionServer_CDriverMicromedIntraEEG_H__
#define __OpenViBE_AcquisitionServer_CDriverMicromedIntraEEG_H__

#include "../ovasIDriver.h"
#include "../ovasCHeader.h"

#include <openvibe/ov_all.h>

#include <socket/IConnectionClient.h>
#include <socket/IConnectionServer.h>

#define MAX_CAN 256
#define MAX_LAB 640
#define MAX_MONT 30
#define MAX_NOTE 200
#define MAX_FLAG 100
#define MAX_SEGM 100
#define MAX_SAMPLE 128
#define MAX_HISTORY 30
#define MAX_EVENT 100
#define MAX_FILE 1024
#define MAX_TRIGGER 8192

#define AVERAGE_FREE 108
#define MAX_FILE 1024
#define MAX_CAN_VIEW 128

#define NB_SAMPLE_MAX  1024

namespace OpenViBEAcquisitionServer
{
	class CDriverMicromedIntraEEG : public OpenViBEAcquisitionServer::IDriver
	{
	public:

		CDriverMicromedIntraEEG(OpenViBEAcquisitionServer::IDriverContext& rDriverContext);
		virtual ~CDriverMicromedIntraEEG(void);
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

		Socket::IConnectionServer* m_pConnectionServer;
		OpenViBE::uint32 m_ui32ServerHostPort;
		Socket::IConnection* m_pConnection;

	protected:

		OpenViBEAcquisitionServer::IDriverCallback* m_pCallback;
		OpenViBEAcquisitionServer::CHeader m_oHeader;

		OpenViBE::uint32 m_ui32SampleCountPerSentBlock;
		OpenViBE::float32* m_pSample;

		OpenViBE::uint32 m_ui32IndexIn;
		OpenViBE::uint32 m_ui32IndexOut;
		OpenViBE::uint32 m_ui32BuffDataIndex;

		#pragma pack(push)
		#pragma pack(1)
		struct
		{
			char fixCode[4];
			short int infoType;
			int lenData;
		} m_structHeader;

		struct
		{
			char title[32];
			char lab[32];

			char subjectLastName[22];
			char subjectFirstName[20];
			unsigned char subjectBirthDate[3];
			unsigned char reserved[19];
			unsigned char recordingDate[3];
			unsigned char recordingTime[3];
			unsigned short int acqEquipement;
			unsigned short int fileTypeEquipement;
			unsigned long  int addressOfData;
			unsigned short int nbOfChannels;
			unsigned short int multiplexer;
			unsigned short int minimumSamplingRate;
			unsigned short int sizeOfEachDataInBytes;
			unsigned short int compressionSatus;
			unsigned short int numberOfMontages;
			unsigned long int digitalVideoStartSample;
			unsigned short int mpegDelay;
			unsigned char tmp2[15];
			unsigned char headerType;

			char getLocationOrInfoForBunchOrStaff[8];
				unsigned long int  SSStartOffsetCode;
				unsigned long int SSStartLengthCode;
			char labcod[8];
				unsigned long int SSStartOffsetElec;
				unsigned long int SSStartLengthElec;
			char note[8];
				unsigned long int SSStartOffsetNote;
				unsigned long int SSStartLengthNote;
			char flags[8];
				unsigned long int SSStartOffsetFlag;
				unsigned long int SSStartLengthFlag;
			char tronca[8];
				unsigned long int SSStartOffsetRedu;
				unsigned long int SSStartLengthRedu;
			char begi[8];
				unsigned long int SSStartOffsetbegi;
				unsigned long int SSStartLengthBegi;
			char endi[8];
				unsigned long int SSStartOffsetEndi;
				unsigned long int SSStartLengthEndi;
			char mont[8];
				unsigned long int SSStartOffsetMont;
				unsigned long int SSStartLengthMont;
			char comp[8];
				unsigned long int SSStartOffsetComp;
				unsigned long int SSStartLengthComp;
			char aver[8];
				unsigned long int SSStartOffsetAver;
				unsigned long int SSStartLengthAver;
			char hist[8];
				unsigned long int SSStartOffsetHist;
				unsigned long int SSStartLengthHist;
			char dvdVideo[8];
				unsigned long int SSStartOffsetDvdVideo;
				unsigned long int SSStartLengthDvdVideo;
			char eva[8];
				unsigned long int SSStartOffsetEva;
				unsigned long int SSStartLengthEva;
			char evb[8];
				unsigned long int SSStartOffsetEvb;
				unsigned long int SSStartLengthEvb;
			char trig[8];
				unsigned long int SSStartOffsetTrig;
				unsigned long int SSStartLengthTrig;
			unsigned char reserved2[224];

			unsigned short int code[MAX_CAN];

			unsigned char statusOfElectrode;
			unsigned char referenceTypeElectrode;
			char positiveInputLabel[6];
			char negativeInputLabel[6];
			long int logicMinimum;
			long int logicMaximum;
			long int logicGround;
			long int physicMinimum;
			long int physicMaximum;
			unsigned short int measurementsUnits;
			unsigned short int PrefilteringHighPassLimit;
			unsigned short int prefilteringHighType;
			unsigned short int prefilteringLowPassLimit;
			unsigned short int prefilteringLowPassType;
			unsigned short int rateCoef;
			unsigned short int positionReserve;
			float latitudine;
			float longitudine;
			unsigned char presentInMep;
			unsigned char isInAvg;
			char description[32];
			float x;
			float y;
			float z;
			unsigned short int coordinateType;
			unsigned char freeReserved[24];

			//~ unsigned char statusOfElectrode[MAX_LAB];
			//~ unsigned char referenceTypeElectrode[MAX_LAB];
			//~ char positiveInputLabel[6][MAX_LAB];
			//~ char negativeInputLabel[6][MAX_LAB];
			//~ long int logicMinimum[MAX_LAB];
			//~ long int logicMaximum[MAX_LAB];
			//~ long int logicGround[MAX_LAB];
			//~ long int physicMinimum[MAX_LAB];
			//~ long int physicMaximum[MAX_LAB];
			//~ unsigned short int measurementsUnits[MAX_LAB];
			//~ unsigned short int PrefilteringHighPassLimit[MAX_LAB];
			//~ unsigned short int prefilteringHighType[MAX_LAB];
			//~ unsigned short int prefilteringLowPassLimit[MAX_LAB];
			//~ unsigned short int prefilteringLowPassType[MAX_LAB];
			//~ unsigned short int rateCoef[MAX_LAB];
			//~ unsigned short int positionReserved[MAX_LAB];
			//~ float latitudine[MAX_LAB];
			//~ float longitudine[MAX_LAB];
			//~ unsigned char presentInMep[MAX_LAB];
			//~ unsigned char isInAvg[MAX_LAB];
			//~ char description[32][MAX_LAB];
			//~ float x[MAX_LAB];
			//~ float y[MAX_LAB];
			//~ float z[MAX_LAB];
			//~ unsigned short int coordinateType[MAX_LAB];
			//~ unsigned char freeReserved[24][MAX_LAB];

			//~ unsigned long int sampleNote[MAX_NOTE];
			//~ char comment[40][MAX_NOTE];

			//~ long int beginFlag[MAX_FLAG];
			//~ long int endFlag[MAX_FLAG];

			//~ unsigned long int time[MAX_SEGM];
			//~ unsigned long int sampleSegment[MAX_SEGM];

			//~ unsigned char positiveStartImp[MAX_CAN];
			//~ unsigned char negativeStartImp[MAX_CAN];

			//~ unsigned char positiveEndImp[MAX_CAN];
			//~ unsigned char negativeEndImp[MAX_CAN];

			//~ unsigned short int linesMontage[MAX_MONT];
			//~ unsigned short int sectorsMontage[MAX_MONT];
			//~ unsigned short int baseTimeMontage[MAX_MONT];
			//~ unsigned short int notchMontage[MAX_MONT];
			//~ unsigned char colourMontage[MAX_CAN_VIEW][MAX_MONT];
			//~ unsigned char selectionMontage[MAX_CAN_VIEW][MAX_MONT];
			//~ char descriptionMontage[64][MAX_MONT];
			//~ unsigned short int nonInv[MAX_CAN_VIEW][MAX_MONT];
			//~ unsigned short int Inv[MAX_CAN_VIEW][MAX_MONT];
			//~ unsigned long int hiPassFilter[MAX_CAN_VIEW][MAX_MONT];
			//~ unsigned long int lowPassFilter[MAX_CAN_VIEW][MAX_MONT];
			//~ unsigned long int referenceMontage[MAX_CAN_VIEW][MAX_MONT];
			//~ unsigned char freeReserved2[1720][MAX_MONT];

			//~ unsigned long int meanTrace;
			//~ unsigned long int meanFile;
			//~ unsigned long int meanPreStim;
			//~ unsigned long int  meanPostStim;
			//~ unsigned long int meanType;
			//~ unsigned char freeReserved3[AVERAGE_FREE];

			//~ unsigned long int historySample[MAX_SAMPLE];

			//~ unsigned short int linesMontageHistory[MAX_HISTORY];
			//~ unsigned short int sectorsMontageHistory[MAX_HISTORY];
			//~ unsigned short int baseTimeMontageHistory[MAX_HISTORY];
			//~ unsigned short int notchMontageHistory[MAX_HISTORY];
			//~ unsigned char colourMontageHistory[MAX_CAN_VIEW][MAX_HISTORY];
			//~ unsigned char selectionMontageHistory[MAX_CAN_VIEW][MAX_HISTORY];
			//~ char descriptionMontageHistory[64][MAX_HISTORY];
			//~ unsigned short int nonInvMontageHistory[MAX_CAN_VIEW][MAX_HISTORY];
			//~ unsigned short int InvMontageHistory[MAX_CAN_VIEW][MAX_HISTORY];
			//~ unsigned long int hiPassFilterMontageHistory[MAX_CAN_VIEW][MAX_HISTORY];
			//~ unsigned long int lowPassFilterMontageHistory[MAX_CAN_VIEW][MAX_HISTORY];
			//~ unsigned long int referenceMontageHistory[MAX_CAN_VIEW][MAX_HISTORY];
			//~ unsigned char freeReserved2MontageHistory[1720][MAX_HISTORY];

			//~ unsigned long int dvBegin[MAX_FILE];

			//~ long int descriptionEventA;
			//~ long int beginEventA[MAX_EVENT];
			//~ long int endEventA[MAX_EVENT];

			//~ long int descriptionEventB;
			//~ long int beginEventB[MAX_EVENT];
			//~ long int endEventB[MAX_EVENT];

			//~ unsigned long int trigSample[MAX_TRIGGER];
			//~ unsigned short int TrigValue[MAX_TRIGGER];

		} m_structHeaderInfo;

		struct
		{
			signed short int data[NB_SAMPLE_MAX];
		} m_structBuffData;
		#pragma pack(pop)

		char* m_pStructHeader;
		char* m_pStructHeaderInfo;
		char* m_pStructBuffData;

	};
};

#endif // __OpenViBE_AcquisitionServer_CDriverMicromedIntraEEG_H__
