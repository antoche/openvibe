#ifndef __OVAS_DEFINES_BRAINAMP_SERIES__
#define __OVAS_DEFINES_BRAINAMP_SERIES__

// __________________________________________________________________//
//                                                                   //
// File created from BrainAmpIoCtl.h in Brain Products low level SDK //
// More compliant with OpenViBE coding rules                         //
// __________________________________________________________________//
//                                                                   //

// Send setup, Parameter [IN]: BA_SETUP struct.
#define BRAINAMP_SETUP CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_WRITE_DATA)

// Start acquisition, Parameter [IN]: long nType, 0 = Impedance check, 1 = Data aquisition, 2 = test signal
#define BRAINAMP_START CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_WRITE_DATA)

// Stop acquisition
#define BRAINAMP_STOP CTL_CODE(FILE_DEVICE_UNKNOWN, 0x803, METHOD_BUFFERED, FILE_WRITE_DATA)

// Get buffer filling state, Parameter [OUT]: long nState, < 0 = Overflow, 0 - 100 = Filling state in percent.
#define BRAINAMP_BUFFERFILLING_STATE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x804, METHOD_BUFFERED, FILE_READ_DATA)

// Get battery voltage of one unit, Parameter [IN]: USHORT nUnit, Parameter [OUT]: long nVoltage in millivolts
#define BRAINAMP_BATTERY_VOLTAGE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x805, METHOD_BUFFERED, FILE_WRITE_DATA | FILE_READ_DATA)

// Set impedance check sine wave frequency, Parameter [IN]: LONG nFrequency in Hertz
#define BRAINAMP_IMPEDANCE_FREQUENCY CTL_CODE(FILE_DEVICE_UNKNOWN, 0x806, METHOD_BUFFERED, FILE_WRITE_DATA)

// Set impedance test mode, i.e. even in recording mode, the amplifier is still in impedance check mode
// Parameter [IN]: long bTestMode, != 0 testmode, 0 normal mode
#define BRAINAMP_IMPEDANCE_TESTMODE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x807, METHOD_BUFFERED, FILE_WRITE_DATA)

// Set impedance group and range
// Parameter [IN]: long nGroupRange, 0 = 100kOhm Data (IMP1 | CAL), 1 = 10kOhm Data (IMP1),
//          2 = 100 kOhm Reference (IMP2 | CAL), 3 = 10 kOhm Reference (IMP2)
//          4 = Ground (IMP2)
#define BRAINAMP_IMPEDANCE_GROUPRANGE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x808, METHOD_BUFFERED, FILE_WRITE_DATA)

// Get error state, Parameter [OUT]: long nState,
//          Highword (amplifier): Bit 0 - 3: amplifier number(s)
//          Loword:  (error code): 0 = no error, 1 = loss lock, 2 = low power,
//                3 = can't establish communication at start.
//                4 = synchronisation error
#define BRAINAMP_ERROR_STATE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x809, METHOD_BUFFERED, FILE_READ_DATA)

// Disable DC offset correction for testing purposes.
// Parameter [IN]: long bTestMode, != 0 DC offset correction off, 0 normal mode
#define BRAINAMP_DCOFFSET_TESTMODE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80A, METHOD_BUFFERED, FILE_WRITE_DATA)

// Send calibration settings, Parameter [IN]: BA_CALIBRATION_SETTINGS struct.
#define BRAINAMP_CALIBRATION_SETTINGS CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80B, METHOD_BUFFERED, FILE_WRITE_DATA)

// Set pullup/pulldown resistors for the digital input (default is pulldown).
// This can not be done for each bit, but for 2 groups.
// Parameter [IN]: USHORT bPullup, (low byte for bit 0 - 7, high byte for bit 8 - 15)
//       != 0 pullup, 0 pulldown
#define BRAINAMP_DIGITALINPUT_PULL_UP CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80C, METHOD_BUFFERED, FILE_WRITE_DATA)

// Get digital input value, Parameter [OUT]: USHORT nValue
#define BRAINAMP_DIGITALINPUT_VALUE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80D, METHOD_BUFFERED, FILE_READ_DATA)

