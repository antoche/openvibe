/************************************************************************/
/* This file is part of openMask(c) INRIA, CNRS, Universite de Rennes 1 */
/* 1993-2002, thereinafter the Software                                 */
/*                                                                      */
/* The Software has been developed within the Siames Project.           */
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
/* accordance with that specific licence                                */
/************************************************************************/
#ifndef __OpenViBEKernel_Kernel_Player_CMaterial_H__
#define __OpenViBEKernel_Kernel_Player_CMaterial_H__

#include <Ogre.h>
#include <stack>
#include <openvibe/ov_all.h>

class IMaterialAction;
class IUpdateMaterial;
class NodeMaterial;
class EntityMaterial;
class SubEntityMaterial;
class NodeMaterialOrig;
class EntityMaterialOrig;
class SubEntityMaterialOrig;
class NodeMaterialCopy;
class EntityMaterialCopy;
class SubEntityMaterialCopy;

//=============================================================================
class IMaterialAction
{
protected:
  ///\brief Constructor.
  /// It is protected because only sons can use it.
	IMaterialAction(
		const OpenViBE::Kernel::IKernelContext& rKernelContext) :
		m_rKernelContext(rKernelContext)
	{}

public :
  ///\brief Destructor.
  virtual ~IMaterialAction() {}
  ///\brief Method called by \ref NodeMaterial::doAction "doAction" for
  /// the current node.
  ///\return \e true if the action must execute the loop for the entities
  ///
  /// The default does nothing. Can call a method of the node.
  virtual bool nodeAction( NodeMaterial* node ) const { return true ; }
  ///\brief Method called by \ref EntityMaterial::doAction "doAction" for
  /// the current entity.
  ///\return \e true if the action must execute the loop for the sub-entities
  ///
  /// The default does nothing. Can call a method of the entity.
  virtual bool entityAction( EntityMaterial* entity ) const { return true ; }
  ///\brief Method called by \ref SubEntityMaterial::doAction "doAction" for
  /// the current sub-entity.
  /// The default does nothing. Most of the time calls a method of the sub-entity.
  virtual void subEntityAction( SubEntityMaterial* subEntity ) const {}
protected:
	/// OpenViBE kernel context
	const OpenViBE::Kernel::IKernelContext& m_rKernelContext;
  /// Get first pass of the subentity
  static Ogre::Pass* getFirstPassOfFirstTechnique( SubEntityMaterial* subEntity ) ;
} ;
//=============================================================================
class IUpdateMaterial
{
  friend class NodeMaterialOrig ;
protected:
  IUpdateMaterial() {}
public:
  virtual ~IUpdateMaterial() {}
  virtual void update( NodeMaterial* node ) = 0 ;
  virtual void undo( NodeMaterial* node ) ;
  virtual Ogre::String getMaterialName( const Ogre::String& name ) const { return getUniqueMaterialName( name ) ; }
protected:
  static Ogre::String getUniqueMaterialName( const Ogre::String& name ) ;
} ;
//Base
//=============================================================================
typedef std::map< std::string, EntityMaterial* > EntityMaterialMap ;
typedef Ogre::MapIterator< EntityMaterialMap > EntityMaterialIterator ;
typedef std::stack< IUpdateMaterial* > UpdateMaterialStack ;
//-----------------------------------------------------------------------------
class  NodeMaterial
{
public:
  NodeMaterial() {}
  virtual ~NodeMaterial() ;
protected:
  friend class NodeMaterialOrig ;
  friend class NodeMaterialCopy ;
  struct Item
  {
    Item( NodeMaterialCopy* n, IUpdateMaterial* u ) : nodeMaterial( n ), update( u ) {}
    NodeMaterialCopy* nodeMaterial ;
    IUpdateMaterial* update ;
  } ;
  typedef std::list< Item > ItemStack ;
  ItemStack _stack ;
  EntityMaterialMap _entities ;
public:
  void activateMaterial() ;
  void clearMaterial() ;
  EntityMaterialIterator getEntityMaterialIterator() { return EntityMaterialIterator( _entities ) ; }
  virtual void doAction( const IMaterialAction& action ) ;
} ;
//=============================================================================
typedef std::vector < SubEntityMaterial* > SubEntityMaterialVector ;
typedef Ogre::VectorIterator < SubEntityMaterialVector > SubEntityMaterialIterator ;
//-----------------------------------------------------------------------------
class  EntityMaterial
{
public:
  EntityMaterial( Ogre::Entity * ogreEntity ) : _ogreEntity( ogreEntity ) {}
  virtual ~EntityMaterial() ;
protected:
  friend class EntityMaterialCopy ;
  SubEntityMaterialVector _subEntities ;
  Ogre::Entity * _ogreEntity ;
public:
  Ogre::Entity * getOgreEntity() { return _ogreEntity ; }
  void activateMaterial() ;
  void clearMaterial() ;
  SubEntityMaterialIterator getSubEntityMaterialIterator() { return SubEntityMaterialIterator( _subEntities ) ; }
  void doAction( const IMaterialAction& action ) ;
} ;
//=============================================================================
class  SubEntityMaterial
{
public:
  SubEntityMaterial( Ogre::SubEntity *ogreSubEntity ) : _ogreSubEntity( ogreSubEntity ) {} ;
  virtual ~SubEntityMaterial() {} ;
protected:
  friend class SubEntityMaterialCopy ;
  Ogre::SubEntity *_ogreSubEntity ;
  Ogre::MaterialPtr _mat ;
public:
  Ogre::MaterialPtr& getMaterial() { return _mat ; }
  Ogre::SubEntity* getOgreSubEntity() { return _ogreSubEntity ; }
  void activateMaterial() ;
  void activateMaterial( const Ogre::String& materialName ) ;
  virtual void clearMaterial() ;
  void doAction( const IMaterialAction& action ) ;
} ;

