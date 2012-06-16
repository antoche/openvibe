/*************************************************************************************/
/* spline_sph.h                                                                      */
/*************************************************************************************/

/*************************************************************************************/
/* spline  subroutines                                                               */
/*************************************************************************************/

int spline_tables(int order, double* pot_table, double* scd_table);
int spline_coef(int nb_value, double** xyz, double* values, double* table, double* coef);
double spline_interp(int nb_value, double** xyz, double* table, double* coef, double xx, double yy, double zz);
