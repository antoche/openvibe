#ifndef __EBML_IWriterHelper_H__
#define __EBML_IWriterHelper_H__

#include "IWriter.h"

namespace EBML
{
	/**
	 * \class IWriterHelper
	 * \author Yann Renard (INRIA/IRISA)
	 * \date 2006-08-07
	 * \brief Helper to write basic EBML types
	 *
	 * This class may be used by the user in order to correctly
	 * format simple types defined in the EBML description such
	 * as integers, floats, strings etc... It directly uses the
	 * EBML::IWriter connected instance so one could simply
	 * use the helper in order to write his EBML stream.
	 *
	 * A similar class exists to help parsing process...
	 * See EBML::IReaderHelper for more details.
	 *
	 * Be sure to look at http://ebml.sourceforge.net/specs/ in
	 * order to understand what EBML is and how it should be used.
	 *
	 * \todo float80 formating implementation
	 * \todo date formating implementation
	 * \todo utf8 string formating implementation
	 */
	class EBML_API IWriterHelper
	{
	public:

		/** \name Writer connection */
		//@{
		/**
		 * \brief Connects an EBML writer to this helper
		 * \param pWriter [in] : The writer to connect
		 * \return \e true on success.
		 * \return \e false on error.
		 *
		 * This function gives the helper a handle to the writer
		 * to use in order to forward requests. Thus, when the
		 * user calls a helper function, the call is forwarded
		 * to the correct writer that effictively does the work.
		 * The aim of this helper is simply to transform standard
		 * EBML types into bytes buffers.
		 *
		 * Once a writer is connected, it could be disconnected
		 * thanks to the \c disconnect function. It must be done
		 * before calling \c connect again.
		 */
		virtual EBML::boolean connect(EBML::IWriter* pWriter)=0;
		/**
		 * \brief Disconnects the currently connected EBML writer
		 * \return \e true on success.
		 * \return \e false on error.
		 *
		 * This function should be called to release the EBML
		 * writer handle. The helper instance may then be used
		 * with another EBML writer instance calling \c connect
		 * again.
		 */
		virtual EBML::boolean disconnect(void)=0;
		//@}

		/** \name Writer binding functions */
		//@{
		/**
		 * \brief Child opening binding
		 * \param rIdentifier [in] : The identifier of the new child node
		 * \return \e true on success.
		 * \return \e false on error.
		 *
		 * This function simply forwards the call to the
		 * corresponding EBML::IWriter function. See
		 * EBML::IWriter::openChild for more details.
		 */
		virtual EBML::boolean openChild(const EBML::CIdentifier& rIdentifier)=0;
		/**
		 * \brief Child closing binding
		 * \return \e true on success.
		 * \return \e false on error.
		 *
		 * This function simply forwards the call to the
		 * corresponding EBML::IWriter function. See
		 * EBML::IWriter::closeChild for more details.
		 */
		virtual EBML::boolean closeChild(void)=0;
		//@}

		/**
		 * \name Standard EBML formating
		 * \brief EBML::IWriter::setChildData replacement
		 * \return \e true on success.
		 * \return \e false on error.
		 *
		 * Those functions should be used in place of the
		 * basic EBML::IWriter::setChildData function. They
		 * format standard EBML types into corresponding
		 * buffers and then send those built buffers to
		 * the writer using the EBML::IWriter::setChildData
		 * function.
		 */
		//@{
		/**
		 * \brief Sets a signed integer as child data
		 * \param iValue [in] : The integer value to set
		 */
		virtual EBML::boolean setSIntegerAsChildData(const EBML::int64 iValue)=0;
		/**
		 * \brief Sets an unsigned integer as child data
		 * \param uiValue [in] : The integer value to set
		 */
		virtual EBML::boolean setUIntegerAsChildData(const EBML::uint64 uiValue)=0;
		/**
		 * \brief Sets a 32 bits float value as child data
		 * \param fValue [in] : The 32 bits float value to set
		 */
		virtual EBML::boolean setFloat32AsChildData(const EBML::float32 fValue)=0;
		/**
		 * \brief Sets a 64 bits float value as child data
		 * \param fValue [in] : The 64 bits float value to set
		 */
		virtual EBML::boolean setFloat64AsChildData(const EBML::float64 fValue)=0;
// virtual EBML::boolean setFloat80AsChildData( ??? value)=0;
// virtual EBML::boolean setDateAsChildData( ??? value)=0;
		/**
		 * \brief Sets a buffer as child data
		 * \param pBuffer [in] : The buffer to send to the writer
		 * \param ui64BufferLength [in] : The buffer size in bytes
		 * \note This function simply calls the basic
		 *       EBML::IWriter::setChildData function with the
		 *       same two parameters.
		 */
		virtual EBML::boolean setBinaryAsChildData(const void* pBuffer, const EBML::uint64 ui64BufferLength)=0;
		/**
		 * \brief Sets an ASCII string as child data
		 * \param sValue [in] : The ASCII string value to set
		 */
		virtual EBML::boolean setASCIIStringAsChildData(const char* sValue)=0;
// virtual EBML::boolean setUTF8StringAsChildData( ??? value)=0;
		//@}

		/**
		 * \brief Tells this object it won't be used anymore
		 *
		 * Instances of this class can not be instanciated
		 * another way than calling \c createWriterHelper. They
		 * can not be deleted either because the destructor is.
		 * protected. The library knows how to create and
		 * delete an instance of this class... Calling
		 * \c release will simply delete this instance and
		 * handle necessary cleanings when needed.
		 *
		 * The current object is invalid after calling this
		 * function. It can not be used anymore.
		 *
		 * \warning Releasing this obbject does not release the
		 *          connected writer instance !
		 */
		virtual void release(void)=0;

	protected:

		/**
		 * \brief Virtual destructor - should be overloaded
		 */
		virtual ~IWriterHelper(void) { }
	};

	/**
	 * \brief Instanciation function for EBML writer helper objects
	 * \return a pointer to the created instance on success.
	 * \return \c NULL when something went wrong.
	 */
	extern EBML_API EBML::IWriterHelper* createWriterHelper(void);
};

#endif // __EBML_IEnhancedWriter_H__