//Original
//=============================================================================
typedef std::vector< NodeMaterialOrig* > NodeMaterialOrigVector ;
//-----------------------------------------------------------------------------
class  NodeMaterialOrig : public NodeMaterial
{
public:
  NodeMaterialOrig( Ogre::SceneNode* node ) ;
  virtual ~NodeMaterialOrig() ;
  void undoFor( IUpdateMaterial* update, bool deleteNode ) ;
  void createCopyAndUpdateWith( IUpdateMaterial* update ) ;
  NodeMaterialOrig* find( Ogre::SceneNode* node ) ;
  virtual void doAction( const IMaterialAction& action ) ;
protected:
  void updateCopyWith( IUpdateMaterial* update ) ;
  NodeMaterialOrigVector _childs ;
  Ogre::SceneNode* _sceneNode ;
} ;
//=============================================================================
class  EntityMaterialOrig : public EntityMaterial
{
public:
  EntityMaterialOrig( Ogre::Entity *e ) ;
  virtual ~EntityMaterialOrig() {}
} ;
//=============================================================================
class  SubEntityMaterialOrig : public SubEntityMaterial
{
public:
  SubEntityMaterialOrig( Ogre::SubEntity *ogreSubEntity ) ;
  virtual ~SubEntityMaterialOrig() ;
  virtual void clearMaterial() {}
protected:
  Ogre::MaterialPtr _currentMat ;
} ;

//Copy
//=============================================================================
class  NodeMaterialCopy : public NodeMaterial
{
public:
  NodeMaterialCopy( NodeMaterial* node, IUpdateMaterial* update ) ;
  virtual ~NodeMaterialCopy() ;
} ;
//=============================================================================
class  EntityMaterialCopy : public EntityMaterial
{
public:
  EntityMaterialCopy( EntityMaterial *e, IUpdateMaterial* update ) ;
  virtual ~EntityMaterialCopy() ;
} ;
//=============================================================================
class  SubEntityMaterialCopy : public SubEntityMaterial
{
public:
  SubEntityMaterialCopy( SubEntityMaterial *se, IUpdateMaterial* update ) ;
  virtual ~SubEntityMaterialCopy() ;
} ;

#endif // __OpenViBEKernel_Kernel_Player_CMaterial_H__
