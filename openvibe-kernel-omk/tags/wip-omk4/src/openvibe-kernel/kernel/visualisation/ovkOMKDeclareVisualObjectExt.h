#ifndef _OMKDeclareVisualObjectEXT_H
#define _OMKDeclareVisualObjectEXT_H

#include "OMKExtension.h"
#include "OMKExtensibleSimulatedObject.h"
#include "OMKConfigurationParameterDescriptor.h"

class OMKDeclareVisualObjectExt : public OMK::ExtensionT< OMK::ExtensibleSimulatedObject >
{
public:

	OMKDeclareVisualObjectExt(
		OMK::ExtensibleSimulatedObject* owner,
		const OMK::Name& id,
		bool registered = true);

	virtual ~OMKDeclareVisualObjectExt();

	virtual bool loadObjectParameters(
		const OMK::ConfigurationParameterDescriptor* node);

public:

	friend class OMK::ExtensionCreatorT< OMKDeclareVisualObjectExt >;
	static OMK::Name OMK_CLASS_ID;
	static const bool REGISTERED_IN_EXTENSION_FACTORY;
};

#endif // _DeclareVisualObjectEXT_H
