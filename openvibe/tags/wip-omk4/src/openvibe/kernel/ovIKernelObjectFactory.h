#ifndef __OpenViBE_Kernel_IKernelObjectFactory_H__
#define __OpenViBE_Kernel_IKernelObjectFactory_H__

#include "ovIKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		/**
		 * \class IKernelObjectFactory
		 * \brief Kernel object factory, creates all kernel objects
		 * \author Yann Renard (INRIA/IRISA)
		 * \date 2006-09-26
		 * \ingroup Group_Kernel
		 *
		 * This class allows to create kernel objects as needed.
		 *
		 * \todo should it be removed ?
		 */
		class OV_API IKernelObjectFactory : public OpenViBE::Kernel::IKernelObject
		{
		public:

			/**
			 * \brief Creates a new kernel object givent its class identifier
			 * \param rClassIdentifier [in] : the class identifier of the object to create
			 * \return a pointer on the created object in case of success.
			 * \return \c NULL in case of error.
			 */
			virtual OpenViBE::IObject* createObject(
				const OpenViBE::CIdentifier& rClassIdentifier)=0;
			/**
			 * \brief Releases an object created by this factory
			 * \param pObject [in] : the object to release
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 * \note The factory should have created the object in order to release it.
			 */
			virtual OpenViBE::boolean releaseObject(
				OpenViBE::IObject* pObject)=0;

			_IsDerivedFromClass_(OpenViBE::Kernel::IKernelObject, OV_ClassId_Kernel_KernelObjectFactory)
		};
	};
};

#endif // __OpenViBE_Kernel_IKernelObjectFactory_H__
