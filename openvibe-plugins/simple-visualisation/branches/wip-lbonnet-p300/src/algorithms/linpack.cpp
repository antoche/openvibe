/*****************************************************************************/
/**                                                                         **/
/** linpack  : matrix computation                                           **/
/**                                                                         **/
/** from Linpack User's guide                                               **/
/** J.J. Dongarra, J.R. Brunch, C.B. Moler, G.W. Stewart                    **/
/** SIAM Philadelphia 1979                                                  **/
/**                                                                         **/
/** Routines are translated to C from original FORTRAN code                 **/
/**                                                                         **/
/*****************************************************************************/

/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/
/*& Libraries to include                                                    &*/
/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/

#include <cmath>
#include <cstdio>
#include <cstdlib>

#define dmax(x,y) ((x > y) ? x : y)

void saxpy(int* n, double* sa, double* sx, int* incx, double* sy, int* incy);
double sdot(int* n, double* sx, int* incx, double* sy, int* incy);
void sswap(int* n, double* sx, int* incx, double* sy, int* incy);
int isamax(int* n, double* sx, int* incx);

/************************************************************************/
/*                               sspfa                                  */
/*                                                                      */
/* Factorization of a symmetric matrix A (n x n)                        */
/* by elimination with symmetric pivoting                               */
/*                                                                      */
/* to solve A * X = B, follow sspfa by sspl                             */
/*                                                                      */
/* IN                                                                   */
/* ap    : 1-D array (double)                                           */
/*         the packed form of the symmetric matrix A                    */
/*         the columns of the upper triangle are stored sequentially    */
/*         in a 1-dimensional array of length n*(n+1)/2                 */
/* n     : order of matrix A (integer)                                  */
/*                                                                      */
/* OUT                                                                  */
/* ap    : 1-D array (double)                                           */
/*         a block diagonal matrix and the multipliers which were used  */
/*         to obtain it stored in packed form.                          */
/*         the factorization can be written A = U * D * trans(U)        */
/*         where U is a product of permutation and unit upper triangular*/
/*         matrices, trans(U) is the transpose of U, and D is block     */
/*         diagonal with 1 by 1 and 2 by 2 blocks.                      */
/* kpvt  : 1-D array (integer)                                          */
/*         vector of pivot indices                                      */
/* info  : return code (integer)                                        */
/*         = 0 normal value                                             */
/*         = k if the k-th pivot block is singular                      */
/*                                                                      */
/************************************************************************/

