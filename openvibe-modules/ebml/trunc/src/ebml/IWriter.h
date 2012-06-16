#ifndef __EBML_IWriter_H__
#define __EBML_IWriter_H__

#include "CIdentifier.h"

namespace EBML
{
	/**
	 * \class IWriterCallback
	 * \author Yann Renard (INRIA/IRISA)
	 * \date 2006-08-07
	 * \brief Callback class to use when creating the EBML stream
	 *
	 * This class is to be overloaded by the user in order
	 * to get rid of the stream writing events. It will be
	 * notified by the EBML::IWriter object of what is to be
	 * written in the stream while the user sends information
	 * to the writer.
	 *
	 * \sa EBML::IWriter
	 */
	class EBML_API IWriterCallback
	{
	public:

		/**
		 * \brief Virtual destructor
		 */
		virtual ~IWriterCallback(void) { }
		/**
		 * \brief Gives the callback object a new stream chunk
		 * \param pBuffer [in] : The buffer to write in the stream
		 * \param ui64BufferSize [in] : The buffer size in bytes
		 *
		 * This function tells the callback object new data
		 * are ready to send in the EBML stream. This function
		 * may be called while the user sends data to the
		 * writer.
		 */
		virtual void write(const void* pBuffer, const EBML::uint64 ui64BufferSize)=0;
	};

	class EBML_API IWriterCallBack : public IWriterCallback { };

	/**
	 * \class IWriter
	 * \author Yann Renard (INRIA/IRISA)
	 * \date 2006-08-07
	 * \brief EBML formating class
	 *
	 * This class is used in order to format datas using EBML
	 * specifications. It gives a minimalistic interface to
	 * allow creating new EBML nodes, and to insert data
	 * in simple child nodes.
	 *
	 * Basic usage of this class consists in :
	 *
	 *  - Create child node
	 *  - Gives information about the node :
	 *    - Whether created node is master node
	 *        (Goto 1)
	 *    - Whether created node is simple child node
	 *        (set data for this child node)
	 *  - Close opened node
	 *
	 * The EBML::IWriterHelper class could be used in order
	 * to send EBML standard data such as integers, floats,
	 * strings, etc...
	 *
	 * To create instances of this class, the user has to call
	 * EBML::createWriter. To delete instances of this class, the
	 * user has to call EBML::IWriter::release.
	 *
	 * Be sure to look at http://ebml.sourceforge.net/specs/ in
	 * order to understand what EBML is and how it should be used.
	 */
	class EBML_API IWriter
	{
	public:

		/**
		 * \brief Starts a new child node
		 * \param rIdentifier [in] : The identifier of the new child node
		 * \return \e true on success.
		 * \return \e false on error.
		 *
		 * This function is called when the user wants to
		 * add a child node to the currently opened master
		 * node. A node containing data can not have child
		 * so if \c setChildData has been called before,
		 * this function returns \e false.
		 *
		 * Once the node has been opened, it should be closed
		 * calling \c closeChild.
		 */
		virtual EBML::boolean openChild(const EBML::CIdentifier& rIdentifier)=0;
		/**
		 * \brief Sets data for simple child node
		 * \param pBuffer [in] : The buffer to set as child data
		 * \param ui64BufferSize [in] : The buffer size in bytes
		 * \return \e true on success.
		 * \return \e false on error.
		 *
		 * This function is called by the user for setting
		 * data in the currently opened simple child node.
		 * If the currently opened node has children, it
		 * is a master node so it can't receive data. In
		 * such case, the function returns \e false. However,
		 * it the currently opened node already has data,
		 * it returns \e false too.
		 */
		virtual EBML::boolean setChildData(const void* pBuffer, const EBML::uint64 ui64BufferSize)=0;
		/**
		 * \brief Closes currently opened child node
		 * \return \e true on success.
		 * \return \e false on error.
		 *
		 * This function is called when the user wants to
		 * close a child node. The node should have received
		 * either child nodes or data depending if it is
		 * a master node or a simple child node.
		 */
		virtual EBML::boolean closeChild(void)=0;
		/**
		 * \brief Tells this object it won't be used anymore
		 *
		 * Instances of this class can not be instanciated
		 * another way than calling \c createWriter. They can
		 * not be deleted either because the destructor is.
		 * protected. The library knows how to create and
		 * delete an instance of this class... Calling
		 * \c release will simply delete this instance and
		 * handle necessary cleanings when needed.
		 *
		 * The current object is invalid after calling this
		 * function. It can not be used anymore.
		 */
		virtual void release(void)=0;

	protected:

		/**
		 * \brief Virtual destructor - should be overloaded
		 */
		virtual ~IWriter(void) { }
	};

	/**
	 * \brief Instanciation function for EBML writer objects
	 * \param rWriterCallback [in] : The callback object the writer should use
	 * \return a pointer to the created instance on success.
	 * \return \c NULL when something went wrong.
	 */
	extern EBML_API EBML::IWriter* createWriter(EBML::IWriterCallback& rWriterCallback);
};

#endif // __EBML_IWriter_H__
