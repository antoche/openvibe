#include "ovkOMKDeclareVisualObjectExt.h" 

using namespace OMK;

REGISTER_EXTENSION_FACTORY( OMKDeclareVisualObjectExt, "OMKDeclareVisualObjectExt" ) ;

OMKDeclareVisualObjectExt::OMKDeclareVisualObjectExt( ExtensibleSimulatedObject* owner, const Name& id, bool registered )
: ExtensionT< ExtensibleSimulatedObject >( owner, id,  registered )
{
}

OMKDeclareVisualObjectExt::~OMKDeclareVisualObjectExt()
{
}

bool OMKDeclareVisualObjectExt::loadObjectParameters( const ConfigurationParameterDescriptor * node )
{
	//retrieve geometry file
  std::string l_sGeometryFile;
  if(ParametersAccessor::get( node, "GeometryFile", l_sGeometryFile) == false)
	{
		//OMTRACEID("OMKDeclareVisualObjectExt::loadObjectParameters() : ","can't retrieve geometry file for object " << *(obj->getExtensionsParameters()));
		return false;
	}

	//retrieve scene manager
  std::string l_sSceneManager;
  if(ParametersAccessor::get( node, "SceneManager", l_sSceneManager) == false)
	{
		//OMTRACEID("OMKDeclareVisualObjectExt::loadObjectParameters() : ","can't retrieve scene manager for object " << *(obj->getExtensionsParameters()));
		//return false; //don't return here, a default scene manager will be looked for by OpenMASK
	}

	//create VO
	//---------
	
	//configure Visual Object
  ConfigurationParameterDescriptor* visualObjectPropertiesNode = 0;  	
	ParametersAccessor::set( visualObjectPropertiesNode, "Class", "OgreObject") ;
  ParametersAccessor::set( visualObjectPropertiesNode, "ResourceGroup", "General") ;  	
	ParametersAccessor::set( visualObjectPropertiesNode, "GeometryFile", l_sGeometryFile.c_str()) ;
	ParametersAccessor::set( visualObjectPropertiesNode, "SceneManager", l_sSceneManager.c_str()) ;
	
	/*
	//animator subsection
	ConfigurationParameterDescriptor* animParam = 0;
				
	//animator properties
	ConfigurationParameterDescriptor* colorParam = 0;		
	ParametersAccessor::set(colorParam, "Class", "OMKColorShaderAnimator") ;
	ParametersAccessor::set(colorParam, "NodeName", "MainNODE") ;
	std::vector<float> color(4);	
	color[0] = pColor[0]; color[1] = pColor[1]; color[2] = pColor[2];  color[3] = pColor[3];		
	ParametersAccessor::set(colorParam, "Color", color) ;

	ParametersAccessor::set(animParam, "color", colorParam) ;		
	
	ParametersAccessor::set(configParam, "Animator", animParam) ;
*/
	//insert it in Visual Object section
	ConfigurationParameterDescriptor* visualObjectSectionNode = 0;
	ParametersAccessor::set( visualObjectSectionNode, "VisualObject", visualObjectPropertiesNode) ;

	//add VisualObject section to extensions node
  ObjectDescriptor* obj = const_cast<ObjectDescriptor*>(&_owner->getObjectDescriptor());
  ConfigurationParameterDescriptor* extensionNode = obj->getExtensionsParameters(true);
	ParametersAccessor::set( extensionNode, "OMKAddVisualObjectExt", visualObjectSectionNode ) ;
  
	//log added extension
	OMTRACEID("ExtDepVO","configParam " << *(obj->getExtensionsParameters()));

  return true;
}
