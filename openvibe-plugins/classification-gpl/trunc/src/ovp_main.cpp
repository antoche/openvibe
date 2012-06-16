#include "ovp_defines.h"

#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#if defined TARGET_HAS_ThirdPartyBLiFF
#include "algorithms/classification/ovpCAlgorithmClassifierBliffLDA.h"
#include "algorithms/classification/ovpCAlgorithmClassifierBliffCFIS.h"
#endif // TARGET_HAS_ThirdPartyBLiFF

#if defined TARGET_HAS_ThirdPartyITPP
#include "algorithms/classification/ovpCAlgorithmClassifierLDA.h"
#endif // TARGET_HAS_ThirdPartyITPP

#include <vector>

OVP_Declare_Begin()

#if defined TARGET_HAS_ThirdPartyBLiFF
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVTK_TypeId_ClassificationAlgorithm, "BLiFF - Linear Discrimimant Analysis (LDA)", OVP_ClassId_Algorithm_ClassifierBliffLDA.toUInteger());
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVTK_TypeId_ClassificationAlgorithm, "BLiFF - Chiu's Fuzzy Inference System (CFIS)", OVP_ClassId_Algorithm_ClassifierBliffCFIS.toUInteger());

	rPluginModuleContext.getTypeManager().registerEnumerationType(OVP_TypeId_CFISMode, "CFIS Mode");
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_CFISMode, "Simple Gaussian", OVP_TypeId_CFISMode_SimpleGaussian.toUInteger());
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_CFISMode, "Two Sided Gaussian", OVP_TypeId_CFISMode_TwoSidedGaussian.toUInteger());

	rPluginModuleContext.getTypeManager().registerEnumerationType(OVP_TypeId_CFISOutputMode, "CFIS Output Mode");
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_CFISOutputMode, "Class Membership", OVP_TypeId_CFISOutputMode_ClassMembership.toUInteger());
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_CFISOutputMode, "Rule Fulfillment", OVP_TypeId_CFISOutputMode_RuleFulfillment.toUInteger());

	OVP_Declare_New(OpenViBEPlugins::Local::CAlgorithmClassifierBliffLDADesc);
	OVP_Declare_New(OpenViBEPlugins::Local::CAlgorithmClassifierBliffCFISDesc);

#endif // TARGET_HAS_ThirdPartyBLiFF

#if defined TARGET_HAS_ThirdPartyITPP
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVTK_TypeId_ClassificationAlgorithm,   "Linear Discrimimant Analysis (LDA)", OVP_ClassId_Algorithm_ClassifierLDA.toUInteger());

	OVP_Declare_New(OpenViBEPlugins::Local::CAlgorithmClassifierLDADesc);
#endif // TARGET_HAS_ThirdPartyITPP

OVP_Declare_End()
