#ifndef __OpenViBE_Kernel_Plugins_IPluginModule_H__
#define __OpenViBE_Kernel_Plugins_IPluginModule_H__

#include "../ovIKernelObject.h"

namespace OpenViBE
{
	namespace Plugins
	{
		class IPluginObjectDesc;
	};

	namespace Kernel
	{
		/**
		 * \class IPluginModule
		 * \brief Plugin module
		 * \author Yann Renard (INRIA/IRISA)
		 * \date 2006-09-26
		 * \ingroup Group_Plugins
		 * \ingroup Group_Kernel
		 *
		 * Each plugin module is exactly responsible for one module
		 * (ie DLL/so file) containing §OpenViBE§ plugins. It is
		 * able to load/unload this file and to enumerate each of
		 * its plugin object descriptor.
		 */
		class OV_API IPluginModule : public OpenViBE::Kernel::IKernelObject
		{
		public:

			/**
			 * \brief Tries to load a file as an §OpenViBE§ module
			 * \param sName [in] : the name of the file to try to load
			 * \param pError [out] : an optional output string containing the error on load failure
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean load(
				const OpenViBE::CString& sName,
				OpenViBE::CString* pError=NULL)=0;
			/**
			 * \brief Tries to unload the loaded §OpenViBE§ module
			 * \param pError [out] : an optional output string containing the error on unload failure
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean unload(
				OpenViBE::CString* pError=NULL)=0;
			/**
			 * \brief Gets the current filename associated with this plugin module
			 * \param rFileName [out] : the filename of this plugin module
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean getFileName(
				OpenViBE::CString& rFileName) const=0;
			/**
			 * \brief Initializes this plugin module
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 *
			 * This function calls the onInitialize main function of the plugin module.
			 */
			virtual OpenViBE::boolean initialize(void)=0;
			/**
			 * \brief Gets a specific plugin object descriptor
			 * \param ui32Index [in] : the index of the plugin object descriptor to get
			 * \param rpPluginObjectDescription [out] : a pointer on the associated plugin object descriptor
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 *
			 * This function should be called with \c ui32Index ranging from 0 to
			 * the number of plugin object descriptor - 1. The number of plugin object
			 * descriptor is known as soon as this function returns \e false meaning
			 * there are no more descriptors to return.
			 *
			 * This function calls the onGetPluginObjectDescription main function of the plugin module.
			 *
			 * \note \c rpPluginObjectDescription can be \c NULL even if the function returned \e true
			 * \note \c rpPluginObjectDescription IS \c NULL if the function returned \e false
			 * \note It is ok to call this function several times for a same index.
			 */
			virtual OpenViBE::boolean getPluginObjectDescription(
				OpenViBE::uint32 ui32Index,
				OpenViBE::Plugins::IPluginObjectDesc*& rpPluginObjectDescription)=0;
			/**
			 * \brief Uninitializes this plugin module
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 *
			 * This function calls the onUninitialize main function of the plugin module.
			 */
			virtual OpenViBE::boolean uninitialize(void)=0;

			_IsDerivedFromClass_(OpenViBE::Kernel::IKernelObject, OV_ClassId_Kernel_Plugins_PluginModule)
		};
	};
};

#endif // __OpenViBE_Kernel_Plugins_IPluginModule_H__
