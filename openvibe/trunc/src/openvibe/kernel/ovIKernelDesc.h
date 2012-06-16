#ifndef __OpenViBE_Kernel_IKernelDesc_H__
#define __OpenViBE_Kernel_IKernelDesc_H__

#include "ovIKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class IKernelContext;

		/**
		 * \class IKernelDesc
		 * \brief A kernel description
		 * \author Yann Renard (INRIA/IRISA)
		 * \date 2006-09-26
		 * \ingroup Group_Kernel
		 *
		 * This interface is implemented to provide information on a specific kernel
		 * implementation and to create instances of this specific kernel implementation.
		 */
		class OV_API IKernelDesc : public OpenViBE::Kernel::IKernelObject
		{
		public:

			/** \name Creation process */
			//@{

			/**
			 * \brief Creates the kernel itself
			 * \param rApplicationName [in] : the name of the application requesting kernel creation
			 *                                (an configuration token will be created so the configuration
			 *                                file can be tweaked according to the targeted application)
			 * \param rConfigurationFilename [in] : a bootstrap configuration file
			 * \return the created kernel.
			 *
			 * This method creates the kernel itself and returns it.
			 */
			virtual OpenViBE::Kernel::IKernelContext* createKernel(const OpenViBE::CString& rApplicationName, const OpenViBE::CString& rConfigurationFilename)=0;
			/**
			 * \brief Creates the kernel itself and make it sub kernel of a master kernel
			 * \param rMasterKernel [in] : the master kernel
			 * \param rApplicationName [in] : the name of the application requesting kernel creation
			 *                                (an configuration token will be created so the configuration
			 *                                file can be tweaked according to the targeted application)
			 * \param rConfigurationFilename [in] : a bootstrap configuration file
			 * \return the created kernel.
			 *
			 * This method creates the kernel itself and returns it.
			 */
			virtual OpenViBE::Kernel::IKernelContext* createKernel(const OpenViBE::Kernel::IKernelContext& rMasterKernel, const OpenViBE::CString& rApplicationName, const OpenViBE::CString& rConfigurationFilename)=0;
			/**
			 * \brief Releases the kernel itself
			 * \param pKernel [in] : the kernel to release
			 *
			 * This method releases an existing kernel.
			 */
			virtual void releaseKernel(OpenViBE::Kernel::IKernelContext* pKernel)=0;

			//@}
			/** \name Textual plugin object description and information */
			//@{

			/**
			 * \brief Gets the plugin name
			 * \return The plugin name.
			 *
			 * Default implementation simply returns empty string.
			 */
			virtual OpenViBE::CString getName(void) const { return CString("no name"); }
			/**
			 * \brief Gets the author name for this plugin
			 * \return The author name for this plugin.
			 *
			 * Default implementation simply returns empty string.
			 */
			virtual OpenViBE::CString getAuthorName(void) const { return CString("unknown"); }
			/**
			 * \brief Gets the author company name for this plugin
			 * \return The author company name for this plugin.
			 *
			 * Default implementation simply returns empty string.
			 */
			virtual OpenViBE::CString getAuthorCompanyName(void) const { return CString("unknown"); }
			/**
			 * \brief Gets a short description of the plugin
			 * \return A short description of the plugin.
			 *
			 * Default implementation simply returns empty string.
			 */
			virtual OpenViBE::CString getShortDescription(void) const { return CString(""); }
			/**
			 * \brief Gets a detailed description of the plugin
			 * \return A detailed description of the plugin.
			 *
			 * Default implementation simply returns empty string.
			 *
			 * \note You can use std::endl to have the description
			 *       on several lines when needed.
			 */
			virtual OpenViBE::CString getDetailedDescription(void) const { return CString(""); }
			/**
			 * \brief Gets the version of the plugin
			 * \return the version of the plugin.
			 *
			 * Default implementation simply returns empty string.
			 */
			virtual OpenViBE::CString getVersion(void) const { return CString("unknown"); }

			//@}

			_IsDerivedFromClass_(OpenViBE::Kernel::IKernelObject, OV_ClassId_Kernel_KernelDesc)
		};
	};
};

#endif // __OpenViBE_Kernel_IKernelDesc_H__
