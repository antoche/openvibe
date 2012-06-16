#ifndef __Automaton_IAutomatonContext_H__
#define __Automaton_IAutomatonContext_H__

#include "CIdentifier.h"
#include "defines.h"
#include "INode.h"

#include <vector>
#include <string>

namespace Automaton
{

	class INode;

	class Automaton_API IAutomatonContext
	{

		public:
			/**
			 * Gets A node object's pointer from its identifier.
			 * \param oIdentifier The identifier of the node.
			 * \return A pointer to this node.
			 */
			virtual INode * getNode(const CIdentifier& oIdentifier) = 0;

			virtual INode * getInitialNode() = 0;

			/**
			 * Adds a new node to the Automaton context.
			 * \param pNode A pointer to the node to add.
			 * \return The identifier associated with this node.
			 */
			virtual CIdentifier addNode(INode* pNode) = 0;

			/**
			 * Adds a node to the successors of an other.
			 * \param The node to which we add a successor.
			 * \param The successor.
			 * \return True if the operation was successful.
			 */
			virtual boolean addSuccessor(CIdentifier oStart, CIdentifier oEnd) = 0;

			/**
			 * Gets the list of events that were received by the automaton.
			 * \return The list of events that were received by the automaton
			 */
			virtual const CIdentifier * getReceivedEvents() = 0;
			virtual Automaton::uint32 getReceivedEventsCount() = 0;
			virtual void clearReceivedEvents()  = 0;

			/**
			 * Adds a new event to the list of received events.
			 * \param oIdentifier The identifier of the new event.
			 */
			virtual void addReceivedEvent(CIdentifier& oIdentifier) = 0;

			virtual const CIdentifier * getSentEvents() = 0;
			virtual Automaton::uint32 getSentEventsCount() = 0;
			virtual void clearSentEvents() = 0;

			/**
			 * Sends a new event to the external world.
			 * \param oIdentifier The identifier of the event sent.
			 */
			virtual void sendEvent(CIdentifier& oIdentifier) = 0;

			/**
			 * Converts an XML node name to an identifier.
			 * \param oXMLNodeName The node's name.
			 * \return This node category's identifier.
			 */
			virtual CIdentifier getIdentifier(const std::string& oXMLNodeName) = 0;

			virtual void setCurrentTime(Automaton::uint64 ui64Time) = 0;
			virtual Automaton::uint64 getCurrentTime() = 0;

			virtual CIdentifier addParameter(const char * pName, Automaton::uint64 ui64Value) = 0;
			virtual CIdentifier getVariableIdentifier(const char * pName) = 0;
			virtual const char* getParameterName(const CIdentifier& oIdentifier) = 0;
			virtual Automaton::uint64 getParameterValue(const CIdentifier& oIdentifier) = 0;


			virtual void release() = 0;

		protected:
			virtual ~IAutomatonContext(){ }
	};

	extern Automaton_API IAutomatonContext* createAutomatonContext();
	extern Automaton_API void releaseAutomatonContext(IAutomatonContext* pContext);
};

#endif

