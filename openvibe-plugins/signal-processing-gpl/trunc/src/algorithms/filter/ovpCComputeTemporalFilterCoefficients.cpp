#include "ovpCComputeTemporalFilterCoefficients.h"

#include <limits>
#include <cmath>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessingGpl;
using namespace itpp;
using namespace std;
// ________________________________________________________________________________________________________________
//

// Add 2complexes
void CComputeTemporalFilterCoefficients::addComplex(cmplex *a, cmplex *b, cmplex *c)
{
	c->real = b->real + a->real;
	c->imag = b->imag + a->imag;
}

// Substract 2 complex
void CComputeTemporalFilterCoefficients::subComplex(cmplex *a, cmplex *b, cmplex *c )
{
	c->real = b->real - a->real;
	c->imag = b->imag - a->imag;
}

// Multiply 2 complexes
void CComputeTemporalFilterCoefficients::mulComplex(cmplex *a, cmplex *b, cmplex *c)
{
	double y;
	y  = b->real * a->real -  b->imag * a->imag;
	c->imag = b->real * a->imag  +  b->imag * a->real;
	c->real= y;
}

// Divide 2 complex numbers
void CComputeTemporalFilterCoefficients::divComplex(cmplex *a, cmplex *b, cmplex *c)
{
	double y, p, q, w;

	y = a->real * a->real + a->imag * a->imag;
	p = b->real * a->real + b->imag * a->imag;
	q = b->imag * a->real - b->real * a->imag;

	if(y < 1.0 )
	{
		w = MAXNUM * y;
		if((fabs(p) > w) || (fabs(q) > w) || (y == 0.0) )
		{
			c->real = MAXNUM;
			c->imag = MAXNUM;
			cout << "divCOMPLEX: OVERFLOW" << std::endl;
			return;
		}
	}
	c->real = p/y;
	c->imag = q/y;
}

// Compute abs of a complex
double CComputeTemporalFilterCoefficients::absComplex(cmplex *z)
{
	double x, y, b, re, im;
	int ex, ey, e;

	re = fabs( z->real );
	im = fabs( z->imag );

	if(re == 0.0 )
	{
		return( im );
	}
	if(im == 0.0 )
	{
		return( re );
	}

	// Get the exponents of the numbers
	x = frexp( re, &ex );
	y = frexp( im, &ey );

	// Check if one number is tiny compared to the other
	e = ex - ey;
	if(e > PREC )
	{
		return( re );
	}
	if(e < -PREC )
	{
		return( im );
	}

	// Find approximate exponent e of the geometric mean.
	e = (ex + ey) >> 1;

	// Rescale so mean is about 1
	x = ldexp( re, -e );
	y = ldexp( im, -e );

	// Hypotenuse of the right triangle
	b = sqrt( x * x  +  y * y );

	// Compute the exponent of the answer.
	y = frexp( b, &ey );
	ey = e + ey;

	// Check it for overflow and underflow.
	if(ey > MAXEXP )
	{
		cout << "absCOMPLEX: OVERFLOW" << std::endl;
		return( std::numeric_limits<double>::infinity() );
	}
	if(ey < MINEXP )
	{
		return(0.0);
	}

	// Undo the scaling
	b = ldexp( b, e );
	return( b );
}

// Compute sqrt of a complex number
void CComputeTemporalFilterCoefficients::sqrtComplex(cmplex *z, cmplex *w )
{
	cmplex q, s;
	double x, y, r, t;

	x = z->real;
	y = z->imag;

	if(y == 0.0 )
	{
		if(x < 0.0 )
		{
			w->real = 0.0;
			w->imag = sqrt(-x);
			return;
		}
		else
		{
			w->real = sqrt(x);
			w->imag = 0.0;
			return;
		}
	}

	if(x == 0.0 )
	{
		r = fabs(y);
		r = sqrt(0.5*r);
		if(y > 0 )
		{
			w->real = r;
		}
		else
		{
			w->real = -r;
		}
		w->imag = r;
		return;
	}

	// Approximate  sqrt(x^2+y^2) - x  =  y^2/2x - y^4/24x^3 + ... .
	// The relative error in the first term is approximately y^2/12x^2 .
	if((fabs(y) < 2.e-4 * fabs(x))  && (x > 0) )
	{
		t = 0.25*y*(y/x);
	}
	else
	{
		r = absComplex(z);
		t = 0.5*(r - x);
	}
	r = sqrt(t);
	q.imag = r;
	q.real = y/(2.0*r);
	// Heron iteration in complex arithmetic
	divComplex( &q, z, &s );
	addComplex( &q, &s, w );
	w->real *= 0.5;
	w->imag *= 0.5;
}

