#ifndef __Automaton_CXMLParameterReader_HPP__
#define __Automaton_CXMLParameterReader_HPP__

#include "../TXMLReader.hpp"
#include "../IXMLNodeReader.h"

#include <iostream>
#include <sstream>
#include <string>
#include <stack>
#include <vector>

namespace Automaton
{

	class CXMLParameterReader : public TXMLReader<IXMLNodeReader>
	{
		public:
			//! Parameters of the associated action
			std::vector<Automaton::CIdentifier>& m_oParameter;

			std::string m_oParameterType;

			EParsingStatus m_eStatus;

		public:
			CXMLParameterReader(std::vector<Automaton::CIdentifier>& oParameter) : m_oParameter(oParameter){};
			virtual ~CXMLParameterReader(){}

			virtual EParsingStatus openChild(const char* sName, const char** sAttributeName, const char** sAttributeValue, uint64 ui64AttributeCount, IAutomatonContext* pContext);
			virtual EParsingStatus processChildData(const char* sData, IAutomatonContext* pContext);
			virtual EParsingStatus closeChild(IAutomatonContext* pContext);

			virtual CIdentifier getNodeIdentifier()
			{
				return Automaton_Node_Identifier_No_Node;
			}

			virtual void release(){ delete this; }


	};

};
#endif

