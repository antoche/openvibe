#ifndef __OpenViBE_AcquisitionServer_CDriverTMSiRefa32B_H__
#define __OpenViBE_AcquisitionServer_CDriverTMSiRefa32B_H__

#include "../ovasIDriver.h"
#include "../ovasCHeader.h"
#include "../ovas_base.h"

#if defined OVAS_OS_Windows

#include "ovasCConfigurationTMSIRefa32B.h"

#define RTLOADER "\\RTINST.Dll"
#include <gtk/gtk.h>
// Get Signal info

#define SIGNAL_NAME 40
#define MAX_BUFFER_SIZE 0xFFFFFFFF

namespace OpenViBEAcquisitionServer
{
	class CDriverTMSiRefa32B : virtual public OpenViBEAcquisitionServer::IDriver
	{
	public:

		CDriverTMSiRefa32B(OpenViBEAcquisitionServer::IDriverContext& rDriverContext);
		~CDriverTMSiRefa32B(void);
		virtual void release(void);
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
		virtual OpenViBE::boolean CDriverTMSiRefa32B::measureMode(OpenViBE::uint32 mode,OpenViBE::uint32 info );
	protected:
		OpenViBEAcquisitionServer::IDriverCallback* m_pCallback;
		OpenViBEAcquisitionServer::CHeader m_oHeader;

		OpenViBE::uint32 m_ui32SampleCountPerSentBlock;
		OpenViBE::float32 *m_pSample;
		OpenViBE::boolean m_bValid;

		OpenViBE::uint32 m_ui32SampleIndex;

		OpenViBE::boolean refreshDevicePath(void);
		OpenViBE::boolean m_bCheckImpedance;

		//----------- TYPE ---------------------
		//constants used by set chantype
		#define EXG (ULONG) 0x0001
		#define AUX (ULONG) 0x0002
		#define DEVICE_FEATURE_TYPE 		0x0303
		//------------ MODE ---------------------
		#define DEVICE_FEATURE_MODE		    0x0302
		//--------------- RTC ----------------------
		#define DEVICE_FEATURE_RTC		    0x0301
		//---------- HIGHPASS ------------------
		#define DEVICE_FEATURE_HIGHPASS 	0x0401
		//------------- LOWPASS ----------------
		#define DEVICE_FEATURE_LOWPASS 	0x0402
		//--------------- GAIN ------------------
		#define DEVICE_FEATURE_GAIN		0x0403
		//--------------- OFFSET -------------------
		#define DEVICE_FEATURE_OFFSET	0x0404
		//------------------ IO ----------------------
		#define DEVICE_FEATURE_IO 0x0500
		//----------------- MEMORY ----------------------
		#define DEVICE_FEATURE_MEMORY 0x0501
		//------------------- STORAGE ---------------------
		#define DEVICE_FEATURE_STORAGE 0x0502
		//------------------ CORRECTION --------------------
		#define DEVICE_FEATURE_CORRECTION 0x0503
		//--------------------- ID ---------------------------
		#define DEVICE_FEATURE_ID 0x0504

		#define MEASURE_MODE_NORMAL			((ULONG)0x0)
		#define MEASURE_MODE_IMPEDANCE		((ULONG)0x1)
		#define MEASURE_MODE_CALIBRATION	((ULONG)0x2)
		#define MEASURE_MODE_IMPEDANCE_EX	((ULONG)0x3)
		#define MEASURE_MODE_CALIBRATION_EX	((ULONG)0x4)
		//for MEASURE_MODE_IMPEDANCE
		#define IC_OHM_002 0	// 2K Impedance limit
		#define IC_OHM_005 1	// 5K Impedance limit
		#define IC_OHM_010 2	// 10K Impedance limit
		#define IC_OHM_020 3	// 20K Impedance limit
		#define IC_OHM_050 4	// 50K Impedance limit
		#define IC_OHM_100 5	// 100K Impedance limit
		//for MEASURE_MODE_CALIBRATION
		#define IC_VOLT_050 0	//50 uV t-t Calibration voltage
		#define IC_VOLT_100 1	//100 uV t-t
		#define IC_VOLT_200 2	//200 uV t-t
		#define IC_VOLT_500 3	//500 uV t-t
	};
};

#endif // OVAS_OS_Windows

#endif // __OpenViBE_AcquisitionServer_CDriverTMSiRefa32B_H__
