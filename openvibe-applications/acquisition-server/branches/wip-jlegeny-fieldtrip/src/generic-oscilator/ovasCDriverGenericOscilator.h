#ifndef __OpenViBE_AcquisitionServer_CDriverGenericOscillator_H__
#define __OpenViBE_AcquisitionServer_CDriverGenericOscillator_H__

#include "../ovasIDriver.h"
#include "../ovasCHeader.h"

namespace OpenViBEAcquisitionServer
{
	class CDriverGenericOscillator : public OpenViBEAcquisitionServer::IDriver
	{
	public:

		CDriverGenericOscillator(OpenViBEAcquisitionServer::IDriverContext& rDriverContext);
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

	protected:

		OpenViBEAcquisitionServer::IDriverCallback* m_pCallback;
		OpenViBEAcquisitionServer::CHeader m_oHeader;

		OpenViBE::uint32 m_ui32SampleCountPerSentBlock;
		OpenViBE::float32* m_pSample;

		OpenViBE::uint32 m_ui32TotalSampleCount;
		OpenViBE::uint32 m_ui32StartTime;
	};
};

#endif // __OpenViBE_AcquisitionServer_CDriverGenericOscillator_H__