// compute s plane poles and zeros
void CComputeTemporalFilterCoefficients::findSPlanePolesAndZeros(void)
{
	m_ui32NbPoles = (m_ui32FilterOrder+1)/2;
	m_ui32NbZeros = 0;
	m_vecZs = zeros(m_ui32ArraySize);

	float64 l_float64m;
	uint32 l_ui32lr = 0;
	uint32 l_ui32ir = 0;
	uint32 l_ui32ii = 0;
	float64 l_float64b;
	float64 l_float64a;

	if(m_ui64FilterMethod==OVP_TypeId_FilterMethod_Butterworth)//poles equally spaced around the unit circle
	{
		if(m_ui32FilterOrder & 1 )
		{
			l_float64m = 0.0;
		}
		else
		{
			l_float64m = pi / (2.0*(float64)m_ui32FilterOrder);
		}

		for(uint32 i=0; i<m_ui32NbPoles; i++ )// poles
		{
			l_ui32lr = i + i;
			m_vecZs[l_ui32lr] = -cos(l_float64m);
			m_vecZs[l_ui32lr+1] = sin(l_float64m);
			l_float64m += pi /(float64)m_ui32FilterOrder;
		}

		if(m_ui64FilterType==OVP_TypeId_FilterType_HighPass || m_ui64FilterType==OVP_TypeId_FilterType_BandStop ) // high pass or band reject
		{
			// map s => 1/s
			for(uint32 j=0; j<m_ui32NbPoles; j++ )
			{
				l_ui32ir = j + j;
				l_ui32ii = l_ui32ir + 1;
				l_float64b = m_vecZs[l_ui32ir]*m_vecZs[l_ui32ir] + m_vecZs[l_ui32ii]*m_vecZs[l_ui32ii];
				m_vecZs[l_ui32ir] = m_vecZs[l_ui32ir] / l_float64b;
				m_vecZs[l_ui32ii] = m_vecZs[l_ui32ii] / l_float64b;
			}

			// The zeros at infinity map to the origin.
			m_ui32NbZeros = m_ui32NbPoles;
			if(m_ui64FilterType==OVP_TypeId_FilterType_BandStop)
			{
				m_ui32NbZeros += m_ui32FilterOrder/2;
			}
			for(uint32 j=0; j<m_ui32NbZeros; j++ )
			{
				l_ui32ir = l_ui32ii + 1;
				l_ui32ii = l_ui32ir + 1;
				m_vecZs[l_ui32ir] = 0.0;
				m_vecZs[l_ui32ii] = 0.0;
			}
		}
	}

	if(m_ui64FilterMethod==OVP_TypeId_FilterMethod_Chebychev)
	{
		//For Chebyshev, find radii of two Butterworth circles (See Gold & Rader, page 60)
		m_float64Rho = (m_float64Phi - 1.0)*(m_float64Phi+1);  // m_float64Rho = m_float64Eps^2 = {sqrt(1+m_float64Eps^2)}^2 - 1
		m_float64Eps = sqrt(m_float64Rho);
		// sqrt( 1 + 1/m_float64Eps^2 ) + 1/m_float64Eps  = {sqrt(1 + m_float64Eps^2)  +  1} / m_float64Eps
		m_float64Phi = (m_float64Phi + 1.0) / m_float64Eps;
		m_float64Phi = pow( m_float64Phi, (float64)1.0/m_ui32FilterOrder );  // raise to the 1/n power
		l_float64b = 0.5 * (m_float64Phi + 1.0/m_float64Phi); // y coordinates are on this circle
		l_float64a = 0.5 * (m_float64Phi - 1.0/m_float64Phi); // x coordinates are on this circle
		if(m_ui32FilterOrder & 1 )
		{
			l_float64m = 0.0;
		}
		else
		{
			l_float64m = pi / (2.0*(float64)m_ui32FilterOrder);
		}

		for(uint32 i=0; i<m_ui32NbPoles; i++ )// poles
		{
			l_ui32lr = i + i;
			m_vecZs[l_ui32lr] = -l_float64a * cos(l_float64m);
			m_vecZs[l_ui32lr+1] = l_float64b * sin(l_float64m);
			l_float64m += pi /(float64)m_ui32FilterOrder;
		}

		if(m_ui64FilterType==OVP_TypeId_FilterType_HighPass || m_ui64FilterType==OVP_TypeId_FilterType_BandStop)// high pass or band reject
		{
			// map s => 1/s
			for(uint32 j=0; j<m_ui32NbPoles; j++ )
			{
				l_ui32ir = j + j;
				l_ui32ii = l_ui32ir + 1;
				l_float64b = m_vecZs[l_ui32ir]*m_vecZs[l_ui32ir] + m_vecZs[l_ui32ii]*m_vecZs[l_ui32ii];
				m_vecZs[l_ui32ir] = m_vecZs[l_ui32ir] / l_float64b;
				m_vecZs[l_ui32ii] = m_vecZs[l_ui32ii] / l_float64b;
			}
			// The zeros at infinity map to the origin.
			m_ui32NbZeros = m_ui32NbPoles;
			if(m_ui64FilterType==OVP_TypeId_FilterType_BandStop)
			{
				m_ui32NbZeros += m_ui32FilterOrder/2;
			}

			for(uint32 j=0; j<m_ui32NbZeros; j++ )
			{
				l_ui32ir = l_ui32ii + 1;
				l_ui32ii = l_ui32ir + 1;
				m_vecZs[l_ui32ir] = 0.0;
				m_vecZs[l_ui32ii] = 0.0;
			}
		}
	}
}

