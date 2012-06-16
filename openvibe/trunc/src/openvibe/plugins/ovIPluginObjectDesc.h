#ifndef __OpenViBE_Plugins_IPluginObjectDesc_H__
#define __OpenViBE_Plugins_IPluginObjectDesc_H__

#include "../ovIObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		/**
		 * \brief Functionality enumeration in order to know what a plugin is capable of
		 */
		enum EPluginFunctionality
		{
			PluginFunctionality_Undefined,
			PluginFunctionality_Visualization,
			PluginFunctionality_Processing,
			PluginFunctionality_Acquisition,
		};

		/**
		 * \brief License type enumeration in order to know what software license a plugin can be released with
		 */
		enum ELicenseType
		{
			LicenseType_Unspecified,  //!< Generic for unspecified license type
			LicenseType_Commercial,   //!< Generic for commercial software
			LicenseType_AFL,          //!< Academic Free License
			LicenseType_AL20,         //!< Apache License, Version 2.0
			LicenseType_ASL,          //!< Apache Software License
			LicenseType_APSL,         //!< Apple Public Source Licenses
			LicenseType_BSD,          //!< BSD License
			LicenseType_CPL,          //!< Common Public License
			LicenseType_CeCILL,       //!< Licence CEA CNRS INRIA Logiciel Libre
			LicenseType_CeCILLB,      //!< Licence CEA CNRS INRIA Logiciel Libre B
			LicenseType_CeCILLC,      //!< Licence CEA CNRS INRIA Logiciel Libre C
			LicenseType_EFL2,         //!< Eiffel Forum License Version 2.0
			LicenseType_GPL,          //!< GNU General Public License
			LicenseType_LGPL,         //!< GNU Lesser General Public License
			LicenseType_IBMPL,        //!< IBM Public License
			LicenseType_IOSL,         //!< Intel Open Source License
			LicenseType_MPL10,        //!< Mozilla Public License Version 1.0
			LicenseType_MPL11,        //!< Mozilla Public License Version 1.1
			LicenseType_NPL10,        //!< Netscape Public License Version 1.0
			LicenseType_NPL11,        //!< Netscape Public License Version 1.1
			LicenseType_OSL,          //!< Open Software License
			LicenseType_PHPL,         //!< PHP License
			LicenseType_PAL,          //!< Perl Artistic License
			LicenseType_CNRIPL,       //!< Python License (CNRI Python License)
			LicenseType_PSFL,         //!< Python Software Foundation License
			LicenseType_QPL,          //!< Q Public Licence (QT)
			LicenseType_SL,           //!< Sleepycat Software Product License
			LicenseType_SISSL,        //!< Sun Industry Standards Source License
			LicenseType_SPL,          //!< Sun Public License
			LicenseType_W3C,          //!< W3C Software License
			LicenseType_WXWLL,        //!< wxWindows Library License
			LicenseType_ZLL,          //!< zlib/libpng License
			LicenseType_ZPL,          //!< Zope Public License
		};
	};

	namespace Plugins
	{
		class IPluginObject;

		/**
		 * \class IPluginObjectDesc
		 * \author Yann Renard (INRIA/IRISA)
		 * \date 2006-06-19
		 * \brief Base class for plugin descriptor
		 * \ingroup Group_Extend
		 *
		 * This class is the base class for all the plugin description
		 * classes. It contains basic functions that could be used for
		 * each plugin description. Derived plugin descriptions will
		 * be used as a prototype of what they can create.
		 *
		 * \sa IBoxAlgorithmDesc
		 * \sa IScenarioImporterDesc
		 * \sa IScenarioExporterDesc
		 * \sa IPluginObject
		 *
		 * \todo details about building new plugins
		 */
		class OV_API IPluginObjectDesc : public OpenViBE::IObject
		{
		public:
			/** \name Memory management */
			//@{

			/**
			 * \brief Informs the plugin description it won't be used any more
			 *
			 * This is called by the §OpenViBE§ platform to inform
			 * the plugin description it is not usefull anymore.
			 * The plugin can chose whether to delete itself or
			 * to stay in memory until it decides it is ok to be
			 * deleted. However, the §OpenViBE§ platform will not
			 * call any of the plugin functions after release is
			 * called.
			 */
			virtual void release(void)=0;

			//@}
			/** \name Creation process */
			//@{

			/**
			 * \brief Gives a tip on what this plugin descriptor is able to create
			 *
			 * This may inform the §OpenViBE§ platform about what
			 * kind of plugin can be created using this plugin
			 * descriptor. It should return the concrete class
			 * identifier of the plugin object itself.
			 */
			virtual OpenViBE::CIdentifier getCreatedClass(void) const=0;
			virtual OpenViBE::CIdentifier getCreatedClassIdentifier(void) const { return this->getCreatedClass(); }
			/**
			 * \brief Creates the plugin object itself
			 * \return the created object.
			 *
			 * This method creates the plugin object itself and returns
			 * it with the lowest level interface. The §OpenVIBE§
			 * platform then uses the IObject::isDerivedFromClass
			 * method to use the plugin correctly.
			 */
			virtual OpenViBE::Plugins::IPluginObject* create(void)=0;

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
			 * Default implementation simply returns "no name".
			 */
			virtual OpenViBE::CString getAuthorName(void) const { return CString("unknown"); }
			/**
			 * \brief Gets the author company name for this plugin
			 * \return The author company name for this plugin.
			 *
			 * Default implementation simply returns "unknown".
			 */
			virtual OpenViBE::CString getAuthorCompanyName(void) const { return CString("unknown"); }
			/**
			 * \brief Gets a short description of the plugin
			 * \return A short description of the plugin.
			 *
			 * Default implementation simply returns "unknown".
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
			 * \brief Gets a basic category of the plugin
			 * \return the category tokens of the plugin
			 *
			 * The tokens should be separated with '/' characters
			 * in order to create sub categories.
			 *
			 * Default implementation returns "unknown".
			 */
			virtual OpenViBE::CString getCategory(void) const { return CString("unknown"); }
			/**
			 * \brief Gets the version of the plugin
			 * \return the version of the plugin.
			 *
			 * Default implementation simply returns "unknown".
			 */
			virtual OpenViBE::CString getVersion(void) const { return CString("unknown"); }
			/**
			 * \brief Tests whether the plugin has a given functionality
			 * \param ePF [in] : functionality of interest
			 * \return \e true in case plugin has this functionality.
			 * \return \e false otherwise.
			 */
			virtual OpenViBE::boolean hasFunctionality(OpenViBE::Kernel::EPluginFunctionality ePF) const { return false; }
			/**
			 * \brief Tests the licence type for this plugin
			 * \param eLT [in] : the licence type that this plugin may have
			 * \return \e true in case this plugin accepts the specified licence.
			 * \return \e false in other case
			 *
			 * Default implementation returns \c false .
			 *
			 * This function should return the licence which can be used to
			 * distribute the plugin. The aim of this functionnality is to
			 * be able to quickly warn the user if the usage he is doing for
			 * the software is incompatible with the licence the plugin uses.
			 * for example, a GPL plugin should not be used in a commercial
			 * context.
			 *
			 * A plugin may be used under different licences. For this reason,
			 * the §OpenViBE§ kernel will loop on this function for each
			 * licence type to request.
			 */
			virtual OpenViBE::boolean hasLicenceType(OpenViBE::Kernel::ELicenseType eLT) const { return false; }

			//@}

			_IsDerivedFromClass_(OpenViBE::IObject, OV_ClassId_Plugins_PluginObjectDesc)
		};
	};
};

#endif // __OpenViBE_Plugins_IPluginObjectDesc_H__
