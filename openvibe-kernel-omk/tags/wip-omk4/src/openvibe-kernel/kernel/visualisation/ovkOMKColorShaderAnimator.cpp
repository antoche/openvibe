/************************************************************************/
/* This file is part of openMask(c) INRIA, CNRS, Universite de Rennes 1 */
/* 1993-2002, thereinafter the Software                                 */
/*                                                                      */
/* The Software has been developped within the Siames Project.          */
/* INRIA, the University of Rennes 1 and CNRS jointly hold intellectual */
/* property rights                                                      */
/*                                                                      */
/* The Software has been registered with the Agence pour la Protection  */
/* des Programmes (APP) under registration number                       */
/* IDDN.FR.001.510008.00.S.P.2001.000.41200                             */
/*                                                                      */
/* This file may be distributed under the terms of the Q Public License */
/* version 1.0 as defined by Trolltech AS of Norway and appearing in    */
/* the file LICENSE.QPL included in the packaging of this file.         */
/*                                                                      */
/* Licensees holding valid specific licenses issued by INRIA, CNRS or   */
/* Universite Rennes 1 for the software may use this file in            */
/* acordance with that specific license                                 */
/************************************************************************/
#include "ovkOMKColorShaderAnimator.h"

#include "OMKOgreVis.h"
#include "OMKOgreObject.h"
#include "OgreMovableObject.h"
#include "OMKParametersAccessor.inl"
#include "OMKTracer.h"

#include "OgreGpuProgram.h"
#include "OgreGpuProgramUsage.h"

using namespace OMK ;
using namespace OMK::Vis ;
using namespace OMK::Type ;
using std::string;
using Ogre::SceneNode;
using Ogre::MovableObject;
using Ogre::Entity;

REGISTER_ANIMATOR_FACTORY( OMKColorShaderAnimator, "OMKColorShaderAnimator" ) ;

//-------------------------------------------------------------------------
// constructor
//-------------------------------------------------------------------------
OMKColorShaderAnimator::OMKColorShaderAnimator(  VisualObject& visualObject,
                                      const OMKName& id,
                                      const OMKConfigurationParameterDescriptor& node )
  :
  OgreAnimatorT< ColorType >( visualObject, id, node ),
#if defined UPDATED_OMK
  _nmi ( NULL),
#endif
  _color (1.f,1.f,1.f,1.f)//Opaque white
{
  init( node );
}

//-------------------------------------------------------------------------
// destructor
//-------------------------------------------------------------------------
OMKColorShaderAnimator::~OMKColorShaderAnimator()
{
  OMTRACEID ("OMK::OMKColorShaderAnimator" ,"On Destructeur d'Animator");

#if defined UPDATED_OMK
  delete _nmi;
#endif
}

//-------------------------------------------------------------------------
// init
//-------------------------------------------------------------------------
void OMKColorShaderAnimator::init( const OMKConfigurationParameterDescriptor& node)
{
	//create node material instance
	_anim = dynamic_cast< Ogre::SceneNode *> (&getNode());
  if(_anim == NULL)
	{
		//log << LogLevel_Warning < "";
		return;
	}
#if defined UPDATED_OMK
  _nmi = new NodeMaterialInstance ( _anim );
#endif

	//retrieve color
	std::vector< float > vec;
  if(OMKParametersAccessor::get(&node,"Color",vec))
  {
		if(vec.size() == 4)
		{
			_color = Ogre::Vector4 (vec[0],vec[1],vec[2],vec[3]);
		}
	}

	//setShader();
}

//---------------------------------------------------------------------------
///\brief Action to set shader.
#if defined UPDATED_OMK
class OVMaterialSetDiffuse : public IEntityMaterialAction
{
public:
  ///\brief The contructor stores the arguments.
	OVMaterialSetDiffuse( const Ogre::ColourValue& diffuse)
		: _diffuse( diffuse )
  {}
  ///\brief Destructor.
  virtual ~OVMaterialSetDiffuse() {}
private:

  /// @name Arguments
  //@{
	Ogre::ColourValue _diffuse ;
  //@}
  ///\brief The action calls the appropriate function with the arguments.
  virtual void subEntityAction( SubEntityMaterialInstance* subEntity )
  {
		subEntity->setDiffuse(_diffuse);
  }
} ;

typedef SubEntityMaterialActionT< Ogre::SceneBlendType, Ogre::SceneBlendType,
                                  MaterialInstance,
                                  &MaterialInstance::setSceneBlending > OVMaterialSetSceneBlending ;
#endif

//-------------------------------------------------------------------------
// selfProcessVis
//-------------------------------------------------------------------------
void OMKColorShaderAnimator::selfProcessVis(const ColorType& rColorPrm)
{
	const Color& rColor( rColorPrm.getValue() );
	//OMTRACEID("OMK::OMKColorShaderAnimator"," got " << value );

	if(rColor[0] != _color.x || rColor.getG() != _color.y || rColor.getB() != _color.z || rColor.getA() != _color.w)
	{
		_color.x = rColor.getR(); _color.y = rColor.getG(); _color.z = rColor.getB(); _color.w = rColor.getA();

#if defined UPDATED_OMK
		_nmi->doAction(OVMaterialSetDiffuse(Ogre::ColourValue(_color.x, _color.y, _color.z, _color.w)));
#endif
	}
}
