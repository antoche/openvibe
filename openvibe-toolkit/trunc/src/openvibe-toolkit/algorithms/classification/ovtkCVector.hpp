#ifndef __OpenViBEToolkit_CVector_H__
#define __OpenViBEToolkit_CVector_H__

#include "../../ovtk_base.h"
#include "../../ovtkIVector.h"

namespace OpenViBEToolkit
{
	template <class CParent>
	class TVector : public CParent
	{
	public:

		TVector(OpenViBE::IMatrix& rMatrix)
			:m_rMatrix(rMatrix)
		{
		}

		virtual OpenViBE::uint32 getSize(void) const
		{
			return m_rMatrix.getBufferElementCount();
		}

		virtual OpenViBE::boolean setSize(const OpenViBE::uint32 ui32Size)
		{
			m_rMatrix.setDimensionCount(1);
			m_rMatrix.setDimensionSize(0, ui32Size);
			return true;
		}

		virtual OpenViBE::float64* getBuffer(void)
		{
			return m_rMatrix.getBuffer();
		}

		virtual const OpenViBE::float64* getBuffer(void) const
		{
			return m_rMatrix.getBuffer();
		}

		virtual const char* getElementLabel(const OpenViBE::uint32 ui32Index) const
		{
			return m_rMatrix.getDimensionLabel(0, ui32Index);
		}

		virtual OpenViBE::boolean setElementLabel(const OpenViBE::uint32 ui32Index, const char* sElementLabel)
		{
			m_rMatrix.setDimensionLabel(0, ui32Index, sElementLabel);
			return true;
		}

		_IsDerivedFromClass_Final_(CParent, OV_UndefinedIdentifier);

	protected:

		OpenViBE::IMatrix& m_rMatrix;
	};

	typedef OpenViBEToolkit::TVector < OpenViBEToolkit::IVector > CVector;
};

#endif // __OpenViBEToolkit_CVector_H__
