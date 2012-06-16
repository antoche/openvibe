#ifndef __OpenViBEPlugins_Algorithm_ClassifierSVM_H__
#define __OpenViBEPlugins_Algorithm_ClassifierSVM_H__

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include <xml/IWriter.h>
#include <xml/IReader.h>

#include <stack>
#include "libSVM/svm.h"

#define OVP_ClassId_Algorithm_ClassifierSVM                                        OpenViBE::CIdentifier(0x50486EC2, 0x6F2417FC)
#define OVP_ClassId_Algorithm_ClassifierSVMDesc                                    OpenViBE::CIdentifier(0x272B056E, 0x0C6502AC)

#define OVP_Algorithm_ClassifierSVM_InputParameterId_SVMType					   OpenViBE::CIdentifier(0x0C347BBA, 0x180577F9)
#define OVP_Algorithm_ClassifierSVM_InputParameterId_SVMKernelType				   OpenViBE::CIdentifier(0x1952129C, 0x6BEF38D7)
#define OVP_Algorithm_ClassifierSVM_InputParameterId_SVMDegree					   OpenViBE::CIdentifier(0x0E284608, 0x7323390E)
#define OVP_Algorithm_ClassifierSVM_InputParameterId_SVMGamma					   OpenViBE::CIdentifier(0x5D4A358F, 0x29043846)
#define OVP_Algorithm_ClassifierSVM_InputParameterId_SVMCoef0					   OpenViBE::CIdentifier(0x724D5EC5, 0x13E56658)
#define OVP_ALgorithm_ClassifierSVM_InputParameterId_SVMCost					   OpenViBE::CIdentifier(0x353662E8, 0x041D7610)
#define OVP_ALgorithm_ClassifierSVM_InputParameterId_SVMNu					       OpenViBE::CIdentifier(0x62334FC3, 0x49594D32)
#define OVP_ALgorithm_ClassifierSVM_InputParameterId_SVMEpsilon					   OpenViBE::CIdentifier(0x09896FD2, 0x523775BA)
#define OVP_ALgorithm_ClassifierSVM_InputParameterId_SVMCacheSize				   OpenViBE::CIdentifier(0x4BCE65A7, 0x6A103468)
#define OVP_ALgorithm_ClassifierSVM_InputParameterId_SVMTolerance				   OpenViBE::CIdentifier(0x2658168C, 0x0914687C)
#define OVP_ALgorithm_ClassifierSVM_InputParameterId_SVMShrinking				   OpenViBE::CIdentifier(0x63F5286A, 0x6A9D18BF)
#define OVP_ALgorithm_ClassifierSVM_InputParameterId_SVMProbabilityEstimate		   OpenViBE::CIdentifier(0x05DC16EA, 0x5DBD51C2)
#define OVP_ALgorithm_ClassifierSVM_InputParameterId_SVMweight					   OpenViBE::CIdentifier(0x0BA132BE, 0x17DD3B8F)
#define OVP_Algorithm_ClassifierSVM_InputParameterId_SVMWeightLabel				   OpenViBE::CIdentifier(0x22C27048, 0x5CC6214A)

#define OVP_TypeId_SVMType														   OpenViBE::CIdentifier(0x2AF426D1, 0x72FB7BAC)
#define OVP_TypeId_SVMKernelType												   OpenViBE::CIdentifier(0x54BB0016, 0x6AA27496)

namespace OpenViBEPlugins
{
	namespace Classification
	{
		class CAlgorithmClassifierSVM : public OpenViBEToolkit::CAlgorithmClassifier, public XML::IWriterCallback, public XML::IReaderCallback
		{
		public:

			CAlgorithmClassifierSVM(void);

			virtual OpenViBE::boolean initialize(void);

			virtual OpenViBE::boolean train(const OpenViBEToolkit::IFeatureVectorSet& rFeatureVectorSet);
			virtual OpenViBE::boolean classify(const OpenViBEToolkit::IFeatureVector& rFeatureVector, OpenViBE::float64& rf64Class, OpenViBEToolkit::IVector& rClassificationValues);

			virtual OpenViBE::boolean saveConfiguration(OpenViBE::IMemoryBuffer& rMemoryBuffer);
			virtual OpenViBE::boolean loadConfiguration(const OpenViBE::IMemoryBuffer& rMemoryBuffer);
			virtual OpenViBE::CString paramToString(svm_parameter *pParam);
			virtual OpenViBE::CString modelToString();
			virtual OpenViBE::CString problemToString(svm_problem *pProb);
			_IsDerivedFromClass_Final_(CAlgorithmClassifier, OVP_ClassId_Algorithm_ClassifierSVM);

