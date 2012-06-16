#ifndef __OpenViBEPlugins_BoxAlgorithm_MatlabScripting_H__
#define __OpenViBEPlugins_BoxAlgorithm_MatlabScripting_H__

#if defined TARGET_HAS_ThirdPartyMatlab

#include <map>

#include "ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include "ovpCMatlabHelper.h"

#define OVP_ClassId_BoxAlgorithm_MatlabScripting     OpenViBE::CIdentifier(0x03303E0E, 0x39FE10DF)
#define OVP_ClassId_BoxAlgorithm_MatlabScriptingDesc OpenViBE::CIdentifier(0x46130E2F, 0x34F90BA1)

namespace OpenViBEPlugins
{
	namespace Matlab
	{
		class CBoxAlgorithmMatlabScripting : virtual public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::uint64 getClockFrequency(void);
			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processClock(OpenViBE::CMessageClock& rMessageClock);
			virtual OpenViBE::boolean process(void);


			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_MatlabScripting);

		private:
			OpenViBE::uint64 m_ui64ClockFrequency;

			std::map<OpenViBE::uint32,OpenViBEToolkit::TDecoder<CBoxAlgorithmMatlabScripting>*> m_mDecoders;
			OpenViBE::uint32 m_NbInputHeaderSent;

			std::map<OpenViBE::uint32,OpenViBEToolkit::TEncoder<CBoxAlgorithmMatlabScripting>*> m_mEncoders;
			std::map<OpenViBE::uint32,OpenViBE::boolean> m_mOutputHeaderState;


			void* m_pMatlabEngineHandle;
			OpenViBE::CString m_sBoxInstanceVariableName; //must be unique
			OpenViBE::boolean OpenMatlabEngineSafely(void);
			OpenViBE::boolean CloseMatlabEngineSafely(void);

			CMatlabHelper m_oMatlabHelper;

			OpenViBE::CString m_sMatlabPath;
			OpenViBE::CString m_sProcessFunction;
			OpenViBE::CString m_sInitializeFunction;
			OpenViBE::CString m_sUninitializeFunction;

			char* m_sMatlabBuffer;
			OpenViBE::boolean m_bErrorDetected;
			OpenViBE::boolean printOutputBufferWithFormat(void);

			//void sendOutputHeader(OpenViBE::CIdentifier idOutputType);
			//void sendOutputBuffer(OpenViBE::CIdentifier idOutputType);
		};

		class CBoxAlgorithmMatlabScriptingListener : public OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >
		{

		public:

			virtual OpenViBE::boolean onSettingTypeChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				//we must have the first STATIC_SETTINGS_COUNT settings
				if(ui32Index < 6)
				{
					this->getLogManager() << OpenViBE::Kernel::LogLevel_Warning << "One of the predefined setting has changed type, falling back to default.\n";
					rBox.setSettingType(ui32Index, OV_TypeId_String); // they are all strings
				}

				return true;
			}
			virtual OpenViBE::boolean onSettingValueChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				//we must have the first STATIC_SETTINGS_COUNT settings
				
				OpenViBE::CString l_sSettingValue;
				rBox.getSettingValue(0, l_sSettingValue);
				OpenViBE::uint64 l_ui64Value=::atoi(l_sSettingValue.toASCIIString());
				if(ui32Index == 0 && (l_ui64Value < 1 || l_ui64Value > 128))
				{
					this->getLogManager() << OpenViBE::Kernel::LogLevel_Warning << "Clock Frequency must be an integer between 1 and 128 Hz. Falling back to default.\n";
					rBox.setSettingValue(ui32Index, "64");
				}

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier);
		};

		class CBoxAlgorithmMatlabScriptingDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Matlab Scripting"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("L. Bonnet"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Executes matlab scripts. To be used correctly, you must have Matlab installed."); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("User must implement the matlab functions:\n[box_out]=bci_Initialize(box_in)\n[box_out]=bci_Process(box_in)\n[box_out]=bci_Uninitialize(box_in)\nPlease refer to the dedicated documentation <openvibe.inria.fr/tutorial-using-matlab-with-openvibe> for more information."); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Scripting"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-execute"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_MatlabScripting; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::Matlab::CBoxAlgorithmMatlabScripting; }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addSetting("Box clock frequency in Hz",   OV_TypeId_Integer, "64");
				
#if defined TARGET_OS_Linux
				rBoxAlgorithmPrototype.addSetting("Matlab launch command",      OV_TypeId_String, "[ssh user@host] /path/to/matlab");
#elif defined TARGET_OS_Windows
				rBoxAlgorithmPrototype.addSetting("Path to Matlab Executables", OV_TypeId_String, "C:/Program Files/MATLAB/R2011a/bin/win32");
#else
#endif
				rBoxAlgorithmPrototype.addSetting("Matlab working directory",   OV_TypeId_String, "[path-to-my-matlab-workspace]");
				rBoxAlgorithmPrototype.addSetting("Initialize function",        OV_TypeId_String, "matlab_Initialize");
				rBoxAlgorithmPrototype.addSetting("Process function",           OV_TypeId_String, "matlab_Process");
				rBoxAlgorithmPrototype.addSetting("Uninitialize function",      OV_TypeId_String, "matlab_Uninitialize");
				
				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddInput);
				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyInput);
				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddOutput);
				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyOutput);
				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddSetting);
				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifySetting);

				//rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_IsUnstable);

				return true;
			}
			
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener(void) const { return new CBoxAlgorithmMatlabScriptingListener; }

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_MatlabScriptingDesc);
		};
	};
};

#endif // TARGET_HAS_ThirdPartyMatlab

#endif // __OpenViBEPlugins_BoxAlgorithm_MatlabScripting_H__
