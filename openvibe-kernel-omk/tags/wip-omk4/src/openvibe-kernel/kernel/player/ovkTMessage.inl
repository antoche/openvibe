#include "ovkTMessage.h"

namespace OpenViBE {
namespace Kernel {

//___________________________________________________________________//
//                                                                   //

template <class T>
TMessage<T>::TMessage(const IKernelContext& rKernelContext)
	:T(rKernelContext)
	,m_oIdentifier(OV_UndefinedIdentifier)
	,m_ui64Time(0)
{
}

//___________________________________________________________________//
//                                                                   //

template <class T>
CIdentifier TMessage<T>::getIdentifier(void) const
{
	return m_oIdentifier;
}

template <class T>
uint64 TMessage<T>::getTime(void) const
{
	return m_ui64Time;
}

template <class T>
boolean TMessage<T>::setIdentifier(
	const CIdentifier& rIdentifier)
{
	if(m_oIdentifier!=OV_UndefinedIdentifier)
	{
		return false;
	}
	if(rIdentifier==OV_UndefinedIdentifier)
	{
		return false;
	}
	m_oIdentifier=rIdentifier;
	return true;
}

template <class T>
boolean TMessage<T>::setTime(
	const uint64 ui64Time)
{
	m_ui64Time=ui64Time;
	return true;
}

//___________________________________________________________________//
//                                                                   //

};
};