//convert s plane poles and zeros to the z plane.
void CComputeTemporalFilterCoefficients::convertSPlanePolesAndZerosToZPlane(void)
{
	// Vars
	cmplex l_complexR, l_complexCnum, l_complexCden, l_complexCwc, l_complexCa, l_complexCb, l_complexB4ac;
	cmplex l_complexCone = {1.0, 0.0};
	cmplex* l_complexZ = new cmplex[m_ui32ArraySize];
	float64* l_float64pp = new float64[m_ui32ArraySize];
	float64* l_float64y = new float64[m_ui32ArraySize];
	float64* l_float64aa = new float64[m_ui32ArraySize];

	float64 l_float64C=0.0;
	float64 l_float64a=0.0;
	float64 l_float64b=0.0;
	float64 l_float64pn=0.0;
	float64 l_float64an=0.0;
	float64 l_float64gam=0.0;
	float64 l_float64ai=0.0;
	float64 l_float64cng=0.0;
	float64 l_float64gain=0.0;
	uint32 l_ui32nc=0;
	uint32 l_ui32jt=0;
	uint32 l_ui32ii=0;
	uint32 l_ui32ir=0;
	uint32 l_uint32jj=0;
	uint32 l_uint32jh=0;
	uint32 l_uint32jl=0;
	uint32 l_uint32mh=0;

	l_float64C = m_float64TanAng;

	for (uint32 i=0; i<m_ui32ArraySize; i++)
	{
		l_complexZ[i].real = 0.0;
		l_complexZ[i].imag = 0.0;
	}

	l_ui32nc = m_ui32NbPoles;
	l_ui32jt = -1;
	l_ui32ii = -1;

	for (uint32 icnt=0; icnt<2; icnt++)
	{
		do
		{
			l_ui32ir = l_ui32ii + 1;
			l_ui32ii = l_ui32ir + 1;

			l_complexR.real = m_vecZs[l_ui32ir];
			l_complexR.imag = m_vecZs[l_ui32ii];

			if(m_ui64FilterType==OVP_TypeId_FilterType_LowPass || m_ui64FilterType==OVP_TypeId_FilterType_HighPass)
			{
				// Substitute  s - r  =  s/wc - r = (1/wc)(z-1)/(z+1) - r
				//
				//     1  1 - r wc (       1 + r wc )
				// =  --- -------- ( z  -  -------- )
				//    z+1    wc    (       1 - r wc )
				//
				// giving the root in the z plane.
				l_complexCnum.real = 1+l_float64C*l_complexR.real;
				l_complexCnum.imag = l_float64C*l_complexR.imag;
				l_complexCden.real = 1-l_float64C*l_complexR.real;
				l_complexCden.imag = -l_float64C*l_complexR.imag;
				l_ui32jt += 1;
				divComplex(&l_complexCden, &l_complexCnum, &l_complexZ[l_ui32jt]);

				if(l_complexR.imag != 0.0 )
				{
					// fill in complex conjugate root
					l_ui32jt += 1;
					l_complexZ[l_ui32jt].real = l_complexZ[l_ui32jt-1].real;
					l_complexZ[l_ui32jt].imag = -l_complexZ[l_ui32jt-1].imag;
				}
			}

			if(m_ui64FilterType==OVP_TypeId_FilterType_BandPass || m_ui64FilterType==OVP_TypeId_FilterType_BandStop)
			{
				// Substitute  s - r  =>  s/wc - r
				//
				//     z^2 - 2 z cgam + 1
				// =>  ------------------  -  r
				//         (z^2 + 1) wc
				//
				//         1
				// =  ------------  [ (1 - r wc) z^2  - 2 cgam z  +  1 + r wc ]
				//    (z^2 + 1) wc
				//
				// and solve for the roots in  the z plane.

				if(m_ui64FilterMethod==OVP_TypeId_FilterMethod_Chebychev)
				{
					l_complexCwc.real = m_float64cbp;
				}
				else
				{
					l_complexCwc.real = m_float64TanAng;
				}
				l_complexCwc.imag = 0.0;

				// r * wc //
				mulComplex(&l_complexR, &l_complexCwc, &l_complexCnum);
				// a = 1 - r wc //
				subComplex(&l_complexCnum, &l_complexCone, &l_complexCa);
				// 1 - (r wc)^2 //
				mulComplex(&l_complexCnum, &l_complexCnum, &l_complexB4ac);
				subComplex(&l_complexB4ac, &l_complexCone, &l_complexB4ac);
				// 4ac //
				l_complexB4ac.real *= 4.0;
				l_complexB4ac.imag *= 4.0;
				// b //
				l_complexCb.real = -2.0*m_float64CosGam;
				l_complexCb.imag = 0.0;
				// b^2 //
				mulComplex(&l_complexCb, &l_complexCb, &l_complexCnum);
				// b^2 - 4ac//
				subComplex(&l_complexB4ac, &l_complexCnum, &l_complexB4ac);
				// sqrt() //
				sqrtComplex(&l_complexB4ac, &l_complexB4ac);
				// -b //
				l_complexCb.real = 	-l_complexCb.real;
				l_complexCb.imag = -l_complexCb.imag;
				// 2a //
				l_complexCa.real *= 2.0;
				l_complexCa.imag *= 2.0;
				// -b +sqrt(b²-4ac) //
				addComplex(&l_complexB4ac, &l_complexCb, &l_complexCnum);
				// ... /2a //
				divComplex(&l_complexCa, &l_complexCnum, &l_complexCnum);
				l_ui32jt += 1;

				l_complexZ[l_ui32jt].real = l_complexCnum.real;
				l_complexZ[l_ui32jt].imag = l_complexCnum.imag;

				if(l_complexCnum.imag!=0.0)
				{
					l_ui32jt += 1;
					l_complexZ[l_ui32jt].real = l_complexCnum.real;
					l_complexZ[l_ui32jt].imag = -l_complexCnum.imag;
				}
				if((l_complexR.imag!=0.0)||l_complexCnum.imag==0.0)
				{
					// -b - sqrt( b^2 - 4ac) //
					subComplex(&l_complexB4ac, &l_complexCb , &l_complexCnum);
					// ... /2a //
					divComplex(&l_complexCa, &l_complexCnum, &l_complexCnum);

					l_ui32jt += 1;
					l_complexZ[l_ui32jt].real = l_complexCnum.real;
					l_complexZ[l_ui32jt].imag = l_complexCnum.imag;

					if(l_complexCnum.imag!=0.0)
					{
						l_ui32jt += 1;
						l_complexZ[l_ui32jt].real = l_complexCnum.real;
						l_complexZ[l_ui32jt].imag = -l_complexCnum.imag;
					}
				}
			}
		}
		while( --l_ui32nc > 0 );

		if(icnt == 0 )
		{
			m_ui32zord = l_ui32jt+1;
			if(m_ui32NbZeros <= 0 )
			{
				icnt = 2;
			}
		}
		l_ui32nc = m_ui32NbZeros;
	}

	// Generate the remaining zeros
	while( 2*m_ui32zord - 1 > l_ui32jt )
	{
		if(m_ui64FilterType!=OVP_TypeId_FilterType_HighPass)
		{
			l_ui32jt += 1;
			l_complexZ[l_ui32jt].real = -1.0;
			l_complexZ[l_ui32jt].imag = 0.0;
		}

		if(m_ui64FilterType==OVP_TypeId_FilterType_BandPass || m_ui64FilterType==OVP_TypeId_FilterType_HighPass )
		{
			l_ui32jt += 1;
			l_complexZ[l_ui32jt].real = 1.0;
			l_complexZ[l_ui32jt].imag = 0.0;
		}
	}

	// Expand the poles and zeros into numerator and denominator polynomials
	for (uint32 j=0; j< m_ui32ArraySize; j++)
	{
		l_float64aa[j] = 0.0;
	}
	for(uint32 icnt=0; icnt<2; icnt++ )
	{
		for (uint32 j=0; j< m_ui32ArraySize; j++)
		{
			l_float64pp[j] = 0.0;
			l_float64y[j] = 0.0;
		}
		l_float64pp[0] = 1.0;

		for(uint32 j=0; j<m_ui32zord; j++ )
		{
			l_uint32jj = j;
			if(icnt)
			{
				l_uint32jj += m_ui32zord;
			}

			l_float64a = l_complexZ[l_uint32jj].real;
			l_float64b = l_complexZ[l_uint32jj].imag;
			for(uint32 k=0; k<=j; k++ )
			{
				l_uint32jh = j - k;
				l_float64pp[l_uint32jh+1] = l_float64pp[l_uint32jh+1] - l_float64a * l_float64pp[l_uint32jh] + l_float64b * l_float64y[l_uint32jh];
				l_float64y[l_uint32jh+1] =  l_float64y[l_uint32jh+1]  - l_float64b * l_float64pp[l_uint32jh] - l_float64a * l_float64y[l_uint32jh];
			}
		}

		if(icnt == 0 )
		{
			for(uint32 j=0; j<=m_ui32zord; j++ )
			{
				l_float64aa[j] = l_float64pp[j];
			}
		}
	}

	// Scale factors of the pole and zero polynomials
	if(m_ui64FilterType==OVP_TypeId_FilterType_HighPass)
	{
		l_float64a = -1.0;
	}
	else
	{
		l_float64a = 1.0;
	}

	if(m_ui64FilterType==OVP_TypeId_FilterType_HighPass || m_ui64FilterType==OVP_TypeId_FilterType_LowPass|| m_ui64FilterType==OVP_TypeId_FilterType_BandStop )
	{
		l_float64pn = 1.0;
		l_float64an = 1.0;
		for(uint32 j=1; j<=m_ui32zord; j++ )
		{
			l_float64pn = l_float64a * l_float64pn + l_float64pp[j];
			l_float64an = l_float64a * l_float64an + l_float64aa[j];
		}
	}

	if(m_ui64FilterType==OVP_TypeId_FilterType_BandPass)
	{
		l_float64gam = pi/2.0 - asin( m_float64CosGam );  // = acos( cgam ) //
		l_uint32mh = m_ui32zord/2;
		l_float64pn = l_float64pp[l_uint32mh];
		l_float64an = l_float64aa[l_uint32mh];
		l_float64ai = 0.0;
		if(l_uint32mh > ((m_ui32zord/4)*2) )
		{
			l_float64ai = 1.0;
			l_float64pn = 0.0;
			l_float64an = 0.0;
		}
		for(uint32 j=1; j<=l_uint32mh; j++ )
		{
			l_float64a = l_float64gam * j - l_float64ai * pi/ 2.0;
			l_float64cng = cos(l_float64a);
			l_uint32jh = l_uint32mh + j;
			l_uint32jl = l_uint32mh - j;
			l_float64pn = l_float64pn + l_float64cng * (l_float64pp[l_uint32jh] + (1.0 - 2.0 * l_float64ai) * l_float64pp[l_uint32jl]);
			l_float64an = l_float64an + l_float64cng * (l_float64aa[l_uint32jh] + (1.0 - 2.0 * l_float64ai) * l_float64aa[l_uint32jl]);
		}
	}

	l_float64gain = l_float64an/(l_float64pn*m_float64Scale);
	if(l_float64pn == 0.0)
	{
		l_float64gain = 1.0;
	}

	for(uint32 j=0; j<=m_ui32zord; j++ )
	{
		l_float64pp[j] = l_float64gain * l_float64pp[j];
	}

	for(uint32 j=0; j<=m_ui32zord; j++ )
	{
		m_vecDenomCoefFilter[j] = l_float64pp[j];
		m_vecNumCoefFilter[j] = l_float64aa[j];
	}

	delete [] l_complexZ;
	delete [] l_float64pp;
	delete [] l_float64y;
	delete [] l_float64aa;
}

