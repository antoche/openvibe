#include "../IXMLNodeReader.h"
#include "../INodeFactory.h"
#include "../node/CNodeAction.hpp"
#include "../TXMLReader.hpp"
#include "CXMLParameterReader.hpp"

#include <string>
#include <stack>
#include <vector>

namespace Automaton
{

	class CXMLActionWaitEventNodeReader : public TXMLReader<IXMLNodeReader>
	{
		public:
			//! The node factory
			INodeFactory * m_pNodeFactory;

			//! Parameters of the associated action
			std::vector<Automaton::CIdentifier> m_oParameter;

			CXMLParameterReader * m_pParameterParser;

			//! The action node being read
			CNodeAction * m_pNode;
			//! The action node identifier
			CIdentifier m_oNodeIdentifier;

			//! XML nodes being parsed by the reader
			std::stack<std::string> m_oParsedXMLNodes;

			EParsingStatus m_eStatus;

		public:
			CXMLActionWaitEventNodeReader();
			virtual ~CXMLActionWaitEventNodeReader();

			virtual EParsingStatus openChild(const char* sName, const char** sAttributeName, const char** sAttributeValue, uint64 ui64AttributeCount, IAutomatonContext* pContext);
			virtual EParsingStatus processChildData(const char* sData, IAutomatonContext* pContext);
			virtual EParsingStatus closeChild(IAutomatonContext* pContext);

			virtual CIdentifier getNodeIdentifier()
			{
				return m_oNodeIdentifier;
			}

			virtual void release();


	};

};

