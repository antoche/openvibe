#ifndef __OpenViBEPlugins_SignalProcessing_CTemporalFilter_H__
#define __OpenViBEPlugins_SignalProcessing_CTemporalFilter_H__

#include "ovp_defines.h"

#include <openvibe-toolkit/ovtk_all.h>

#include <ebml/IReader.h>
#include <ebml/IReaderHelper.h>
#include <ebml/IWriter.h>
#include <ebml/IWriterHelper.h>

#include <ebml/TReaderCallbackProxy.h>
#include <ebml/TWriterCallbackProxy.h>

#include <vector>
#include <map>
#include <string>

//~ #if defined OVP_OS_Windows
 //~ #include <itpp/itsignal.h>
//~ #elif defined OVP_OS_Linux
 //~ #include <itpp/itbase.h>
//~ #else
//~ #endif
	#include <itpp/itstat.h>
	#include <itpp/itsignal.h>

#define PREC 27
#define MAXEXP 1024
#define MINEXP -1077
#define MAXNUM 1.79769313486231570815E308

typedef struct
{
	double real;
	double imag;
} cmplx;

namespace OpenViBEPlugins
{
	namespace SignalProcessingGpl
	{
		namespace TemporalFilter
		{
			// Used to store information about the signal stream
			class CSignalDescription
			{
				public:

					CSignalDescription(void)
						:m_ui32StreamVersion(1)
						,m_ui32SamplingRate(0)
						,m_ui32ChannelCount(0)
						,m_ui32SampleCount(0)
						,m_ui32CurrentChannel(0)
						,m_bReadyToSend(false)
					{
					}

				public:

					EBML::uint32 m_ui32StreamVersion;
					EBML::uint32 m_ui32SamplingRate;
					EBML::uint32 m_ui32ChannelCount;
					EBML::uint32 m_ui32SampleCount;
					std::vector<std::string> m_pChannelName;
					EBML::uint32 m_ui32CurrentChannel;

					OpenViBE::boolean m_bReadyToSend;
			};
		}

		/**
		* The Temporal Filter plugin's main class.
		*/
		class CTemporalFilter : virtual public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, virtual public OpenViBEToolkit::IBoxAlgorithmSignalInputReaderCallback::ICallback
		{
		public:

			CTemporalFilter(void);

