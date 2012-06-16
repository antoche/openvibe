#ifndef __OpenViBE_Kernel_Scenario_IProcessingUnit_H__
#define __OpenViBE_Kernel_Scenario_IProcessingUnit_H__

#include "ovIAttributable.h"

namespace OpenViBE
{
	namespace Kernel
	{
		/**
		 * \class IProcessingUnit
		 * \author Yann Renard (IRISA/INRIA)
		 * \date 2006-08-16
		 * \brief Processing unit information for §OpenViBE§ boxes
		 * \ingroup Group_Scenario
		 * \ingroup Group_Kernel
		 */
		class OV_API IProcessingUnit : public OpenViBE::Kernel::IAttributable
		{
		public:

			/** \name Identification */
			//@{

			/**
			 * \brief Changes this processing unit's identifier
			 * \param rIdentifier [in] : The new identifier
			 *        this processing unit should have
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setIdentifier(
				const OpenViBE::CIdentifier& rIdentifier)=0;
			/**
			 * \brief Gets the identifier of this processing unit
			 * \return The identifier of this processing unit.
			 */
			virtual OpenViBE::CIdentifier getIdentifier(void) const=0;
			/**
			 * \brief Sets the name of this processing unit
			 * \param sName [in] : the new name for this processing unit
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setName(
				const OpenViBE::CString& sName)=0;
			/**
			 * \brief Gets the name of this processing unit
			 * \return the name of this processing unit
			 */
			virtual OpenViBE::CString getName(void) const=0;

			//@}

			_IsDerivedFromClass_(OpenViBE::Kernel::IAttributable, OV_ClassId_Kernel_Scenario_ProcessingUnit)
		};
	};
};

#endif // __OpenViBE_Kernel_Scenario_IProcessingUnit_H__
