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
#ifndef __OVKOMKCOLORSHADERANIMATOR_H__
#define __OVKOMKCOLORSHADERANIMATOR_H__

#include "OMKNamespaceCompatibility.h"
#include "OMKOgreAnimatorT.h"
#if defined UPDATED_OMK
#include "OMKNodeMaterialInstance.h"
#endif
#include "OMKColorType.h"

namespace OMK
{
	namespace Vis
	{
		class OgreVis;

		class OMKColorShaderAnimator : public OgreAnimatorT< Type::ColorType >
		{
			DECLARE_ANIMATOR_FACTORY( OMKColorShaderAnimator ) ;
		private:
			///\brief Called by the partner to process the visualisation
			///
			/// Called by the public version if the value was changed.
			/// Must be overwritten by sons.
			virtual void selfProcessVis(const Type::ColorType& color) ;
			/// init, load param
			virtual void init(const OMKConfigurationParameterDescriptor& node);

		protected:
			Ogre::SceneNode* _anim;
#if defined UPDATED_OMK
			NodeMaterialInstance* _nmi;
#endif
			Ogre::Vector4 _color;
		};
	};// namespace Vis
};// namespace OMK

#endif // __OVKOMKCOLORSHADERANIMATOR_H__