// Get driver version, Parameter [OUT]: ULONG nValue
// The version is coded as Major.Minor.DLL.
// The "DLL" part is used for intermediate versions and contains 4 digits.
// The minor part contains 2 digits.
// The number 1010041 for example means version 1.01.0041.
// If the highest bit (bit 31) is set, it means "test version".
#define BRAINAMP_DRIVERVERSION CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80E, METHOD_BUFFERED, FILE_READ_DATA)

// Send Commands during recording
// Parameter [IN]: USHORT nCommand (1 = DC Correction), USHORT nChannel (-1 = all channels)
#define BRAINAMP_COMMAND CTL_CODE(FILE_DEVICE_UNKNOWN, 0x80F, METHOD_BUFFERED, FILE_WRITE_DATA)

// Set Control register directly (WRCON)
// Parameter [IN]: USHORT nAddress, ulong[4] channel mask
#define BRAINAMP_CONTROLREGISTER CTL_CODE(FILE_DEVICE_UNKNOWN, 0x810, METHOD_BUFFERED, FILE_WRITE_DATA)

// Set DC offset value for potentiometer A (coarse)
// Parameter [IN]: USHORT nChannel, UCHAR nValue
#define BRAINAMP_DCOFFSET_COARSE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x811, METHOD_BUFFERED, FILE_WRITE_DATA)

// Do DC Offset correction for potentiometer A (coarse)
#define BRAINAMP_DCOFFSET_CORRECTION_COARSE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x812, METHOD_NEITHER, FILE_WRITE_DATA)

// Set DC offset value for potentiometer B (fine)
// Parameter [IN]: USHORT nChannel, UCHAR nValue
#define BRAINAMP_DCOFFSET_FINE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x813, METHOD_BUFFERED, FILE_WRITE_DATA)

// Do DC Offset correction for potentiometer B (fine)
#define BRAINAMP_DCOFFSET_CORRECTION_FINE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x814, METHOD_NEITHER, FILE_WRITE_DATA)

// Set Cal, Imp1, and Imp2 lines (testing only!)
// Parameter [IN]: BYTE Cal, BYTE, Imp1, Byte Imp2
#define BRAINAMP_SET_CAL_IMP_LINES CTL_CODE(FILE_DEVICE_UNKNOWN, 0x815, METHOD_BUFFERED, FILE_WRITE_DATA)

// Get amplifier type of all 4 units,
// Parameter [IN]: USHORT nUnit, Parameter [OUT]: USHORT Type[4]: 0 no Amp, 1 BrainAmp, 2 MR, 3 DC
#define BRAINAMP_AMPLIFIER_TYPE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x816, METHOD_BUFFERED, FILE_WRITE_DATA | FILE_READ_DATA)

// Do DC Offset correction, no parameters
#define BRAINAMP_DCOFFSET_CORRECTION CTL_CODE(FILE_DEVICE_UNKNOWN, 0x817, METHOD_NEITHER, FILE_WRITE_DATA)

// Retrieve serial number of device. Parameter [OUT]: ULONG
#define BRAINAMP_GET_SERIALNUMBER CTL_CODE(FILE_DEVICE_UNKNOWN, 0x818, METHOD_BUFFERED, FILE_READ_DATA)

// Retrieve number of supported amplifiers for the device. Parameter [OUT]: USHORT
#define BRAINAMP_GET_SUPPORTED_AMPLIFIERS CTL_CODE(FILE_DEVICE_UNKNOWN, 0x819, METHOD_BUFFERED, FILE_READ_DATA)

// Prestart, used only if more than one device is involved.
#define BRAINAMP_PRESTART CTL_CODE(FILE_DEVICE_UNKNOWN, 0x81a, METHOD_BUFFERED, FILE_WRITE_DATA)

// Poststart, used only if more than one device is involved.
#define BRAINAMP_POSTSTART CTL_CODE(FILE_DEVICE_UNKNOWN, 0x81b, METHOD_BUFFERED, FILE_WRITE_DATA)

