#ifndef __OpenViBEPlugins_BoxAlgorithm_VRPNAnalogClient_H__
#define __OpenViBEPlugins_BoxAlgorithm_VRPNAnalogClient_H__

#include "ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include <vector>
#include <deque>

#include <vrpn_Analog.h>

#define OVP_ClassId_BoxAlgorithm_VRPNAnalogClient                                        OpenViBE::CIdentifier(0x7CF4A95E, 0x7270D07B)
#define OVP_ClassId_BoxAlgorithm_VRPNAnalogClientDesc                                        OpenViBE::CIdentifier(0x77B2AE79, 0xFDC31871)

namespace OpenViBEPlugins
{
	namespace VRPN
	{
		class CBoxAlgorithmVRPNAnalogClient : public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::uint64 getClockFrequency(void);
			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processClock(OpenViBE::CMessageClock& rMessageClock);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_VRPNAnalogClient);

			void setAnalog(OpenViBE::uint32 ui32AnalogCount, const OpenViBE::float64* pAnalog);

		protected:

			OpenViBE::uint64 m_ui64LastChunkEndTime;
			OpenViBE::uint64 m_ui64ChunkDuration;
			OpenViBE::uint64 m_ui64SamplingRate;
			OpenViBE::uint32 m_ui32ChannelCount;
			OpenViBE::uint32 m_ui32SampleCountPerSentBlock;
			OpenViBE::uint32 m_ui32SampleIndex;

			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamEncoder;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > ip_pMatrix;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > ip_ui64SamplingRate;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMemoryBuffer* > op_pMemoryBuffer;

			OpenViBE::CString m_sPeripheralName;

			std::deque < std::vector < OpenViBE::float64 > > m_vAnalog;
			std::vector < OpenViBE::float64 > m_vSwapAnalog;

			vrpn_Analog_Remote* m_pVRPNAnalogRemote;
		};

		class CBoxAlgorithmVRPNAnalogClientDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Analog VRPN Client"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Connects to an external VRPN device and translate an analog information into OpenViBE signal"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("-"); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("VRPN"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-connect"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_VRPNAnalogClient; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::VRPN::CBoxAlgorithmVRPNAnalogClient; }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				// rBoxAlgorithmPrototype.addInput  ("input name", /* input type (OV_TypeId_Signal) */);
				rBoxAlgorithmPrototype.addOutput ("Output",                      OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addSetting("Peripheral name",             OV_TypeId_String, "openvibe-vrpn@localhost");
				rBoxAlgorithmPrototype.addSetting("Sampling Rate",               OV_TypeId_Integer, "512");
				rBoxAlgorithmPrototype.addSetting("Number of Channels",          OV_TypeId_Integer, "16");
				rBoxAlgorithmPrototype.addSetting("Sample Count per Sent Block", OV_TypeId_Integer, "32");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_VRPNAnalogClientDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_VRPNAnalogClient_H__
