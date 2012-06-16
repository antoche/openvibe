#ifndef __OpenViBEToolkit_Tools_Matrix_H__
#define __OpenViBEToolkit_Tools_Matrix_H__

#include "../ovtk_base.h"

namespace OpenViBEToolkit
{
	namespace Tools
	{
		namespace Matrix
		{
			OVTK_API OpenViBE::boolean copy(OpenViBE::IMatrix& rDestinationMatrix, const OpenViBE::IMatrix& rSourceMatrix);
			OVTK_API OpenViBE::boolean copyDescription(OpenViBE::IMatrix& rDestinationMatrix, const OpenViBE::IMatrix& rSourceMatrix);
			OVTK_API OpenViBE::boolean copyContent(OpenViBE::IMatrix& rDestinationMatrix, const OpenViBE::IMatrix& rSourceMatrix);
			OVTK_API OpenViBE::boolean clearContent(OpenViBE::IMatrix& rMatrix);
			OVTK_API OpenViBE::boolean isDescriptionSimilar(const OpenViBE::IMatrix& rSourceMatrix1, const OpenViBE::IMatrix& rSourceMatrix2, const OpenViBE::boolean bCheckLabels=true);
			OVTK_API OpenViBE::boolean isContentSimilar(const OpenViBE::IMatrix& rSourceMatrix1, const OpenViBE::IMatrix& rSourceMatrix2);
			OVTK_API OpenViBE::boolean isContentValid(const OpenViBE::IMatrix& rSourceMatrix, const OpenViBE::boolean bCheckNotANumber=true, const OpenViBE::boolean bCheckInfinity=true);
		};

		namespace MatrixManipulation
		{
			inline OpenViBE::boolean copy(OpenViBE::IMatrix& rDestinationMatrix, const OpenViBE::IMatrix& rSourceMatrix)
			{
				return OpenViBEToolkit::Tools::Matrix::copy(rDestinationMatrix, rSourceMatrix);
			}

			inline OpenViBE::boolean copyDescription(OpenViBE::IMatrix& rDestinationMatrix, const OpenViBE::IMatrix& rSourceMatrix)
			{
				return OpenViBEToolkit::Tools::Matrix::copyDescription(rDestinationMatrix, rSourceMatrix);
			}

			inline OpenViBE::boolean copyContent(OpenViBE::IMatrix& rDestinationMatrix, const OpenViBE::IMatrix& rSourceMatrix)
			{
				return OpenViBEToolkit::Tools::Matrix::copyContent(rDestinationMatrix, rSourceMatrix);
			}

			inline OpenViBE::boolean clearContent(OpenViBE::IMatrix& rMatrix)
			{
				return OpenViBEToolkit::Tools::Matrix::clearContent(rMatrix);
			}
		};
	};
};

#endif // __OpenViBEToolkit_Tools_Matrix_H__
