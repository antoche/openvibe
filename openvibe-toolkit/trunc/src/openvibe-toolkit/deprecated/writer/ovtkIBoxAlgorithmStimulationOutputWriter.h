#ifndef __OpenViBEToolkit_Writer_IBoxAlgorithmStimulationOutputWriter_H__
#define __OpenViBEToolkit_Writer_IBoxAlgorithmStimulationOutputWriter_H__

#include "ovtkIBoxAlgorithmEBMLOutputWriter.h"

namespace OpenViBEToolkit
{
	class OVTK_API IBoxAlgorithmStimulationOutputWriter : public OpenViBEToolkit::IBoxAlgorithmEBMLOutputWriter
	{
	public:

		virtual OpenViBE::boolean setStimulationCount(const OpenViBE::uint32 ui32StimulationCount)=0;
		virtual OpenViBE::boolean setStimulation(const OpenViBE::uint32 ui32StimulationIndex, const OpenViBE::uint64 ui64StimulationIdentifier, const OpenViBE::uint64 ui64StimulationDate)=0;

		_IsDerivedFromClass_(OpenViBEToolkit::IBoxAlgorithmEBMLOutputWriter, OVTK_ClassId_);
	};

	extern OVTK_API OpenViBEToolkit::IBoxAlgorithmStimulationOutputWriter* createBoxAlgorithmStimulationOutputWriter(void);
	extern OVTK_API void releaseBoxAlgorithmStimulationOutputWriter(OpenViBEToolkit::IBoxAlgorithmStimulationOutputWriter* pOutputWriter);
};

#endif // __OpenViBEToolkit_Writer_IBoxAlgorithmStimulationOutputWriter_H__
