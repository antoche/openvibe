#ifndef __SamplePlugin_CLog_H__
#define __SamplePlugin_CLog_H__

#include "ovp_defines.h"

#include <openvibe-toolkit/ovtk_all.h>

namespace OpenViBEPlugins
{
	namespace Samples
	{
		class CLog : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:

			virtual void release(void);
			virtual OpenViBE::uint64 getClockFrequency(void) { return 1LL<<32; }
			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processClock(OpenViBE::Kernel::IMessageClock& rMessageClock);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithm, OVP_ClassId_Log)
		};

		class CLogListener : public OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >
		{
		public:

			CLogListener(void) : m_eLogLevel(OpenViBE::Kernel::LogLevel_Trace) {}
			virtual OpenViBE::boolean initialize(void)                                                                                  { this->getLogManager() << m_eLogLevel << "initialize\n";                        return true; };
			virtual OpenViBE::boolean uninitialize(void)                                                                                { this->getLogManager() << m_eLogLevel << "uninitialize\n";                      return true; };
			virtual OpenViBE::boolean onInitialized(OpenViBE::Kernel::IBox& rBox)                                                       { this->getLogManager() << m_eLogLevel << "onInitialized\n";                     return true; };
			virtual OpenViBE::boolean onNameChanged(OpenViBE::Kernel::IBox& rBox)                                                       { this->getLogManager() << m_eLogLevel << "onNameChanged\n";                     return true; };
			virtual OpenViBE::boolean onIdentifierChanged(OpenViBE::Kernel::IBox& rBox)                                                 { this->getLogManager() << m_eLogLevel << "onIdentifierChanged\n";               return true; };
			virtual OpenViBE::boolean onAlgorithmClassIdentifierChanged(OpenViBE::Kernel::IBox& rBox)                                   { this->getLogManager() << m_eLogLevel << "onAlgorithmClassIdentifierChanged\n"; return true; };
			virtual OpenViBE::boolean onProcessingUnitChangedon(OpenViBE::Kernel::IBox& rBox)                                           { this->getLogManager() << m_eLogLevel << "onProcessingUnitChangedon\n";         return true; };
			virtual OpenViBE::boolean onInputConnected(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)                  { this->getLogManager() << m_eLogLevel << "onInputConnected\n";                  return true; };
			virtual OpenViBE::boolean onInputDisconnected(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)               { this->getLogManager() << m_eLogLevel << "onInputDisconnected\n";               return true; };
			virtual OpenViBE::boolean onInputAdded(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)                      { this->getLogManager() << m_eLogLevel << "onInputAdded\n";                      return true; };
			virtual OpenViBE::boolean onInputRemoved(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)                    { this->getLogManager() << m_eLogLevel << "onInputRemoved\n";                    return true; };
			virtual OpenViBE::boolean onInputTypeChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)                { this->getLogManager() << m_eLogLevel << "onInputTypeChanged\n";                return true; };
			virtual OpenViBE::boolean onInputNameChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)                { this->getLogManager() << m_eLogLevel << "onInputNameChanged\n";                return true; };
			virtual OpenViBE::boolean onOutputConnected(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)                 { this->getLogManager() << m_eLogLevel << "onOutputConnected\n";                 return true; };
			virtual OpenViBE::boolean onOutputDisconnected(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)              { this->getLogManager() << m_eLogLevel << "onOutputDisconnected\n";              return true; };
			virtual OpenViBE::boolean onOutputAdded(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)                     { this->getLogManager() << m_eLogLevel << "onOutputAdded\n";                     return true; };
			virtual OpenViBE::boolean onOutputRemoved(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)                   { this->getLogManager() << m_eLogLevel << "onOutputRemoved\n";                   return true; };
			virtual OpenViBE::boolean onOutputTypeChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)               { this->getLogManager() << m_eLogLevel << "onOutputTypeChanged\n";               return true; };
			virtual OpenViBE::boolean onOutputNameChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)               { this->getLogManager() << m_eLogLevel << "onOutputNameChanged\n";               return true; };
			virtual OpenViBE::boolean onSettingAdded(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)                    { this->getLogManager() << m_eLogLevel << "onSettingAdded\n";                    return true; };
			virtual OpenViBE::boolean onSettingRemoved(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)                  { this->getLogManager() << m_eLogLevel << "onSettingRemoved\n";                  return true; };
			virtual OpenViBE::boolean onSettingTypeChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)              { this->getLogManager() << m_eLogLevel << "onSettingTypeChanged\n";              return true; };
			virtual OpenViBE::boolean onSettingNameChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)              { this->getLogManager() << m_eLogLevel << "onSettingNameChanged\n";              return true; };
			virtual OpenViBE::boolean onSettingDefaultValueChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)      { this->getLogManager() << m_eLogLevel << "onSettingDefaultValueChanged\n";      return true; };
			virtual OpenViBE::boolean onSettingValueChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)             { this->getLogManager() << m_eLogLevel << "onSettingValueChanged\n";             return true; };

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier);

		protected:

			OpenViBE::Kernel::ELogLevel m_eLogLevel;
		};

		class CLogDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }
			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Log"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Logs stuffs (init, uninit, input, clock, process)"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("This sample box shows how stuffs could be logged in the log manager"); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Samples"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_Log; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::Samples::CLog(); }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rPrototype) const
			{
				// Adds box inputs
				rPrototype.addInput("Input stream", OV_UndefinedIdentifier);

				// Adds box outputs

				// Adds box settings
				rPrototype.addSetting("Enumeration", OpenViBE::CIdentifier(0x1234, 0x0000), "");
				rPrototype.addSetting("Bit mask", OpenViBE::CIdentifier(0x0000, 0x5678), "");

				rPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddInput);
				rPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyInput);
				rPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddOutput);
				rPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyOutput);
				rPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddSetting);
				rPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifySetting);

				return true;
			}

			virtual OpenViBE::CString getStockItemName(void) const
			{
				return OpenViBE::CString("gtk-edit");
			}

			virtual OpenViBE::Plugins::IBoxListener* createBoxListener(void) const { return new CLogListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) { delete pBoxListener; }

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_LogDesc)
		};
	};
};

#endif // __SamplePlugin_CLog_H__
