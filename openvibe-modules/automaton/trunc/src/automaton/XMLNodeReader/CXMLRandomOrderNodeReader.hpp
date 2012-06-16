#include "../IXMLNodeReader.h"
#include "../INodeFactory.h"
#include "../IXMLNodeReaderFactory.h"
#include "../node/CNodeRandomOrder.hpp"

#include "../TXMLReader.hpp"
#include "CXMLParameterReader.hpp"

#include <string>
#include <stack>
#include <vector>

namespace Automaton
{

	class CXMLRandomOrderNodeReader : public TXMLReader<IXMLNodeReader>
	{
		public:
			//! The node reader factory
			IXMLNodeReaderFactory * m_pNodeReaderFactory;

			//! The node factory
			INodeFactory * m_pNodeFactory;

			std::vector<Automaton::CIdentifier> m_oParameter;
			Automaton::uint64 m_ui64BranchCount;

			CXMLParameterReader * m_pParameterParser;

			CNodeRandomOrder * m_pNode;
			CIdentifier m_oNodeIdentifier;

			std::vector <CIdentifier> m_oNodes;

			std::stack<IXMLNodeReader*> m_oReaderStack;

			std::stack<std::string> m_oParsedXMLNodes;

			EParsingStatus m_eStatus;

		public:
			CXMLRandomOrderNodeReader();
			virtual ~CXMLRandomOrderNodeReader();

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

