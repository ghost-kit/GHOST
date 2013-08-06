/*********************************************************************
 *   Copyright 1993, UCAR/Unidata
 *   See netcdf/COPYRIGHT file for copying and redistribution conditions.
 *   $Id: close.c 5339 2010-01-28 23:39:48Z epourmal $
 *********************************************************************/

#include <stdio.h>
#include "ncgen.h"
#ifdef EIP
#include "genlib.h"
#endif

extern void fline(), cline();
extern int netcdf_flag;
extern int c_flag;
extern int fortran_flag;

static void
cl_netcdf()
{
    if (ncclose(ncid) == -1)
      derror ("error closing netcdf");
}

#define fpr    (void) fprintf

static void
cl_c()
{
    cline("   ncclose (ncid);");
#ifndef vms
    cline("   return 0;");
#else
    cline("   return 1;");
#endif
    cline("}");
}


static void
cl_fortran()
{
    fline("call ncclos (ncid, iret)");
    fline("end");
}



void
close_netcdf()
{
    if (netcdf_flag)
      cl_netcdf();		/* close netcdf */
    if (c_flag)			/* create C code to close netcdf */
      cl_c();
    if (fortran_flag)		/* create Fortran code to close netcdf */
      cl_fortran();
}

