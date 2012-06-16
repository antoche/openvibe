#ifndef __OpenViBEToolkit_TExperimentInformationEncoder_H__
#define __OpenViBEToolkit_TExperimentInformationEncoder_H__

#ifdef TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines

#include "../../ovtk_base.h"

#include "ovtkTEncoder.h"

namespace OpenViBEToolkit
{
	template <class T>
	class TExperimentInformationEncoderLocal : public T
	{

	protected:

		OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > m_ui64InputExperimentIdentifier;
		OpenViBE::Kernel::TParameterHandler < OpenViBE::CString* > m_pInputpExperimentDate;

		OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > m_ui64InputSubjectIdentifier;
		OpenViBE::Kernel::TParameterHandler < OpenViBE::CString* > m_pInputpSubjectName;
		OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > m_ui64InputSubjectAge;
		OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > m_ui64InputSubjectGender;

		OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > m_ui64InputLaboratoryIdentifier;
		OpenViBE::Kernel::TParameterHandler < OpenViBE::CString* > m_pInputpLaboratoryName;
		OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > m_ui64InputTechnicianIdentifier;
		OpenViBE::Kernel::TParameterHandler < OpenViBE::CString* > m_pInputTechnicianName;

		using T::m_pCodec;
		using T::m_pBoxAlgorithm;
		using T::m_pOutputMemoryBuffer;

		OpenViBE::boolean initializeImpl()
		{
			m_pCodec = &m_pBoxAlgorithm->getAlgorithmManager().getAlgorithm(m_pBoxAlgorithm->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_ExperimentInformationStreamEncoder));
			m_pCodec->initialize();
			m_ui64InputExperimentIdentifier.initialize(m_pCodec->getInputParameter(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_ExperimentIdentifier));
			m_pInputpExperimentDate.initialize(m_pCodec->getInputParameter(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_ExperimentDate));

			m_ui64InputSubjectIdentifier.initialize(m_pCodec->getInputParameter(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_SubjectIdentifier));
			m_pInputpSubjectName.initialize(m_pCodec->getInputParameter(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_SubjectName));
			m_ui64InputSubjectAge.initialize(m_pCodec->getInputParameter(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_SubjectAge));
			m_ui64InputSubjectGender.initialize(m_pCodec->getInputParameter(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_SubjectGender));

			m_ui64InputLaboratoryIdentifier.initialize(m_pCodec->getInputParameter(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_LaboratoryIdentifier));
			m_pInputpLaboratoryName.initialize(m_pCodec->getInputParameter(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_LaboratoryName));
			m_ui64InputTechnicianIdentifier.initialize(m_pCodec->getInputParameter(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_TechnicianIdentifier));
			m_pInputTechnicianName.initialize(m_pCodec->getInputParameter(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_TechnicianName));

			m_pOutputMemoryBuffer.initialize(m_pCodec->getOutputParameter(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_OutputParameterId_EncodedMemoryBuffer));

			return true;
		}

	public:
		using T::initialize;

		OpenViBE::boolean uninitialize(void)
		{
			if(m_pBoxAlgorithm == NULL || m_pCodec == NULL)
			{
				return false;
			}

			m_ui64InputExperimentIdentifier.uninitialize();
			m_pInputpExperimentDate.uninitialize();
			m_ui64InputSubjectIdentifier.uninitialize();
			m_pInputpSubjectName.uninitialize();
			m_ui64InputSubjectAge.uninitialize();
			m_ui64InputSubjectGender.uninitialize();
			m_ui64InputLaboratoryIdentifier.uninitialize();
			m_pInputpLaboratoryName.uninitialize();
			m_ui64InputTechnicianIdentifier.uninitialize();
			m_pInputTechnicianName.uninitialize();

			m_pOutputMemoryBuffer.uninitialize();
			m_pCodec->uninitialize();
			m_pBoxAlgorithm->getAlgorithmManager().releaseAlgorithm(*m_pCodec);
			m_pBoxAlgorithm = NULL;

			return true;
		}

		OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 >& getInputExperimentIdentifier()
		{
			return m_ui64InputExperimentIdentifier;
		}

		OpenViBE::Kernel::TParameterHandler < OpenViBE::CString* >& getInputExperimentDate()
		{
			return m_pInputpExperimentDate;
		}

		OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 >& getInputSubjectIdentifier()
		{
			return m_ui64InputSubjectIdentifier;
		}

		OpenViBE::Kernel::TParameterHandler < OpenViBE::CString* >& getInputSubjectName()
		{
			return m_pInputpSubjectName;
		}

		OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 >& getInputSubjectAge()
		{
			return m_ui64InputSubjectAge;
		}

		OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 >& getInputSubjectGender()
		{
			return m_ui64InputSubjectGender;
		}

		OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 >& getInputLaboratoryIdentifier()
		{
			return m_ui64InputLaboratoryIdentifier;
		}

		OpenViBE::Kernel::TParameterHandler < OpenViBE::CString* >& getInputLaboratoryName()
		{
			return m_pInputpLaboratoryName;
		}

		OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 >& getInputTechnicianIdentifier()
		{
			return m_ui64InputTechnicianIdentifier;
		}

		OpenViBE::Kernel::TParameterHandler < OpenViBE::CString* >& getInputTechnicianName()
		{
			return m_pInputTechnicianName;
		}

	protected:
		OpenViBE::boolean encodeHeaderImpl(void)
		{
			return m_pCodec->process(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputTriggerId_EncodeHeader);
		}

		OpenViBE::boolean encodeBufferImpl(void)
		{
			return m_pCodec->process(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputTriggerId_EncodeBuffer);
		}

		OpenViBE::boolean encodeEndImpl(void)
		{
			return m_pCodec->process(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputTriggerId_EncodeEnd);
		}

	};

	template <class T>
	class TExperimentInformationEncoder : public TExperimentInformationEncoderLocal < TEncoder < T > >
	{
	private:
		using TExperimentInformationEncoderLocal < TEncoder < T > >::m_pBoxAlgorithm;
	public:
		using TExperimentInformationEncoderLocal < TEncoder < T > >::uninitialize;

		TExperimentInformationEncoder()
		{
		}
		TExperimentInformationEncoder(T& rBoxAlgorithm)
		{
			m_pBoxAlgorithm = NULL;
			this->initialize(rBoxAlgorithm);
		}
		virtual ~TExperimentInformationEncoder()
		{
			this->uninitialize();
		}
	};
};

#endif // TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines

#endif //__OpenViBEToolkit_TExperimentInformationEncoder_H__
