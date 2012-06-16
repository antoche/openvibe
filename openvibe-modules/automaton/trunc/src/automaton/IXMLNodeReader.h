#ifndef __Automata_CXMLNodeReader_H__
#define __Automata_CXMLNodeReader_H__

#include "INode.h"
#include "IAutomatonContext.h"
#include "CIdentifier.h"

#include "xml/IReader.h"

namespace Automaton
{
	enum EParsingStatus{
		ParsingStatus_Nothing,
		ParsingStatus_Automaton,
		ParsingStatus_Variable,
		ParsingStatus_Node,
		ParsingStatus_Action,
		ParsingStatus_Property,
		ParsingStatus_Parameter,
		ParsingStatus_Loop,
		ParsingStatus_Child,
		ParsingStatus_Switch,
		ParsingStatus_Complete,
		ParsingStatus_RandomOrder,
		ParsingStatus_Error

	};

	class Automaton_API IXMLNodeReader
	{
		protected:
			virtual ~IXMLNodeReader(){}

		public:
			virtual EParsingStatus openChild(const char* sName, const char** sAttributeName, const char** sAttributeValue, uint64 ui64AttributeCount, IAutomatonContext* pContext) = 0;
			virtual EParsingStatus processChildData(const char* sData, IAutomatonContext* pContext) = 0;
			virtual EParsingStatus closeChild(IAutomatonContext* pContext) = 0;

			virtual CIdentifier getNodeIdentifier() = 0;

			virtual void release() = 0;
	};
};

#endif

