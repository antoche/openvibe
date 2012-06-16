#ifndef __OpenViBEPlugins_FileIO_GDFHelpers_H__
#define __OpenViBEPlugins_FileIO_GDFHelpers_H__

#include <openvibe/ov_all.h>

#include <system/Memory.h>

#include <fstream>
#include <vector>

namespace OpenViBEPlugins
{
	namespace FileIO
	{
		/**
		 * Useful classes for GDF file format handling
		*/
		class GDF
		{
		public:

			class CFixedGDFHeader
			{
			public:
				virtual ~CFixedGDFHeader(){}

				virtual OpenViBE::boolean read(std::ifstream& oFile) = 0;

				virtual OpenViBE::boolean save(std::ofstream& oFile) = 0;
				virtual OpenViBE::boolean update(std::ofstream& oFile)=0;

				virtual OpenViBE::uint64 getSubjectIdentifier();
				virtual std::string getSubjectName();
				virtual OpenViBE::uint64 getSubjectSex();
				virtual OpenViBE::uint64 getSubjectAge();
				virtual OpenViBE::uint64 getExperimentIdentifier();
				virtual std::string getExperimentDate();
				virtual OpenViBE::uint64 getLaboratoryIdentifier();
				virtual OpenViBE::uint64 getTechnicianIdentifier();
				virtual std::string getLaboratoryName();
				virtual std::string getTechnicianName();

				virtual OpenViBE::float64 getDataRecordDuration() = 0;
				virtual OpenViBE::uint64 getNumberOfDataRecords() = 0;
				virtual OpenViBE::uint64 getChannelCount() = 0;
			};

			/**
		 	 * An helper class to manipulate GDF1 fixed-size headers
			 */
			class CFixedGDF1Header : public CFixedGDFHeader
			{
			public:
				CFixedGDF1Header();

				virtual ~CFixedGDF1Header(){}

				/**
				 * Reads a GDF1 fixed Header from a file
				 * \param oFile The input file.
				 * \return true if the operation was successful
				 */
				virtual OpenViBE::boolean read(std::ifstream& oFile);

				/**
				 * Saves a GDF1 fixed Header in a file
				 * \param oFile The output file.
				 * \return true if the operation was successful
				 */
				virtual OpenViBE::boolean save(std::ofstream& oFile);

				/**
				 * Updates the number of data records field in
				 * a GDF1 fixed Header in a file
				 * \param oFile The output file.
				 * \return true if the operation was successful
				 */
				virtual OpenViBE::boolean update(std::ofstream& oFile);

				virtual std::string getSubjectName();
				virtual OpenViBE::uint64 getLaboratoryIdentifier();
				virtual OpenViBE::uint64 getTechnicianIdentifier();

				virtual OpenViBE::float64 getDataRecordDuration();
				virtual OpenViBE::uint64 getNumberOfDataRecords();
				virtual OpenViBE::uint64 getChannelCount();

				char m_sVersionId[8];
				char m_sPatientId[80];
				char m_sRecordingId[80];
				char m_sStartDateAndTimeOfRecording[16];
				OpenViBE::int64 m_i64NumberOfBytesInHeaderRecord;
				OpenViBE::uint64 m_ui64EquipmentProviderId;
				OpenViBE::uint64 m_ui64LaboratoryId;
				OpenViBE::uint64 m_ui64TechnicianId;
				char m_sReservedSerialNumber[20];
				OpenViBE::int64 m_i64NumberOfDataRecords;
				OpenViBE::uint32 m_ui32DurationOfADataRecordNumerator;
				OpenViBE::uint32 m_ui32DurationOfADataRecordDenominator;
				OpenViBE::uint32 m_ui32NumberOfSignals;
			};


			/**
			 * An helper class to manipulate GDF2 fixed-size headers
			 */
			class CFixedGDF2Header : public CFixedGDFHeader
			{
			public:

				CFixedGDF2Header();

				virtual ~CFixedGDF2Header(){}

				/**
				* Reads a GDF2 fixed Header from a file
				* \param oFile The input file.
				* \return true if the operation was successful
				 */
				virtual OpenViBE::boolean read(std::ifstream& oFile);

				/**
				 * Saves a GDF2 fixed Header in a file
				 * \param oFile The output file.
				 * \return true if the operation was successful
				 */
				virtual OpenViBE::boolean save(std::ofstream& oFile);

				/**
				 * Updates the number of data records field in
				 * a GDF2 fixed Header in a file
				 * \param oFile The output file.
				 * \return true if the operation was successful
				 */
				virtual OpenViBE::boolean update(std::ofstream& oFile){ return true;}

				virtual std::string getExperimentDate();
				virtual std::string getSubjectName();
				virtual OpenViBE::uint64 getSubjectSex();
				virtual OpenViBE::uint64 getSubjectAge();