void sspfa(double* ap, int* n, int* kpvt, int* info)
{
double alpha, ak, bk, denom, t, tulk, tulkm1, absakk;
double akm1, bkm1, colmax, rowmax;
int    ij, ijj, ik, ikm1, im=0, imj, imk, imax, imaxp1, imim;
int    j, jj, jk, jkm1, jmax, jmim;
int    k, kk, km1, km2, km1k, km1km1;
int    kstep, swap;
int    one, itmp;

one    = 1;
alpha = 0.6404;
*info = 0;
k = *n;
ik = (*n * (*n-1))/2;

begin_dspfa:;
if (k == 0) goto end_dspfa;
if (k <= 1)
    {
    *kpvt = 1;
    if (*ap == 0.0) *info = 1;
    goto end_dspfa;
    }
else
    {
    km1 = k-1;
    kk  = ik + k;
    absakk = fabs(*(ap+kk-1));
    itmp = k-1;
    imax = isamax(&itmp, (ap+ik), &one);
    imk = ik + imax;
    colmax = fabs(*(ap+imk-1));
    if (absakk >= (alpha*colmax))
        {
        kstep = 1;
        swap = 0;
        }
    else
        {
        rowmax = 0.0;
        imaxp1 = imax + 1;
        im = (imax*(imax-1))/2;
        imj = im + 2*imax;
        for (j=imaxp1; j<=k; j++)
            {
            rowmax = dmax(rowmax,fabs(*(ap+imj-1)));
            imj += j;
            }
        if (imax != 1)
            {
            itmp = imax-1;
            jmax = isamax(&itmp, (ap+im), &one);
            jmim = jmax + im;
            rowmax = dmax(rowmax,fabs(*(ap+jmim-1)));
            }
        imim = imax + im;
        if (fabs(*(ap+imim-1)) >= (alpha*rowmax))
            {
            kstep = 1;
            swap = 1;
            }
        else
            {
            if (absakk >= (alpha*colmax*(colmax/rowmax)))
                {
                kstep = 1;
                swap = 0;
                }
            else
                {
                kstep = 2;
                swap = (imax != km1);
                }
            }
        }
    if (dmax(absakk,colmax) == 0.0)
        {
        *(kpvt+k-1) = k;
        *info = k;
        }
    else
        {
        if (kstep != 2)
            {
            if (swap)
                {
                sswap(&imax, (ap+im), &one, (ap+ik), &one);
                imj = ik + imax;
                for (jj=imax; jj <=k; jj++)
                    {
                    j = k + imax - jj;
                    jk = ik + j;
                    t = *(ap+jk-1);
                    *(ap+jk-1) = *(ap+imj-1);
                    *(ap+imj-1) = t;
                    imj -= j-1;
                    }
                }
            ij = ik - (k-1);
            for (jj=1; jj<=km1; jj++)
                {
                j = k - jj;
                jk = ik + j;
                tulk = -(*(ap+jk-1))/(*(ap+kk-1));
                t = tulk;
                saxpy(&j, &t, (ap+ik), &one, (ap+ij), &one);
                ijj = ij + j;
                *(ap+jk-1) = tulk;
                ij -= j-1;
                }
            *(kpvt+k-1) = k;
            if (swap) *(kpvt+k-1) = imax;
            }
        else
            {
            km1k = ik + k - 1;
            ikm1 = ik - (k-1);
            if (swap)
                {
                sswap(&imax, (ap+im), &one, (ap+ikm1), &one);
                imj = ikm1 + imax;
                for (jj=imax; jj<=km1; jj++)
                    {
                    j = km1 + imax -jj;
                    jkm1 = ikm1 + j;
                    t = *(ap+jkm1-1);
                    *(ap+jkm1-1) = *(ap+imj-1);
                    *(ap+imj-1) = t;
                    imj -= j-1;
                    }
                t = *(ap+km1k-1);
                *(ap+km1k-1) = *(ap+imk-1);
                *(ap+imk-1) = t;
                }
            km2 = k - 2;
            if (km2 != 0)
                {
                ak = *(ap+kk-1)/(*(ap+km1k-1));
                km1km1 = ikm1 +k -1;
                akm1 = *(ap+km1km1-1)/(*(ap+km1k-1));
                denom = 1.0 - ak*akm1;
                ij = ik - (k-1) - (k-2);
                for (jj=1; jj<=km2; jj++)
                    {
                    j = km1 - jj;
                    jk = ik + j;
                    bk = *(ap+jk-1)/(*(ap+km1k-1));
                    jkm1 = ikm1 + j;
                    bkm1 = *(ap+jkm1-1)/(*(ap+km1k-1));
                    tulk = (akm1*bk - bkm1)/denom;
                    tulkm1 = (ak*bkm1 - bk)/denom;
                    t = tulk;
                    saxpy(&j, &t, (ap+ik), &one, (ap+ij), &one);
                    t = tulkm1;
                    saxpy(&j, &t, (ap+ikm1), &one, (ap+ij), &one);
                    *(ap+jk-1) = tulk;
                    *(ap+jkm1-1) = tulkm1;
                    ijj = ij + j;
                    ij -= j-1;
                    }
                }
            *(kpvt+k-1) = 1-k;
            if (swap) *(kpvt+k-1) = -imax;
            *(kpvt+k-2) = *(kpvt+k-1);
            }
        }
    ik -= k-1;
    if (kstep == 2) ik -= k-2;
    k -= kstep;
    }
goto begin_dspfa;
end_dspfa:;
}

