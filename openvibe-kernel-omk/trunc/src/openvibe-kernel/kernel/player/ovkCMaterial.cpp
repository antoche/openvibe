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

#include "ovkCMaterial.h"

using namespace OpenViBE::Kernel;

//TODO : replace OMKTRACEID with m_rKernelContext.getLogManager() << LogLevel_Trace << "..." << "\n";

//=============================================================================
// NodeMaterial
//=============================================================================
NodeMaterial::~NodeMaterial()
{
  for( EntityMaterialIterator it = getEntityMaterialIterator() ;
       it.hasMoreElements() ;
       it.moveNext() )
  {
    delete it.peekNextValue() ;
  }
}
//-----------------------------------------------------------------------------
void NodeMaterial::doAction( const IMaterialAction& action )
{
  if( action.nodeAction( this ) )
  {
    for( EntityMaterialIterator it = getEntityMaterialIterator() ;
         it.hasMoreElements() ;
         it.moveNext() )
    {
      it.peekNextValue()->doAction( action ) ;
    }
  }
}
//-----------------------------------------------------------------------------
void NodeMaterial::activateMaterial()
{
  for( EntityMaterialIterator it = getEntityMaterialIterator() ;
       it.hasMoreElements() ;
       it.moveNext() )
  {
    it.peekNextValue()->activateMaterial() ;
  }
}
//-----------------------------------------------------------------------------
void NodeMaterial::clearMaterial()
{
  for( EntityMaterialIterator it = getEntityMaterialIterator() ;
       it.hasMoreElements() ;
       it.moveNext() )
  {
    it.peekNextValue()->clearMaterial() ;
  }
}
//-----------------------------------------------------------------------------

//=============================================================================
// EntityMaterial
//=============================================================================
EntityMaterial::~EntityMaterial()
{
  for( SubEntityMaterialIterator it = getSubEntityMaterialIterator() ;
       it.hasMoreElements() ;
       it.moveNext() )
  {
    delete it.peekNext() ;
  }
}
//-----------------------------------------------------------------------------
void EntityMaterial::doAction( const IMaterialAction& action )
{
  if( action.entityAction( this ) )
  {
    for( SubEntityMaterialIterator it = getSubEntityMaterialIterator() ;
         it.hasMoreElements() ;
         it.moveNext() )
    {
      it.peekNext()->doAction( action ) ;
    }
  }
}
//-----------------------------------------------------------------------------
void EntityMaterial::activateMaterial()
{
  for( SubEntityMaterialIterator it = getSubEntityMaterialIterator() ;
       it.hasMoreElements() ;
       it.moveNext() )
  {
    it.peekNext()->activateMaterial() ;
  }
}
//-----------------------------------------------------------------------------
void EntityMaterial::clearMaterial()
{
  for( SubEntityMaterialIterator it = getSubEntityMaterialIterator() ;
       it.hasMoreElements() ;
       it.moveNext() )
  {
    it.peekNext()->clearMaterial() ;
  }
}
//-----------------------------------------------------------------------------

//=============================================================================
// SubEntityMaterial
//=============================================================================
void SubEntityMaterial::doAction( const IMaterialAction& action )
{
  action.subEntityAction( this ) ;
}
//-----------------------------------------------------------------------------
void SubEntityMaterial::activateMaterial()
{
  if( !_mat.isNull() )
  {
    //OMTRACEID("OMK::Material", "set Material " << _mat->getName());
    _ogreSubEntity->setMaterialName( _mat->getName() ) ;
  }
}
//-----------------------------------------------------------------------------
void SubEntityMaterial::activateMaterial( const Ogre::String& materialName )
{
  if( _mat.isNull() || materialName != _mat->getName() )
  {
    //OMTRACEID("OMK::Material", "set Material start " << materialName);
    clearMaterial() ;
    _mat = Ogre::MaterialManager::getSingleton().getByName( materialName ) ;
    _ogreSubEntity->setMaterialName( materialName ) ;
  }
}
//-----------------------------------------------------------------------------
void SubEntityMaterial::clearMaterial()
{
  if( !_mat.isNull() )
  {
    Ogre::MaterialManager::getSingleton().remove( _mat->getName() ) ;
    _mat.setNull() ;
  }
}
//-----------------------------------------------------------------------------

