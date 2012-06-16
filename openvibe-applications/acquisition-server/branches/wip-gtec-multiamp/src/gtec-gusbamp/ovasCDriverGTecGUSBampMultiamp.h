#ifndef __OpenViBE_AcquisitionServer_CDriverGTecGUSBampMultiamp_H__
#define __OpenViBE_AcquisitionServer_CDriverGTecGUSBampMultiamp_H__

#if defined TARGET_HAS_ThirdPartyGUSBampCAPI

#include "../ovasIDriver.h"
#include "../ovasCHeader.h"

#include <gtk/gtk.h>
#include <vector>

namespace OpenViBEAcquisitionServer
{
	class CDriverGTecGUSBampMultiamp : public OpenViBEAcquisitionServer::IDriver
	{
	public:

		CDriverGTecGUSBampMultiamp(OpenViBEAcquisitionServer::IDriverContext& rDriverContext);
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

		OpenViBEAcquisitionServer::IDriverCallback* m_pCallback;
		OpenViBEAcquisitionServer::CHeader m_oHeader;

		OpenViBE::uint32 m_ui32SampleCountPerSentBlock;
		
		// The openvibe buffer
		OpenViBE::uint32 m_ui32BufferSize;
		OpenViBE::float32* m_pSampleTranspose;
		OpenViBE::float32* m_pSample;
		
		// Common configuration (filters, GND REF, Sampling frequency, etc)
		OpenViBE::uint32 m_ui32ActualImpedanceIndex;
		OpenViBE::uint8 m_ui8CommonGndAndRefBitmap;
		OpenViBE::int32 m_i32NotchFilterIndex;
		OpenViBE::int32 m_i32BandPassFilterIndex;
		
		struct SDevice{
			OpenViBE::CString m_sSerial; // serial ID
			OpenViBE::boolean m_bSlave; // if not it's master
			void * m_pEvent; //Windows Event
			void * m_pHandler; //handler to device
			OpenViBE::uint8* m_pBuffer;
			OpenViBE::float32* m_pSampleTranspose; // temp buffer
			void * m_pOverlapped;
		};
		std::vector<SDevice> m_vDevices;

		void closeAllDevices(void);

	};
}

#endif // TARGET_HAS_ThirdPartyGUSBampCAPI

#endif // __OpenViBE_AcquisitionServer_CDriverGTecGUSBampMultiamp_H__
