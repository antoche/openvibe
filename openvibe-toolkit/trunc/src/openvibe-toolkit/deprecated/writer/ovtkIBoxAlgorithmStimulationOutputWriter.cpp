#include "ovtkIBoxAlgorithmStimulationOutputWriter.h"
#include "ovtkCBoxAlgorithmStimulationOutputWriterPart.inl"
#include "ovtkCBoxAlgorithmEBMLOutputWriterPart.inl"

namespace OpenViBEToolkit
{
	class CBoxAlgorithmStimulationOutputWriter : public OpenViBEToolkit::IBoxAlgorithmStimulationOutputWriter
	{
	public:

		CBoxAlgorithmStimulationOutputWriter(void);

		virtual OpenViBE::boolean setStimulationCount(const OpenViBE::uint32 ui32StimulationCount);
		virtual OpenViBE::boolean setStimulation(const OpenViBE::uint32 ui32StimulationIndex, const OpenViBE::uint64 ui64StimulationIdentifier, const OpenViBE::uint64 ui64StimulationDate);

		virtual OpenViBE::boolean writeHeader(EBML::IWriter& rWriter);
		virtual OpenViBE::boolean writeBuffer(EBML::IWriter& rWriter);
		virtual OpenViBE::boolean writeEnd(EBML::IWriter& rWriter);

		_IsDerivedFromClass_Final_(OpenViBEToolkit::IBoxAlgorithmStimulationOutputWriter, OVTK_ClassId_);

	protected:

		OpenViBEToolkit::CBoxAlgorithmEBMLOutputWriterPart m_oEBMLOutputWriterPart;
		OpenViBEToolkit::CBoxAlgorithmStimulationOutputWriterPart m_oStimulationOutputWriterPart;
	};

	CBoxAlgorithmStimulationOutputWriter::CBoxAlgorithmStimulationOutputWriter(void)
	{
		m_oEBMLOutputWriterPart.registerBoxAlgorithmEBMLOutputWriterPart(m_oStimulationOutputWriterPart);
	}

	OpenViBE::boolean CBoxAlgorithmStimulationOutputWriter::setStimulationCount(const OpenViBE::uint32 ui32StimulationCount)
	{
		return m_oStimulationOutputWriterPart.setStimulationCount(ui32StimulationCount);
	}

	OpenViBE::boolean CBoxAlgorithmStimulationOutputWriter::setStimulation(const OpenViBE::uint32 ui32StimulationIndex, const OpenViBE::uint64 ui64StimulationIdentifier, const OpenViBE::uint64 ui64StimulationDate)
	{
		return m_oStimulationOutputWriterPart.setStimulation(ui32StimulationIndex, ui64StimulationIdentifier, ui64StimulationDate);
	}

	OpenViBE::boolean CBoxAlgorithmStimulationOutputWriter::writeHeader(EBML::IWriter& rWriter)
	{
		return m_oEBMLOutputWriterPart.writeHeader(rWriter);
	}

	OpenViBE::boolean CBoxAlgorithmStimulationOutputWriter::writeBuffer(EBML::IWriter& rWriter)
	{
		return m_oEBMLOutputWriterPart.writeBuffer(rWriter);
	}

	OpenViBE::boolean CBoxAlgorithmStimulationOutputWriter::writeEnd(EBML::IWriter& rWriter)
	{
		return m_oEBMLOutputWriterPart.writeEnd(rWriter);
	}

	OpenViBEToolkit::IBoxAlgorithmStimulationOutputWriter* createBoxAlgorithmStimulationOutputWriter(void)
	{
		return new OpenViBEToolkit::CBoxAlgorithmStimulationOutputWriter();
	}

	void releaseBoxAlgorithmStimulationOutputWriter(OpenViBEToolkit::IBoxAlgorithmStimulationOutputWriter* pOutputWriter)
	{
		delete pOutputWriter;
	}
};
