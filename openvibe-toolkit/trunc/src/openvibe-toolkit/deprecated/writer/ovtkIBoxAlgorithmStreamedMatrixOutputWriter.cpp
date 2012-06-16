#include "ovtkIBoxAlgorithmStreamedMatrixOutputWriter.h"
#include "ovtkCBoxAlgorithmStreamedMatrixOutputWriterPart.inl"
#include "ovtkCBoxAlgorithmEBMLOutputWriterPart.inl"

namespace OpenViBEToolkit
{
	class CBoxAlgorithmStreamedMatrixOutputWriter : public OpenViBEToolkit::IBoxAlgorithmStreamedMatrixOutputWriter
	{
	public:

		CBoxAlgorithmStreamedMatrixOutputWriter(void);

		virtual OpenViBE::boolean setDimmensionCount(const OpenViBE::uint32 ui32DimmensionCount);
		virtual OpenViBE::boolean setDimmensionSize(const OpenViBE::uint32 ui32DimmensionIndex, const OpenViBE::uint32 ui32DimmensionSize);
		virtual OpenViBE::boolean setDimmensionLabel(const OpenViBE::uint32 ui32DimmensionIndex, const OpenViBE::uint32 ui32DimmensionEntryIndex, const char* sDimmensionLabel);
		virtual OpenViBE::boolean setBuffer(const OpenViBE::float64* pBuffer);

		virtual OpenViBE::boolean writeHeader(EBML::IWriter& rWriter);
		virtual OpenViBE::boolean writeBuffer(EBML::IWriter& rWriter);
		virtual OpenViBE::boolean writeEnd(EBML::IWriter& rWriter);

		_IsDerivedFromClass_Final_(OpenViBEToolkit::IBoxAlgorithmStreamedMatrixOutputWriter, OVTK_ClassId_);

	protected:

		OpenViBEToolkit::CBoxAlgorithmEBMLOutputWriterPart m_oEBMLOutputWriterPart;
		OpenViBEToolkit::CBoxAlgorithmStreamedMatrixOutputWriterPart m_oStreamedMatrixOutputWriterPart;
	};

	CBoxAlgorithmStreamedMatrixOutputWriter::CBoxAlgorithmStreamedMatrixOutputWriter(void)
	{
		m_oEBMLOutputWriterPart.registerBoxAlgorithmEBMLOutputWriterPart(m_oStreamedMatrixOutputWriterPart);
	}

	OpenViBE::boolean CBoxAlgorithmStreamedMatrixOutputWriter::setDimmensionCount(const OpenViBE::uint32 ui32DimmensionCount)
	{
		return m_oStreamedMatrixOutputWriterPart.setDimmensionCount(ui32DimmensionCount);
	}

	OpenViBE::boolean CBoxAlgorithmStreamedMatrixOutputWriter::setDimmensionSize(const OpenViBE::uint32 ui32DimmensionIndex, const OpenViBE::uint32 ui32DimmensionSize)
	{
		return m_oStreamedMatrixOutputWriterPart.setDimmensionSize(ui32DimmensionIndex, ui32DimmensionSize);
	}

	OpenViBE::boolean CBoxAlgorithmStreamedMatrixOutputWriter::setDimmensionLabel(const OpenViBE::uint32 ui32DimmensionIndex, const OpenViBE::uint32 ui32DimmensionEntryIndex, const char* sDimmensionLabel)
	{
		return m_oStreamedMatrixOutputWriterPart.setDimmensionLabel(ui32DimmensionIndex, ui32DimmensionEntryIndex, sDimmensionLabel);
	}

	OpenViBE::boolean CBoxAlgorithmStreamedMatrixOutputWriter::setBuffer(const OpenViBE::float64* pBuffer)
	{
		return m_oStreamedMatrixOutputWriterPart.setBuffer(pBuffer);
	}

	OpenViBE::boolean CBoxAlgorithmStreamedMatrixOutputWriter::writeHeader(EBML::IWriter& rWriter)
	{
		return m_oEBMLOutputWriterPart.writeHeader(rWriter);
	}

	OpenViBE::boolean CBoxAlgorithmStreamedMatrixOutputWriter::writeBuffer(EBML::IWriter& rWriter)
	{
		return m_oEBMLOutputWriterPart.writeBuffer(rWriter);
	}

	OpenViBE::boolean CBoxAlgorithmStreamedMatrixOutputWriter::writeEnd(EBML::IWriter& rWriter)
	{
		return m_oEBMLOutputWriterPart.writeEnd(rWriter);
	}

	OpenViBEToolkit::IBoxAlgorithmStreamedMatrixOutputWriter* createBoxAlgorithmStreamedMatrixOutputWriter(void)
	{
		return new OpenViBEToolkit::CBoxAlgorithmStreamedMatrixOutputWriter();
	}

	void releaseBoxAlgorithmStreamedMatrixOutputWriter(OpenViBEToolkit::IBoxAlgorithmStreamedMatrixOutputWriter* pOutputWriter)
	{
		delete pOutputWriter;
	}
};
