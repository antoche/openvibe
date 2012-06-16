#ifndef __OpenViBE_Kernel_Plugins_IPluginManager_H__
#define __OpenViBE_Kernel_Plugins_IPluginManager_H__

#include "../ovIKernelObject.h"

namespace OpenViBE
{
	namespace Plugins
	{
		class IPluginObject;
		class IPluginObjectDesc;

		class IBoxAlgorithm;
		class IBoxAlgorithmDesc;

		class IAlgorithm;
		class IAlgorithmDesc;
	};

	namespace Kernel
	{
		// class IPluginModule;

		/**
		 * \class IPluginManager
		 * \brief Log manager
		 * \author Yann Renard (INRIA/IRISA)
		 * \date 2006-09-26
		 * \ingroup Group_Plugins
		 * \ingroup Group_Kernel
		 *
		 * The plugin manager is in charge of loading/unloading plugin
		 * modules (ie DLL/so files) containing §OpenViBE§ plugins.
		 * It also provides functions in order to list plugin descriptors,
		 * create or release plugin objects...
		 */
		class OV_API IPluginManager : public OpenViBE::Kernel::IKernelObject
		{
		public:

			/** \name Plugin modules/descriptors management */
			//@{

			/**
			 * \brief Loads new DLL/so plugin module file(s)
			 * \param rFileNameWildCard [in] : a wild card with the file(s) to search plugins in
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean addPluginsFromFiles(
				const OpenViBE::CString& rFileNameWildCard)=0;
			/**
			 * \brief Gets next plugin object descriptor identifier
			 * \param rPreviousIdentifier [in] : The identifier
			 *        for the preceeding plugin object descriptor
			 * \return The identifier of the next plugin object descriptor in case of success.
			 * \return \c OV_UndefinedIdentifier on error.
			 * \note Giving \c OV_UndefinedIdentifier as \c rPreviousIdentifier
			 *       will cause this function to return the first plugin object
			 *       descriptor identifier.
			 */
			virtual OpenViBE::CIdentifier getNextPluginObjectDescIdentifier(
				const OpenViBE::CIdentifier& rPreviousIdentifier) const=0;
			/**
			 * \brief Gets next plugin object descriptor identifier given a base class identifier
			 * \param rPreviousIdentifier [in] : The identifier
			 *        for the preceeding plugin object descriptor
			 * \param rBaseClassIdentifier [in] : the class the plugin object descriptor should derive from
			 * \return The identifier of the next plugin object descriptor in case of success.
			 * \return \c OV_UndefinedIdentifier on error.
			 * \note Giving \c OV_UndefinedIdentifier as \c rPreviousIdentifier
			 *       will cause this function to return the first plugin object
			 *       descriptor identifier.
			 */
			virtual OpenViBE::CIdentifier getNextPluginObjectDescIdentifier(
				const OpenViBE::CIdentifier& rPreviousIdentifier,
				const OpenViBE::CIdentifier& rBaseClassIdentifier) const=0;
			/**
			 * \brief Checks if a plugin object can be created or not
			 * \param rClassIdentifier [in] : the class identifier a descriptor should be able to create
			 * \return \e true in case this manager is able to create a plugin object with the provided class identifier.
			 * \return \e false in other case.
			 */
			virtual OpenViBE::boolean canCreatePluginObject(
				const OpenViBE::CIdentifier& rClassIdentifier)=0;
			/**
			 * \brief Gets details on a specific plugin object descriptor
			 * \param rIdentifier [in] : the plugin object descriptor identifier which details should be returned
			 * \return the corresponding plugin object descriptor pointer.
			 * \sa getNextPluginObjectDescIdentifier
			 */
			virtual const OpenViBE::Plugins::IPluginObjectDesc* getPluginObjectDesc(
				const OpenViBE::CIdentifier& rIdentifier) const=0;
			/**
			 * \brief Gets details on a specific plugin object descriptor given the class identifier it should create
			 * \param rClassIdentifier [in] : the plugin object class identifier of the descriptor which details should be returned
			 * \return the corresponding plugin object descriptor pointer.
			 * \sa canCreatePluginObject
			 */
			virtual const OpenViBE::Plugins::IPluginObjectDesc* getPluginObjectDescCreating(
				const OpenViBE::CIdentifier& rClassIdentifier) const=0;

			//@}
			/** \name Plugin lifetime management */
			//@{

