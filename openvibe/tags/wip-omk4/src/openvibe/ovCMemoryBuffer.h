#ifndef __OpenViBE_CMemoryBuffer_H__
#define __OpenViBE_CMemoryBuffer_H__

#include "ovIMemoryBuffer.h"

namespace OpenViBE
{
	/**
	 * \class CMemoryBuffer
	 * \author Yann Renard (INRIA/IRISA)
	 * \date 2007-11-21
	 * \brief Basic standalone §OpenViBE§ memory buffer implementation
	 * \ingroup Group_Base
	 *
	 * This class offers a basic standalone impementation of the OpenViBE::IMemoryBuffer
	 * interface. This class can be directly instanciated and used.
	 *
	 * Instances of this class use an internal implementation of the OpenViBE::IMemoryBuffer
	 * interface and redirect their calls to this implementation.
	 */

	class OV_API CMemoryBuffer : public OpenViBE::IMemoryBuffer
	{
	public:

		/** \name Constructors */
		//@{

		/**
		 * \brief Default constructor
		 *
		 * This constructor builds the internal implementation of this memory buffer.
		 */
		CMemoryBuffer(void);
		/**
		 * \brief Destructor
		 *
		 * The internal implementation is released.
		 */
		virtual ~CMemoryBuffer(void);

		//@}

		virtual OpenViBE::boolean setSize(
			const OpenViBE::uint64 ui64Size,
			const OpenViBE::boolean bDiscard);
		virtual OpenViBE::uint64 getSize(void) const;
		virtual OpenViBE::uint8* getDirectPointer(void);
		virtual const OpenViBE::uint8* getDirectPointer(void) const;

		_IsDerivedFromClass_Final_(OpenViBE::IMemoryBuffer, OV_ClassId_MemoryBufferBridge);

	protected:

		OpenViBE::IMemoryBuffer* m_pMemoryBufferImpl; //!< Internal implementation
	};
};

#endif // __OpenViBE_CMemoryBuffer_H__
