#ifndef __OpenViBEToolkit_Reader_TBoxAlgorithmEBMLInputReaderCallback_INL__
#define __OpenViBEToolkit_Reader_TBoxAlgorithmEBMLInputReaderCallback_INL__

#include "ovtkIBoxAlgorithmEBMLInputReaderCallback.h"

#include <ebml/IReader.h>
#include <ebml/IReaderHelper.h>

#include <stack>
#include <vector>
#include <string>

namespace OpenViBEToolkit
{
	template <class IBaseInterface>
	class TBoxAlgorithmEBMLInputReaderCallback : public IBaseInterface
	{
	public:

		virtual EBML::boolean isMasterChild(const EBML::CIdentifier& rIdentifier)
		{
			if(rIdentifier==OVTK_NodeId_Header)                    { return true; }
			else if(rIdentifier==OVTK_NodeId_Header_StreamVersion) { return false; }
			else if(rIdentifier==OVTK_NodeId_Header_StreamType)    { return false; }
			else if(rIdentifier==OVTK_NodeId_Header)               { return true; }
			else if(rIdentifier==OVTK_NodeId_Buffer)               { return true; }
			else if(rIdentifier==OVTK_NodeId_End)                  { return true; }
			return false;
		}

		virtual void openChild(const EBML::CIdentifier& rIdentifier)
		{
		}

		virtual void processChildData(const void* pBuffer, const EBML::uint64 ui64BufferSize)
		{
		}

		virtual void closeChild(void)
		{
		}

		_IsDerivedFromClass_Final_(IBaseInterface, OVTK_ClassId_);
	};
};

#endif // __OpenViBEToolkit_Reader_TBoxAlgorithmEBMLInputReaderCallback_INL__
