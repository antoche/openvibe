#ifndef __OpenViBEPlugins_BoxAlgorithm_MeanVariance_H__
#define __OpenViBEPlugins_BoxAlgorithm_MeanVariance_H__

//#include "../ovp_defines.h"
#include "ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>
#include <vector>

// TODO:
// - please move the identifier definitions in ovp_defines.h
// - please include your desciptor in ovp_main.cpp

namespace OpenViBEPlugins
{
	namespace Samples
	{
		class CBoxAlgorithmMeanVariance : public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::uint64 getClockFrequency(void);
			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processEvent(OpenViBE::CMessageEvent& rMessageEvent);
			virtual OpenViBE::boolean processSignal(OpenViBE::CMessageSignal& rMessageSignal);
			virtual OpenViBE::boolean processClock(OpenViBE::CMessageClock& rMessageClock);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_MeanVariance);

		protected:

			OpenViBE::CString m_sDSPConfigurationFilename;
			OpenViBE::CString m_sMinMaxConfigurationFilename;
			OpenViBE::uint32 m_ui32StandardDeviationFactor;

			//variables for incremental estimation of mean and variance
			std::vector<OpenViBE::float64> m_f64S1List; //sum of input data for each channel
			std::vector<OpenViBE::float64> m_f64S2List; //sum of square of input  for each channel
			OpenViBE::uint64 m_ui64N; //number of data received per channel

			//algorithms for encoding and decoding EBML stream
			OpenViBE::Kernel::IAlgorithmProxy* m_pEncoderOutput1; //mean of each channel
			OpenViBE::Kernel::IAlgorithmProxy* m_pEncoderOutput2; //variance of each channel
			OpenViBE::Kernel::IAlgorithmProxy* m_pDecoderInput1;

			//input and output buffers
			OpenViBE::Kernel::TParameterHandler < const OpenViBE::IMemoryBuffer* > m_oEBMLMemoryBufferHandleInput1;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMemoryBuffer* > m_oEBMLMemoryBufferHandleOutput1;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMemoryBuffer* > m_oEBMLMemoryBufferHandleOutput2;

			//the signal matrices
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > m_oMatrixInput1Handle;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > m_oMatrixOutput1Handle;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > m_oMatrixOutput2Handle;

			//start and end times
			OpenViBE::uint64 m_ui64StartTime;
			OpenViBE::uint64 m_ui64EndTime;

			//number of channels and number of samples per buffer
			OpenViBE::uint32 m_ui32NbChannels;
			OpenViBE::uint32 m_ui32NbSamplesPerBuffer;

			//activity start and end times
			OpenViBE::uint64 m_ui64ActivityStartTime;
			OpenViBE::uint64 m_ui64ActivityEndTime;
			OpenViBE::boolean m_bFinished;
		};

		class CBoxAlgorithmMeanVarianceDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Online mean-variance"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Fabien Lotte"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("IRISA-INSA Rennes"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("This box computes, in an incremental way, the mean and variance of the upcoming signal for each channel"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("not better"); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Signal processing/Statistics"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("3.141592653589793238462"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_MeanVariance; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::Samples::CBoxAlgorithmMeanVariance; }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput("Input signal", OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addOutput("Output mean", OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addOutput("Output variance", OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addSetting("DSP configuration filename", OV_TypeId_Filename, "");
				rBoxAlgorithmPrototype.addSetting("Min max configuration filename", OV_TypeId_Filename, "");
				rBoxAlgorithmPrototype.addSetting("Start time (in sec)", OV_TypeId_Float, "5");
				rBoxAlgorithmPrototype.addSetting("Stop time (in sec)", OV_TypeId_Float, "25");
				rBoxAlgorithmPrototype.addSetting("Standard Deviation Factor", OV_TypeId_Float, "3");
				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_IsUnstable);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_MeanVarianceDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_MeanVariance_H__
