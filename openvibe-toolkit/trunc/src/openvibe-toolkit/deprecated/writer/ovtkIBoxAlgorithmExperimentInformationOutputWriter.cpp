#include "ovtkIBoxAlgorithmExperimentInformationOutputWriter.h"
#include "ovtkCBoxAlgorithmExperimentInformationOutputWriterPart.inl"
#include "ovtkCBoxAlgorithmEBMLOutputWriterPart.inl"

namespace OpenViBEToolkit
{
	class CBoxAlgorithmExperimentInformationOutputWriter : public OpenViBEToolkit::IBoxAlgorithmExperimentInformationOutputWriter
	{
	public:

		CBoxAlgorithmExperimentInformationOutputWriter(void);

		virtual OpenViBE::boolean setValue(const OpenViBE::uint32 ui32ValueIdentifier, const OpenViBE::uint32 ui32Value);
		virtual OpenViBE::boolean setValue(const OpenViBE::uint32 ui32ValueIdentifier, const char* sValue);

		virtual OpenViBE::boolean writeHeader(EBML::IWriter& rWriter);
		virtual OpenViBE::boolean writeBuffer(EBML::IWriter& rWriter);
		virtual OpenViBE::boolean writeEnd(EBML::IWriter& rWriter);

		_IsDerivedFromClass_Final_(OpenViBEToolkit::IBoxAlgorithmExperimentInformationOutputWriter, OVTK_ClassId_);

	protected:

		OpenViBEToolkit::CBoxAlgorithmEBMLOutputWriterPart m_oEBMLOutputWriterPart;
		OpenViBEToolkit::CBoxAlgorithmExperimentInformationOutputWriterPart m_oExperimentInformationOutputWriterPart;
	};

	CBoxAlgorithmExperimentInformationOutputWriter::CBoxAlgorithmExperimentInformationOutputWriter(void)
	{
		m_oEBMLOutputWriterPart.registerBoxAlgorithmEBMLOutputWriterPart(m_oExperimentInformationOutputWriterPart);
	}

	OpenViBE::boolean CBoxAlgorithmExperimentInformationOutputWriter::setValue(const OpenViBE::uint32 ui32ValueIdentifier, const OpenViBE::uint32 ui32Value)
	{
		return m_oExperimentInformationOutputWriterPart.setValue(ui32ValueIdentifier, ui32Value);
	}

	OpenViBE::boolean CBoxAlgorithmExperimentInformationOutputWriter::setValue(const OpenViBE::uint32 ui32ValueIdentifier, const char* sValue)
	{
		return m_oExperimentInformationOutputWriterPart.setValue(ui32ValueIdentifier, sValue);
	}

	OpenViBE::boolean CBoxAlgorithmExperimentInformationOutputWriter::writeHeader(EBML::IWriter& rWriter)
	{
		return m_oEBMLOutputWriterPart.writeHeader(rWriter);
	}

	OpenViBE::boolean CBoxAlgorithmExperimentInformationOutputWriter::writeBuffer(EBML::IWriter& rWriter)
	{
		return m_oEBMLOutputWriterPart.writeBuffer(rWriter);
	}

	OpenViBE::boolean CBoxAlgorithmExperimentInformationOutputWriter::writeEnd(EBML::IWriter& rWriter)
	{
		return m_oEBMLOutputWriterPart.writeEnd(rWriter);
	}

	OpenViBEToolkit::IBoxAlgorithmExperimentInformationOutputWriter* createBoxAlgorithmExperimentInformationOutputWriter(void)
	{
		return new OpenViBEToolkit::CBoxAlgorithmExperimentInformationOutputWriter();
	}

	void releaseBoxAlgorithmExperimentInformationOutputWriter(OpenViBEToolkit::IBoxAlgorithmExperimentInformationOutputWriter* pOutputWriter)
	{
		delete pOutputWriter;
	}
};
