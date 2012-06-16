#ifndef __OpenViBE_Plugins_IPluginObject_H__
#define __OpenViBE_Plugins_IPluginObject_H__

#include "../ovIObject.h"

namespace OpenViBE
{
	namespace Plugins
	{
		/**
		 * \class IPluginObject
		 * \author Yann Renard (INRIA/IRISA)
		 * \date 2006-06-19
		 * \brief Base class for plugin object
		 * \ingroup Group_Extend
		 *
		 * This class is the base class for all the concrete plugin
		 * classes that extend the §OpenViBE§ platform. It comes with
		 * several basic functions each plugin should have in order
		 * to work in the §OpenViBE§ platform.
		 *
		 * Each plugin class comes with a plugin descriptor class
		 * (which you can write deriving the IPluginObjectDesc
		 * interface).
		 *
		 * \sa IBoxAlgorithm
		 * \sa IScenarioImporter
		 * \sa IScenarioExporter
		 * \sa IPluginObjectDesc
		 *
		 * \todo details about building new plugins
		 */
		class OV_API IPluginObject : public OpenViBE::IObject
		{
		public:

			/** \name Memory management */
			//@{

			/**
			 * \brief Informs the plugin object it won't be used anymore
			 *
			 * This is called by the §OpenViBE§ platform to inform
			 * the plugin object it is not usefull anymore. The plugin
			 * can chose whether to delete itself or to stay in
			 * memory until it decides is is ok to be deleted.
			 * However, the §OpenViBE§ platform will not call any
			 * of the plugin functions after release is called.
			 */
			virtual void release(void)=0;

			//@}
			_IsDerivedFromClass_(OpenViBE::IObject, OV_ClassId_Plugins_PluginObject)
		};
	};
};

#endif // __OpenViBE_Plugins_IPluginObject_H__
