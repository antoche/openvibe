#ifndef __OpenViBEToolkit_Writer_IBoxAlgorithmStreamedMatrixOutputWriter_H__
#define __OpenViBEToolkit_Writer_IBoxAlgorithmStreamedMatrixOutputWriter_H__

#include "ovtkIBoxAlgorithmEBMLOutputWriter.h"

namespace OpenViBEToolkit
{
	class OVTK_API IBoxAlgorithmStreamedMatrixOutputWriter : public OpenViBEToolkit::IBoxAlgorithmEBMLOutputWriter
	{
	public:

		virtual OpenViBE::boolean setDimmensionCount(const OpenViBE::uint32 ui32DimmensionCount)=0;
		virtual OpenViBE::boolean setDimmensionSize(const OpenViBE::uint32 ui32DimmensionIndex, const OpenViBE::uint32 ui32DimmensionSize)=0;
		virtual OpenViBE::boolean setDimmensionLabel(const OpenViBE::uint32 ui32DimmensionIndex, const OpenViBE::uint32 ui32DimmensionEntryIndex, const char* sDimmensionLabel)=0;
		virtual OpenViBE::boolean setBuffer(const OpenViBE::float64* pBuffer)=0;

		virtual OpenViBE::boolean writeHeader(EBML::IWriter& rWriter)=0;
		virtual OpenViBE::boolean writeBuffer(EBML::IWriter& rWriter)=0;

		_IsDerivedFromClass_(OpenViBEToolkit::IBoxAlgorithmEBMLOutputWriter, OVTK_ClassId_);
	};

	extern OVTK_API OpenViBEToolkit::IBoxAlgorithmStreamedMatrixOutputWriter* createBoxAlgorithmStreamedMatrixOutputWriter(void);
	extern OVTK_API void releaseBoxAlgorithmStreamedMatrixOutputWriter(OpenViBEToolkit::IBoxAlgorithmStreamedMatrixOutputWriter* pOutputWriter);
};

#endif // __OpenViBEToolkit_Writer_IBoxAlgorithmStreamedMatrixOutputWriter_H__
