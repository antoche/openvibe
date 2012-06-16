#ifndef __Automaton_TNode_INL__
#define __Automaton_TNode_INL__

#include "../INode.h"
#include "../CIdentifier.h"
#include <vector>

namespace Automaton
{
	template <class T> class TNode : public T
	{
		protected:
			std::vector<Automaton::CIdentifier> m_oSuccessor;

		public:
			virtual ~TNode(){}

			virtual Automaton::boolean preProcess(IAutomatonContext * pContext){ return true; }
			virtual Automaton::boolean postProcess(IAutomatonContext * pContext){ return true; }
			virtual Automaton::boolean process(IAutomatonContext * pContext){ return true; }

			virtual CIdentifier getSuccessor(IAutomatonContext * pContext){ return Automaton_No_Successor; }

			virtual void addSuccessor(CIdentifier oSuccessorIdentifier)
			{
				m_oSuccessor.push_back(oSuccessorIdentifier);
			}



	};
};

#endif

