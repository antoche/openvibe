#ifndef __OpenViBEToolkit_TExperimentInformationDecoder_H__
#define __OpenViBEToolkit_TExperimentInformationDecoder_H__

#ifdef TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines

#include "../../ovtk_base.h"

#include "ovtkTDecoder.h"

namespace OpenViBEToolkit
{
	template <class T>
	class TExperimentInformationDecoderLocal : public T
	{

	protected:

		OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > m_ui64OutputExperimentIdentifier;
		OpenViBE::Kernel::TParameterHandler < OpenViBE::CString* > m_pOutputpExperimentDate;

		OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > m_ui64OutputSubjectIdentifier;
		OpenViBE::Kernel::TParameterHandler < OpenViBE::CString* > m_pOutputpSubjectName;
		OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > m_ui64OutputSubjectAge;
		OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > m_ui64OutputSubjectGender;

		OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > m_ui64OutputLaboratoryIdentifier;
		OpenViBE::Kernel::TParameterHandler < OpenViBE::CString* > m_pOutputpLaboratoryName;
		OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > m_ui64OutputTechnicianIdentifier;
		OpenViBE::Kernel::TParameterHandler < OpenViBE::CString* > m_pOutputpTechnicianName;

		using T::m_pCodec;
		using T::m_pBoxAlgorithm;
		using T::m_pInputMemoryBuffer;

		OpenViBE::boolean initializeImpl()
		{
			m_pCodec = &m_pBoxAlgorithm->getAlgorithmManager().getAlgorithm(m_pBoxAlgorithm->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_ExperimentInformationStreamDecoder));
			m_pCodec->initialize();

			m_pInputMemoryBuffer.initialize(m_pCodec->getInputParameter(OVP_GD_Algorithm_ExperimentInformationStreamDecoder_InputParameterId_MemoryBufferToDecode));

			m_ui64OutputExperimentIdentifier.initialize(m_pCodec->getOutputParameter(OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_ExperimentIdentifier));
			m_pOutputpExperimentDate.initialize(m_pCodec->getOutputParameter(OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_ExperimentDate));

			m_ui64OutputSubjectIdentifier.initialize(m_pCodec->getOutputParameter(OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_SubjectIdentifier));
			m_pOutputpSubjectName.initialize(m_pCodec->getOutputParameter(OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_SubjectName));
			m_ui64OutputSubjectAge.initialize(m_pCodec->getOutputParameter(OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_SubjectAge));
			m_ui64OutputSubjectGender.initialize(m_pCodec->getOutputParameter(OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_SubjectGender));

			m_ui64OutputLaboratoryIdentifier.initialize(m_pCodec->getOutputParameter(OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_LaboratoryIdentifier));
			m_pOutputpLaboratoryName.initialize(m_pCodec->getOutputParameter(OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_LaboratoryName));
			m_ui64OutputTechnicianIdentifier.initialize(m_pCodec->getOutputParameter(OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_TechnicianIdentifier));
			m_pOutputpTechnicianName.initialize(m_pCodec->getOutputParameter(OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_TechnicianName));

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

			m_ui64OutputExperimentIdentifier.uninitialize();
			m_pOutputpExperimentDate.uninitialize();
			m_ui64OutputSubjectIdentifier.uninitialize();
			m_pOutputpSubjectName.uninitialize();
			m_ui64OutputSubjectAge.uninitialize();
			m_ui64OutputSubjectGender.uninitialize();
			m_ui64OutputLaboratoryIdentifier.uninitialize();
			m_pOutputpLaboratoryName.uninitialize();
			m_ui64OutputTechnicianIdentifier.uninitialize();
			m_pOutputpTechnicianName.uninitialize();

			m_pInputMemoryBuffer.uninitialize();
			m_pCodec->uninitialize();
			m_pBoxAlgorithm->getAlgorithmManager().releaseAlgorithm(*m_pCodec);
			m_pBoxAlgorithm = NULL;

			return true;
		}

		OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 >& getOutputExperimentIdentifier()
		{
			return m_ui64OutputExperimentIdentifier;
		}

		OpenViBE::Kernel::TParameterHandler < OpenViBE::CString* >& getOutputExperimentDate()
		{
			return m_pOutputpExperimentDate;
		}

		OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 >& getOutputSubjectIdentifier()
		{
			return m_ui64OutputSubjectIdentifier;
		}

		OpenViBE::Kernel::TParameterHandler < OpenViBE::CString* >& getOutputSubjectName()
		{
			return m_pOutputpSubjectName;
		}

		OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 >& getOutputSubjectAge()
		{
			return m_ui64OutputSubjectAge;
		}

		OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 >& getOutputSubjectGender()
		{
			return m_ui64OutputSubjectGender;
		}

		OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 >& getOutputLaboratoryIdentifier()
		{
			return m_ui64OutputLaboratoryIdentifier;
		}

		OpenViBE::Kernel::TParameterHandler < OpenViBE::CString* >& getOutputLaboratoryName()
		{
			return m_pOutputpLaboratoryName;
		}

		OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 >& getOutputTechnicianIdentifier()
		{
			return m_ui64OutputTechnicianIdentifier;
		}

		OpenViBE::Kernel::TParameterHandler < OpenViBE::CString* >& getOutputTechnicianName()
		{
			return m_pOutputpTechnicianName;
		}

		virtual OpenViBE::boolean isHeaderReceived()
		{
			return m_pCodec->isOutputTriggerActive(OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputTriggerId_ReceivedHeader);
		}

		virtual OpenViBE::boolean isBufferReceived()
		{
			return m_pCodec->isOutputTriggerActive(OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputTriggerId_ReceivedBuffer);
		}

		virtual OpenViBE::boolean isEndReceived()
		{
			return m_pCodec->isOutputTriggerActive(OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputTriggerId_ReceivedEnd);
		}
	};

	template <class T>
	class TExperimentInformationDecoder : public TExperimentInformationDecoderLocal < TDecoder < T > >
	{
	private:
		using TExperimentInformationDecoderLocal < TDecoder < T > >::m_pBoxAlgorithm;
	public:
		using TExperimentInformationDecoderLocal < TDecoder < T > >::uninitialize;

		TExperimentInformationDecoder()
		{
		}
		TExperimentInformationDecoder(T& rBoxAlgorithm)
		{
			m_pBoxAlgorithm = NULL;
			this->initialize(rBoxAlgorithm);
		}
		virtual ~TExperimentInformationDecoder()
		{
			this->uninitialize();
		}
	};
};

#endif // TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines

#endif //__OpenViBEToolkit_TExperimentInformationDecoder_H__
