#ifndef __OpenViBE_IObject_H__
#define __OpenViBE_IObject_H__

#include "ov_base.h"
#include "ovCIdentifier.h"

namespace OpenViBE
{
	class IObjectVisitor;

	namespace Kernel
	{
		class CKernelObjectFactory;
	};

#define _IsDerivedFromClass_(_SuperClassName_,_ClassIdentifier_) \
	virtual OpenViBE::boolean isDerivedFromClass( \
		const OpenViBE::CIdentifier& rClassIdentifier) const \
	{ \
		return ((rClassIdentifier==_ClassIdentifier_) \
		     || _SuperClassName_::isDerivedFromClass(rClassIdentifier)); \
	} \
	\
	virtual OpenViBE::boolean getClassHierarchy( \
		OpenViBE::CIdentifier* pClassIdentifier, \
		OpenViBE::uint32* pClassNumber) const \
	{ \
		getClassHierarchy(pClassIdentifier+1, pClassNumber); \
		(*pClassIdentifier)=_ClassIdentifier_; \
		(*pClassNumber)++; \
		return true; \
	}

#define _IsDerivedFromClass_Final_(_SuperClassName_,_ClassIdentifier_) \
	_IsDerivedFromClass_(_SuperClassName_,_ClassIdentifier_) \
	virtual OpenViBE::CIdentifier getClassIdentifier(void) const \
	{ \
		return _ClassIdentifier_; \
	}

	/**
	 * \class IObject
	 * \author Yann Renard (INRIA/IRISA)
	 * \date 2006-06-16
	 * \brief Base class for all the §OpenViBE§ platform objects
	 *
	 * Most of the complex objects existing in the §OpenViBE§ platform
	 * should derive this base interface. Thus, several operations can
	 * be performed in order to work on the object. The most important
	 * may be the isDerivedFromClass method that allows the caller to
	 * know it the object has specific interface implementation and if
	 * the object could finally be casted in this interface or subclass.
	 * Several interfaces are provided in the §OpenViBE§ specficiation
	 * but custom class could also be created. It is the responsability
	 * of the developper to notify the user of what interfaces are
	 * implemented in a concrete class.
	 *
	 * See isDerivedFromClass to have a sample of how this function
	 * could be used.
	 */
	class OV_API IObject
	{
	public:

		friend class OpenViBE::Kernel::CKernelObjectFactory;

		/** \name Class identification */
		//@{

		/**
		 * \brief Returns the final class identifier of the concrete class
		 * \return The class identifier of this object.
		 *
		 * This method should return the class identifier of the
		 * concrete instanciated class.
		 */
		virtual OpenViBE::CIdentifier getClassIdentifier(void) const=0;
		/**
		 * \brief Checks if this object is compatible with a class identifier
		 * \param rClassIdentifier [in] : the class identifier you want
		 *        to test this object compatibility with
		 * \return \e true if this object si compatible with the given
		 *         class identifier (this means the concrete class
		 *         overloads the class with given class identifier)
		 *         and \e false when this object is not compatible.
		 *
		 * This method should be used to check object compatibility
		 * with super classes and interfaces. For any concrete class
		 * instance derived from OpenViBE::IObject, one can check if
		 * plugin functions are implemented and so on... see
		 * OpenViBE::Plugins::IPluginObject for an example...
		 */
		virtual OpenViBE::boolean isDerivedFromClass(
			const OpenViBE::CIdentifier& rClassIdentifier) const
		{
			return (rClassIdentifier==OV_ClassId_Object);
		}

		//@}
		/** \name Visiting processes */
		//@{

		/**
		 * \brief Requests this object to accept a visitor
		 * \param rObjectVisitor [in] : the visitor reference which should act on this object
		 * \return \e true in case of success.
		 * \return \e false in case of error.
		 */
		virtual OpenViBE::boolean acceptVisitor(OpenViBE::IObjectVisitor& rObjectVisitor)
		{
			return true;
		}

		//@}

	protected:

		virtual ~IObject(void) { }
	};
};

namespace OpenViBE
{
	class CNullObject : public OpenViBE::IObject
	{
	public:

		_IsDerivedFromClass_Final_(OpenViBE::IObject, OV_ClassId_Object);
	};
};

#endif // __OpenViBE_IObject_H__
