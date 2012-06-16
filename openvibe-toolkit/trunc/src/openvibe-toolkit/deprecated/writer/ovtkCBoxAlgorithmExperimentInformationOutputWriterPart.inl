#ifndef __OpenViBEToolkit_Writer_CBoxAlgorithmExperimentInformationOutputWriter_INL__
#define __OpenViBEToolkit_Writer_CBoxAlgorithmExperimentInformationOutputWriter_INL__

#include "ovtkIBoxAlgorithmEBMLOutputWriter.h"
#include "ovtkCBoxAlgorithmEBMLOutputWriterPartBase.inl"
#include "ovtkIBoxAlgorithmExperimentInformationOutputWriter.h"

#include <ebml/CWriterHelper.h>

#include <string>

namespace OpenViBEToolkit {

	class CBoxAlgorithmExperimentInformationOutputWriterPart : public OpenViBEToolkit::CBoxAlgorithmEBMLOutputWriterPartBase
	{
	public:

		CBoxAlgorithmExperimentInformationOutputWriterPart(void);

		virtual OpenViBE::boolean setValue(const OpenViBE::uint32 ui32ValueIdentifier, const OpenViBE::uint32 ui32Value);
		virtual OpenViBE::boolean setValue(const OpenViBE::uint32 ui32ValueIdentifier, const char* sValue);

		virtual OpenViBE::boolean writeHeaderStart(EBML::IWriter& rWriter);
		virtual OpenViBE::boolean writeHeaderStop(EBML::IWriter& rWriter);
		// virtual OpenViBE::boolean writeBufferStart(EBML::IWriter& rWriter);
		// virtual OpenViBE::boolean writeBufferStop(EBML::IWriter& rWriter);
		// virtual OpenViBE::boolean writeEndStart(EBML::IWriter& rWriter);
		// virtual OpenViBE::boolean writeEndStop(EBML::IWriter& rWriter);

		_IsDerivedFromClass_Final_(OpenViBEToolkit::CBoxAlgorithmEBMLOutputWriterPartBase, OVTK_ClassId_);

	protected:

		OpenViBE::uint32 m_ui32ExperimentIdentifier;
		std::string m_sExperimentDate;

		OpenViBE::uint32 m_ui32SubjectIdentifier;
		std::string m_sSubjectName;
		OpenViBE::uint32 m_ui32SubjectAge;
		OpenViBE::uint32 m_ui32SubjectSex;

		OpenViBE::uint32 m_ui32LaboratoryIdentifier;
		std::string m_sLaboratoryName;
		OpenViBE::uint32 m_ui32TechnicianIdentifier;
		std::string m_sTechnicianName;

		EBML::CWriterHelper m_oWriterHelper;
	};
};

#endif // __OpenViBEToolkit_Writer_CBoxAlgorithmExperimentInformationOutputWriter_INL__
