#include "../IXMLNodeReader.h"
#include "../INodeFactory.h"
#include "../IXMLNodeReaderFactory.h"
#include "../node/CNodeSwitch.hpp"
#include "../TXMLReader.hpp"
#include "CXMLParameterReader.hpp"

#include <string>
#include <stack>
#include <vector>

namespace Automaton
{

	class CXMLSwitchEventNodeReader : public TXMLReader<IXMLNodeReader>
	{
		public:
			//! The node reader factory
			IXMLNodeReaderFactory * m_pNodeReaderFactory;

			//! The node factory
			INodeFactory * m_pNodeFactory;

			std::vector<Automaton::CIdentifier> m_oParameter;

			CXMLParameterReader * m_pParameterParser;

			CNodeSwitch * m_pNode;
			CIdentifier m_oNodeIdentifier;

			std::vector <CIdentifier> m_oNodes;

			std::stack<IXMLNodeReader*> m_oReaderStack;

			std::stack<std::string> m_oParsedXMLNodes;

			EParsingStatus m_eStatus;

		public:
			CXMLSwitchEventNodeReader();
			virtual ~CXMLSwitchEventNodeReader();

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

