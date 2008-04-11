#include "LFMpara.h"
#include <cassert>
#include <iostream.h>
using namespace std; 

/**************************************************************************/

/// Default Constructor
LFMpara::LFMpara(void)
{
  fid = -999;
}

LFMpara::LFMpara(const char *file)
{
  fid = -999;
  if (! open(file) ){
    cerr << "*** Error opening " << filename << "\n";
  }
}

/**************************************************************************/

/// Destructor - closes a file that's already been opened.
LFMpara::~LFMpara(void)
{
  if (fid != -999)
    close();
}

/**************************************************************************/

/// Open filename for reading
/**
 *  @param filename - path to hdf file that we'll read.
 *
 *  @return true if success, false otherwise.
 */
bool LFMpara::open(const char *file)
{
  // copy filename
  strcpy(filename, file);

  // Open HDF file and initialize an SD interface.
  fid = SDstart(filename, DFACC_RDONLY);

  if (fid < 0){
    cerr << "*** LFMpara::open failed\n";
    strcpy(filename, "\0");
    return false;
  }
  else{
    return true;    
  }
}

/**************************************************************************/

/// Get ni, nj, and nk dimension information from the hdf file.
/**
 *
 *  @param variable - INPUT string name of variable to get dimension information for
 *  @param dimensions - OUTPUT integer array containing number of elements in each dimension
 *
 *  @return rank - the number of dimensions for variable (and the length of the dimensions array).  Returns 0 if there's a problem reading the dimension information.
 */
int LFMpara::getDimensions(const char *variable, int *dimensions)
{
  int32 variableId;                   // HDF variable ID
  int32 SDselectId;                   // HDF selection ID
  int32 errorId;                      // HDF error ID
  char dataName[MAX_NC_NAME];         // Meta-data name of variable
  int32 rank;                         // should be 1 or 3 (scalar or 3-d vector) for this data.
  int32 dimensions_i32[MAX_VAR_DIMS]; // dimensions of the variable
  int32 dataType;                     // Internal HDF datatype of variable
  int32 nAttributes;                  // number of attributes for the data set

  variableId = SDnametoindex(fid, variable);
  if (-1 == variableId)
  {
    cerr << "*** LFMpara::getDimensions unable to open variable " << variable << "\n";
    return 0;
  }

  // select the variable ID that we're going to read.
  SDselectId = SDselect(fid, variableId);

  // Read data name, rank of the variable, XYZ_dims, datatype and number of stored battributes
  errorId = SDgetinfo(SDselectId, dataName, &rank, dimensions_i32, &dataType, &nAttributes);
  if (errorId < 0){
    cerr << "*** LFMpara::getDimensions unable to read info for variable " << variable << "\n";
    return 0;
  }
  errorId = SDendaccess(SDselectId);
  if (errorId < 0){
    cerr << "*** LFMpara::getDimensions unable to end information access for variable " << variable << "\n";
    return 0;
  }

  // Rank should always be lass than 3
  assert( rank <= 3);

  // We should always have floating-point numbers.
  // data type table is available at:
  // http://idlastro.gsfc.nasa.gov/idl_html_help/HDF_Scientific_Dataset_ID_Numbers.html
  assert(dataType == DFNT_FLOAT32);

  // FIXME: I dont really understand nAttributes...
  //assert(nAttributes == 1);
  //cout << "nAttributes = " << nAttributes << "\n";

  // set dimensions as integers
  for (int i = 0; i < MAX_VAR_DIMS; i++){
    dimensions[i] = dimensions_i32[i];
  }
  
  return rank;
}

/**************************************************************************/

// Read floating-point data from file
/**
 *
 *  @param variable - INPUT string name of variable to get read data
 *  @param data - 1-D floating-point data to read data from file into data in memory.  This memory should be allocated/deallocated BEFORE calling LFMpara::getData
 *
 *  @return true if success, false otherwise.
 */
bool LFMpara::getData(const char *variable, float *data)
{
  int32 errorId;                       // HDF error ID
  int32 SDselectId;                    // HDF selection ID
  int32 varId;                         // HDF variable ID
  int32 dataType;                      // Internal HDF datatype of variable
  int32 nAttributes;                   // number of attributes for the data set
  int32 rank;                          // rank of the variable.
  int32 dimensions_i32[MAX_VAR_DIMS];  // used to compute indexEnd
  int32 indexStart[MAX_VAR_DIMS];      // [x,y,z] starting location from where data is read
  int32 indexEnd[MAX_VAR_DIMS];        // [x,y,z] number of values to be read along each dimension
  char dataName[MAX_NC_NAME];          // 

  int i;
  
  for(i=0;i<MAX_VAR_DIMS;i++) {
    dimensions_i32[i] = 0;
    indexStart[i] = 0;
    indexEnd[i] = 0;
  }
  /*Get the info about thie dataset */
  varId = SDnametoindex(fid,variable);
  if (varId < 0) {
    cerr << "*** LFMpara::getData Unable to open " << variable << "\n";
    return false;
  }
  else {
    SDselectId = SDselect(fid,varId);
    errorId = SDgetinfo(SDselectId,dataName,&rank,dimensions_i32,&dataType,&nAttributes);
    if (errorId < 0){
      cerr << "*** LFMpara::getData unable to read info for variable " << variable << "\n";
      return false;
    }
    /* Create reterival information arrays and allocate space */
    for(i=0;i<rank;i++) {
      indexStart[i]=0;
      indexEnd[i]=dimensions_i32[rank-i-1];
      indexEnd[i]=dimensions_i32[i];
    }
    
    /* Get dataset */
    errorId = SDreaddata(SDselectId,indexStart,NULL,indexEnd,data);
    if (errorId < 0){
      cerr << "*** LFMpara::getData unable to read data for variable " << variable << "\n";
      return false;
    }

    errorId = SDendaccess(SDselectId);
    if (errorId < 0){
      cerr << "*** LFMpara::getData unable to end access for variable " << variable << "\n";
      return false;
    }
  }

  // if we made it this far, success!
  return true;
}

/**************************************************************************/

/// close access to the file (note: this should be called by the destructor)
bool LFMpara::close(void)
{
  if ( SDend(fid) < 0 ){
    cerr << "*** LFMpara::close() failed\n";
    return false;
  }
  else{
    fid = -999;
    strcpy(filename, "\0");
    return true;
  }
}
