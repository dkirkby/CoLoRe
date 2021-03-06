///////////////////////////////////////////////////////////////////////
//                                                                   //
//   Copyright 2012 David Alonso                                     //
//                                                                   //
//                                                                   //
// This file is part of CoLoRe.                                      //
//                                                                   //
// CoLoRe is free software: you can redistribute it and/or modify it //
// under the terms of the GNU General Public License as published by //
// the Free Software Foundation, either version 3 of the License, or //
// (at your option) any later version.                               //
//                                                                   //
// CoLoRe is distributed in the hope that it will be useful, but     //
// WITHOUT ANY WARRANTY; without even the implied warranty of        //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU //
// General Public License for more details.                          //
//                                                                   //
// You should have received a copy of the GNU General Public License //
// along with CoLoRe.  If not, see <http://www.gnu.org/licenses/>.   //
//                                                                   //
///////////////////////////////////////////////////////////////////////
#ifndef _COMMON_
#define _COMMON_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <time.h>
#include <complex.h>
#ifdef _HAVE_OMP
#include <omp.h>
#endif //_HAVE_OMP
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_spline.h>
#include "fftw3.h"
#ifdef _HAVE_MPI
#include <mpi.h>
#include <fftw3-mpi.h>
#endif //_HAVE_MPI

#define DYNAMIC_SIZE 1
#define RTOD 57.2957795
#define DTOR 0.01745329251
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))

#define TWOPIPIINVLOGTEN  0.1166503235296796 //ln(10)/(2*pi^2)
#define TWOPIPIINV  0.05066059182116889 //1/(2*pi^2)
#define DZ 0.001
#define NZ 5001

#ifdef _HAVE_MPI

#ifdef _SPREC
#define FLOUBLE_MPI MPI_FLOAT
#else //_SPREC
#define FLOUBLE_MPI MPI_DOUBLE
#endif //_SPREC

#ifdef _LONGIDS
#define LINT_MPI MPI_INT
#else //_LONGIDS
#define LINT_MPI MPI_LONG
#endif //_LONGIDS

#endif //_HAVE_MPI

#ifdef _LONGIDS
typedef long lint;
#else //_LONGIDS
typedef int lint;
#endif //_LONGIDS

#ifdef _SPREC
typedef float flouble;
typedef float complex fcomplex;
typedef fftwf_complex dftw_complex;
#else //_SPREC
typedef double flouble;
typedef double complex fcomplex;
typedef fftw_complex dftw_complex;
#endif //_SPREC

//Defined in common.c
extern int NodeThis;
extern int NodeLeft;
extern int NodeRight;
extern int NNodes;
extern int IThread0;
extern int MPIThreadsOK;

typedef struct {
  float ra;     //Right ascension
  float dec;    //Declination
  float z0;     //Cosmological redshift
  float dz_rsd; //RSD contribution
} Gal;

typedef struct {
  char fnamePk[256];
  char fnameBz[256];
  char fnameNz[256];
  double OmegaM;
  double OmegaL;
  double OmegaB;
  double hhub;
  double weos;
  double n_scal;
  double sig8;
  //Derived parameters
  double fgrowth_0;
  double hubble_0;
  double z_max;
  double z_min;
  double r_max;
  double r_min;
  double r2_smooth;
  int do_smoothing;

  //Only used in common.c
  int numk;
  double logkmax;
  double logkmin;
  double idlogk;
  double *logkarr;
  double *pkarr;
  double z_arr_z2r[NZ];
  double r_arr_z2r[NZ];
  double z_arr_r2z[NZ];
  double r_arr_r2z[NZ];
  double growth_d_arr[NZ];
  double growth_v_arr[NZ];
  double glob_idr;

  unsigned int seed_rng;

  int n_grid;
  double l_box;
  int nz_here;
  int iz0_here;

  char prefixOut[256];
  int output_format; //0-> ASCII, 1-> FITS, 2-> HDF5
  int output_density;
  double pos_obs[3];

  dftw_complex *grid_dens_f;
  flouble *grid_dens;
  dftw_complex *grid_vpot_f;
  flouble *grid_vpot;
  flouble *slice_left;
  flouble *slice_right;
  flouble *grid_rvel;
  double sigma2_gauss;
  int *nsources;
  
  gsl_spline *spline_bz;
  gsl_spline *spline_nz;
  gsl_interp_accel *intacc_bz;
  gsl_interp_accel *intacc_nz;

  lint nsources_this;
  lint nsources_total;
  Gal *gals;
} ParamCoLoRe;
void mpi_init(int* p_argc,char*** p_argv);
void *my_malloc(size_t size);
void *my_calloc(size_t nmemb,size_t size);
size_t my_fwrite(const void *ptr, size_t size, size_t nmemb,FILE *stream);
void error_open_file(char *fname);
void error_read_line(char *fname,int nlin);
void print_info(char *fmt,...);
void report_error(int level,char *fmt,...);
int linecount(FILE *f);
void timer(int i);
gsl_rng *init_rng(unsigned int seed);
double rng_01(gsl_rng *rng);
int rng_poisson(double lambda,gsl_rng *rng);
void rng_delta_gauss(double *module,double *phase,
		     gsl_rng *rng,double sigma2);
void end_rng(gsl_rng *rng);


//////
// Functions defined in cosmo.c
double pk_linear0(ParamCoLoRe *par,double lgk);
void cosmo_set(ParamCoLoRe *par);
double r_of_z(ParamCoLoRe *par,double z);
double z_of_r(ParamCoLoRe *par,double r);
double dgrowth_of_r(ParamCoLoRe *par,double r);
double vgrowth_of_r(ParamCoLoRe *par,double r);
double ndens_of_z(ParamCoLoRe *par,double z);
double bias_of_z(ParamCoLoRe *par,double z);


//////
// Functions defined in io_gh.c
ParamCoLoRe *read_run_params(char *fname);
void write_catalog(ParamCoLoRe *par);
void write_grid(ParamCoLoRe *par);
void param_colore_free(ParamCoLoRe *par);


//////
// Functions defined in fourier.c
void init_fftw(ParamCoLoRe *par);
void create_d_and_vr_fields(ParamCoLoRe *par);
void end_fftw(void);


//////
// Functions defined in grid_tools.c
void get_sources(ParamCoLoRe *par);


//////
// Functions defined in healpix_extra.c
/*
long he_nalms(int lmax);
long he_indexlm(int l,int m,int lmax);
void he_alm2map(int nside,int lmax,int ntrans,flouble **maps,fcomplex **alms);
void he_map2alm(int nside,int lmax,int ntrans,flouble **maps,fcomplex **alms);
void he_write_healpix_map(float **tmap,int nfields,long nside,char *fname);
flouble *he_read_healpix_map(char *fname,long *nside,int nfield);
int he_ring_num(long nside,double z);
long *he_query_strip(long nside,double theta1,double theta2,
		     long *npix_strip);
void he_udgrade(flouble *map_in,long nside_in,
		flouble *map_out,long nside_out,
		int nest);
double *he_generate_beam_window(int lmax,double fwhm_amin);
void he_alter_alm(int lmax,double fwhm_amin,fcomplex *alms,double *window);
*/

#endif //_COMMON_
