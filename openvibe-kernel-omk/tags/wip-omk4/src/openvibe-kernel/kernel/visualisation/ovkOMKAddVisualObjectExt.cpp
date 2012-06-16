#include "ovkOMKAddVisualObjectExt.h" 
#include "OMKParametersAccessor.h"
#include "OMKConfigurationParameterDescriptor.h"
#include "OMKAddVisualObjectPrm.h"
#include "OMKAddAnimatorPrm.h"
#include "OMKVisBase.h"

using namespace OMK;

REGISTER_EXTENSION_FACTORY( OMKAddVisualObjectExt, "OMKAddVisualObjectExt" ) ;

OMKAddVisualObjectExt::OMKAddVisualObjectExt( ExtensibleSimulatedObject* owner, const Name& id ,bool registered)
: ExtensionT< ExtensibleSimulatedObject >( owner, id, registered )
{
  OMTRACEID( "OMKAddDVisualObjectExt", "Construction ok");
}

OMKAddVisualObjectExt::~OMKAddVisualObjectExt()
{
	_owner->fireValuedSignal(Vis::VisBase::DELETE_VISUAL_OBJECT_EVENT_ID, _visualObjectName);
}

bool OMKAddVisualObjectExt::loadObjectParameters( const ConfigurationParameterDescriptor * node )
{
  OMTRACEID( "OMKAddVisualObjectExt", "param ");
  //_visualObjectName = std::string (_owner->getName().getCString()) /*+ std::string("_VO")*/;

  ParametersAccessor::get(node,"VisualObjectName",_visualObjectName);
  return true;
}

bool OMKAddVisualObjectExt::loadExtensionParameters( const ConfigurationParameterDescriptor * node )
{
	OMTRACEID( "AddDVisualObjectExt", "Ext param ");

	std::cout << *node;

	Type::AddVisualObjectPrm visualObject;
	bool l_bLoadParameterResult=visualObject.loadParameters(node ? node->getSubDescriptorByName( "VisualObject") : 0, "", _owner );
	OMASSERTM(l_bLoadParameterResult, "Incorrect Configuration File, no Definition of VisualObject");
	visualObject.setObjectName(_visualObjectName); 
	Name visualisation;
	//Don't send a signal if it's a local object ie on only one visu
	if ( ParametersAccessor::get( node, "LocalVisualisation", visualisation))
		_owner->sendValuedEvent(visualisation, Vis::VisBase::ADD_VISUAL_OBJECT_EVENT_ID, visualObject ) ;
	else
		_owner->fireValuedSignal ( Vis::VisBase::ADD_VISUAL_OBJECT_EVENT_ID, visualObject ) ;
	return true;
}

const Name& OMKAddVisualObjectExt::getVisualObjectName()
{
  return _visualObjectName;
}
