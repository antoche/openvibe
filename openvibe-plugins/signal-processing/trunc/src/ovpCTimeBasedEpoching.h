#ifndef __SamplePlugin_CTimeBasedEpoching_H__
#define __SamplePlugin_CTimeBasedEpoching_H__

#include "ovp_defines.h"
#include <openvibe-toolkit/ovtk_all.h>
#include <ebml/TWriterCallbackProxy.h>
#include <vector>
#include <cstdio>

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CTimeBasedEpoching : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
			class COutputHandler;

		public:

			CTimeBasedEpoching(void);

			virtual void release(void);

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);

			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, OVP_ClassId_TimeBasedEpoching)

		public:

			virtual void setChannelCount(const OpenViBE::uint32 ui32ChannelCount);
			virtual void setChannelName(const OpenViBE::uint32 ui32ChannelIndex, const char* sChannelName);
			virtual void setSampleCountPerBuffer(const OpenViBE::uint32 ui32SampleCountPerBuffer);
			virtual void setSamplingRate(const OpenViBE::uint32 ui32SamplingFrequency);
			virtual void setSampleBuffer(const OpenViBE::float64* pBuffer);

		protected:

			std::vector<OpenViBEPlugins::SignalProcessing::CTimeBasedEpoching::COutputHandler*> m_vOutputHandler;
			OpenViBE::uint32 m_ui32InputSampleCountPerBuffer;
			OpenViBE::uint64 m_ui64LastStartTime;
			OpenViBE::uint64 m_ui64LastEndTime;

			EBML::IReader* m_pSignalInputReader;
			OpenViBEToolkit::IBoxAlgorithmSignalInputReaderCallback* m_pSignalInputReaderCallback;
			OpenViBEToolkit::IBoxAlgorithmSignalInputReaderCallback::TCallbackProxy1<CTimeBasedEpoching> m_oSignalInputReaderCallbackProxy;
		};

		class CTimeBasedEpochingListener : public OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >
		{
		public:

			OpenViBE::boolean check(OpenViBE::Kernel::IBox& rBox)
			{
				char l_sName[1024];
				OpenViBE::uint32 i;

				for(i=0; i<rBox.getOutputCount(); i++)
				{
					sprintf(l_sName, "Epoched signal %u", i+1);
					rBox.setOutputName(i, l_sName);
					rBox.setOutputType(i, OV_TypeId_Signal);
				}

				for(i=0; i<rBox.getOutputCount(); i++)
				{
					sprintf(l_sName, "Epoch %u duration (in sec)", i+1);
					rBox.setSettingName(i*2, l_sName);
					rBox.setSettingType(i*2, OV_TypeId_Float);

					sprintf(l_sName, "Epoch %u intervals (in sec)", i+1);
					rBox.setSettingName(i*2+1, l_sName);
					rBox.setSettingType(i*2+1, OV_TypeId_Float);
				}

				return true;
			}

			virtual OpenViBE::boolean onOutputRemoved(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				rBox.removeSetting(ui32Index*2);
				rBox.removeSetting(ui32Index*2);

				return this->check(rBox);
			};

			virtual OpenViBE::boolean onOutputAdded(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				rBox.addSetting("", OV_TypeId_Float, "1");
				rBox.addSetting("", OV_TypeId_Float, "0.5");

				return this->check(rBox);
			};

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier);
		};

		class CTimeBasedEpochingDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }
			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Time based epoching"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Signal processing/Epoching"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-cut"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_TimeBasedEpoching; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SignalProcessing::CTimeBasedEpoching(); }
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener(void) const               { return new CTimeBasedEpochingListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rPrototype) const
			{
				rPrototype.addInput  ("Input signal",               OV_TypeId_Signal);
				rPrototype.addOutput ("Epoched signal 1",           OV_TypeId_Signal);
				rPrototype.addSetting("Epoch 1 duration (in sec)",  OV_TypeId_Float,  "1");
				rPrototype.addSetting("Epoch 1 intervals (in sec)", OV_TypeId_Float,  "0.5");
				rPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_CanAddOutput);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_TimeBasedEpochingDesc)
		};
	};
};

#endif // __SamplePlugin_CTimeBasedEpoching_H__
