#include "Check_IC_Status.h"

void failed_status_check(IC_XJD *xjd, char *filename, int line_number, char *ic_function, int status)
{
  std::cerr << "InterComm status error on line " << line_number << " (" << filename <<  ")" << ":  " << ic_function << " returned status of " << status << std::endl;
  //FIXME: Comment finalize/exit code out for now. 
  //
  //       ... because some InterComm functions
  //       (i.e. IC_EndPointSet::registerArray) return bad error codes
  //       when registering variables/connections not in the XJD file.
  //       I think this is the wrong behavior since the InterComm
  //       documentation states 
  //
  //          "If the XJD does not specify any connections for the
  //          specied region, so that no program will import the
  //          region, the function becomes a no-op."
  //
  //int iStatus;
  //IC_Finalize(xjd, &iStatus);
  //exit(1); 
}


void failed_status_check(char *filename, int line_number, char *ic_function, int status)
{
  std::cerr << "InterComm error on line " << line_number << " (" << filename <<  ")" << ":  " << ic_function << " returned status of " << status << std::endl;

  switch(status){
  case 0:
    std::cerr << "IC_OK                         =   0,/**<no error*/" << std::endl;
    break;
  case -1:
    std::cerr << "IC_GENERIC_ERROR              =  -1,/**<generic error*/" << std::endl;
    break;
  case -2:
    std::cerr << "IC_INVALID_NDIM               =  -2,/**<invalid number of array dimensions*/" << std::endl;
    break;
  case -3:
    std::cerr << "IC_CANT_ALLOC_REGION          =  -3,/**<can't allocate InterComm array regions*/" << std::endl;
    break;
  case -4:
    std::cerr << "IC_CANT_GET_DA_DESCRIPTOR     =  -4,/**<can't obtain distributed array descriptor*/" << std::endl;
    break;
  case -5:
    std::cerr << "IC_CANT_COMPUTE_COMM_SCHEDULE =  -5,/**<can't compute communication schedule*/" << std::endl;
    break;
  case -6:
    std::cerr << "IC_COMM_FAILURE               =  -6,/**<communication (send/recv) failure */" << std::endl;
    break;
  case -7:
    std::cerr << "IC_INVALID_ENDPOINT_NAME      =  -7,/**<invalid endpoint name*/" << std::endl;
    break;
  case -8:
    std::cerr << "IC_INITIALIZATION_FAILURE     =  -8,/**<local program initialization failed*/" << std::endl;
    break;
  case -9:
    std::cerr << "IC_CANT_CONNECT_TO_REMOTE     =  -9,/**<can't connect to remote program*/" << std::endl;
    break;
  case -10:
    std::cerr << "IC_PVM_ERROR                  = -10,/**<PVM error*/" << std::endl;
    break;
  case -11:
    std::cerr << "IC_MPI_ERROR                  = -11,/**<MPI error*/" << std::endl;
    break;
  case -12:
    std::cerr << "IC_POINTER_TABLE_ERROR        = -12,/**<internal pointer translation table error*/" << std::endl;
    break;
  case -13:
    std::cerr << "IC_EPSET_INIT_FAILURE      = -13,/**<fail to initialize EndPointSet*/" << std::endl;
    break;
  case -14:
    std::cerr << "IC_ARRAY_REGISTER_FAILURE  = -14,/**<fail to register an array */" << std::endl;
    break;
  case -15:
    std::cerr << "IC_COMMIT_ARRAY_FAILURE    = -15,/**<fail to commit the registered arrays */" << std::endl;
    break;
  default:
    std::cerr << "Unknown error code" << std::endl;
    break;
  }

  //
  //FIXME: Comment exit() out for now.
  //
  //       ... because some InterComm functions
  //       (i.e. IC_EndPointSet::registerArray) return bad error codes
  //       when registering variables/connections not in the XJD file.
  //       I think this is the wrong behavior since the InterComm
  //       documentation states 
  //
  //          "If the XJD does not specify any connections for the
  //          specied region, so that no program will import the
  //          region, the function becomes a no-op."
  //
  //exit(1);
}
