#include "ovtkIBoxAlgorithmSpectrumOutputWriter.h"
#include "ovtkCBoxAlgorithmSpectrumOutputWriterPart.inl"
#include "ovtkCBoxAlgorithmStreamedMatrixOutputWriterPart.inl"
#include "ovtkCBoxAlgorithmEBMLOutputWriterPart.inl"

namespace OpenViBEToolkit
{
	class CBoxAlgorithmSpectrumOutputWriter : public OpenViBEToolkit::IBoxAlgorithmSpectrumOutputWriter
	{
	public:

		CBoxAlgorithmSpectrumOutputWriter(void);

		virtual OpenViBE::boolean setChannelCount(const OpenViBE::uint32 ui32ChannelCount);
		virtual OpenViBE::boolean setChannelName(const OpenViBE::uint32 ui32ChannelIndex, const char* sChannelName);
		virtual OpenViBE::boolean setFrequencyBandCount(const OpenViBE::uint32 ui32FrequencyBandCount);
		virtual OpenViBE::boolean setFrequencyBandName(const OpenViBE::uint32 ui32FrequencyBandIndex, const char* sFrequencyBandName);
		virtual OpenViBE::boolean setFrequencyBandStart(const OpenViBE::uint32 ui32FrequencyBandIndex, const OpenViBE::float64 f64FrequencyBandStart);
		virtual OpenViBE::boolean setFrequencyBandStop(const OpenViBE::uint32 ui32FrequencyBandIndex, const OpenViBE::float64 f64FrequencyBandStop);
		virtual OpenViBE::boolean setBuffer(const OpenViBE::float64* pBuffer);

		virtual OpenViBE::boolean writeHeader(EBML::IWriter& rWriter);
		virtual OpenViBE::boolean writeBuffer(EBML::IWriter& rWriter);
		virtual OpenViBE::boolean writeEnd(EBML::IWriter& rWriter);

		_IsDerivedFromClass_Final_(OpenViBEToolkit::IBoxAlgorithmSpectrumOutputWriter, OVTK_ClassId_);

	protected:

		OpenViBEToolkit::CBoxAlgorithmEBMLOutputWriterPart m_oEBMLOutputWriterPart;
		OpenViBEToolkit::CBoxAlgorithmStreamedMatrixOutputWriterPart m_oStreamedMatrixOutputWriterPart;
		OpenViBEToolkit::CBoxAlgorithmSpectrumOutputWriterPart m_oSpectrumOutputWriterPart;
	};

	CBoxAlgorithmSpectrumOutputWriter::CBoxAlgorithmSpectrumOutputWriter(void)
	{
		m_oEBMLOutputWriterPart.registerBoxAlgorithmEBMLOutputWriterPart(m_oStreamedMatrixOutputWriterPart);
		m_oEBMLOutputWriterPart.registerBoxAlgorithmEBMLOutputWriterPart(m_oSpectrumOutputWriterPart);

		m_oStreamedMatrixOutputWriterPart.setDimmensionCount(2);
	}

	OpenViBE::boolean CBoxAlgorithmSpectrumOutputWriter::setChannelCount(const OpenViBE::uint32 ui32ChannelCount)
	{
		return m_oStreamedMatrixOutputWriterPart.setDimmensionSize(0, ui32ChannelCount);
	}

	OpenViBE::boolean CBoxAlgorithmSpectrumOutputWriter::setChannelName(const OpenViBE::uint32 ui32ChannelIndex, const char* sChannelName)
	{
		return m_oStreamedMatrixOutputWriterPart.setDimmensionLabel(0, ui32ChannelIndex, sChannelName);
	}

	OpenViBE::boolean CBoxAlgorithmSpectrumOutputWriter::setFrequencyBandCount(const OpenViBE::uint32 ui32FrequencyBandCount)
	{
		return m_oStreamedMatrixOutputWriterPart.setDimmensionSize(1, ui32FrequencyBandCount) && m_oSpectrumOutputWriterPart.setFrequencyBandCount(ui32FrequencyBandCount);
	}

	OpenViBE::boolean CBoxAlgorithmSpectrumOutputWriter::setFrequencyBandName(const OpenViBE::uint32 ui32FrequencyBandIndex, const char* sFrequencyBandName)
	{
		return m_oStreamedMatrixOutputWriterPart.setDimmensionLabel(1, ui32FrequencyBandIndex, sFrequencyBandName);
	}

	OpenViBE::boolean CBoxAlgorithmSpectrumOutputWriter::setFrequencyBandStart(const OpenViBE::uint32 ui32FrequencyBandIndex, const OpenViBE::float64 f64FrequencyBandStart)
	{
		return m_oSpectrumOutputWriterPart.setFrequencyBandStart(ui32FrequencyBandIndex, f64FrequencyBandStart);
	}

	OpenViBE::boolean CBoxAlgorithmSpectrumOutputWriter::setFrequencyBandStop(const OpenViBE::uint32 ui32FrequencyBandIndex, const OpenViBE::float64 f64FrequencyBandStop)
	{
		return m_oSpectrumOutputWriterPart.setFrequencyBandStop(ui32FrequencyBandIndex, f64FrequencyBandStop);
	}

	OpenViBE::boolean CBoxAlgorithmSpectrumOutputWriter::setBuffer(const OpenViBE::float64* pBuffer)
	{
		return m_oStreamedMatrixOutputWriterPart.setBuffer(pBuffer);
	}

	OpenViBE::boolean CBoxAlgorithmSpectrumOutputWriter::writeHeader(EBML::IWriter& rWriter)
	{
		return m_oEBMLOutputWriterPart.writeHeader(rWriter);
	}

	OpenViBE::boolean CBoxAlgorithmSpectrumOutputWriter::writeBuffer(EBML::IWriter& rWriter)
	{
		return m_oEBMLOutputWriterPart.writeBuffer(rWriter);
	}

	OpenViBE::boolean CBoxAlgorithmSpectrumOutputWriter::writeEnd(EBML::IWriter& rWriter)
	{
		return m_oEBMLOutputWriterPart.writeEnd(rWriter);
	}

	OpenViBEToolkit::IBoxAlgorithmSpectrumOutputWriter* createBoxAlgorithmSpectrumOutputWriter(void)
	{
		return new OpenViBEToolkit::CBoxAlgorithmSpectrumOutputWriter();
	}

	void releaseBoxAlgorithmSpectrumOutputWriter(OpenViBEToolkit::IBoxAlgorithmSpectrumOutputWriter* pOutputWriter)
	{
		delete pOutputWriter;
	}
};
