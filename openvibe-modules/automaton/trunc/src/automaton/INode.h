#ifndef __Automaton_INode_H__
#define __Automaton_INode_H__

#include "CIdentifier.h"
#include "defines.h"
#include "IAutomatonContext.h"

namespace Automaton
{
	class IAutomatonContext;

	class Automaton_API INode
	{
		public:
			virtual ~INode(){}

		public:
			virtual Automaton::boolean preProcess(Automaton::IAutomatonContext * pContext) = 0;

			virtual Automaton::boolean postProcess(Automaton::IAutomatonContext * pContext) = 0;

			virtual Automaton::boolean process(Automaton::IAutomatonContext * pContext) = 0;

			virtual Automaton::CIdentifier getSuccessor(IAutomatonContext * pContext) = 0;
			virtual void addSuccessor(CIdentifier oSuccessorIdentifier) = 0;

	};


};

#endif

