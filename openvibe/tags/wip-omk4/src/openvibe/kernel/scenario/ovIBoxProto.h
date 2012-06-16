#ifndef __OpenViBE_Kernel_Scenario_IBoxProto_H__
#define __OpenViBE_Kernel_Scenario_IBoxProto_H__

#include "../ovIKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		/**
		 * \class IBoxProto
		 * \author Yann Renard (INRIA/IRISA)
		 * \date 2006-07-05
		 * \brief §OpenViBE§ box prototype
		 * \ingroup Group_Scenario
		 * \ingroup Group_Kernel
		 * \ingroup Group_Extend
		 *
		 * This class is used by a plugin algorithm descriptor
		 * to let the §OpenViBE§ platform know what an algorithm
		 * box looks like. It declares several things, like
		 * it input types, output types and settings.
		 *
		 * \sa OpenViBE::Kernel::IBoxAlgorithmDesc
		 */
		class OV_API IBoxProto : public OpenViBE::Kernel::IKernelObject
		{
		public:

			/**
			 * \brief Adds an input to the box
			 * \param sName [in] : the name of the input to add
			 * \param rTypeIdentifier [in] : the type of the input
			 * \return The created input index.
			 */
			virtual OpenViBE::uint32 addInput(
				const OpenViBE::CString& sName,
				const OpenViBE::CIdentifier& rTypeIdentifier)=0;
			/**
			 * \brief Adds an output to the box
			 * \param sName [in] : the name of the output to add
			 * \param rTypeIdentifier [in] : the type of the output
			 * \return The created output index.
			 */
			virtual OpenViBE::uint32 addOutput(
				const OpenViBE::CString& sName,
				const OpenViBE::CIdentifier& rTypeIdentifier)=0;
			/**
			 * \brief Adds an setting to the box
			 * \param sName [in] : the name of the setting to add
			 * \param rTypeIdentifier [in] : the type of the setting
			 * \param sDefaultValue [in] : the default value of this
			 *        setting (used to initialize the box itself)
			 * \return The created setting index.
			 */
			virtual OpenViBE::uint32 addSetting(
				const OpenViBE::CString& sName,
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const OpenViBE::CString& sDefaultValue)=0;

			_IsDerivedFromClass_(OpenViBE::Kernel::IKernelObject, OV_ClassId_Kernel_Scenario_BoxProto)
		};
	};
};

#endif // __OpenViBE_Kernel_Scenario_IBoxProto_H__
