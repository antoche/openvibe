#ifndef __SamplePlugin_Algorithms_CStreamedMatrixDecoder_H__
#define __SamplePlugin_Algorithms_CStreamedMatrixDecoder_H__

#include "ovpCEBMLBaseDecoder.h"

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CStreamedMatrixDecoder : public OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoder
		{
		public:

			CStreamedMatrixDecoder(void);

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoder, OVP_ClassId_Algorithm_StreamedMatrixStreamDecoder);

			// ebml callbacks
			virtual EBML::boolean isMasterChild(const EBML::CIdentifier& rIdentifier);
			virtual void openChild(const EBML::CIdentifier& rIdentifier);
			virtual void processChildData(const void* pBuffer, const EBML::uint64 ui64BufferSize);
			virtual void closeChild(void);

		protected:

			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* > op_pMatrix;

		private:

			enum
			{
				Status_ParsingNothing,
				Status_ParsingHeader,
				Status_ParsingBuffer,
				Status_ParsingDimension,
			};

			std::stack<EBML::CIdentifier> m_vNodes;

			OpenViBE::uint32 m_ui32Status;
			OpenViBE::uint32 m_ui32DimensionIndex;
			OpenViBE::uint32 m_ui32DimensionEntryIndex;
			OpenViBE::uint64 m_ui64MatrixBufferSize;
		};

		class CStreamedMatrixDecoderDesc : public OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoderDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Streamed matrix stream decoder"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Stream codecs/Decoders"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_Algorithm_StreamedMatrixStreamDecoder; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::StreamCodecs::CStreamedMatrixDecoder(); }

			virtual OpenViBE::boolean getAlgorithmPrototype(
				OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoderDesc::getAlgorithmPrototype(rAlgorithmPrototype);

				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_StreamedMatrixStreamDecoder_OutputParameterId_Matrix, "Matrix", OpenViBE::Kernel::ParameterType_Matrix);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoderDesc, OVP_ClassId_Algorithm_StreamedMatrixStreamDecoderDesc);
		};
	};
};

#endif // __SamplePlugin_Algorithms_CStreamedMatrixDecoder_H__
