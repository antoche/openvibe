#ifndef __OpenViBE_Kernel_Scenario_IBox_H__
#define __OpenViBE_Kernel_Scenario_IBox_H__

#include "ovIAttributable.h"

namespace OpenViBE
{
	namespace Kernel
	{
		/**
		 * \class IBox
		 * \author Yann Renard (IRISA/INRIA)
		 * \date 2006-08-16
		 * \brief Complete §OpenViBE§ box interface
		 * \ingroup Group_Scenario
		 * \ingroup Group_Kernel
		 *
		 * This interface can be used in order to fully describe an
		 * §OpenViBE§ black box. It describes its identification values,
		 * its inputs, its outputs and its settings.
		 */
		class OV_API IBox : public OpenViBE::Kernel::IAttributable
		{
		public:

			/** \name Box naming and identification */
			//@{

			/**
			 * \brief Gets the identifier of this box
			 * \return The identifier of this §OpenViBE§ box.
			 */
			virtual OpenViBE::CIdentifier getIdentifier(void) const=0;
			/**
			 * \brief Gets the display name of this box
			 * \return The name of this §OpenViBE§ box.
			 */
			virtual OpenViBE::CString getName(void) const=0;
			/**
			 * \brief Gets the algorithm class identifier
			 * \return This box' algorithm class identifier
			 */
			virtual OpenViBE::CIdentifier getAlgorithmClassIdentifier(void) const=0;
			/**
			 * \brief Gets processing unit identifier for this box
			 * \return This box' processing unit identifier
			 */
			virtual OpenViBE::CIdentifier getProcessingUnitIdentifier(void) const=0;
			/**
			 * \brief Changes the identifier of this box
			 * \param rIdentifier [in] : The new identifier
			 *        this box should take.
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setIdentifier(
				const OpenViBE::CIdentifier& rIdentifier)=0;
			/**
			 * \brief Renames this box
			 * \param sName [in] : The name this box should take
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setName(
				const OpenViBE::CString& sName)=0;
			/**
			 * \brief Changes the algorithm identifier of this box
			 * \param rAlgorithmClassIdentifier [in] : The new algorithm
			 *        identifier this box should take.
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setAlgorithmClassIdentifier(
				const OpenViBE::CIdentifier& rAlgorithmClassIdentifier)=0;
			/**
			 * \brief Changes the processing unit identifier of this box
			 * \param rProcessingUnitIdentifier [in] : The new processing
			 *        unit identifier this box should take.
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setProcessingUnitIdentifier(
				const OpenViBE::CIdentifier& rProcessingUnitIdentifier)=0;

			//@}
			/** \name Initialisation from prototypes etc... */
			//@{

			/**
			 * \brief Initializes the box from box algorithm descriptor
			 * \param rAlgorithmClassIdentifier [in] : The new algorithm
			 *        identifier this box should take.
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 *
			 * Resets the box and initializes its input/output/settings
			 * according to the box algorithm descriptor
			 */
			virtual OpenViBE::boolean initializeFromAlgorithmClassIdentifier(
				const OpenViBE::CIdentifier& rAlgorithmClassIdentifier)=0;
			/**
			 * \brief Initializes the box from an already existing box
			 * \param rExisitingBox [in] : The existing box.
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 *
			 * Resets the box and initializes its input/output/settings
			 * according to the existing box.
			 */
			virtual OpenViBE::boolean initializeFromExistingBox(
				const OpenViBE::Kernel::IBox& rExisitingBox)=0;

			//@}
			/** \name Input management */
			//@{