		protected:

			virtual void write(const char* sString); // XML IWriterCallback

			virtual void openChild(const char* sName, const char** sAttributeName, const char** sAttributeValue, XML::uint64 ui64AttributeCount); // XML IReaderCallback
			virtual void processChildData(const char* sData); // XML IReaderCallback
			virtual void closeChild(void); // XML ReaderCallback
			std::stack<OpenViBE::CString> m_vNode;

			std::vector <OpenViBE::float64> m_vClass;
			struct svm_parameter m_oParam;

			//struct svm_parameter *m_oParam; // set by parse_command_line
			//struct svm_problem m_oProb;     // set by read_problem
			struct svm_model *m_pModel;
			OpenViBE::int32 m_i32IndexSV;
			OpenViBE::uint32 m_ui32NumberOfFeatures;
			OpenViBE::CMemoryBuffer m_oConfiguration;
			//todo à modifier en fonction de svn_save_model
			//itpp::vec m_oCoefficients;
		};

		class CAlgorithmClassifierSVMDesc : public OpenViBEToolkit::CAlgorithmClassifierDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("SVM classifier"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Laurent Bougrain / Baptiste Payan"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("UHP_Nancy1/LORIA INRIA/LORIA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_Algorithm_ClassifierSVM; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::Classification::CAlgorithmClassifierSVM; }

			virtual OpenViBE::boolean getAlgorithmPrototype(
				OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				CAlgorithmClassifierDesc::getAlgorithmPrototype(rAlgorithmPrototype);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_ClassifierSVM_InputParameterId_SVMType,"SVM type",OpenViBE::Kernel::ParameterType_Enumeration, OVP_TypeId_SVMType);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_ClassifierSVM_InputParameterId_SVMKernelType,"Kernel type",OpenViBE::Kernel::ParameterType_Enumeration, OVP_TypeId_SVMKernelType);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_ClassifierSVM_InputParameterId_SVMDegree,"Degree",OpenViBE::Kernel::ParameterType_Integer);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_ClassifierSVM_InputParameterId_SVMGamma,"Gamma",OpenViBE::Kernel::ParameterType_Float);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_ClassifierSVM_InputParameterId_SVMCoef0,"Coef 0",OpenViBE::Kernel::ParameterType_Float);
				rAlgorithmPrototype.addInputParameter(OVP_ALgorithm_ClassifierSVM_InputParameterId_SVMCost,"Cost",OpenViBE::Kernel::ParameterType_Float);
				rAlgorithmPrototype.addInputParameter(OVP_ALgorithm_ClassifierSVM_InputParameterId_SVMNu,"Nu", OpenViBE::Kernel::ParameterType_Float);
				rAlgorithmPrototype.addInputParameter(OVP_ALgorithm_ClassifierSVM_InputParameterId_SVMEpsilon,"Epsilon",OpenViBE::Kernel::ParameterType_Float);
				rAlgorithmPrototype.addInputParameter(OVP_ALgorithm_ClassifierSVM_InputParameterId_SVMCacheSize,"Cache size",OpenViBE::Kernel::ParameterType_Float);
				rAlgorithmPrototype.addInputParameter(OVP_ALgorithm_ClassifierSVM_InputParameterId_SVMTolerance,"Epsilon tolerance",OpenViBE::Kernel::ParameterType_Float);
				rAlgorithmPrototype.addInputParameter(OVP_ALgorithm_ClassifierSVM_InputParameterId_SVMShrinking,"Shrinking",OpenViBE::Kernel::ParameterType_Boolean);
				//rAlgorithmPrototype.addInputParameter(OVP_ALgorithm_ClassifierSVM_InputParameterId_SVMProbabilityEstimate,"Probability estimate",OpenViBE::Kernel::ParameterType_Boolean);
				rAlgorithmPrototype.addInputParameter(OVP_ALgorithm_ClassifierSVM_InputParameterId_SVMweight,"Weight",OpenViBE::Kernel::ParameterType_String);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_ClassifierSVM_InputParameterId_SVMWeightLabel,"Weight Label",OpenViBE::Kernel::ParameterType_String);
				return true;
			}

			_IsDerivedFromClass_Final_(CAlgorithmClassifierDesc, OVP_ClassId_Algorithm_ClassifierSVMDesc);
		};
	};
};

#endif // __OpenViBEPlugins_Algorithm_ClassifierSVM_H__
