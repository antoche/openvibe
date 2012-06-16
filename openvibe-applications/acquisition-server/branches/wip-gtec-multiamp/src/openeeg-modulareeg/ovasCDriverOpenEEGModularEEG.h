#ifndef __OpenViBE_AcquisitionServer_CDriverOpenEEGModularEEG_H__
#define __OpenViBE_AcquisitionServer_CDriverOpenEEGModularEEG_H__

#include "../ovasIDriver.h"
#include "../ovasCHeader.h"

#if defined OVAS_OS_Windows
 typedef void * FD_TYPE;
#elif defined OVAS_OS_Linux
 typedef OpenViBE::int32 FD_TYPE;
#else
#endif

#include <vector>

namespace OpenViBEAcquisitionServer
{
	class CDriverOpenEEGModularEEG : public OpenViBEAcquisitionServer::IDriver
	{
	public:

		CDriverOpenEEGModularEEG(OpenViBEAcquisitionServer::IDriverContext& rDriverContext);
		virtual void release(void);
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

		// void logPacket(void);
		OpenViBE::boolean parseByteP2(OpenViBE::uint8 ui8Actbyte);

		OpenViBE::boolean initTTY(::FD_TYPE * pFileDescriptor, OpenViBE::uint32 ui32TtyNumber);
		OpenViBE::int32 readPacketFromTTY(::FD_TYPE i32FileDescriptor);
		void closeTTY(::FD_TYPE i32FileDescriptor);

	protected:

		OpenViBEAcquisitionServer::IDriverCallback* m_pCallback;
		OpenViBEAcquisitionServer::CHeader m_oHeader;

		OpenViBE::uint32 m_ui32ChannelCount;
		OpenViBE::uint32 m_ui32DeviceIdentifier;
		OpenViBE::float32* m_pSample;

		::FD_TYPE  m_i32FileDescriptor;
		OpenViBE::uint16 m_ui16Readstate;
		OpenViBE::uint16 m_ui16ExtractPosition;
		OpenViBE::uint8  m_ui8PacketNumber;
		OpenViBE::uint8  m_ui8LastPacketNumber;
		OpenViBE::uint16 m_ui16Switches;

		std::vector < std::vector < OpenViBE::int32 > > m_vChannelBuffer;
		std::vector < OpenViBE::int32 > m_vChannelBuffer2;

		OpenViBE::CString m_sTTYName;
	};
};

#endif // __OpenViBE_AcquisitionServer_CDriverOpenEEGModularEEG_H__
