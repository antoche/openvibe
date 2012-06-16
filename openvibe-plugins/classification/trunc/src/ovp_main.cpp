#include "ovp_defines.h"

#include "algorithms/classification/ovpCAlgorithmClassifierNULL.h"
#include "algorithms/classification/ovpCAlgorithmClassifierSVM.h"

#include "algorithms/confusion-matrix/ovpCAlgorithmConfusionMatrix.h"

#include "box-algorithms/classification/ovpCBoxAlgorithmVotingClassifier.h"
#include "box-algorithms/classification/ovpCBoxAlgorithmClassifierTrainer.h"
#include "box-algorithms/classification/ovpCBoxAlgorithmClassifierProcessor.h"
#include "box-algorithms/confusion-matrix/ovpCBoxAlgorithmConfusionMatrix.h"

#include <vector>

OVP_Declare_Begin();

//	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVTK_TypeId_ClassificationAlgorithm, "NULL Classifier (does nothing)",OVP_ClassId_Algorithm_ClassifierNULL.toUInteger());
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVTK_TypeId_ClassificationAlgorithm, "Support Vector Machine (SVM)",OVP_ClassId_Algorithm_ClassifierSVM.toUInteger());

	rPluginModuleContext.getTypeManager().registerEnumerationType(OVP_TypeId_SVMType,"SVM Type");
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_SVMType,"C-SVC",C_SVC);
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_SVMType,"Nu-SVC",NU_SVC);
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_SVMType,"One class SVM",ONE_CLASS);
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_SVMType,"Epsilon SVR",EPSILON_SVR);
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_SVMType,"Nu SVR",NU_SVR);
	
	rPluginModuleContext.getTypeManager().registerEnumerationType(OVP_TypeId_SVMKernelType, "SVM Kernel Type");
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_SVMKernelType,"Linear",LINEAR);
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_SVMKernelType,"Polinomial",POLY);
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_SVMKernelType,"Radial basis function",RBF);
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_SVMKernelType,"Sigmoid",SIGMOID);

//	OVP_Declare_New(OpenViBEPlugins::Classification::CAlgorithmClassifierNULLDesc);
	OVP_Declare_New(OpenViBEPlugins::Classification::CAlgorithmClassifierSVMDesc);

	OVP_Declare_New(OpenViBEPlugins::Classification::CAlgorithmConfusionMatrixDesc);

	OVP_Declare_New(OpenViBEPlugins::Classification::CBoxAlgorithmVotingClassifierDesc);
	OVP_Declare_New(OpenViBEPlugins::Classification::CBoxAlgorithmClassifierTrainerDesc);
	OVP_Declare_New(OpenViBEPlugins::Classification::CBoxAlgorithmClassifierProcessorDesc);
	OVP_Declare_New(OpenViBEPlugins::Classification::CBoxAlgorithmConfusionMatrixDesc);

OVP_Declare_End();