			/**
			 * \brief Gets a hash value for a given plugin
			 * \param rClassIdentifier [in] : the class identifier of the plugin which hash value
			 *        has to be returned
			 * \return a hash code for the corresponding plugin object
			 *
			 * This function can be used to compute a has code of the described plugin object
			 * If this hash code differs from session to session, it means that the plugin
			 * descriptor changed in some way.
			 *
			 * \note The method used to compute the hash code is specific to the class
			 *       of plugin that is considered (for example, for box algorithms, the
			 *       hash code is based on what OpenViBE::Kernel::IBoxProto receives at
			 *       description stage)
			 */
			virtual OpenViBE::CIdentifier getPluginObjectHashValue(
				const OpenViBE::CIdentifier& rClassIdentifier) const=0;
			/**
			 * \brief Gets a hint whether a plugin is deprecated or not
			 * \param rClassIdentifier [in] : the class identifier of the plugin which deprecation should be returned
			 * \return \e true in case the plugin is deprecated
			 * \return \e false in case the plugin is not deprecated
			 *
			 * If this function returns \e true, this means that the considered
			 * plugin is still valid and functionnal but that it will
			 * be removed soon or later. Code relying on this plugin
			 * should consider any alternative available to avoid future problems.
			 */
			virtual OpenViBE::boolean isPluginObjectFlaggedAsDeprecated(
				const OpenViBE::CIdentifier& rClassIdentifier) const=0;
			/**
			 * \brief Gets a hint whether a plugin is fully functionnal or not
			 * \param rClassIdentifier [in] : the class identifier of the plugin which stability should be returned
			 * \return \e true in case the plugin is considered as unstable
			 * \return \e false in case the plugin is considered as stable
			 *
			 * Getting \e true from this function for a given plugin
			 * reflects the fact that this plugin is known not to be
			 * fully usable. This could either be because of work in progress
			 * for this plugin or either because there are known limitations
			 * that could not be bypassed. This plugin probably is functionnal
			 * enough for well known cases, and may produce unexpected results
			 * in other cases. Use it at your own risks (expect crashes for example)
			 */
			virtual OpenViBE::boolean isPluginObjectFlaggedAsUnstable(
				const OpenViBE::CIdentifier& rClassIdentifier) const=0;

			//@}
			/** \name Plugin creation and destruction */
			//@{

			/**
			 * \brief Creates a new plugin object given its class identifier
			 * \param rClassIdentifier [in] : the class identifier of the plugin object to create
			 * \return a pointer on the newly created plugin object.
			 * \return \c NULL in case of error.
			 * \sa releasePluginObject
			 */
			virtual OpenViBE::Plugins::IPluginObject* createPluginObject(
				const OpenViBE::CIdentifier& rClassIdentifier)=0;
			/**
			 * \brief Tells the plugin manager a plugin object won't be ever used
			 * \param pPluginObject [in] : the plugin object to release
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 *
			 * The client code should not call any function of the plugin object after this
			 * call has been made. However, the plugin manager is free to release allocated memory
			 * and resources for this plugin object.
			 */
			virtual OpenViBE::boolean releasePluginObject(
				OpenViBE::Plugins::IPluginObject* pPluginObject)=0;

			//@{
			/**\name Helpers */
			//@{

			/**
			 * \brief Creates a new algorithm given its class identifier and eventually returns the associated descriptor
			 * \param rClassIdentifier [in] : the class identifier of the algorithm to create
			 * \param ppAlgorithmDesc [out] : a pointer where to store the descriptor information
			 * \return The newly created algorithm in case of success.
			 * \return \c NULL in case of error.
			 *
			 * This function is a helper for the use of \c createPluginObject and co.
			 */
			virtual OpenViBE::Plugins::IAlgorithm* createAlgorithm(
				const OpenViBE::CIdentifier& rClassIdentifier,
				const OpenViBE::Plugins::IAlgorithmDesc** ppAlgorithmDesc)=0;
			/**
			 * \brief Creates a new box algorithm given its class identifier and eventually returns the associated descriptor
			 * \param rClassIdentifier [in] : the class identifier of the box algorithm to create
			 * \param ppBoxAlgorithmDesc [out] : a pointer where to store the descriptor information
			 * \return The newly created box algorithm in case of success.
			 * \return \c NULL in case of error.
			 *
			 * This function is a helper for the use of \c createPluginObject and co.
			 */
			virtual OpenViBE::Plugins::IBoxAlgorithm* createBoxAlgorithm(
				const OpenViBE::CIdentifier& rClassIdentifier,
				const OpenViBE::Plugins::IBoxAlgorithmDesc** ppBoxAlgorithmDesc)=0;

			//@}

			_IsDerivedFromClass_(OpenViBE::Kernel::IKernelObject, OV_ClassId_Kernel_Plugins_PluginManager)

		};
	};
};

#endif // __OpenViBE_Kernel_Plugins_IPluginManager_H__