//=============================================================================
// NodeMaterialOrig
//=============================================================================
NodeMaterialOrig::NodeMaterialOrig( Ogre::SceneNode* sceneNode )
: NodeMaterial(),
  _sceneNode( sceneNode )
{
  for( Ogre::SceneNode::ObjectIterator it = sceneNode->getAttachedObjectIterator() ;
       it.hasMoreElements() ;
       it.moveNext() )
  {
    Ogre::MovableObject * currentMO = it.peekNextValue() ;
    if ( currentMO->getMovableType() == Ogre::EntityFactory::FACTORY_TYPE_NAME )
    {
      //OMTRACEID( "DEBUG", "Create entity " << currentMO->getName() ) ;
      _entities[ it.peekNextKey() ] = new EntityMaterialOrig( static_cast< Ogre::Entity *> ( currentMO ) ) ;
    }
  }
  for( Ogre::Node::ChildNodeIterator it = sceneNode->getChildIterator() ;
       it.hasMoreElements() ;
       it.moveNext() )
  {
    Ogre::SceneNode* childSceneNode = static_cast< Ogre::SceneNode* >( it.peekNextValue() ) ;
    //OMTRACEID( "DEBUG", "Create ChildSceneNode " << childSceneNode->getName() ) ;
    NodeMaterialOrig* child = new NodeMaterialOrig( childSceneNode ) ;
    _childs.push_back( child ) ;
  }
}
//-----------------------------------------------------------------------------
NodeMaterialOrig::~NodeMaterialOrig()
{
  for( NodeMaterialOrigVector::iterator it = _childs.begin() ;
       it != _childs.end() ;
       it++ )
  {
    delete *it ;
  }
  for( ItemStack::iterator it = _stack.begin() ;
       it != _stack.end() ;
       it++ )
  {
    delete it->nodeMaterial ;
  }
}
//-----------------------------------------------------------------------------
void NodeMaterialOrig::doAction( const IMaterialAction& action )
{
  if( action.nodeAction( this ) )
  {
    for( EntityMaterialIterator it = getEntityMaterialIterator() ;
         it.hasMoreElements() ;
         it.moveNext() )
    {
      it.peekNextValue()->doAction( action ) ;
    }
  }
  for( NodeMaterialOrigVector::iterator it = _childs.begin() ;
       it != _childs.end() ;
       it++ )
  {
    (*it)->doAction( action ) ;
  }
}
//-----------------------------------------------------------------------------
NodeMaterialOrig* NodeMaterialOrig::find( Ogre::SceneNode* node )
{
  NodeMaterialOrig* result = node != _sceneNode ? 0 : this ;
  for( NodeMaterialOrigVector::iterator it = _childs.begin() ;
       it != _childs.end() && !result ;
       it++ )
  {
    result = (*it)->find( node ) ;
  }
  return result ;
}
//-----------------------------------------------------------------------------
void NodeMaterialOrig::updateCopyWith( IUpdateMaterial* update )
{
  NodeMaterialCopy* newNode = new NodeMaterialCopy( _stack.empty() ? (NodeMaterial*)this : (NodeMaterial*)_stack.back().nodeMaterial, update ) ;
  _stack.push_back( Item( newNode, update ) ) ;
  //OMTRACEID("DEBUG", "" << update->getMaterialName(".")
  //      << " Stack size " << _stack.size() );
    for( EntityMaterialIterator it = getEntityMaterialIterator() ;
       it.hasMoreElements() ;
       it.moveNext() )
  {
   //OMTRACEID("DEBUG", "Entities "  <<   it.peekNextKey()  );
     for( SubEntityMaterialIterator it2 = it.peekNextValue()->getSubEntityMaterialIterator() ;
       it2.hasMoreElements() ;
       it2.moveNext() )
  {
    //if ( !  it2.peekNext()->getMaterial().isNull())
      //OMTRACEID("DEBUG", "SubEntitity Mat "<<   it2.peekNext()->getMaterial()->getName()) ;
  }

  }

  update->update( newNode ) ;
}
//-----------------------------------------------------------------------------
void NodeMaterialOrig::createCopyAndUpdateWith( IUpdateMaterial* update )
{
  updateCopyWith( update ) ;
  for( NodeMaterialOrigVector::iterator it = _childs.begin() ;
       it != _childs.end() ;
       it++ )
  {
    //OMTRACEID("DEBUG", "Child --------------- " );
      (*it)->createCopyAndUpdateWith( update ) ;
  }
}
//-----------------------------------------------------------------------------
void NodeMaterialOrig::undoFor( IUpdateMaterial* update, bool deleteNode )
{
  // Undo the current node level
  UpdateMaterialStack updateStack ;
  for( ; _stack.back().update != update ; _stack.pop_back() )
  {
    updateStack.push( _stack.back().update ) ;
    _stack.back().update->undo( _stack.back().nodeMaterial ) ;
    delete _stack.back().nodeMaterial ;
  }
  // delete the node which is undoing
  if( deleteNode )
  {
    delete _stack.back().nodeMaterial ;
    _stack.pop_back() ;
  }
  // activate the new current material
  ( _stack.empty() ? (NodeMaterial*)this : (NodeMaterial*)( _stack.back().nodeMaterial ) )->activateMaterial() ;
  // Do the same for children
  for( NodeMaterialOrigVector::iterator it = _childs.begin() ;
       it != _childs.end() ;
       it++ )
  {
    (*it)->undoFor( update, deleteNode ) ;
  }
  // Redo the action
  if( !deleteNode )
  {
    update->update( _stack.back().nodeMaterial ) ;
  }
  for( ; !updateStack.empty(); updateStack.pop() )
  {
    updateCopyWith( updateStack.top() ) ;
  }
}
//-----------------------------------------------------------------------------

