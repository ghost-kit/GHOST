#ifndef __CHECK_IC_STATUS__
#define __CHECK_IC_STATUS__

#include <mpi.h>
#include <Ppp_InterComm.h>
#include <iostream>

#define CHECK_IC_EPS_STATUS(FUNCTION, STATUS)		        \
{                                                               \
  if (STATUS != IC_EndPoint::IC_OK)				\
    failed_status_check(__FILE__, __LINE__, FUNCTION, STATUS);  \
}

#define CHECK_IC_STATUS(xjd, FUNCTION, STATUS)			    \
{                                                                   \
  if (STATUS < 0)                                                   \
    failed_status_check(xjd, __FILE__, __LINE__, FUNCTION, STATUS); \
}
#endif

void failed_status_check(IC_XJD *xjd, char *filename, int line_number, char *ic_function, int status);
void failed_status_check(char *filename, int line_number, char *ic_function, int status);
