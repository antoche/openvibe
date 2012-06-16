#ifndef __Automaton_CNodeAction_H__
#define __Automaton_CNodeAction_H__

#include "../INode.h"
#include "TNode.inl"
#include "../CIdentifier.h"
#include "../IAutomatonContext.h"

#include <vector>
#include <string>
#include <algorithm>

#include <iostream>

namespace Automaton
{
	class IActionFunctor
	{
		public:
			virtual Automaton::boolean operator()(IAutomatonContext*) = 0;
			virtual ~IActionFunctor(){}
	};

	class CSendEvent : public IActionFunctor
	{
		private:
			Automaton::CIdentifier m_oEvent;

		public:
			CSendEvent(Automaton::uint64 ui64Event)
				: m_oEvent(ui64Event) {}

			virtual ~CSendEvent(){}

			virtual Automaton::boolean operator()(IAutomatonContext* pContext)
			{
#ifdef DEBUG
				std::cout<<"<<<<<<<<<<<<<<sending event : "<<std::dec<<(uint64)m_oEvent<<std::endl;
#endif
				pContext->sendEvent(m_oEvent);

				return true;
			}
	};

	class CWaitEvent : public IActionFunctor
	{
		private:
			Automaton::CIdentifier m_oEvent;
			size_t m_uiFoundEventIndex;

		public:
			CWaitEvent(Automaton::uint64 ui64Event)
				: m_oEvent(ui64Event), m_uiFoundEventIndex(size_t(-1)) {}

			virtual ~CWaitEvent(){}

			virtual Automaton::boolean operator()(IAutomatonContext* pContext)
			{
#ifdef DEBUG
				std::cout<<"waiting for event : "<<std::dec<<(uint64)m_oEvent<<std::endl;
#endif
				const CIdentifier* l_pReceivedEvents = pContext->getReceivedEvents();

				if(l_pReceivedEvents)
				{
					const CIdentifier* l_pEnd = l_pReceivedEvents + pContext->getReceivedEventsCount();
					size_t l_uiFoundEventIndex = (size_t)(std::find(l_pReceivedEvents+(m_uiFoundEventIndex==size_t(-1)?0:m_uiFoundEventIndex+1), l_pEnd, m_oEvent) - l_pReceivedEvents);
					if(l_uiFoundEventIndex != pContext->getReceivedEventsCount())
					{
						m_uiFoundEventIndex=l_uiFoundEventIndex;
						return true;
					}
					return false;
				}
				return false;
			}
	};

	class CWaitTime : public IActionFunctor
	{
		private:
			Automaton::boolean m_bTimeSet;
			Automaton::uint64 m_ui64TimeLength;
			Automaton::uint64 m_ui64LowerBound;
			Automaton::uint64 m_ui64HigherBound;
			Automaton::uint64 m_ui64EndTime;
		public:

			Automaton::uint64 convertTime(Automaton::uint64 ui64TimeLength)
			{
				return (ui64TimeLength<<32)/1000;
			}

			CWaitTime(Automaton::uint64 ui64TimeLength) :
				m_bTimeSet(false),
				m_ui64LowerBound(0),
				m_ui64HigherBound(0)
			{
				m_ui64TimeLength = convertTime(ui64TimeLength);
			}

			CWaitTime(Automaton::uint64 ui64Low, Automaton::uint64 ui64High) :
	       			m_bTimeSet(false),
				m_ui64LowerBound(0),
				m_ui64HigherBound(0)
			{

				m_ui64LowerBound=ui64Low;
				m_ui64HigherBound=ui64High;
			}

			virtual ~CWaitTime(){}

			virtual Automaton::boolean operator()(IAutomatonContext* pContext)
			{
				if(!m_bTimeSet)
				{
					//case of random duration
					if(m_ui64LowerBound!=m_ui64HigherBound)
					{
						uint64 l_ui64DurationMs = (rand() % (m_ui64HigherBound - m_ui64LowerBound)) + m_ui64LowerBound;

						//converts it to 32:32 format
						m_ui64TimeLength = convertTime(l_ui64DurationMs);
					}

					m_ui64EndTime = pContext->getCurrentTime() + m_ui64TimeLength;
					m_bTimeSet = true;
				}
				else
				{
					if(pContext->getCurrentTime() >= m_ui64EndTime)
					{
						//the next time we go through this node, the timer will be reset
						m_bTimeSet=false;

						return true;
					}
				}

				return false;
			}

	};

	class CNodeAction : public virtual TNode<INode>
	{
		IActionFunctor * m_pAction;

		public:
			CNodeAction() : m_pAction(NULL) {}

			virtual ~CNodeAction(){ delete m_pAction; }

			virtual Automaton::boolean process(IAutomatonContext * pContext);

			virtual Automaton::CIdentifier getSuccessor(IAutomatonContext * pContext)
			{
				if(TNode<INode>::m_oSuccessor.size() == 0)
				{
					return Automaton_No_Successor;
				}
				else
				{
					return TNode<INode>::m_oSuccessor[0];
				}
			}

			virtual Automaton::boolean setAction(const std::string& oAction, const std::vector<Automaton::CIdentifier>& oParameter, IAutomatonContext * pContext);

	};

};

#endif