			/**
			 * \brief Adds an input to this box
			 * \param sName [in] : The input name
			 * \param rTypeIdentifier [in] : The
			 *        input type identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 *
			 * The input is always added after the last
			 * already existing input.
			 */
			virtual OpenViBE::boolean addInput(
				const OpenViBE::CString& sName,
				const OpenViBE::CIdentifier& rTypeIdentifier)=0;
			/**
			 * \brief Removes an input for this box
			 * \param ui32InputIndex [in] : The index
			 *        of the input to be removed
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 *
			 * Inputs coming after the removed input
			 * have their indices changing after this,
			 * they all decrease by 1.
			 */
			virtual OpenViBE::boolean removeInput(
				const OpenViBE::uint32 ui32InputIndex)=0;
			/**
			 * \brief Gets the number of inputs for this box
			 * \return The number of inputs for this box.
			 */
			virtual OpenViBE::uint32 getInputCount(void) const=0;
			/**
			 * \brief Gets an input type identifier
			 * \param ui32InputIndex [in] : The input index
			 * \param rTypeIdentifier [out] : The type identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rTypeIdentifier remains unchanged.
			 */
			virtual OpenViBE::boolean getInputType(
				const OpenViBE::uint32 ui32InputIndex,
				OpenViBE::CIdentifier& rTypeIdentifier) const=0;
			/**
			 * \brief Gets an input name
			 * \param ui32InputIndex [in] : The input index
			 * \param rName [out] : The name of this input
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rName remains unchanged.
			 */
			virtual OpenViBE::boolean getInputName(
				const OpenViBE::uint32 ui32InputIndex,
				OpenViBE::CString& rName) const=0;
			/**
			 * \brief Sets an input type identifier
			 * \param ui32InputIndex [in] : The input index
			 * \param rTypeIdentifier [in] : The type identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setInputType(
				const OpenViBE::uint32 ui32InputIndex,
				const OpenViBE::CIdentifier& rTypeIdentifier)=0;
			/**
			 * \brief Sets an input name
			 * \param ui32InputIndex [in] : The input index
			 * \param rName [in] : The name of this input
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setInputName(
				const OpenViBE::uint32 ui32InputIndex,
				const OpenViBE::CString& rName)=0;

			//@}
			/** \name Output management */
			//@{

			/**
			 * \brief Adds an output to this box
			 * \param sName [in] : The output name
			 * \param rTypeIdentifier [in] : The
			 *        output type idenfitier
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 *
			 * The output is always added after the last
			 * already existing output.
			 */
			virtual OpenViBE::boolean addOutput(
				const OpenViBE::CString& sName,
				const OpenViBE::CIdentifier& rTypeIdentifier)=0;
			/**
			 * \brief Removes an output for this box
			 * \param ui32OutputIndex [in] : The index
			 *        of the output to remove
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 *
			 * Outputs coming after the removed output
			 * have their indices changing after this,
			 * they all decrease by 1.
			 */
			virtual OpenViBE::boolean removeOutput(
				const OpenViBE::uint32 ui32OutputIndex)=0;
			/**
			 * \brief Gets the number of outputs for this box
			 * \return The number of outputs for this box.
			 */
			virtual OpenViBE::uint32 getOutputCount(void) const=0;
			/**
			 * \brief Gets an output type identifier
			 * \param ui32OutputIndex [in] : The output index
			 * \param rTypeIdentifier [out] : The type identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rTypeIdentifier remains unchanged.
			 */
			virtual OpenViBE::boolean getOutputType(
				const OpenViBE::uint32 ui32OutputIndex,
				OpenViBE::CIdentifier& rTypeIdentifier) const=0;
			/**
			 * \brief Gets an output name
			 * \param ui32OutputIndex [in] : The output index
			 * \param rName [out] : The name of this output
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rName remains unchanged.
			 */
			virtual OpenViBE::boolean getOutputName(
				const OpenViBE::uint32 ui32OutputIndex,
				OpenViBE::CString& rName) const=0;
			/**
			 * \brief Sets an output type identifier
			 * \param ui32OutputIndex [in] : The output index
			 * \param rTypeIdentifier [in] : The type identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setOutputType(
				const OpenViBE::uint32 ui32OutputIndex,
				const OpenViBE::CIdentifier& rTypeIdentifier)=0;
			/**
			 * \brief Sets an output name
			 * \param ui32OutputIndex [in] : The output index
			 * \param rName [in] : The name of this output
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setOutputName(
				const OpenViBE::uint32 ui32OutputIndex,
				const OpenViBE::CString& rName)=0;

			//@}
			/** \name Setting management */
			//@{

