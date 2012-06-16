#ifndef __OpenViBE_AcquisitionServer_CDriverGenericRawReader_H__
#define __OpenViBE_AcquisitionServer_CDriverGenericRawReader_H__

#include "../ovasIDriver.h"
#include "../ovasCHeader.h"

namespace OpenViBEAcquisitionServer
{
	class CDriverGenericRawReader : public OpenViBEAcquisitionServer::IDriver
	{
	public:

		enum
		{
			Endian_Little=0,
			Endian_Big=1,

			Format_UnsignedInteger8=0,
			Format_UnsignedInteger16=1,
			Format_UnsignedInteger32=2,
			Format_SignedInteger8=3,
			Format_SignedInteger16=4,
			Format_SignedInteger32=5,
			Format_Float32=6,
			Format_Float64=7,
		} EParameter;

		CDriverGenericRawReader(OpenViBEAcquisitionServer::IDriverContext& rDriverContext);
		virtual void release(void);

		virtual OpenViBE::boolean initialize(
			const OpenViBE::uint32 ui32SampleCountPerSentBlock,
			OpenViBEAcquisitionServer::IDriverCallback& rCallback);
		virtual OpenViBE::boolean uninitialize(void);

		virtual OpenViBE::boolean start(void);
		virtual OpenViBE::boolean stop(void);
		virtual OpenViBE::boolean loop(void);

		virtual const OpenViBEAcquisitionServer::IHeader* getHeader(void) { return &m_oHeader; }

	protected:

		virtual OpenViBE::boolean open(void)=0;
		virtual OpenViBE::boolean close(void)=0;
		virtual OpenViBE::boolean read(void)=0;

	protected:

		OpenViBEAcquisitionServer::IDriverCallback* m_pCallback;
		OpenViBEAcquisitionServer::CHeader m_oHeader;
		OpenViBE::uint32 m_ui32SampleCountPerSentBlock;
		OpenViBE::uint32 m_ui32SampleSize;
		OpenViBE::uint32 m_ui32SampleFormat;
		OpenViBE::uint32 m_ui32SampleEndian;
		OpenViBE::uint32 m_ui32HeaderSkip;
		OpenViBE::uint32 m_ui32FrameSkip;
		OpenViBE::uint32 m_ui32DataFrameSize;
		OpenViBE::boolean m_bLimitSpeed;
		OpenViBE::uint8* m_pDataFrame;
		OpenViBE::float32* m_pSample;
		OpenViBE::uint64 m_ui64StartTime;
		OpenViBE::uint64 m_ui64TotalSampleCount;
	};
};

#endif // __OpenViBE_AcquisitionServer_CDriverGenericRawReader_H__
