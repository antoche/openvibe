#include "ovtkIBoxAlgorithmSignalOutputWriter.h"
#include "ovtkCBoxAlgorithmSignalOutputWriterPart.inl"
#include "ovtkCBoxAlgorithmStreamedMatrixOutputWriterPart.inl"
#include "ovtkCBoxAlgorithmEBMLOutputWriterPart.inl"

namespace OpenViBEToolkit
{
	class CBoxAlgorithmSignalOutputWriter : public OpenViBEToolkit::IBoxAlgorithmSignalOutputWriter
	{
	public:

		CBoxAlgorithmSignalOutputWriter(void);

		virtual OpenViBE::boolean setSamplingRate(const OpenViBE::uint32 ui32SamplingRate);
		virtual OpenViBE::boolean setChannelCount(const OpenViBE::uint32 ui32ChannelCount);
		virtual OpenViBE::boolean setChannelName(const OpenViBE::uint32 ui32ChannelIndex, const char* sChannelName);
		virtual OpenViBE::boolean setSampleCountPerBuffer(const OpenViBE::uint32 ui32SampleCountPerBuffer);
		virtual OpenViBE::boolean setSampleBuffer(const OpenViBE::float64* pSampleBuffer);

		virtual OpenViBE::boolean writeHeader(EBML::IWriter& rWriter);
		virtual OpenViBE::boolean writeBuffer(EBML::IWriter& rWriter);
		virtual OpenViBE::boolean writeEnd(EBML::IWriter& rWriter);

		_IsDerivedFromClass_Final_(OpenViBEToolkit::IBoxAlgorithmSignalOutputWriter, OVTK_ClassId_);

	protected:

		OpenViBEToolkit::CBoxAlgorithmEBMLOutputWriterPart m_oEBMLOutputWriterPart;
		OpenViBEToolkit::CBoxAlgorithmStreamedMatrixOutputWriterPart m_oStreamedMatrixOutputWriterPart;
		OpenViBEToolkit::CBoxAlgorithmSignalOutputWriterPart m_oSignalOutputWriterPart;
	};

	CBoxAlgorithmSignalOutputWriter::CBoxAlgorithmSignalOutputWriter(void)
	{
		m_oEBMLOutputWriterPart.registerBoxAlgorithmEBMLOutputWriterPart(m_oSignalOutputWriterPart);
		m_oEBMLOutputWriterPart.registerBoxAlgorithmEBMLOutputWriterPart(m_oStreamedMatrixOutputWriterPart);

		m_oStreamedMatrixOutputWriterPart.setDimmensionCount(2);
	}

	OpenViBE::boolean CBoxAlgorithmSignalOutputWriter::setSamplingRate(const OpenViBE::uint32 ui32SamplingRate)
	{
		return m_oSignalOutputWriterPart.setSamplingRate(ui32SamplingRate);
	}

	OpenViBE::boolean CBoxAlgorithmSignalOutputWriter::setChannelCount(const OpenViBE::uint32 ui32ChannelCount)
	{
		return m_oStreamedMatrixOutputWriterPart.setDimmensionSize(0, ui32ChannelCount);
	}

	OpenViBE::boolean CBoxAlgorithmSignalOutputWriter::setChannelName(const OpenViBE::uint32 ui32ChannelIndex, const char* sChannelName)
	{
		return m_oStreamedMatrixOutputWriterPart.setDimmensionLabel(0, ui32ChannelIndex, sChannelName);
	}

	OpenViBE::boolean CBoxAlgorithmSignalOutputWriter::setSampleCountPerBuffer(const OpenViBE::uint32 ui32SampleCountPerBuffer)
	{
		return m_oStreamedMatrixOutputWriterPart.setDimmensionSize(1, ui32SampleCountPerBuffer);
	}

	OpenViBE::boolean CBoxAlgorithmSignalOutputWriter::setSampleBuffer(const OpenViBE::float64* pSampleBuffer)
	{
		return m_oStreamedMatrixOutputWriterPart.setBuffer(pSampleBuffer);
	}

	OpenViBE::boolean CBoxAlgorithmSignalOutputWriter::writeHeader(EBML::IWriter& rWriter)
	{
		return m_oEBMLOutputWriterPart.writeHeader(rWriter);
	}

	OpenViBE::boolean CBoxAlgorithmSignalOutputWriter::writeBuffer(EBML::IWriter& rWriter)
	{
		return m_oEBMLOutputWriterPart.writeBuffer(rWriter);
	}

	OpenViBE::boolean CBoxAlgorithmSignalOutputWriter::writeEnd(EBML::IWriter& rWriter)
	{
		return m_oEBMLOutputWriterPart.writeEnd(rWriter);
	}

	OpenViBEToolkit::IBoxAlgorithmSignalOutputWriter* createBoxAlgorithmSignalOutputWriter(void)
	{
		return new OpenViBEToolkit::CBoxAlgorithmSignalOutputWriter();
	}

	void releaseBoxAlgorithmSignalOutputWriter(OpenViBEToolkit::IBoxAlgorithmSignalOutputWriter* pOutputWriter)
	{
		delete pOutputWriter;
	}
};
