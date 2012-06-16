#ifndef _OMKADDVisualObjectEXT_H
#define _OMKADDVisualObjectEXT_H

#include "OMKExtension.h"
#include "OMKExtensibleSimulatedObject.h"
#include "OMKConfigurationParameterDescriptor.h"
#include "OMKName.h"

class OMKAddVisualObjectExt : public OMK::ExtensionT< OMK::ExtensibleSimulatedObject >
{
public:

	OMKAddVisualObjectExt(
		OMK::ExtensibleSimulatedObject* owner,
		const OMK::Name& id,
		bool registered = true);

	virtual ~OMKAddVisualObjectExt(void);

	virtual bool loadObjectParameters(const OMK::ConfigurationParameterDescriptor* node);

	virtual bool loadExtensionParameters(const OMK::ConfigurationParameterDescriptor* node);

	const OMK::Name& getVisualObjectName();

public:

	friend class OMK::ExtensionCreatorT< OMKAddVisualObjectExt >;
	static OMK::Name OMK_CLASS_ID;
	static const bool REGISTERED_IN_EXTENSION_FACTORY;

	OMK::Name _visualObjectName;
};
#endif // _OMKADDVisualObjectEXT_H
