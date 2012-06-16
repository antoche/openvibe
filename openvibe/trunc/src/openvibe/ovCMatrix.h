#ifndef __OpenViBE_CMatrix_H__
#define __OpenViBE_CMatrix_H__

#include "ovIMatrix.h"

namespace OpenViBE
{
	/**
	 * \class CMatrix
	 * \author Yann Renard (INRIA/IRISA)
	 * \date 2007-11-21
	 * \brief Basic standalone §OpenViBE§ matrix implementation
	 * \ingroup Group_Base
	 *
	 * This class offers a basic standalone impementation of the OpenViBE::IMatrix
	 * interface. This class can be directly instanciated and used.
	 *
	 * Instances of this class use an internal implementation of the OpenViBE::IMatrix
	 * interface and redirect their calls to this implementation.
	 */
	class OV_API CMatrix : public OpenViBE::IMatrix
	{
	public:

		/** \name Constructors */
		//@{

		/**
		 * \brief Default constructor
		 *
		 * This constructor builds the internal implementation of this matrix.
		 */
		CMatrix(void);
		/**
		 * \brief Destructor
		 *
		 * The internal implementation is released.
		 */
		virtual ~CMatrix(void);

		//@}

		virtual const OpenViBE::uint32 getDimensionCount(void) const;
		virtual const OpenViBE::uint32 getDimensionSize(
			const OpenViBE::uint32 ui32DimensionIndex) const;
		virtual const char* getDimensionLabel(
			const OpenViBE::uint32 ui32DimensionIndex,
			const OpenViBE::uint32 ui32DimensionEntryIndex) const;
		virtual const OpenViBE::float64* getBuffer(void) const;
		virtual const OpenViBE::uint32 getBufferElementCount(void) const;

		virtual OpenViBE::boolean setDimensionCount(
			const OpenViBE::uint32 ui32DimensionCount);
		virtual OpenViBE::boolean setDimensionSize(
			const OpenViBE::uint32 ui32DimensionIndex,
			const OpenViBE::uint32 ui32DimensionSize);
		virtual OpenViBE::boolean setDimensionLabel(
			const OpenViBE::uint32 ui32DimensionIndex,
			const OpenViBE::uint32 ui32DimensionEntryIndex,
			const char* sDimensionLabel);
		virtual OpenViBE::float64* getBuffer(void);

		_IsDerivedFromClass_Final_(OpenViBE::IMatrix, OV_ClassId_MatrixBridge);

	private:

		OpenViBE::IMatrix* m_pMatrixImpl; //!< Internal implementation
	};
};

#endif // __OpenViBE_CMatrix_H__
