#ifndef __OpenViBE_Plugins_IScenarioImporter_H__
#define __OpenViBE_Plugins_IScenarioImporter_H__

#include "ovIPluginObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class IScenarioImporterContext;
	};

	namespace Plugins
	{
		class OV_API IScenarioImporter : public OpenViBE::Plugins::IPluginObject
		{
		public:

			/** \name Initialization / Uninitialization */
			//@{

			/**
			 * \brief Prepares plugin object
			 * \param rContext [in] : the plugin object context
			 * \return \e true when this object successfully initialized
			 *         or \e false if it didn't succeed to initialize.
			 * \note Default implementation simply returns \e true.
			 *
			 * After a successfull initialization, the caller knows
			 * the object can safely be used... On failure, this object
			 * should be ready to be uninitialized and then released.
			 *
			 * \sa uninitialize
			 */
			virtual OpenViBE::boolean initialize(
				OpenViBE::Kernel::IScenarioImporterContext& rContext) { return true; }
			/**
			 * \brief Unprepares the object so it can be deleted
			 * \param rContext [in] : the plugin object context
			 * \return \e true when this object sucessfully uninitialized
			 *         or \e false if didn't succeed to uninitialize.
			 * \note Default implementation simply returns \e true.
			 *
			 * If this function returns \e false, it means it could not
			 * uninitialize the object members correctly. Thus, the
			 * object won't be released and none of its method will
			 * be called anymore for security. Note that this results
			 * in memory leaks. This is why this method should return
			 * \e true as often as possible.
			 *
			 * \sa initialize
			 */
			virtual OpenViBE::boolean uninitialize(
				OpenViBE::Kernel::IScenarioImporterContext& rContext) { return true; }

			//@}

			virtual OpenViBE::boolean doImport(
				OpenViBE::Kernel::IScenarioImporterContext& rContext)=0;

			_IsDerivedFromClass_(OpenViBE::Plugins::IPluginObject, OV_ClassId_Plugins_ScenarioImporter);
		};
	};
};

#endif // __OpenViBE_Plugins_IScenarioImporter_H__
