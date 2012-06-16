#ifndef __OpenViBE_Plugins_IBoxAlgorithmDesc_H__
#define __OpenViBE_Plugins_IBoxAlgorithmDesc_H__

#include "ovIPluginObjectDesc.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class IBoxProto;
		class IBoxAlgorithmContext;

		/**
		 * \brief This enum lists all the way a box can be modified
		 * \sa OpenViBE::Plugins::IBoxAlgorithmDesc::boxModificationCallback
		 */
		enum EBoxModification
		{
			BoxModification_InputAdded,
			BoxModification_InputRemoved,
			BoxModification_InputTypeChanged,
			BoxModification_OutputAdded,
			BoxModification_OutputRemoved,
			BoxModification_OutputTypeChanged,
			BoxModification_SettingAdded,
			BoxModification_SettingRemoved,
			BoxModification_SettingTypeChanged,
			BoxModification_SettingDefaultValueChanged,
			BoxModification_SettingValueChanged,
		};
	};

	namespace Plugins
	{
		/**
		 * \class IBoxAlgorithmDesc
		 * \author Yann Renard (INRIA/IRISA)
		 * \date 2006-06-19
		 * \brief Box algorithm plugin descriptor
		 * \ingroup Group_Extend
		 *
		 * This class should be derived by any plugin developper in
		 * order to describe a specific §OpenViBE§ box algorithm.
		 *
		 * \sa OpenViBE::Plugins::IBoxAlgorithm
		 *
		 * \todo details about building new plugins
		 */
		class OV_API IBoxAlgorithmDesc : public OpenViBE::Plugins::IPluginObjectDesc
		{
		public:

			/** \name Box prototype and description */
			//@{

			/**
			 * \brief Gets the box prototype for this algorithm
			 * \param rPrototype [out] : the box prototype to fill
			 * \return \e true in case of success \e false in other cases.
			 *
			 * When this function is called by the §OpenViBE§
			 * platform, the plugin descriptor should fill in
			 * the structure to let the §OpenViBE§ platform know
			 * whet the corresponding box should look like
			 * (inputs/outputs/settings).
			 *
			 * \sa OpenViBE::Plugins::IBoxProto
			 */
			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rPrototype) const=0;
			/**
			 * \brief Gets the stock item to display with this algorithm
			 * \return The stock item to display with this algorithm.
			 *
			 * This item name will be used by the GUI to display
			 * a symbol to the algorithm list so a user can quickly
			 * find them in the list.
			 *
			 * Default implementation returns empty string. If
			 * the item can not be found by name, or an empty string
			 * is returned, a default item will be displayed.
			 */
			virtual OpenViBE::CString getStockItemName(void) const
			{
				return OpenViBE::CString("");
			}

			//@}
			/** \name Box modifications callbacks */
			//@{

			/**
			 * \brief This callback is called when the box is modified in some way
			 * \param rBoxAlgorithmContext [in] : the box algorithm context
			 *        containing the static description of the box
			 * \param eBoxModificationType [in] : the type of modification
			 *        applied to the box
			 *
			 * This function is called as soon as a modification is done
			 * on a box of this algorithm class. The algorithm descriptor
			 * is then allowed to examine and check box status validity
			 * and to adpat the box itself according to this validity.
			 *
			 * In such context, the box algorithm descriptor should
			 * only access the static box context. Neither the dynamic box,
			 * the player context nor the box state marker will be valid !
			 *
			 * \sa OpenViBE::Plugins::IBoxProto
			 * \sa OpenViBE::Plugins::EBoxModification
			 */
			virtual void boxModificationCallback(
				OpenViBE::Kernel::IBoxAlgorithmContext& rBoxAlgorithmContext,
				const OpenViBE::Kernel::EBoxModification eBoxModificationType) const
			{
			}

			//@}

			_IsDerivedFromClass_(OpenViBE::Plugins::IPluginObjectDesc, OV_ClassId_Plugins_BoxAlgorithmDesc)
		};
	};
};

#endif // __OpenViBE_Plugins_IBoxAlgorithmDesc_H__