				virtual OpenViBE::float64 getDataRecordDuration();
				virtual OpenViBE::uint64 getNumberOfDataRecords();
				virtual OpenViBE::uint64 getChannelCount();

				char m_sVersionId[8];
				char m_sPatientId[66];

				OpenViBE::uint8 m_ui8Reserved[10];
				OpenViBE::uint8 m_ui8HealthInformation; //smoking...
				OpenViBE::uint8 m_ui8Weight;
				OpenViBE::uint8 m_ui8Height;
				OpenViBE::uint8 m_ui8SubjectInformation;//gender...

				char m_sRecordingId[64];
				OpenViBE::uint32 m_ui32RecordingLocation[4];
				OpenViBE::uint32 m_ui32StartDateAndTimeOfRecording[2];
				OpenViBE::uint32 m_ui32Birthday[2];
				OpenViBE::uint16 m_ui16NumberOfBlocksInHeader;
				OpenViBE::uint8 m_ui8Reserved2[6];
				OpenViBE::uint64 m_ui64EquipmentProviderId;
				OpenViBE::uint8 m_ui8IPAdress[6];
				OpenViBE::uint16 m_ui16HeadSize[3];
				OpenViBE::float32 m_f32PositionReferenceElectrode[3];
				OpenViBE::float32 m_f32GroundElectrode[3];
				OpenViBE::int64 m_i64NumberOfDataRecords;
				OpenViBE::uint32 m_ui32DurationOfADataRecordNumerator;
				OpenViBE::uint32 m_ui32DurationOfADataRecordDenominator;
				OpenViBE::uint16 m_ui16NumberOfSignals;
				OpenViBE::uint16 m_ui16Reserved3;
			};

			/**
			* Base class for GDF file's variable headers
			*/
			class CVariableGDFHeader
			{
				public:
					virtual ~CVariableGDFHeader(){};
					virtual OpenViBE::boolean save(std::ofstream& oFile) = 0;
			};

			/**
			 * GDF1 variable header class
			 */
			class CVariableGDF1Header : public CVariableGDFHeader
			{
				//! Stores information for one channel
				class CVariableGDF1HeaderPerChannel
				{
					public:

						CVariableGDF1HeaderPerChannel(void);

						char m_sLabel[16];
						char m_sTranducerType[80];
						char m_sPhysicalDimension[8];
						OpenViBE::float64 m_f64PhysicalMinimum;
						OpenViBE::float64 m_f64PhysicalMaximum;
						OpenViBE::int64 m_i64DigitalMinimum;
						OpenViBE::int64 m_i64DigitalMaximum;
						char m_sPreFiltering[80];
						OpenViBE::uint32 m_ui32NumberOfSamplesInEachRecord;
						OpenViBE::uint32 m_ui32ChannelType;
						char m_sReserved[32];
				};

				public:

					virtual ~CVariableGDF1Header(){}

					/**
					* Saves a GDF1 variable Header in a file
					* \param oFile The output file.
					* \return true if the operation was successful
					 */
					virtual OpenViBE::boolean save(std::ofstream& oFile);

					/**
					 * Updates the Physical/digital Min/max fields in
					 * a GDF1 variable Header in a file
					 * \param oFile The output file.
					 * \return true if the operation was successful
					 */
					virtual OpenViBE::boolean update(std::ofstream& oFile);

					/**
					 * Sets the number of channels in the file.
					 * \param ui32ChannelCount Number of channels.
					 */
					virtual void setChannelCount(OpenViBE::uint32 ui32ChannelCount);
					virtual CVariableGDF1HeaderPerChannel& operator[](OpenViBE::uint32 ui32Channel);

					std::vector<CVariableGDF1HeaderPerChannel> m_vVariableHeader;

			};



			class CGDFEvent
			{
			public:
				OpenViBE::uint32 m_ui32Position;
				OpenViBE::uint16 m_ui16Type;
			};

			enum ChannelType
			{
				ChannelType_int8 = 1,
				ChannelType_uint8 = 2,
				ChannelType_int16 = 3,
				ChannelType_uint16 = 4,
				ChannelType_int32 = 5,
				ChannelType_uint32 = 6,
				ChannelType_int64 = 7,
				ChannelType_uint64 = 8,
				ChannelType_float32 = 16,
				ChannelType_float64 = 17,
				ChannelType_float128 = 18,
				ChannelType_int24 = 279,
				ChannelType_uint24 = 535
			};

			/**
			 * Gets the data size in bytes of the GDF data type
			 * \param ui32ChannelType The GDF type
			 * \return The size in bytes of this GDF type's data
			 */
			static OpenViBE::uint16 GDFDataSize(OpenViBE::uint32 ui32ChannelType);
		};

	};
};

#endif // __OpenViBEPlugins_FileIO_GDFHelpers_H__
