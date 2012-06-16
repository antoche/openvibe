#include "ovtkIBoxAlgorithmFeatureVectorOutputWriter.h"
#include "ovtkCBoxAlgorithmFeatureVectorOutputWriterPart.inl"
#include "ovtkCBoxAlgorithmStreamedMatrixOutputWriterPart.inl"
#include "ovtkCBoxAlgorithmEBMLOutputWriterPart.inl"

namespace OpenViBEToolkit
{
	class CBoxAlgorithmFeatureVectorOutputWriter : public OpenViBEToolkit::IBoxAlgorithmFeatureVectorOutputWriter
	{
	public:

		CBoxAlgorithmFeatureVectorOutputWriter(void);

		virtual OpenViBE::boolean setFeatureCount(const OpenViBE::uint32 ui32FeatureCount);
		virtual OpenViBE::boolean setFeatureName(const OpenViBE::uint32 ui32FeatureIndex, const char* sFeatureName);
		virtual OpenViBE::boolean setFeatureVector(const OpenViBE::float64* pFeatureVector);

		virtual OpenViBE::boolean writeHeader(EBML::IWriter& rWriter);
		virtual OpenViBE::boolean writeBuffer(EBML::IWriter& rWriter);
		virtual OpenViBE::boolean writeEnd(EBML::IWriter& rWriter);

		_IsDerivedFromClass_Final_(OpenViBEToolkit::IBoxAlgorithmFeatureVectorOutputWriter, OVTK_ClassId_);

	protected:

		OpenViBEToolkit::CBoxAlgorithmEBMLOutputWriterPart m_oEBMLOutputWriterPart;
		OpenViBEToolkit::CBoxAlgorithmStreamedMatrixOutputWriterPart m_oStreamedMatrixOutputWriterPart;
		OpenViBEToolkit::CBoxAlgorithmFeatureVectorOutputWriterPart m_oFeatureVectorOutputWriterPart;
	};

	CBoxAlgorithmFeatureVectorOutputWriter::CBoxAlgorithmFeatureVectorOutputWriter(void)
	{
		m_oEBMLOutputWriterPart.registerBoxAlgorithmEBMLOutputWriterPart(m_oFeatureVectorOutputWriterPart);
		m_oEBMLOutputWriterPart.registerBoxAlgorithmEBMLOutputWriterPart(m_oStreamedMatrixOutputWriterPart);

		m_oStreamedMatrixOutputWriterPart.setDimmensionCount(1);
	}

	OpenViBE::boolean CBoxAlgorithmFeatureVectorOutputWriter::setFeatureCount(const OpenViBE::uint32 ui32FeatureCount)
	{
		return m_oStreamedMatrixOutputWriterPart.setDimmensionSize(0, ui32FeatureCount);
	}

	OpenViBE::boolean CBoxAlgorithmFeatureVectorOutputWriter::setFeatureName(const OpenViBE::uint32 ui32FeatureIndex, const char* sFeatureName)
	{
		return m_oStreamedMatrixOutputWriterPart.setDimmensionLabel(0, ui32FeatureIndex, sFeatureName);
	}

	OpenViBE::boolean CBoxAlgorithmFeatureVectorOutputWriter::setFeatureVector(const OpenViBE::float64* pFeatureVector)
	{
		return m_oStreamedMatrixOutputWriterPart.setBuffer(pFeatureVector);
	}

	OpenViBE::boolean CBoxAlgorithmFeatureVectorOutputWriter::writeHeader(EBML::IWriter& rWriter)
	{
		return m_oEBMLOutputWriterPart.writeHeader(rWriter);
	}

	OpenViBE::boolean CBoxAlgorithmFeatureVectorOutputWriter::writeBuffer(EBML::IWriter& rWriter)
	{
		return m_oEBMLOutputWriterPart.writeBuffer(rWriter);
	}

	OpenViBE::boolean CBoxAlgorithmFeatureVectorOutputWriter::writeEnd(EBML::IWriter& rWriter)
	{
		return m_oEBMLOutputWriterPart.writeEnd(rWriter);
	}

	OpenViBEToolkit::IBoxAlgorithmFeatureVectorOutputWriter* createBoxAlgorithmFeatureVectorOutputWriter(void)
	{
		return new OpenViBEToolkit::CBoxAlgorithmFeatureVectorOutputWriter();
	}

	void releaseBoxAlgorithmFeatureVectorOutputWriter(OpenViBEToolkit::IBoxAlgorithmFeatureVectorOutputWriter* pOutputWriter)
	{
		delete pOutputWriter;
	}
};