/************************************************************************/
/*                                sspsl                                 */
/*                                                                      */
/* Solving a symmetric system A * X = B  using the factorization        */
/* computed by sspfa (with return code = 0)                             */
/*                                                                      */
/* IN                                                                   */
/* ap    : 1-D array (double)                                           */
/*         the output array from sspfa                                  */
/* n     : order of matrix A (integer)                                  */
/* kpvt  : 1-D array (integer)                                          */
/*         the pivot vector from sspfa                                  */
/* b     : 1-D array (double)                                           */
/*         the right hand side vector                                   */
/*                                                                      */
/* OUT                                                                  */
/* b     : 1-D array (double)                                           */
/*         the solution vector X                                        */
/*                                                                      */
/************************************************************************/

void sspsl(double* ap, int* n, int* kpvt, double* b)
{
double temp, ak, bk, akm1, bkm1, denom;
int    ik, ikm1, ikp1;
int    k, kk, km1k, km1km1, kp;
int    one, oneb, itmp;

one = 1;
oneb = 1;
k = *n;
ik = (*n * (*n-1))/2;

while (k > 0)
    {
    kk = ik + k;
    if (*(kpvt+k-1) >= 0)
        {
        if (k != 1)
            {
            kp = *(kpvt+k-1);
            if (kp != k)
                {
                temp = *(b+k-1);
                *(b+k-1) = *(b+kp-1);
                *(b+kp-1) = temp;
                }
            itmp = k-1;
            saxpy(&itmp, (b+k-1), (ap+ik), &one, b, &oneb);
            }
        *(b+k-1) /= *(ap+kk-1);
        k--;
        ik -= k;
        }
    else
        {
        ikm1 = ik - (k-1);
        if (ik != 2)
            {
            kp = abs(*(kpvt+k-1));
            if (kp != (k-1))
                {
                temp = *(b+k-2);
                *(b+k-2) = *(b+kp-1);
                *(b+kp-1) = temp;
                }
            itmp = k-2;
            saxpy(&itmp, (b+k-1), (ap+ik), &one, b, &oneb);
            saxpy(&itmp, (b+k-2), (ap+ikm1), &one, b, &oneb);
            }
        km1k = ik + k - 1;
        kk = ik + k;
        ak = *(ap+kk-1)/(*(ap+km1k-1));
        km1km1 = ikm1 + k - 1;
        akm1 = *(ap+km1km1-1)/(*(ap+km1k-1));
        bk = *(b+k-1)/(*(ap+km1k-1));
        bkm1 = *(b+k-2)/(*(ap+km1k-1));
        denom = ak*akm1 - 1.0;
        *(b+k-1) = (akm1*bk-bkm1)/denom;
        *(b+k-2) = (ak*bkm1-bk)/denom;
        k -= 2;
        ik -= (2*k + 1);
        }
    }
    {
    k = 1;
    ik = 0;
    while (k <= *n)
        {
        if (*(kpvt+k-1) >= 0)
            {
            if (k != 1)
                {
                itmp = k-1;
                *(b+k-1) += sdot(&itmp, (ap+ik), &one, b, &oneb);
                kp = *(kpvt+k-1);
                if (kp != k)
                    {
                    temp = *(b+k-1);
                    *(b+k-1) = *(b+kp-1);
                    *(b+kp-1) = temp;
                    }
                }
            ik += k;
            k++;
            }
        else
            {
            if (k != 1)
                {
                itmp = k-1;
                *(b+k-1) += sdot(&itmp, (ap+ik), &one, b, &oneb);
                ikp1 = ik + k;
                *(b+k) += sdot(&itmp, (ap+ikp1), &one, b, &oneb);
                kp = abs(*(kpvt+k-1));
                if (kp != k)
                    {
                    temp = *(b+k-1);
                    *(b+k-1) = *(b+kp-1);
                    *(b+kp-1) = temp;
                    }
                }
            ik += (2*k + 1);
            k += 2;
            }
        }
    }
}

