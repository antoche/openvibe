#ifndef __OpenViBEKernel_Tools_Identifier_Map_Iterator_H__
#define __OpenViBEKernel_Tools_Identifier_Map_Iterator_H__

#include "../ovk_base.h"

#include <openvibe/ov_all.h>

#include <map>

namespace
{
	template <class T>
	OpenViBE::CIdentifier getNextIdentifier(const std::map<OpenViBE::CIdentifier, T>& rMap, const OpenViBE::CIdentifier& rPreviousIdentifier)
	{
		typename std::map<OpenViBE::CIdentifier, T>::const_iterator it;

		if(rPreviousIdentifier==OV_UndefinedIdentifier)
		{
			it=rMap.begin();
		}
		else
		{
			it=rMap.find(rPreviousIdentifier);
			if(it==rMap.end())
			{
				return OV_UndefinedIdentifier;
			}
			it++;
		}

		return it!=rMap.end()?it->first:OV_UndefinedIdentifier;
	}
};

#endif // __OpenViBEKernel_Tools_Identifier_Map_Iterator_H__