// Disable busmastering
#define BRAINAMP_DISABLE_BUSMASTERING CTL_CODE(FILE_DEVICE_UNKNOWN, 0x81c, METHOD_BUFFERED, FILE_WRITE_DATA)

namespace OpenViBEAcquisitionServer
{
	#pragma pack(1)

	// Brainamp Setup
	typedef struct
	{
	public:
		OpenViBE::uint32 m_ui32ChannelCount;        // Number of channels                                    - was /*long*/
		OpenViBE::uint8 m_pChannelLookup[256];      // Channel lookup table, -1 to -8 means PolyBox channels - was /*CHAR*/
		OpenViBE::uint32 m_ui32SampleCountPerSentBlock; // Number of points per block                            - was /*long*/
		OpenViBE::uint16 m_ui16HoldValue;           // Hold value for digital input                          - was /*USHORT*/

		// Version 1.01.0003 and higher
		// The following tables are based on logical channel positions
		OpenViBE::uint8 m_pLowPassFilter[256];  // Low pass 250 Hz (0 = 1000Hz)                    - was /*UCHAR*/
		OpenViBE::uint8 m_pResolution[256];     // 0 = 100 nV, 1 = 500 nV, 2 = 10 µV, 3 = 152.6 µV - was /*UCHAR*/
		OpenViBE::uint8 m_pDCCoupling[256];     // DC coupling (0 = AC)                            - was /*UCHAR*/
		OpenViBE::uint8 m_ui8LowImpedance;      // Low impedance i.e. 10 MOhm, (0 = > 100MOhm)     - was /*UCHAR*/
	} CBrainampSetup;

	// Brainamp Calibration Settings (default: square waves, 5 Hz)
	typedef struct
	{
	public:
		OpenViBE::uint16 m_ui16WaveForm;    // 0 = ramp, 1 = triangle, 2 = square, 3 = sine wave - was /*USHORT*/
		OpenViBE::uint32 m_ui32Frequency;   // Frequency in millihertz - was /*ULONG*/
	} CBrainampCalibrationSettings;

	#pragma pack()

	// Brainamp Parameter Types
	typedef enum
	{
		Parameter_Default=-1,

		Channel_Unselected=0,
		Channel_Selected=1,

		DecimationFactor_None=0,
		DecimationFactor_2=1,
		DecimationFactor_4=2,
		DecimationFactor_5=3,
		DecimationFactor_8=4,
		DecimationFactor_10=5,

		LowPass_1000=0,
		LowPass_250=1,

		Resolution_100nV=0,
		Resolution_500nV=1,
		Resolution_10uV=2,
		Resolution_152uV=3,

		DCCouping_AC=0,
		DCCouping_DC=1,

		Impedance_High=0,
		Impedance_Low=1,

		AcquisitionType_Impedance=0,
		AcquisitionType_EEG=1,
		AcquisitionType_TestSignal=2,

		WaveForm_Ramp=0,
		WaveForm_Triangle=1,
		WaveForm_Square=2,
		WaveForm_SineWave=3,

		AmplifierType_None=0,
		AmplifierType_BrainAmp=1,
		AmplifierType_BrainAmpMR=2,
		AmplifierType_BrainAmpDC=3,
	} EParameter;

	inline const char* getDeviceType(const OpenViBE::uint32 ui32DeviceType)
	{
		const char* l_sAmplifierType;
		switch(ui32DeviceType)
		{
			case AmplifierType_None:
				l_sAmplifierType="-- Not found --";
				break;
			case AmplifierType_BrainAmp:
				l_sAmplifierType="BrainAmp Standard";
				break;
			case AmplifierType_BrainAmpMR:
				l_sAmplifierType="BrainAmp MR";
				break;
			case AmplifierType_BrainAmpDC:
				l_sAmplifierType="BrainAmp DC / MR Plus";
				break;
			default:
				l_sAmplifierType="-- Unknown --";
				break;
		}
		return l_sAmplifierType;
	}

	#define BrainAmp_MaximumAmplifierCount 4
};

#endif // __OVAS_DEFINES_BRAINAMP_SERIES__