			/**
			 * \brief Adds a setting to this box
			 * \param sName [in] : The setting name
			 * \param rTypeIdentifier [in] : The
			 *        setting type identifier
			 * \param sDefaultValue [in] : The default
			 *        value for this setting
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 *
			 * The setting is always added after the last
			 * already existing setting.
			 */
			virtual OpenViBE::boolean addSetting(
				const OpenViBE::CString& sName,
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const OpenViBE::CString& sDefaultValue)=0;
			/**
			 * \brief Removes a setting for this box
			 * \param ui32SettingIndex [in] : The index
			 *        of the setting to remove
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 *
			 * Settings coming after the removed setting
			 * have their indices changing after this,
			 * they all decrease by 1.
			 */
			virtual OpenViBE::boolean removeSetting(
				const OpenViBE::uint32 ui32SettingIndex)=0;
			/**
			 * \brief Gets the number of settings for this box
			 * \return The number of settings for this box.
			 */
			virtual OpenViBE::uint32 getSettingCount(void) const=0;
			/**
			 * \brief Gets a seting type identifier
			 * \param ui32SettingIndex [in] : The setting index
			 * \param rTypeIdentifier [out] : The type identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rTypeIdentifier remains unchanged.
			 */
			virtual OpenViBE::boolean getSettingType(
				const OpenViBE::uint32 ui32SettingIndex,
				OpenViBE::CIdentifier& rTypeIdentifier) const=0;
			/**
			 * \brief Gets a setting name
			 * \param ui32SettingIndex [in] : The setting index
			 * \param rName [out] : The name of this setting
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rName remains unchanged.
			 */
			virtual OpenViBE::boolean getSettingName(
				const OpenViBE::uint32 ui32SettingIndex,
				OpenViBE::CString& rName) const=0;
			/**
			 * \brief Gets the default setting value
			 * \param ui32SettingIndex [in] : The setting index
			 * \param rDefaultValue [out] : The default value
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rDefaultValue remains unchanged.
			 */
			virtual OpenViBE::boolean getSettingDefaultValue(
				const OpenViBE::uint32 ui32SettingIndex,
				OpenViBE::CString& rDefaultValue) const=0;
			/**
			 * \brief Gets the setting value
			 * \param ui32SettingIndex [in] : The setting index
			 * \param rValue [out] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rValue remains unchanged.
			 */
			virtual OpenViBE::boolean getSettingValue(
				const OpenViBE::uint32 ui32SettingIndex,
				OpenViBE::CString& rValue) const=0;
			/**
			 * \brief Sets a setting type identifier
			 * \param ui32SettingIndex [in] : The setting index
			 * \param rTypeIdentifier [in] : The type identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setSettingType(
				const OpenViBE::uint32 ui32SettingIndex,
				const OpenViBE::CIdentifier& rTypeIdentifier)=0;
			/**
			 * \brief Sets an setting name
			 * \param ui32SettingIndex [in] : The setting index
			 * \param rName [in] : The name of this setting
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setSettingName(
				const OpenViBE::uint32 ui32SettingIndex,
				const OpenViBE::CString& rName)=0;
			/**
			 * \brief Sets the default setting value
			 * \param ui32SettingIndex [in] : The setting index
			 * \param rDefaultValue [in] : The default value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setSettingDefaultValue(
				const OpenViBE::uint32 ui32SettingIndex,
				const OpenViBE::CString& rDefaultValue)=0;
			/**
			 * \brief Sets the setting value
			 * \param ui32SettingIndex [in] : The setting index
			 * \param rValue [in] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setSettingValue(
				const OpenViBE::uint32 ui32SettingIndex,
				const OpenViBE::CString& rValue)=0;

			//@}

			_IsDerivedFromClass_(OpenViBE::Kernel::IAttributable, OV_ClassId_Kernel_Scenario_Box)
		};

		typedef OpenViBE::Kernel::IBox IStaticBoxContext;
	};
};

#endif // __OpenViBE_Kernel_Scenario_IBox_H__
