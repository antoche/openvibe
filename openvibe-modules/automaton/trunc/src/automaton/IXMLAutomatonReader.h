#ifndef __Automaton_IXMLAutomatonReader_H__
#define __Automaton_IXMLAutomatonReader_H__

#include "defines.h"
#include "IAutomatonContext.h"
#include "IAutomatonController.h"

namespace Automaton
{

	class Automaton_API IXMLAutomatonReader
	{
		public:
			virtual void release() = 0;

			virtual Automaton::boolean processData(const void * pBuffer, const Automaton::uint64 ui64BufferSize) = 0;

		virtual IAutomatonController* getAutomatonController() = 0;

		protected:
			virtual ~IXMLAutomatonReader() {}

	};

	extern Automaton_API IXMLAutomatonReader* createXMLAutomatonReader();
	extern Automaton_API void releaseXMLAutomatonReader(IXMLAutomatonReader* pReader);

};

#endif

