#ifndef __Automaton_INodeFactory_H__
#define __Automaton_INodeFactory_H__

#include "CIdentifier.h"
#include "INode.h"

namespace Automaton
{
	class Automaton_API INodeFactory
	{
		public:
			virtual INode * createNode(const CIdentifier& oIdentifier) = 0;
			virtual void releaseNode(INode * pNode) = 0;
			virtual void release() = 0;

		protected:
			virtual ~INodeFactory(){}
	};

	extern Automaton_API Automaton::INodeFactory* createNodeFactory();
	extern Automaton_API void releaseNodeFactory(Automaton::INodeFactory* pFactory);
};

#endif

