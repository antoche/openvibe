#ifndef __OpenViBE_Plugins_IBoxAlgorithmDesc_H__
#define __OpenViBE_Plugins_IBoxAlgorithmDesc_H__

#include "ovIPluginObjectDesc.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class IBoxProto;
		class IBoxAlgorithmContext;
	};

	namespace Plugins
	{
		class IBoxListener;

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
			 * what the corresponding box should look like
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

			//@{
			/** \name Box modification monitoring */
			//@{

			/**
			 * \brief Creates a new box listener
			 * \return a new box listener
			 *
			 * This function is called by the kernel when a box instance
			 * is created if any modification flag is set in its prototype.
			 * This box listener will be notified each time the box is modified.
			 *
			 * \sa OpenViBE::Kernel::IBoxProto
			 * \sa OpenViBE::Plugins::IBoxListener
			 */
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener(void) const
			{
				return NULL;
			}
			/**
			 * \brief Releases an existing box listener
			 * \param pBoxListener [in] : the box listener to release
			 *
			 * This function is called by the kernel as soon as it knows
			 * a box listener won't be used any more. In case this descriptor
			 * allocated some memory for this box listener, this memory
			 * can be freed safely, no more call will be done on this
			 * box listener.
			 */
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const
			{
			}

			//@}

			_IsDerivedFromClass_(OpenViBE::Plugins::IPluginObjectDesc, OV_ClassId_Plugins_BoxAlgorithmDesc)
		};
	};
};

#endif // __OpenViBE_Plugins_IBoxAlgorithmDesc_H__
