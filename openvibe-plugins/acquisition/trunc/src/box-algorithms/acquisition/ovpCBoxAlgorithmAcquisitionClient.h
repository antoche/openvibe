#ifndef __OpenViBEPlugins_BoxAlgorithm_AcquisitionClient_H__
#define __OpenViBEPlugins_BoxAlgorithm_AcquisitionClient_H__

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include <socket/IConnectionClient.h>

namespace OpenViBEPlugins
{
	namespace Acquisition
	{
		class CBoxAlgorithmAcquisitionClient : public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::uint64 getClockFrequency(void);
			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processClock(OpenViBE::CMessageClock& rMessageClock);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_AcquisitionClient);

		protected:

			OpenViBE::Kernel::IAlgorithmProxy* m_pAcquisitionStreamDecoder;

			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMemoryBuffer* > ip_pAcquisitionMemoryBuffer;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > op_ui64BufferDuration;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMemoryBuffer* > op_pExperimentInformationMemoryBuffer;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMemoryBuffer* > op_pSignalMemoryBuffer;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMemoryBuffer* > op_pStimulationMemoryBuffer;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMemoryBuffer* > op_pChannelLocalisationMemoryBuffer;

			Socket::IConnectionClient* m_pConnectionClient;

			OpenViBE::uint64 m_ui64LastChunkStartTime;
			OpenViBE::uint64 m_ui64LastChunkEndTime;
		};

		class CBoxAlgorithmAcquisitionClientDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Acquisition client"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("A generic network based acquisition client"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("This algorithm waits for EEG data from the network and distributes it into the scenario"); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Acquisition"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_AcquisitionClient; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::Acquisition::CBoxAlgorithmAcquisitionClient; }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-connect"); }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addOutput ("Experiment information",      OV_TypeId_ExperimentationInformation);
				rBoxAlgorithmPrototype.addOutput ("Signal strea",                OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addOutput ("Stimulations",                OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addOutput ("Channel localisation",        OV_TypeId_ChannelLocalisation);
				rBoxAlgorithmPrototype.addSetting("Acquisition server hostname", OV_TypeId_String,  "${AcquisitionServer_HostName}");
				rBoxAlgorithmPrototype.addSetting("Acquisition server port",     OV_TypeId_Integer, "1024");
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_AcquisitionClientDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_AcquisitionClient_H__
