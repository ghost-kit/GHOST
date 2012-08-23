/* This header defines the C interface to fortran Geopack-2008 routines. */

#ifdef LC_UNSC
#define  RECALC recalc_
#define SMGSW_08 smgsw_08_
#define GEOGSW_08 geogsw_08_
#elif LC_NOUNSC
#define RECALC recalc
#define SMGSW_08 smgsw_08
#define GEOGSW_08 geogsw_08
#else
#error "Must define one of (LC_UNSC,LC_NOUNSC) for proper FORTRAN name mangling"
#endif

/**\name GEOPACK functions*/
extern "C" {
  void RECALC(int *, int *, int *, int *, int *);
  void SMGSW_08 (real *, real *, real *, real *, real *, real *, int *);
  void GEOGSW_08 (real *, real *, real *, real *, real *, real *, int *);
} 
