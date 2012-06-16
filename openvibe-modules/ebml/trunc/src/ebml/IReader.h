#ifndef __EBML_IReader_H__
#define __EBML_IReader_H__

#include "CIdentifier.h"

namespace EBML
{
	/**
	 * \class IReaderCallback
	 * \author Yann Renard (INRIA/IRISA)
	 * \date 2006-08-07
	 * \brief Callback class to use when parsing the EBML stream
	 *
	 * This class is to be overloaded by the user in order
	 * to get rid of the parsing events. It will be notified
	 * by the EBML::IReader object of what is found in the
	 * stream.
	 *
	 * \sa EBML::IReader
	 */
	class EBML_API IReaderCallback
	{
	public:

		/**
		 * \brief Virtual destructor
		 */
		virtual ~IReaderCallback(void) { }
		/**
		 * \brief Kind of child querry
		 * \param rIdentifier [in] : The identifier which type has to be known
		 * \return \e true when the node is a master node
		 * \return \e false when it is a simple child node.
		 *
		 * When called by the reader, this function should tell it
		 * whether the node identified by \c rIdentifier is a master
		 * node (has child) or not (has data). For that, it has to
		 * return \e true when the node should have children, and
		 * \e false in other cases.
		 */
		virtual EBML::boolean isMasterChild(const EBML::CIdentifier& rIdentifier)=0;
		/**
		 * \brief Informs the callback object a new node parsing is starting
		 * \param rIdentifier [in] : The idenfier of the newly parsing node
		 *
		 * This is called to notify the callback object that the
		 * parser has started a new EBML node parsing. This EBML
		 * node is identified by \c rIdentifier. After this call
		 * will follow whether a new \c openChild if this node is
		 * a master one, whher a processData if this node is a
		 * simple child one.
		 */
		virtual void openChild(const EBML::CIdentifier& rIdentifier)=0;
		/**
		 * \brief Gives the callback object the data associated with the currently opened child node
		 * \param pBuffer [in] : The buffer corresponding to the current simple child node
		 * \param ui64BufferSize [in] : The buffer size in bytes
		 *
		 * This function is called when the data reading is
		 * terminated for a simple child node and so the callback
		 * object can process it. Whether the callback object
		 * knows how to process the data, whether it requests
		 * a reader helper object to do the work... See
		 * EBML::IReaderHelper for more details on that subject.
		 */
		virtual void processChildData(const void* pBuffer, const EBML::uint64 ui64BufferSize)=0;
		/**
		 * \brief Informs the callback object the current node parsing is terminated
		 *
		 * This function tells the callback object the current node
		 * parsing is terminated. Thus the parsing continues for the
		 * top level node, whether opening a new child or closing
		 * itself and so on...
		 */
		virtual void closeChild(void)=0;
	};

	class EBML_API IReaderCallBack : public IReaderCallback { };

	/**
	 * \class IReader
	 * \author Yann Renard (INRIA/IRISA)
	 * \date 2006-08-07
	 * \brief EBML processing class
	 *
	 * This class should be used in order to parse an EBML stream.
	 * It should be given a callback object that can handle the
	 * content of the stream. The parser itself as no understanding
	 * of what is included in the EBML structure.
	 *
	 * The parsing interface looks like the eXpat XML parser,
	 * allowing on-the-fly parsing. See http://expat.sourceforge.net
	 * for more details.
	 *
	 * To create instances of this class, the user has to call
	 * EBML::createReader. To delete instances of this class, the
	 * user has to call EBML::IReader::release.
	 *
	 * Be sure to look at http://ebml.sourceforge.net/specs/ in
	 * order to understand what EBML is and how it should be used.
	 */
	class EBML_API IReader
	{
	public:

		/**
		 * \brief Processes a chunk of data
		 * \param pBuffer [in] : The buffer of data to process
		 * \param ui64BufferSize [in] : The buffer size in bytes
		 * \return \e true when data processing was ok
		 * \return \e false in other cases.
		 *
		 * This function has to be called as soon as the stream
		 * reader (file, socket, whatever) has data. Those data
		 * are sent to the parser and parsing for this chunk is
		 * started immediatly. The callback object is called
		 * according to the EBML structure contained in the
		 * chunk and the previously parsed chunks. See
		 * EBML::IReaderCallback for more details on the
		 * callback object.
		 */
		virtual EBML::boolean processData(const void* pBuffer, const EBML::uint64 ui64BufferSize)=0;
		/**
		 * \brief Gets the identifier of the current node
		 * \return the identifier of the current node
		 */
		virtual EBML::CIdentifier getCurrentNodeIdentifier(void) const=0;
		/**
		 * \brief Gets the size of the current node
		 * \return the size of the current node
		 */
		virtual EBML::uint64 getCurrentNodeSize(void) const=0;
		/**
		 * \brief Tells this object it won't be used anymore
		 *
		 * Instances of this class can not be instanciated
		 * another way than calling \c createReader. They can
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
		virtual ~IReader(void) { }
	};

	/**
	 * \brief Instanciation function for EBML reader objects
	 * \param rReaderCallback [in] : The callback object the reader should use
	 * \return a pointer to the created instance on success.
	 * \return \c NULL when something went wrong.
	 */
	extern EBML_API EBML::IReader* createReader(EBML::IReaderCallback& rReaderCallback);
};

#endif // __EBML_IReader_H__
