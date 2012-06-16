#ifndef __OpenViBE_AcquisitionServer_CDriverMitsarEEG202A_H__
#define __OpenViBE_AcquisitionServer_CDriverMitsarEEG202A_H__

#if defined TARGET_OS_Windows

#include "../ovasIDriver.h"
#include "../ovasCHeader.h"

namespace OpenViBEAcquisitionServer
{
	/**
	 * \class CDriverMitsarEEG202A
	 * \author Gelu Ionescu (GIPSA-lab)
	 * \date 26 April 2012
	 * \erief The CDriverMitsarEEG202A allows the acquisition server to acquire data from a Mitsar EEG 202A amplifier.
	 *
	 * submitted by Anton Andreev (GIPSA-lab)
	 */
	class CDriverMitsarEEG202A : public OpenViBEAcquisitionServer::IDriver
	{
	private:
		typedef enum
		{
			CHANNEL_NB      = 33,
			SAMPLING_RATE	= 500,
			SAMPLES_NB		= 33,
			STIMULATION_0	= 0,
			STIMULATION_128	= 128, 
			STIMULATION_64	= 64, 
			STIMULATION_192	= (STIMULATION_128 + STIMULATION_64), //both buttons pressed
		} misc_t;
	public:

		CDriverMitsarEEG202A(OpenViBEAcquisitionServer::IDriverContext& rDriverContext);//modif new Idriver
		virtual void release(void);
		virtual const char* getName(void);

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

		//??? virtual void processData (cf neXus)

	protected:

		OpenViBEAcquisitionServer::IDriverCallback*  m_pCallback;
		OpenViBEAcquisitionServer::CHeader           m_oHeader;

		OpenViBE::uint32                             m_ui32RefIndex;
		OpenViBE::boolean                            m_bEventAndBioChannelsState;

		OpenViBE::uint32                             m_ui32LastStimulation;
		OpenViBE::float32*                           m_pfl32StimulationChannel;
		std::vector<OpenViBE::uint32>                m_vStimulationIdentifier;
		std::vector<OpenViBE::uint32>                m_vStimulationDate;
	
		std::vector<OpenViBE::float32>               m_vInputSamples;
		std::vector<OpenViBE::float32>               m_vOutputSamples;

	};
};

#endif

#endif // __OpenViBE_AcquisitionServer_CDriverMitsarEEG202A_H__