//=============================================================================
// EntityMaterialOrig
//=============================================================================
EntityMaterialOrig::EntityMaterialOrig( Ogre::Entity *ogreEntity )
: EntityMaterial( ogreEntity )
{
  for( unsigned int i = 0;
       i < ogreEntity->getNumSubEntities() ;
       i++ )
  {
    //OMTRACEID( "DEBUG", "Create SubEntityMaterial " << i  ) ;
    _subEntities.push_back( new SubEntityMaterialOrig( ogreEntity->getSubEntity( i ) ) ) ;
  }
}
//-----------------------------------------------------------------------------

//=============================================================================
// SubEntityMaterialOrig
//=============================================================================
SubEntityMaterialOrig::SubEntityMaterialOrig( Ogre::SubEntity *ogreSubEntity )
: SubEntityMaterial( ogreSubEntity )
{
  _mat = Ogre::MaterialManager::getSingleton().getByName( _ogreSubEntity->getMaterialName() ) ;
  //OMTRACEID( "DEBUG", "get Material of " <<_ogreSubEntity->getMaterialName()  << (_mat.isNull() ? " NULL" : " OK" )) ;
}
//-----------------------------------------------------------------------------
SubEntityMaterialOrig::~SubEntityMaterialOrig()
{
	if(!_mat.isNull())
	{
		_ogreSubEntity->setMaterialName( _mat->getName() ) ;
	}
}
//-----------------------------------------------------------------------------

//=============================================================================
// NodeMaterialCopy
//=============================================================================
NodeMaterialCopy::NodeMaterialCopy( NodeMaterial* node, IUpdateMaterial* update )
: NodeMaterial()
{
  for( EntityMaterialIterator it = node->getEntityMaterialIterator() ;
       it.hasMoreElements() ;
       it.moveNext() )
  {
    _entities[ it.peekNextKey() ] = new EntityMaterialCopy( it.peekNextValue(), update ) ;
  }
}
//-----------------------------------------------------------------------------
NodeMaterialCopy::~NodeMaterialCopy()
{
}
//-----------------------------------------------------------------------------

//=============================================================================
// EntityMaterialCopy
//=============================================================================
EntityMaterialCopy::EntityMaterialCopy( EntityMaterial *e, IUpdateMaterial* update )
: EntityMaterial( e->_ogreEntity )
{
  for( SubEntityMaterialIterator it = e->getSubEntityMaterialIterator() ;
       it.hasMoreElements() ;
       it.moveNext() )
  {
    _subEntities.push_back( new SubEntityMaterialCopy( it.peekNext(), update ) ) ;
  }
}
//-----------------------------------------------------------------------------
EntityMaterialCopy::~EntityMaterialCopy()
{
}
//-----------------------------------------------------------------------------

//=============================================================================
// SubEntityMaterialCopy
//=============================================================================
SubEntityMaterialCopy::SubEntityMaterialCopy( SubEntityMaterial *se, IUpdateMaterial* update )
: SubEntityMaterial( se->_ogreSubEntity )
{
  if( !se->_mat.isNull() )
  {
    Ogre::MaterialPtr matSrc( se->_mat ) ;
    Ogre::String nameDest( update->getMaterialName( matSrc->getName() ) ) ;
    // Try to reuse existing material
    if( Ogre::MaterialManager::getSingleton().resourceExists( nameDest ) )
    { // ok => get the material
      _mat = Ogre::MaterialManager::getSingleton().getByName( nameDest ) ;
    }
    else
    { // none => create a copy
      _mat = matSrc->clone( nameDest ) ;
    }
    _ogreSubEntity->setMaterialName( nameDest ) ;
  }
}
//-----------------------------------------------------------------------------
SubEntityMaterialCopy::~SubEntityMaterialCopy()
{
}
//-----------------------------------------------------------------------------

//=============================================================================
// IMaterialAction
//=============================================================================
Ogre::Pass* IMaterialAction::getFirstPassOfFirstTechnique( SubEntityMaterial* subEntity )
{
	//OMASSERTM( 0 < subEntity->getMaterial()->getNumTechniques(), "Don't get Any Tecnnique on Material" << subEntity->getMaterial()->getName() );
  Ogre::Technique * technique = subEntity->getMaterial()->getTechnique( 0 );
	//OMASSERTM( 0 < technique->getNumPasses(), "Don't get Any PASS on Material " << subEntity->getMaterial()->getName() );
  return technique->getPass( 0 ) ;
}
//-----------------------------------------------------------------------------

//=============================================================================
// IUpdateMaterial
//=============================================================================
void IUpdateMaterial::undo( NodeMaterial* node )
{
  node->clearMaterial() ;
}
//-----------------------------------------------------------------------------
Ogre::String IUpdateMaterial::getUniqueMaterialName( const Ogre::String& name )
{
  Ogre::String uniqueName( name ) ;
  for( ;
    Ogre::MaterialManager::getSingleton().resourceExists( uniqueName ) ;
    uniqueName = name + Ogre::StringConverter::toString( Ogre::Math::UnitRandom() ) )
       ;
  return uniqueName ;
}
//-----------------------------------------------------------------------------

