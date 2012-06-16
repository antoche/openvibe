/*****************************************************************************/
/*****************************************************************************/
/**                                                                         **/
/** spline_sph :                                                            **/
/**                                                                         **/
/**      spline_tables                                                      **/
/**      spline_coef                                                        **/
/**      spline_interp                                                      **/
/**                                                                         **/
/** note : spline_coef calls linpack routines (linpack.c)                   **/
/**                                                                         **/
/*****************************************************************************/
/*****************************************************************************/

/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/
/*& Libraries to include                                                    &*/
/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/

#include <cmath>
#include <cstdio>
#include <cstdlib>

#include "linpack.h"

/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/
/*& Define                                                                  &*/
/*&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/

#define imin(x,y) ((x < y) ? x : y)

/**********************************************************************************/
/*                               spline_tables                                    */
/*                                                                                */
/* Computes the tabulated functions km(cos(gamma)) and hm(cos(gamma))             */
/* for cos(gamma) varying from -1 to 1 (see Patent, columns 2 and 6)              */
/* km and hm consist in series of Legendre polynomials                            */
/* km and hm are used for potential and SCD interpolation respectively            */
/* m is the order of the spline interpolation function                            */
/*                                                                                */
/* IN                                                                             */
/* order      : spline order m (integer)                                          */
/*                                                                                */
/* OUT                                                                            */
/* pot_table  : vector of tabulated km (array of 2004 double)                     */
/* scd_table  : vector of tabulated hm (array of 2004 double)                     */
/* function value  : return code (integer)                                        */
/*                   =  0 normal value                                            */
/*                   = -1 error                                                   */
/*                                                                                */
/* Note : this function should be called once at the beginning of the program     */
/*                                                                                */
/**********************************************************************************/

int spline_tables(int order, double* pot_table, double* scd_table)
{
double *c, *p;
double p0, p1, pn, usfn, gamma;
double fs, fsv, fsc;
double dexp, s1, s2, cn, cx, cnpn, fn;
int    j, kc, kv, n, ig;

if (order <= 2)
    {
    printf("spline_table error : spline order <= 2\n");
    return (-1);
    }
else
    {
    /*===========================================================*/
    /* Estimate the number of terms for the Legendre series      */
    /* to have an error lower than 1e-10                         */
    /*===========================================================*/
    dexp = 10./(float)(2*order-2);
    kv = imin(400,(int)(pow(10.0,dexp)-1.0));
    fsv = 1.0;
    if ((int)fmod((double)kv,2.0) == 1) fsv = -1.0;

    dexp = 10./(float)(2*order-4);
    kc = imin(400,(int)(pow(10.0,dexp)-1.0));
    fsc = 1.0;
    if ((int)fmod((double)kc,2.0) == 1) fsc = -1.0;

    c = (double *)malloc(sizeof(double)*kc);
    p = (double *)malloc(sizeof(double)*kc);

    /*=========================*/
    /* Coefficient computation */
    /*=========================*/
    cn = 1.0;
    for (j=1; j<order; j++) { cn /= 2.0; }
    c[0] = cn*3.0;
    for (n=2; n<=kc; n++)
        {
        fn = (double)n;
        cx = (fn-1.0)/(fn+1.0);
        for (j=1; j<order; j++) { cn *= cx; }
        c[n-1] = (2.0*fn + 1.0)*cn;
        }

    /*========================*/
    /* Table generation       */
    /*========================*/
    for (ig=0; ig<=1000; ig++)
        {
        /*-------------------------*/
        /* Pn polynomial           */
        /*-------------------------*/
        gamma = (double)ig/1000.0;
        gamma = 1.0 - gamma;
        p0 = 1.0;
        p1 = gamma;
        p[0] = p1;
        for (n=2; n<=kc; n++)
            {
            fn = (double)n;
            usfn = 1.0/fn;
            pn = (2.0-usfn)*gamma*p1 - (1.0-usfn)*p0;
            p0 = p1;
            p1 = pn;
            p[n-1] = pn;
            }

        /*-----------------------*/
        /* pot_table computation */
        /*-----------------------*/
        s1 = 0.0; s2 = 0.0;
        fs = fsv;
        for (n=kv; n>=1; n--)
            {
            fn = (double)n;
            cnpn = c[n-1]*p[n-1]/(fn*(fn+1.0));
            s1 += cnpn;
            s2 += fs*cnpn;
            fs = -fs;
            }
        *(pot_table+2001-ig) = s1*1000.0;
        *(pot_table+1+ig)    = s2*1000.0;

        /*-----------------------*/
        /* scd_table computation */
        /*-----------------------*/
        s1 = 0.0; s2 = 0.0;
        fs = fsc;
        for (n=kc; n>=1; n--)
            {
            cnpn = c[n-1]*p[n-1];
            s1 += cnpn;
            s2 += fs*cnpn;
            fs = -fs;
            }
        *(scd_table+2001-ig) = s1*1000.0;
        *(scd_table+1+ig)    = s2*1000.0;
        }

    *(pot_table+2002) = *(pot_table+2001);
    *(scd_table+2002) = *(scd_table+2001);
    *(pot_table+2003) = *(pot_table+2002);
    *(scd_table+2003) = *(scd_table+2002);
    *pot_table = *(pot_table+1);
    *scd_table = *(scd_table+1);

    free(c);
    free(p);
    }
return (0);
}

