#ifndef __OpenViBEToolkit_Tools_ColorGradient_H__
#define __OpenViBEToolkit_Tools_ColorGradient_H__

#include "../ovtk_base.h"

namespace OpenViBEToolkit
{
	namespace Tools
	{
		namespace ColorGradient
		{
			OVTK_API OpenViBE::boolean parse(OpenViBE::IMatrix& rColorGradient, const OpenViBE::CString& rString);
			OVTK_API OpenViBE::boolean format(OpenViBE::CString& rString, const OpenViBE::IMatrix& rColorGradient);
			OVTK_API OpenViBE::boolean interpolate(OpenViBE::IMatrix& rInterpolatedColorGradient, const OpenViBE::IMatrix& rColorGradient, const OpenViBE::uint32 ui32Steps);
		};
	};
};

#endif // __OpenViBEToolkit_Tools_ColorGradient_H__