/************************************************************************/
/*                             saxpy                                    */
/*                                                                      */
/* Constant times a vector plus a vector                                */
/* Y = A*X + Y                                                          */
/* uses enrolled loops for increments equal to 1                        */
/*                                                                      */
/* IN                                                                   */
/* n     : length of vector X and Y (integer)                           */
/* sa    : real constant A (double)                                     */
/* sx    : vector X (double)                                            */
/* incx  : increment for X (integer)                                    */
/* sy    : vector Y (double)                                            */
/* incy  : increment for Y (integer)                                    */
/*                                                                      */
/* OUT                                                                  */
/* sy    : the result vector A*X + Y (double)                           */
/*                                                                      */
/************************************************************************/

void saxpy(int* n, double* sa, double* sx, int* incx, double* sy, int* incy)
{
int i, m, ok;
int ix, iy;

if (*n > 0)
    {
    if (*sa != 0)
        {
        if ( (*incx != 1)||(*incy != 1))
            {
            ix=1; iy=1;
            if (*incx < 0) ix = (1-*n)*(*incx) + 1;
            if (*incy < 0) iy = (1-*n)*(*incy) + 1;
            for (i=1; i<=*n; i++)
                {
                *(sy+iy-1) += *sa * (*(sx+ix-1));
                ix += *incx;
                iy += *incy;
                }
            }
        else
            {
            ok = 1;
            m = (int)fmod((double)*n,4.0);
            if (m != 0)
                {
                for (i=1; i<=m; i++)
                    {
                    *(sy+i-1) += *sa * (*(sx+i-1));
                    }
                if (*n < 4) ok = 0;
                }
            if (ok == 1)
                {
                for (i=++m; i<=*n; i+=4)
                    {
                    *(sy+i-1) += *sa * (*(sx+i-1));
                    *(sy+i  ) += *sa * (*(sx+i  ));
                    *(sy+i+1) += *sa * (*(sx+i+1));
                    *(sy+i+2) += *sa * (*(sx+i+2));
                    }
                }
            }
        }
    }
}

/************************************************************************/
/*                             sdot                                     */
/*                                                                      */
/* Forms the dot product of 2 vectors                                   */
/*  = X.Y                                                               */
/* uses enrolled loops for increments equal to 1                        */
/*                                                                      */
/* IN                                                                   */
/* n     : length of vector X and Y (integer)                           */
/* sx    : vector X (double)                                            */
/* incx  : increment for X (integer)                                    */
/* sy    : vector Y (double)                                            */
/* incy  : increment for Y (integer)                                    */
/*                                                                      */
/* OUT                                                                  */
/* function value  : the result value X.Y (double)                      */
/*                                                                      */
/************************************************************************/

double sdot(int* n, double* sx, int* incx, double* sy, int* incy)
{
int    i, m, ok;
int    ix, iy;
double stemp = 0.0;

if (*n > 0)
    {
    if ( (*incx != 1)||(*incy != 1))
        {
        ix=1; iy=1;
        if (*incx < 0) ix = (1-*n)*(*incx) + 1;
        if (*incy < 0) iy = (1-*n)*(*incy) + 1;
        for (i=1; i<=*n; i++)
            {
            stemp += *(sx+ix-1) * (*(sy+iy-1));
            ix += *incx;
            iy += *incy;
            }
        }
    else
        {
        ok = 1;
        m = (int)fmod((double)*n,5.0);
        if (m != 0)
            {
            for (i=1; i<=m; i++)
                {
                stemp += *(sx+i-1) * (*(sy+i-1));
                }
            if (*n < 5) ok = 0;
            }
        if (ok == 1)
            {
            for (i=++m; i<=*n; i+=5)
                {
                stemp += *(sx+i-1)*(*(sy+i-1)) + *(sx+i  )*(*(sy+i  ))
                       + *(sx+i+1)*(*(sy+i+1)) + *(sx+i+2)*(*(sy+i+2))
                       + *(sx+i+3)*(*(sy+i+3));
                }
            }
        }
    }
return stemp;
}

