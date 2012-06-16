#ifndef __OpenViBEPlugins_SignalProcessing_CFirstDifferenceDetrending_H__
#define __OpenViBEPlugins_SignalProcessing_CFirstDifferenceDetrending_H__

#include "ovp_defines.h"

#include <openvibe-toolkit/ovtk_all.h>

#include <ebml/IReader.h>
#include <ebml/IReaderHelper.h>
#include <ebml/IWriter.h>
#include <ebml/IWriterHelper.h>

#include <ebml/TReaderCallbackProxy.h>
#include <ebml/TWriterCallbackProxy.h>

#include <string>
#include <vector>

namespace OpenViBEPlugins
{
  namespace SignalProcessing
    {

      /**
       * The class enabling to perform detrending using the first differencing method.
       * this means the new time series w_t, corresponding to the detrending of the time series x_t
       * will simply be such that w_t = x_t - x_{t-1}
       */
      class CFirstDifferenceDetrending : virtual public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
	{

	public:

	  CFirstDifferenceDetrending(void);

	  virtual void release(void);

	  virtual OpenViBE::boolean initialize(void);
	  virtual OpenViBE::boolean uninitialize(void);

	  virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);

	  virtual OpenViBE::boolean process(void);

	  _IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, OVP_ClassId_FirstDifferenceDetrending)

	    public:
	  void appendSignalOutputChunkData(const void* pBuffer,  EBML::uint64 ui64BufferSize)
	    {
	      appendOutputChunkData(0, pBuffer, ui64BufferSize);
	    }

	  virtual void setChannelCount(const OpenViBE::uint32 ui32ChannelCount);
	  virtual void setChannelName(const OpenViBE::uint32 ui32ChannelIndex, const char* sChannelName);
	  virtual void setSampleCountPerBuffer(const OpenViBE::uint32 ui32SampleCountPerBuffer);
	  virtual void setSamplingRate(const OpenViBE::uint32 ui32SamplingFrequency);
	  virtual void setSampleBuffer(const OpenViBE::float64* pBuffer);

	public:

	  // Needed to read the input
	  EBML::IReader* m_pSignalReader;
	  OpenViBEToolkit::IBoxAlgorithmSignalInputReaderCallback* m_pSignalReaderCallBack;
	  OpenViBEToolkit::IBoxAlgorithmSignalInputReaderCallback::TCallbackProxy1<OpenViBEPlugins::SignalProcessing::CFirstDifferenceDetrending> m_oSignalReaderCallBackProxy;

	  //start time and end time of the last arrived chunk
	  OpenViBE::uint64 m_ui64LastChunkStartTime;
	  OpenViBE::uint64 m_ui64LastChunkEndTime;

	  // Needed to write on the plugin output
	  EBML::IWriter* m_pSignalOutputWriter;
	  EBML::TWriterCallbackProxy1<OpenViBEPlugins::SignalProcessing::CFirstDifferenceDetrending> m_oSignalOutputWriterCallbackProxy;
	  OpenViBEToolkit::IBoxAlgorithmSignalOutputWriter* m_pSignalOutputWriterHelper;

	  // Last sample validity flag
	  OpenViBE::boolean m_bAreLastSamplesValid;

	  ///number of channels
	  OpenViBE::uint32 m_ui32ChannelCount;

	  ///number of samples per buffer
	  OpenViBE::uint32 m_ui32SamplePerBufferCount;

	  //the current input buffer being processed
	  const OpenViBE::float64* m_pInputBuffer;

	  //the current output buffer being generated
	  OpenViBE::float64* m_pOutputBuffer;

	  /**
	   *the last samples (one for each electrode) of the last buffer processed.
	   *they are needed to perform the detrending at the boundary of the current buffer
	   */
	  OpenViBE::float64* m_pLastSamples;

	};

      /**
       * Description of the channel selection plugin
       */
      class CFirstDifferenceDetrendingDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
	{
	public:

	  virtual void release(void) { }
	  virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("First difference detrending"); }
	  virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Fabien Lotte"); }
	  virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("IRISA/INSA Rennes"); }
	  virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Detrend the data using the first differencing method."); }
	  virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("This box algorithm enable to perform detrending using the first differencing method. This means the new time series w_t, corresponding to the detrending of the time series x_t, will simply be such that w_t = x_t - x_{t-1}"); }
	  virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Signal processing/Detrending"); }
	  virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("0.1"); }

	  virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_FirstDifferenceDetrending; }
	  virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SignalProcessing::CFirstDifferenceDetrending(); }

	  virtual OpenViBE::boolean getBoxPrototype(OpenViBE::Kernel::IBoxProto& rPrototype) const
		{
	      rPrototype.addInput("Input signal", OV_TypeId_Signal);
	      rPrototype.addOutput("Detrended signal", OV_TypeId_Signal);

		  rPrototype.addFlag(OpenViBE::Kernel::BoxFlag_IsDeprecated);
		  return true;
	    }

	  _IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_FirstDifferenceDetrendingDesc)
	    };
    }
}

#endif // __OpenViBEPlugins_SignalProcessing_CFirstDifferenceDetrending_H__

