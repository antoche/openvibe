#ifndef __OpenViBEToolkit_Writer_CBoxAlgorithmStimulationOutputWriterPart_INL__
#define __OpenViBEToolkit_Writer_CBoxAlgorithmStimulationOutputWriterPart_INL__

#include "ovtkCBoxAlgorithmEBMLOutputWriterPartBase.inl"

#include <ebml/CWriterHelper.h>

#include <map>

#define OVTK_Version_StimulationOutputWriter 1

namespace OpenViBEToolkit
{
	class CBoxAlgorithmStimulationOutputWriterPart : public OpenViBEToolkit::CBoxAlgorithmEBMLOutputWriterPartBase
	{
	public:

		CBoxAlgorithmStimulationOutputWriterPart(void);

		virtual OpenViBE::boolean setStimulationCount(const OpenViBE::uint32 ui32StimulationCount);
		virtual OpenViBE::boolean setStimulation(const OpenViBE::uint32 ui32StimulationIndex, const OpenViBE::uint64 ui64StimulationIdentifier, const OpenViBE::uint64 ui64StimulationDate);

		_IsDerivedFromClass_Final_(OpenViBEToolkit::CBoxAlgorithmEBMLOutputWriterPartBase, OVTK_ClassId_);

	protected:

		// virtual OpenViBE::boolean writeHeaderStart(EBML::IWriter& rWriter);
		// virtual OpenViBE::boolean writeHeaderStop(EBML::IWriter& rWriter);
		virtual OpenViBE::boolean writeBufferStart(EBML::IWriter& rWriter);
		virtual OpenViBE::boolean writeBufferStop(EBML::IWriter& rWriter);
		// virtual OpenViBE::boolean writeEndStart(EBML::IWriter& rWriter);
		// virtual OpenViBE::boolean writeEndStop(EBML::IWriter& rWriter);

	protected:

		OpenViBE::uint32 m_ui32SitmulationCount;
		std::map< OpenViBE::uint32, std::pair< OpenViBE::uint64, OpenViBE::uint64 > > m_vStimulation;

		EBML::CWriterHelper m_oWriterHelper;
	};
};

#endif // __OpenViBEToolkit_Writer_CBoxAlgorithmStimulationOutputWriterPart_INL__
