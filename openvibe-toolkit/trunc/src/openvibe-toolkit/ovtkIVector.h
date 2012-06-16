#ifndef __OpenViBEToolkit_IVector_H__
#define __OpenViBEToolkit_IVector_H__

#include "ovtkIObject.h"

namespace OpenViBEToolkit
{
	class OVTK_API IVector : public OpenViBEToolkit::IObject
	{
	public:

		virtual OpenViBE::uint32 getSize(void) const=0;
		virtual OpenViBE::boolean setSize(const OpenViBE::uint32 ui32Size)=0;
		virtual OpenViBE::float64* getBuffer(void)=0;
		virtual const OpenViBE::float64* getBuffer(void) const=0;
		virtual const char* getElementLabel(const OpenViBE::uint32 ui32Index) const=0;
		virtual OpenViBE::boolean setElementLabel(const OpenViBE::uint32 ui32Index, const char* sElementLabel)=0;

		_IsDerivedFromClass_(OpenViBEToolkit::IObject, OVTK_ClassId_Vector);

		const OpenViBE::float64& operator [] (const OpenViBE::uint32 ui32Index) const
		{
			return this->getBuffer()[ui32Index];
		}

		OpenViBE::float64& operator [] (const OpenViBE::uint32 ui32Index)
		{
			return this->getBuffer()[ui32Index];
		}
	};
};

#endif // __OpenViBEToolkit_IVector_H__