			virtual void release(void);

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);

			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);

			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithm, OVP_ClassId_TemporalFilter)

		public:

			virtual void writeSignalOutput(const void* pBuffer, const EBML::uint64 ui64BufferSize);

			virtual void setChannelCount(const OpenViBE::uint32 ui32ChannelCount);
			virtual void setChannelName(const OpenViBE::uint32 ui32ChannelIndex, const char* sChannelName);
			virtual void setSampleCountPerBuffer(const OpenViBE::uint32 ui32SampleCountPerBuffer);
			virtual void setSamplingRate(const OpenViBE::uint32 ui32SamplingFrequency);
			virtual void setSampleBuffer(const OpenViBE::float64* pBuffer);

			// Functions for Butterworth and Chebychev filters
			void findSPlanePolesAndZeros(void);
			void convertSPlanePolesAndZerosToZPlane(void);
			// Functions for Complex arithmetic
			double absComplex(cmplx *z);
			void divComplex(cmplx *a, cmplx *b, cmplx *c);
			void sqrtComplex(cmplx *z, cmplx  *w);
			void addComplex(cmplx *a, cmplx *b,cmplx *c);
			void mulComplex(cmplx *a, cmplx *b,cmplx *c);
			void subComplex(cmplx *a, cmplx *b,cmplx *c);

		public:

			// Needed to read the input
			EBML::IReader* m_pReader;
			OpenViBEToolkit::IBoxAlgorithmSignalInputReaderCallback* m_pSignalReaderCallBack;

			// The current node identifier
			EBML::CIdentifier m_oCurrentIdentifier;

			//start time and end time of the last arrived chunk
			OpenViBE::uint64 m_ui64LastChunkStartTime;
			OpenViBE::uint64 m_ui64LastChunkEndTime;

			// Needed to write on the plugin output
			EBML::IWriter* m_pWriter;
			EBML::TWriterCallbackProxy1<OpenViBEPlugins::SignalProcessingGpl::CTemporalFilter> m_oSignalOutputWriterCallbackProxy;
			OpenViBEToolkit::IBoxAlgorithmSignalOutputWriter* m_pSignalOutputWriterHelper;

			//! Structure containing information about the signal stream
			TemporalFilter::CSignalDescription * m_pSignalDescription;
			//! True if the signal description has been sent
			OpenViBE::boolean m_bSignalDescriptionSent;

			//! Size of the matrix buffer (output signal)
			OpenViBE::uint64 m_ui64MatrixBufferSize;
			//! Output signal's matrix buffer
			EBML::float64* m_pMatrixBuffer;
			//! True if the matrix is ready to be sent
			OpenViBE::boolean m_bMatrixReadyToSend;

			OpenViBE::boolean m_bCoefComputed;

			OpenViBE::uint64 m_ui64FilterMethod;
			OpenViBE::uint64 m_ui64FilterType;

			OpenViBE::float64 m_float64LowPassBandEdge;
			OpenViBE::float64 m_float64HighPassBandEdge;
			OpenViBE::float64 m_float64PassBandRipple;
			OpenViBE::uint32 m_ui32FilterOrder;

			itpp::vec m_vecNumCoefFilter;
			itpp::vec m_vecDenomCoefFilter;
			std::vector<itpp::vec> m_oCurrentStates;

			OpenViBE::boolean m_bShouldCareOfHistory;

			OpenViBE::uint32 m_ui32NyquistFrequency;
			OpenViBE::float64 m_float64TanAng;
			OpenViBE::float64 m_float64CosGam;
			OpenViBE::float64 m_float64cbp;
			OpenViBE::float64 m_float64Scale;
			OpenViBE::uint32  m_ui32NbPoles;
			OpenViBE::uint32  m_ui32NbZeros;
			OpenViBE::uint32  m_ui32ArraySize;
			itpp::vec m_vecZs;
			OpenViBE::uint32  m_ui32zord;
			OpenViBE::float64 m_float64Phi;
			OpenViBE::float64 m_float64Rho;
			OpenViBE::float64 m_float64Eps;

		};

		class CTemporalFilterDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }
			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Temporal filter - GPL (no algorithm)"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Guillaume Gibert"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INSERM"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Applies a temporal filter to input signal"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Signal processing/Filtering"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("0.1"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_TemporalFilter; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SignalProcessingGpl::CTemporalFilter(); }

			virtual OpenViBE::boolean getBoxPrototype(OpenViBE::Kernel::IBoxProto& rPrototype) const
			{
				rPrototype.addInput  ("Input signal",            OV_TypeId_Signal);
				rPrototype.addOutput ("Filtered signal",         OV_TypeId_Signal);
				rPrototype.addSetting("Filter method",           OVP_TypeId_FilterMethod, "Butterworth");
				rPrototype.addSetting("Filter type",             OVP_TypeId_FilterType,   "Low pass");
				rPrototype.addSetting("Filter order",            OV_TypeId_Integer,       "4");
				rPrototype.addSetting("Low cut frequency (Hz)",  OV_TypeId_Float,         "29");
				rPrototype.addSetting("High cut frequency (Hz)", OV_TypeId_Float,         "40");
				rPrototype.addSetting("Pass band ripple (dB)",   OV_TypeId_Float,         "0.5");
				rPrototype.addFlag(OpenViBE::Kernel::BoxFlag_IsDeprecated);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_TemporalFilterDesc)
		};
	}
}

#endif // __SamplePlugin_CTemporalFilter_H__
