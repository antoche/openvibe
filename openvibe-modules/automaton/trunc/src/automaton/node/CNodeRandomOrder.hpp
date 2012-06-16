#ifndef __Automaton_CNodeRandomOrder_HPP__
#define __Automaton_CNodeRandomOrder_HPP__

#include "../INode.h"
#include "TNode.inl"
#include "../IAutomatonContext.h"

#include "../CIdentifier.h"

#include <string>
#include <vector>
#include <iostream>

#include <cstdlib>
#include <ctime>

namespace Automaton
{
	class CNodeRandomOrder : public TNode<INode>
	{
		protected:

			Automaton::uint64 m_ui64BranchNumber;

			Automaton::uint64 m_ui64IterationCount;
			Automaton::uint64 m_ui64BranchCount;

			std::vector<std::pair<Automaton::uint64, Automaton::uint64> > m_oBranchCount;

		public:

			CNodeRandomOrder(void)
				:m_ui64BranchNumber(0)
				,m_ui64IterationCount(0)
			{
			}

			virtual ~CNodeRandomOrder(void)
			{
			}

			virtual Automaton::boolean process(IAutomatonContext*);
			virtual Automaton::CIdentifier getSuccessor(IAutomatonContext*);

			virtual void setIterationCount(const std::vector<Automaton::CIdentifier>& oParameter, IAutomatonContext * pContext);
	};
};

#endif