boolean CComputeTemporalFilterCoefficients::initialize(void)
{
	ip_ui64SamplingFrequency.initialize(getInputParameter(OVP_Algorithm_ComputeTemporalFilterCoefficients_InputParameterId_SamplingFrequency));
	ip_ui64FilterMethod.initialize(getInputParameter(OVP_Algorithm_ComputeTemporalFilterCoefficients_InputParameterId_FilterMethod));
	ip_ui64FilterType.initialize(getInputParameter(OVP_Algorithm_ComputeTemporalFilterCoefficients_InputParameterId_FilterType));
	ip_ui64FilterOrder.initialize(getInputParameter(OVP_Algorithm_ComputeTemporalFilterCoefficients_InputParameterId_FilterOrder));
	ip_f64LowCutFrequency.initialize(getInputParameter(OVP_Algorithm_ComputeTemporalFilterCoefficients_InputParameterId_LowCutFrequency));
	ip_f64HighCutFrequency.initialize(getInputParameter(OVP_Algorithm_ComputeTemporalFilterCoefficients_InputParameterId_HighCutFrequency));
	ip_f64BandPassRipple.initialize(getInputParameter(OVP_Algorithm_ComputeTemporalFilterCoefficients_InputParameterId_BandPassRipple));

	op_pOutputMatrix.initialize(getOutputParameter(OVP_Algorithm_ComputeTemporalFilterCoefficients_OutputParameterId_Matrix));

	return true;
}

