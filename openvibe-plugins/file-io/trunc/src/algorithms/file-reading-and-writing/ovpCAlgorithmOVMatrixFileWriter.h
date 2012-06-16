#ifndef __OpenViBEPlugins_Algorithm_OVMatrixFileWriter_H__
#define __OpenViBEPlugins_Algorithm_OVMatrixFileWriter_H__

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <openvibe-toolkit/ovtk_all.h>

#include <fstream>

namespace OpenViBEPlugins
{
	namespace FileIO
	{
		class CAlgorithmOVMatrixFileWriter : public OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >, OVP_ClassId_Algorithm_OVMatrixFileWriter);

		protected:
			OpenViBE::boolean dumpHeader();

			OpenViBE::boolean dumpBuffer(
				OpenViBE::uint32 ui32DimensionIndex,
				OpenViBE::uint32& ui32ElementIndex);

			OpenViBE::Kernel::TParameterHandler < OpenViBE::CString* > ip_sFilename;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::CMatrix* > ip_pMatrix;
			std::ofstream m_oDataFile;
		};

		class CAlgorithmOVMatrixFileWriterDesc : public OpenViBE::Plugins::IAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("OpenViBE Matrix file writer"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Vincent Delannoy"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("File reading and writing"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_Algorithm_OVMatrixFileWriter; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::FileIO::CAlgorithmOVMatrixFileWriter; }

			virtual OpenViBE::boolean getAlgorithmPrototype(
				OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				rAlgorithmPrototype.addInputParameter (OVP_Algorithm_OVMatrixFileWriter_InputParameterId_Filename, "Filename", OpenViBE::Kernel::ParameterType_String);
				rAlgorithmPrototype.addInputParameter (OVP_Algorithm_OVMatrixFileWriter_InputParameterId_Matrix,   "Matrix",   OpenViBE::Kernel::ParameterType_Matrix);
				//rAlgorithmPrototype.addOutputTrigger  (OVP_Algorithm_OVMatrixFileWriter_OutputTriggerId_Error,              "Error");
				//rAlgorithmPrototype.addOutputTrigger  (OVP_Algorithm_OVMatrixFileWriter_OutputTriggerId_DataProduced,       "Data produced");
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IAlgorithmDesc, OVP_ClassId_Algorithm_OVMatrixFileWriterDesc);
		};
	};
};

#endif // __OpenViBEPlugins_Algorithm_OVMatrixFileWriter_H__