/************************************************************************/
/*                             sswap                                    */
/*                                                                      */
/* Interchanges 2 vectors X and Y                                       */
/* uses enrolled loops for increments equal to 1                        */
/*                                                                      */
/* IN                                                                   */
/* n     : length of vector X and Y (integer)                           */
/* sx    : vector X (double)                                            */
/* incx  : increment for X (integer)                                    */
/* sy    : vector Y (double)                                            */
/* incy  : increment for Y (integer)                                    */
/*                                                                      */
/* OUT                                                                  */
/* sx    : vector Y (double)                                            */
/* sy    : vector X (double)                                            */
/*                                                                      */
/************************************************************************/

void sswap(int* n, double* sx, int* incx, double* sy, int* incy)
{
double stemp;
int    i, m, ok;
int    ix, iy;

if (*n > 0)
    {
    if ((*incx != 1)||(*incy != 1))
        {
        ix = 1; iy = 1;
        if (*incx < 0) ix = (1-*n)*(*incx) + 1;
        if (*incy < 0) iy = (1-*n)*(*incy) + 1;
        for (i=1; i<=*n; i++)
            {
            stemp = *(sx+ix-1);
            *(sx+ix-1) = *(sy+iy-1);
            *(sy+iy-1) = stemp;
            ix += *incx;
            iy += *incy;
            }
        }
    else
        {
        ok = 1;
        m = (int)fmod((double)*n,3.0);
        if (m != 0)
            {
            for (i=1; i<=m; i++)
                {
                stemp = *(sx+i-1);
                *(sx+i-1) = *(sy+i-1);
                *(sy+i-1) = stemp;
                }
            if (*n < 3) ok = 0;
            }
        if (ok == 1)
            {
            for (i=++m; i<=*n; i+=3)
                {
                stemp = *(sx+i-1);
                *(sx+i-1) = *(sy+i-1);
                *(sy+i-1) = stemp;
                stemp = *(sx+i);
                *(sx+i) = *(sy+i);
                *(sy+i) = stemp;
                stemp = *(sx+i+1);
                *(sx+i+1) = *(sy+i+1);
                *(sy+i+1) = stemp;
                }
            }
        }
    }
}

/************************************************************************/
/*                            isamax                                    */
/*                                                                      */
/* Finds the index of element having max absolute value in vector X     */
/*                                                                      */
/* IN                                                                   */
/* n     : length of vector X (integer)                                 */
/* sx    : vector X (double)                                            */
/* incx  : increment for X (integer)                                    */
/*                                                                      */
/* OUT                                                                  */
/* function value  : index of max absolute value of X (integer)         */
/*                                                                      */
/************************************************************************/

int isamax(int* n, double* sx, int* incx)
{
double smax;
int    i, ix, ida;

ida = 0;
if (*n < 1) return ida;
ida = 1;
if (*n == 1) return ida;
if (*incx != 1)
    {
    ix = 1;
    smax = fabs(*sx);
    ix += *incx;
    for (i=2; i<=*n; i++)
        {
        if (fabs(*(sx+ix-1)) > smax)
            {
            ida = i;
            smax = fabs(*(sx+ix-1));
            }
        ix += *incx;
        }
    }
else
    {
    smax = fabs(*sx);
    for (i=2; i<=*n; i++)
        {
        if (fabs(*(sx+i-1)) > smax)
            {
            ida = i;
            smax = fabs(*(sx+i-1));
            }
        }
    }
return ida;
}