boolean CComputeTemporalFilterCoefficients::uninitialize(void)
{
	op_pOutputMatrix.uninitialize();

	ip_f64BandPassRipple.uninitialize();
	ip_f64HighCutFrequency.uninitialize();
	ip_f64LowCutFrequency.uninitialize();
	ip_ui64FilterOrder.uninitialize();
	ip_ui64FilterType.uninitialize();
	ip_ui64FilterMethod.uninitialize();
	ip_ui64SamplingFrequency.uninitialize();

	return true;
}

// ________________________________________________________________________________________________________________
//

boolean CComputeTemporalFilterCoefficients::process(void)
{
	if(isInputTriggerActive(OVP_Algorithm_ComputeTemporalFilterCoefficients_InputTriggerId_Initialize))
	{
		m_ui32SamplingRate = ip_ui64SamplingFrequency;
		m_ui64FilterMethod = ip_ui64FilterMethod;
		m_ui64FilterType = ip_ui64FilterType;
		m_ui32FilterOrder = ip_ui64FilterOrder;
		m_float64LowPassBandEdge = ip_f64LowCutFrequency;
		m_float64HighPassBandEdge =ip_f64HighCutFrequency;
		m_float64PassBandRipple = ip_f64BandPassRipple;

		m_ui32ArraySize = 4*m_ui32FilterOrder; // Maximum size of array involved in computation
	}

	if(isInputTriggerActive(OVP_Algorithm_ComputeTemporalFilterCoefficients_InputTriggerId_ComputeCoefficients))
	{
		if(m_ui64FilterMethod==OVP_TypeId_FilterMethod_Butterworth || m_ui64FilterMethod==OVP_TypeId_FilterMethod_Chebychev)
		{
			if(m_ui64FilterType==OVP_TypeId_FilterType_LowPass || m_ui64FilterType==OVP_TypeId_FilterType_HighPass)
			{
				m_ui32DimensionSize = m_ui32FilterOrder+1;
				m_vecDenomCoefFilter = zeros(m_ui32DimensionSize);
				m_vecNumCoefFilter = zeros(m_ui32DimensionSize);
			}
			else
			{
				m_ui32DimensionSize =2*m_ui32FilterOrder+1;
				m_vecDenomCoefFilter = zeros(m_ui32DimensionSize);
				m_vecNumCoefFilter = zeros(m_ui32DimensionSize);
			}

			if(m_ui64FilterMethod==OVP_TypeId_FilterMethod_Chebychev)
			{
				// For Chebyshev filter, ripples go from 1.0 to 1/sqrt(1+m_float64Eps^2)
				m_float64Phi = exp( 0.5*m_float64PassBandRipple/ (10.0/log(10.0)));

				if((m_ui32FilterOrder & 1) == 0)
				{
					m_float64Scale = m_float64Phi;
				}
				else
				{
					m_float64Scale = 1.0;
				}
			}

			m_ui32NyquistFrequency = m_ui32SamplingRate/2;

			//local variables
			float64 l_float64BandWidth=0.0;
			float64 l_float64HighFrequencyEdge=0.0;
			float64 l_float64Ang;
			float64 l_float64CosAng;

			//locate edges
			if(m_ui64FilterType==OVP_TypeId_FilterType_LowPass)
			{
				m_float64LowPassBandEdge=0.0;
			}
			if(m_ui64FilterType==OVP_TypeId_FilterType_HighPass)
			{
				l_float64BandWidth = m_float64HighPassBandEdge;
				l_float64HighFrequencyEdge = (float64)m_ui32NyquistFrequency;
			}
			else
			{
				l_float64BandWidth = m_float64HighPassBandEdge - m_float64LowPassBandEdge;
				l_float64HighFrequencyEdge = m_float64HighPassBandEdge;
			}

			//convert to Frequency correspondence for bilinear transformation
			// Wanalog = tan( 2 pi Fdigital T / 2 )
			// where T = 1/fs
			l_float64Ang = (float64)l_float64BandWidth * pi / (float64)m_ui32SamplingRate;
			l_float64CosAng = cos(l_float64Ang);
			m_float64TanAng = sin(l_float64Ang) / l_float64CosAng; // Wanalog

			// Transformation from low-pass to band-pass critical frequencies
			// Center frequency
			//                      cos( 1/2 (Whigh+Wlow) T )
			//  cos( Wcenter T ) =  -------------------------
			//                      cos( 1/2 (Whigh-Wlow) T )
			//
			// Band edges
			//            cos( Wcenter T) - cos( Wdigital T )
			//  Wanalog = -----------------------------------
			//                     sin( Wdigital T )

			l_float64HighFrequencyEdge = pi * (l_float64HighFrequencyEdge+m_float64LowPassBandEdge) / (float64)m_ui32SamplingRate ;
			m_float64CosGam = cos(l_float64HighFrequencyEdge) / l_float64CosAng;
			l_float64HighFrequencyEdge = 2.0 * pi * m_float64HighPassBandEdge / (float64)m_ui32SamplingRate;
			m_float64cbp = (m_float64CosGam - cos(l_float64HighFrequencyEdge))/sin(l_float64HighFrequencyEdge);

			if(m_ui64FilterMethod==OVP_TypeId_FilterMethod_Butterworth)
			{
				m_float64Scale = 1.0;
			}

			findSPlanePolesAndZeros();
			convertSPlanePolesAndZerosToZPlane();
		}
		else if(m_ui64FilterMethod==OVP_TypeId_FilterMethod_YuleWalker)
		{
			// Compute RII Band Pass Filter coefs
			m_ui32DimensionSize = m_ui32FilterOrder+1;
			m_vecDenomCoefFilter = zeros(m_ui32FilterOrder+1);
			m_vecNumCoefFilter = zeros(m_ui32FilterOrder+1);
			vec l_vecFrecuency;
			vec l_vecAmplitude;
			vec l_vecResultAutocorr;

			if(m_ui64FilterType==OVP_TypeId_FilterType_LowPass)
			{
				//frequency table
				l_vecFrecuency = zeros(4);
				l_vecFrecuency[0] = 0;
				l_vecFrecuency[1] = (2*(double)m_float64HighPassBandEdge)/(double)m_ui32SamplingRate;
				l_vecFrecuency[2] = (2*((double)m_float64HighPassBandEdge+1))/(double)m_ui32SamplingRate;
				l_vecFrecuency[3] = 1;
				//amplitude table
				l_vecAmplitude= zeros(4);
				l_vecAmplitude[0] = 1;
				l_vecAmplitude[1] = 1;
			}
			else if(m_ui64FilterType==OVP_TypeId_FilterType_HighPass)
			{
				//frequency table
				l_vecFrecuency = zeros(4);
				l_vecFrecuency[0] = 0;
				l_vecFrecuency[1] = (2*(double)m_float64LowPassBandEdge)/(double)(m_ui32SamplingRate);
				l_vecFrecuency[2] = (2*((double)m_float64LowPassBandEdge+1))/(double)(m_ui32SamplingRate);
				l_vecFrecuency[3] = 1;
				//amplitude table
				l_vecAmplitude= zeros(4);
				l_vecAmplitude[2] = 1;
				l_vecAmplitude[3] = 1;
			}
			else if(m_ui64FilterType==OVP_TypeId_FilterType_BandPass)
			{
				//frequency table
				l_vecFrecuency = zeros(6);
				l_vecFrecuency[0] = 0;
				l_vecFrecuency[1] = (2*((double)m_float64LowPassBandEdge-1))/(double)m_ui32SamplingRate;
				l_vecFrecuency[2] = (2*(double)m_float64LowPassBandEdge)/(double)m_ui32SamplingRate;
				l_vecFrecuency[3] = (2*(double)m_float64HighPassBandEdge)/(double)m_ui32SamplingRate;
				l_vecFrecuency[4] = (2*((double)m_float64HighPassBandEdge+1))/(double)m_ui32SamplingRate;
				l_vecFrecuency[5] = 1;
				//amplitude table
				l_vecAmplitude= zeros(6);
				l_vecAmplitude[2] = 1;
				l_vecAmplitude[3] = 1;
			}
			else if(m_ui64FilterType==OVP_TypeId_FilterType_BandStop)
			{
				//frequency table
				l_vecFrecuency = zeros(6);
				l_vecFrecuency[0] = 0;
				l_vecFrecuency[1] = (2*((double)m_float64LowPassBandEdge-1))/(double)m_ui32SamplingRate;
				l_vecFrecuency[2] = (2*(double)m_float64LowPassBandEdge)/(double)m_ui32SamplingRate;
				l_vecFrecuency[3] = (2*(double)m_float64HighPassBandEdge)/(double)m_ui32SamplingRate;
				l_vecFrecuency[4] = (2*((double)m_float64HighPassBandEdge+1))/(double)m_ui32SamplingRate;
				l_vecFrecuency[5] = 1;
				//amplitude table
				l_vecAmplitude= ones(6);
				l_vecAmplitude[2] = 0;
				l_vecAmplitude[3] = 0;
			}

			filter_design_autocorrelation(4*m_ui32FilterOrder, l_vecFrecuency, l_vecAmplitude, l_vecResultAutocorr);
			arma_estimator(m_ui32FilterOrder, m_ui32FilterOrder, l_vecResultAutocorr, m_vecDenomCoefFilter, m_vecNumCoefFilter);
		}

		IMatrix* l_pOutputMatrix=op_pOutputMatrix;
		l_pOutputMatrix->setDimensionCount(2); // numerator + denominator
		l_pOutputMatrix->setDimensionSize(0,m_ui32DimensionSize);
		l_pOutputMatrix->setDimensionSize(1,m_ui32DimensionSize);

		float64* l_pFilterCoefMatrix=l_pOutputMatrix->getBuffer();
		for (uint32 i =0; i< m_ui32DimensionSize; i++)
		{
			l_pFilterCoefMatrix[i] = m_vecDenomCoefFilter[i];
			l_pFilterCoefMatrix[i+m_ui32DimensionSize] = m_vecNumCoefFilter[i];
		}
	}

	return true;
}
