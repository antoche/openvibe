#ifndef __OpenViBE_IMemoryBuffer_H__
#define __OpenViBE_IMemoryBuffer_H__

#include "ovIObject.h"

namespace OpenViBE
{
	/**
	 * \class IMemoryBuffer
	 * \author Yann Renard (INRIA/IRISA)
	 * \date 2007-11-21
	 * \brief Basic §OpenViBE§ memory buffer interface
	 *
	 * This interface offers functionalities to basically manipulate a raw memory buffer.
	 * It allows the buffer to be resized and manipulated easily with no care of allocation
	 * reallocation. Implementations for this interface may provide optimisations for such
	 * operations.
	 *
	 * §OpenViBE§ provides an standalone implementation of this interface in OpenViBE::CMemoryBuffer
	 */
	class OV_API IMemoryBuffer : public OpenViBE::IObject
	{
	public:

		/**
		 * \brief Reserves some memory for this memory buffer
		 * \param ui64Size [in] : the amount of memory to reserve
		 * \return \e true in case of success.
		 * \return \e false in case of error.
		 *
		 * This function reserves some memory space for later use. This does not
		 * affect the actual size of the buffer but allows later calls to \c append
		 * not to reallocate the whole buffer.
		 *
		 * \note if \c ui64Size is lower than the actual buffer size
		 *       then \e true is returned and nothing is done.
		 */
		virtual OpenViBE::boolean reserve(
			const OpenViBE::uint64 ui64Size)=0;
		/**
		 * \brief Changes the size of this memory buffer
		 * \param ui64Size [in] : the new size to give to the buffer
		 * \param bDiscard [in] : tells the reallocation process whether it should presever currently stored data or not
		 * \return \e true in case of success.
		 * \return \e false in case of error.
		 * \note On error, the buffer is left unchanged.
		 * \note If the new size if lower than the current size and \c bDiscard is true, the
		 *       buffer is simply truncated to the \c ui64Size first bytes.
		 * \sa getSize
		 */
		virtual OpenViBE::boolean setSize(
			const OpenViBE::uint64 ui64Size,
			const OpenViBE::boolean bDiscard)=0;
		/**
		 * \brief Gets the current size of this memory buffer
		 * \return the current size of this memory buffer
		 * \sa setSize
		 */
		virtual OpenViBE::uint64 getSize(void) const=0;
		/**
		 * \brief Gets a direct pointer to the byte array for read/write access
		 * \return a direct pointer to the byte array for read/write access
		 * \sa getSize
		 */
		virtual OpenViBE::uint8* getDirectPointer(void)=0;
		/**
		 * \brief Gets a direct pointer to the byte array for read access
		 * \return a direct pointer to the byte array for read access
		 * \sa getSize
		 */
		virtual const OpenViBE::uint8* getDirectPointer(void) const=0;
		/**
		 * \brief Appends data to this memory buffer
		 * \param pBuffer [in] : the buffer containing data that should be appended
		 * \param ui64BufferSize [in] : the buffer size that should be appended
		 * \return \e true in case of success.
		 * \return \e false in case of error.
		 */
		virtual OpenViBE::boolean append(
			const OpenViBE::uint8* pBuffer,
			const OpenViBE::uint64 ui64BufferSize)=0;
		/**
		 * \brief Appends data to this memory buffer
		 * \param rMemoryBuffer [in] : the memory buffer containing data that should be appended
		 * \return \e true in case of success.
		 * \return \e false in case of error.
		 */
		virtual OpenViBE::boolean append(
			const OpenViBE::IMemoryBuffer& rMemoryBuffer)
		{
			return this->append(rMemoryBuffer.getDirectPointer(), rMemoryBuffer.getSize());
		}

		_IsDerivedFromClass_(OpenViBE::IObject, OV_ClassId_MemoryBuffer);

		const OpenViBE::uint8& operator [] (const OpenViBE::uint64 ui64Index) const
		{
			return this->getDirectPointer()[ui64Index];
		}

		OpenViBE::uint8& operator [] (const OpenViBE::uint64 ui64Index)
		{
			return this->getDirectPointer()[ui64Index];
		}
	};
};

#endif // __OpenViBE_IMemoryBuffer_H__