/**********************************************************************************/
/*                               spline_coef                                      */
/*                                                                                */
/* Computes the interpolation coefficients P=(p1,p2,...,pn) and q                 */
/* (see Patent, columns 2 and 6)                                                  */
/*                                                                                */
/* IN                                                                             */
/* nb_value   : number of electrodes and related potential values (integer)       */
/* xyz        : array[nb_value] of array[3] of double                             */
/*              X, Y, Z electrode coordinates on a spherical surface              */
/* values     : array[nb_value] of double                                         */
/*              potential values at the electrode locations                       */
/* table      : array[2004] of double                                             */
/*              tabulated function km (array pot_table computed by spline_tables  */
/*                                                                                */
/* OUT                                                                            */
/* coef       : array[nb_value + 1] of double                                     */
/*              spline coefficients P=(p1,p2,...,pn) and q                        */
/* function value  : return code (integer)                                        */
/*                   =  0 normal value                                            */
/*                   = -1 error                                                   */
/*                                                                                */
/* Note : this function should be called once for a given set of electrodes       */
/*        and a set of potential values                                           */
/*                                                                                */
/**********************************************************************************/

int spline_coef(int nb_value, double** xyz, double* values, double* table, double* coef)
{
int    i, j, l0, ih, igam, info, itmp;
double t1, t2, t3, tp, v1, v2, fgam;
double xj, yj, zj;

double *p_mat_a = NULL;
int    *p_iwork = NULL;

/*=========================================*/
/* allocation of temporary arrays          */
/*=========================================*/
p_mat_a = (double *)malloc(sizeof(double)*((nb_value+1)*(nb_value+2))/2);
if (p_mat_a == NULL)
    {
    printf("spline_coef error : allocation p_mat_a\n");
    return (-1);
    }
p_iwork = (int *)malloc(sizeof(int)*(nb_value+1));
if (p_iwork == NULL)
    {
    printf("spline_coef error : allocation p_iwork\n");
    return (-1);
    }

/*================================*/
/* Initialization of matrix A     */
/*================================*/
l0 = ((nb_value+1)*(nb_value))/2;
for (i=l0; i<l0+nb_value; i++)
    {
    *(p_mat_a+i) = 1.0;
    }
*(p_mat_a+i) = 0.0;

/*=========================*/
/* computation of matrix A */
/*=========================*/
ih = 0;
for (j=0; j<nb_value; j++)
    {
    xj = xyz[j][0];
    yj = xyz[j][1];
    zj = xyz[j][2];
    for (i=0; i<j; i++)
        {
        t1 = xyz[i][0] - xj;
	t2 = xyz[i][1] - yj;
	t3 = xyz[i][2] - zj;
	tp = (t1*t1 + t2*t2 + t3*t3)/2.0;
	fgam = (1.0 -tp)*1000.0 + 1002.0;
	igam = (int)fgam;
	fgam -= (float)igam;
	v1 = *(table+igam-1);
	v2 = *(table+igam) - v1;
	*(p_mat_a + ih++) = v2*fgam + v1;
        }
    *(p_mat_a + ih++) = *(table + 2001);
    }

/*=================================*/
/* Triangularization of matrix A   */
/*=================================*/
itmp = nb_value + 1;
sspfa(p_mat_a, &itmp, p_iwork, &info);
if (info != 0)
    {
    printf("spline_coef error : triangularization of matrix a (sspfa : %d) \n",info);
    return (-1);
    }

/*=======================================================*/
/* Coefficient computation (solving a triangular system) */
/*=======================================================*/
for (i=0; i<nb_value; i++)
    {
    coef[i] = values[i];
    }
coef[nb_value] = 0.0;
sspsl(p_mat_a, &itmp, p_iwork, coef);
free(p_mat_a);
free(p_iwork);
return (0);
}

/**********************************************************************************/
/*                               spline_interp                                    */
/*                                                                                */
/* Computes the interpolated potential or SCD value at a location on the sphere   */
/* (see Patent, columns 2, 4, 7 and 9)                                            */
/*                                                                                */
/* IN                                                                             */
/* nb_value   : number of electrodes and related potential values (integer)       */
/* xyz        : array[nb_value] of array[3] of double                             */
/*              X, Y, Z electrode coordinates on a spherical surface (radius = 1) */
/* table      : array[2004] of double                                             */
/*              tabulated function computed by spline_tables                      */
/*              use pot_table (km) for potential interpolation                    */
/*              use scd_table (hm) for SCD interpolation                          */
/* coef       : array[nb_value + 1] of double                                     */
/*              spline coefficients P=(p1,p2,...,pn) and q                        */
/*              array coef computed by spline_coef                                */
/*              IMPORTANT : coef[nb_value] should be set to 0.0 for computing     */
/*                          the interpolated SCD (this corresponds to q=0)        */
/* xx, yy, zz : double                                                            */
/*              X, Y, Z coordinates of a point on a spherical surface (radius = 1)*/
/*              where to compute the interpolated value                           */
/*                                                                                */
/* OUT                                                                            */
/* function value  : interpolated potential or SCD value (double)                 */
/*                                                                                */
/* Note : this function should be called for every point of the spherical surface */
/*        where the potential or the SCD value should be estimated                */
/*                                                                                */
/**********************************************************************************/

double spline_interp(int nb_value, double** xyz, double* table, double* coef, double xx, double yy, double zz)
{
double t1, t2, t3;
double t123, fgam, v1, v2;
double ffn;
int    i, k, igam;

ffn = coef[nb_value];
k = 0;
for (i=0;i<nb_value;i++)
   {
   t1 = xx - xyz[i][0];
   t2 = yy - xyz[i][1];
   t3 = zz - xyz[i][2];
   t123 = (t1*t1+t2*t2+t3*t3)/2.;
   fgam = (1.0 - t123)*1000. + 1002.;
   igam = (int)fgam;
   fgam -= (double)igam;
   v1 = table[igam-1];
   v2 = table[igam] - v1;
   ffn += coef[k] * (v2*fgam+v1);
   k++;
   }
return (ffn);
}
