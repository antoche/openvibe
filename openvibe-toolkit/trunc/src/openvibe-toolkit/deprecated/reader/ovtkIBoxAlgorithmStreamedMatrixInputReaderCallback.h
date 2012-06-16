#ifndef __OpenViBEToolkit_Reader_IBoxAlgorithmStreamedMatrixInputReaderCallback_H__
#define __OpenViBEToolkit_Reader_IBoxAlgorithmStreamedMatrixInputReaderCallback_H__

#include "ovtkIBoxAlgorithmEBMLInputReaderCallback.h"

namespace OpenViBEToolkit
{
	class OVTK_API IBoxAlgorithmStreamedMatrixInputReaderCallback : public OpenViBEToolkit::IBoxAlgorithmEBMLInputReaderCallback
	{
	public:

		class OVTK_API ICallback
		{
		public:
			virtual void setMatrixDimmensionCount(const OpenViBE::uint32 ui32DimmensionCount)=0;
			virtual void setMatrixDimmensionSize(const OpenViBE::uint32 ui32DimmensionIndex, const OpenViBE::uint32 ui32DimmensionSize)=0;
			virtual void setMatrixDimmensionLabel(const OpenViBE::uint32 ui32DimmensionIndex, const OpenViBE::uint32 ui32DimmensionEntryIndex, const char* sDimmensionLabel)=0;
			virtual void setMatrixBuffer(const OpenViBE::float64* pBuffer)=0;
			virtual ~ICallback(void) { }
		};

		template <class COwnerClass>
		class TCallbackProxy1 : public OpenViBEToolkit::IBoxAlgorithmStreamedMatrixInputReaderCallback::ICallback
		{
		public:
			TCallbackProxy1(
				COwnerClass& rOwnerObject,
				void (COwnerClass::*fpSetMatrixDimmensionCount)(const OpenViBE::uint32 ui32DimmensionCount),
				void (COwnerClass::*fpSetMatrixDimmensionSize)(const OpenViBE::uint32 ui32DimmensionIndex, const OpenViBE::uint32 ui32DimmensionSize),
				void (COwnerClass::*fpSetMatrixDimmensionLabel)(const OpenViBE::uint32 ui32DimmensionIndex, const OpenViBE::uint32 ui32DimmensionEntryIndex, const char* sDimmensionLabel),
				void (COwnerClass::*fpSetMatrixBuffer)(const OpenViBE::float64* pBuffer))
				:m_rOwnerObject(rOwnerObject)
				,m_fpSetMatrixDimmensionCount(fpSetMatrixDimmensionCount)
				,m_fpSetMatrixDimmensionSize(fpSetMatrixDimmensionSize)
				,m_fpSetMatrixDimmensionLabel(fpSetMatrixDimmensionLabel)
				,m_fpSetMatrixBuffer(fpSetMatrixBuffer)
			{
			}
			virtual void setMatrixDimmensionCount(const OpenViBE::uint32 ui32DimmensionCount)
			{
				if(m_fpSetMatrixDimmensionCount)
				{
					(m_rOwnerObject.*m_fpSetMatrixDimmensionCount)(ui32DimmensionCount);
				}
			}
			virtual void setMatrixDimmensionSize(const OpenViBE::uint32 ui32DimmensionIndex, const OpenViBE::uint32 ui32DimmensionSize)
			{
				if(m_fpSetMatrixDimmensionSize)
				{
					(m_rOwnerObject.*m_fpSetMatrixDimmensionSize)(ui32DimmensionIndex, ui32DimmensionSize);
				}
			}
			virtual void setMatrixDimmensionLabel(const OpenViBE::uint32 ui32DimmensionIndex, const OpenViBE::uint32 ui32DimmensionEntryIndex, const char* sDimmensionLabel)
			{
				if(m_fpSetMatrixDimmensionLabel)
				{
					(m_rOwnerObject.*m_fpSetMatrixDimmensionLabel)(ui32DimmensionIndex, ui32DimmensionEntryIndex, sDimmensionLabel);
				}
			}
			virtual void setMatrixBuffer(const OpenViBE::float64* pBuffer)
			{
				if(m_fpSetMatrixBuffer)
				{
					(m_rOwnerObject.*m_fpSetMatrixBuffer)(pBuffer);
				}
			}
		protected:
			COwnerClass& m_rOwnerObject;
			void (COwnerClass::*m_fpSetMatrixDimmensionCount)(const OpenViBE::uint32 ui32DimmensionCount);
			void (COwnerClass::*m_fpSetMatrixDimmensionSize)(const OpenViBE::uint32 ui32DimmensionIndex, const OpenViBE::uint32 ui32DimmensionSize);
			void (COwnerClass::*m_fpSetMatrixDimmensionLabel)(const OpenViBE::uint32 ui32DimmensionIndex, const OpenViBE::uint32 ui32DimmensionEntryIndex, const char* sDimmensionLabel);
			void (COwnerClass::*m_fpSetMatrixBuffer)(const OpenViBE::float64* pBuffer);
		};

		_IsDerivedFromClass_(OpenViBEToolkit::IBoxAlgorithmEBMLInputReaderCallback, OVTK_ClassId_);
	};

	OVTK_API OpenViBEToolkit::IBoxAlgorithmStreamedMatrixInputReaderCallback* createBoxAlgorithmStreamedMatrixInputReaderCallback(OpenViBEToolkit::IBoxAlgorithmStreamedMatrixInputReaderCallback::ICallback& rCallback);
	OVTK_API void releaseBoxAlgorithmStreamedMatrixInputReaderCallback(OpenViBEToolkit::IBoxAlgorithmStreamedMatrixInputReaderCallback* pStreamedMatrixInputReaderCallback);
};

#endif // __OpenViBEToolkit_Reader_IBoxAlgorithmStreamedMatrixInputReaderCallback_H__
