#ifndef __OpenViBE_IMatrix_H__
#define __OpenViBE_IMatrix_H__

#include "ovIObject.h"

namespace OpenViBE
{
	/**
	 * \class IMatrix
	 * \author Yann Renard (INRIA/IRISA)
	 * \date 2007-11-21
	 * \brief Basic §OpenViBE§ matrix interface
	 *
	 * This interface offers functionalities to basically manipulate an n-dimmensional
	 * matrix. The manipulation that can be done are very limited and nearly limits
	 * to memory manipulation. This is basically an unified way for storing matrices
	 * content in §OpenViBE§
	 *
	 * §OpenViBE§ provides an standalone implementation of this interface in OpenViBE::CMatrix
	 */
	class OV_API IMatrix : public OpenViBE::IObject
	{
	public:

		/** \name Accessors */
		//@{

		/**
		 * \brief Gets the dimension count for this matrix
		 * \return the dimension count for this matrix
		 * \note Uninitialized matrices should return \c 0
		 */
		virtual const OpenViBE::uint32 getDimensionCount(void) const=0;
		/**
		 * \brief Gets the dimension size for a given dimension index
		 * \param ui32DimensionIndex [in] : the dimension index which size has to be returned
		 * \return the dimension size for the specified dimension index
		 * \note Each dimension can have a different size
		 */
		virtual const OpenViBE::uint32 getDimensionSize(
			const OpenViBE::uint32 ui32DimensionIndex) const=0;
		/**
		 * \brief Gets the dimension label for a given dimension index / dimension entry index pair
		 * \param ui32DimensionIndex [in] : the dimension index
		 * \param ui32DimensionEntryIndex [in] : the dimension entry index for the specified dimension index
		 * \return the dimension label for the given dimension index / dimension entry index pair
		 * \note The string is permanent pointer until the dimension size or the label itself changes
		 */
		virtual const char* getDimensionLabel(
			const OpenViBE::uint32 ui32DimensionIndex,
			const OpenViBE::uint32 ui32DimensionEntryIndex) const=0;
		/**
		 * \brief Gets the raw buffer for this matrix
		 * \return the raw buffer for this matrix
		 * \note The raw buffer is permanent pointer until the dimension count or any dimension size changes
		 *
		 * When the raw buffer is acquired, the elements are accessed sequentially, using each dimension
		 * after the other. Suppose a matrix of 2 dimensions, of size 2 x 3 :
		 *
		 * \code
		 *
		 * IMatrix* l_pMatrix= // ...
		 *
		 * float64* l_pMatrixRawBuffer=l_pMatrix->getBuffer();
		 *
		 * l_pMatrixRawBuffer[0]; // this is first line first column
		 * l_pMatrixRawBuffer[1]; // this is first line second column
		 * l_pMatrixRawBuffer[2]; // this is second line first column
		 * l_pMatrixRawBuffer[3]; // this is second line second column
		 *
		 * l_pMatrixRawBuffer[i*2+j]; // this is ith line jth column
		 *
		 * \endcode
		 * \sa getBufferElementCount
		 */
		virtual const OpenViBE::float64* getBuffer(void) const=0;
		/**
		 * \brief Gets the total number of elements in the matrix
		 * \return the total number of elements in the matrix
		 * \note The value returned by this function is the size of the raw buffer returned by \c getBuffer
		 */
		virtual const OpenViBE::uint32 getBufferElementCount(void) const=0;

		//@}
		/** \name Modifiers */
		//@{

		/**
		 * \brief Sets dimension count for this matrix
		 * \param ui32DimensionCount [in] : the number of dimension for this matrix
		 * \return \e true in case of success.
		 * \return \e false in case of error.
		 */
		virtual OpenViBE::boolean setDimensionCount(
			const OpenViBE::uint32 ui32DimensionCount)=0;
		/**
		 * \brief Sets the dimension size for this matrix on a given dimension index
		 * \param ui32DimensionIndex [in] : the dimension index which size has to be changed
		 * \param ui32DimensionSize [in] : the new dimension size for this dimension
		 * \return \e true in case of success.
		 * \return \e false in case of error.
		 */
		virtual OpenViBE::boolean setDimensionSize(
			const OpenViBE::uint32 ui32DimensionIndex,
			const OpenViBE::uint32 ui32DimensionSize)=0;
		/**
		 * \brief Sets the dimension label for this matrix on a given dimension index / dimension entry index pair
		 * \param ui32DimensionIndex [in] : the dimension index
		 * \param ui32DimensionEntryIndex [in] : the entry index in the specificed dimension which label has to be changed
		 * \param sDimensionLabel [in] : the new dimension label to apply to this dimension index / dimension entry index pair
		 * \return \e true in case of success.
		 * \return \e false in case of error.
		 */
		virtual OpenViBE::boolean setDimensionLabel(
			const OpenViBE::uint32 ui32DimensionIndex,
			const OpenViBE::uint32 ui32DimensionEntryIndex,
			const char* sDimensionLabel)=0;
		/**
		 * \brief Gets a writable raw buffer for this matrix
		 * \return the raw buffer for this matrix
		 * \note The raw buffer is permanent pointer until the dimension count or any dimension size changes
		 *
		 * \sa IMatrix::getBufferElementCount
		 * \sa IMatrix::getBuffer const
		 */
		virtual OpenViBE::float64* getBuffer(void)=0;

		//@}

		_IsDerivedFromClass_(OpenViBE::IObject, OV_ClassId_Matrix);
	};
};

#endif // __OpenViBE_IMatrix_H__
