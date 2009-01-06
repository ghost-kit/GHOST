#ifndef __HDF_DATA_H__
#define __HDF_DATA_H__

//std
#include <cassert>
//3rd-party
#include <mfhdf.h>
// custom
#include "Data.h"

/// Class to read HDF data files for RMS error application.
/**
 *  Manages reading dimension and data information for HDF4 files.
 *
 *  Accepts the following LFM-para magnetosphere variables:
 *   
 *    - X_grid
 *    - Y_grid
 *    - Z_grid
 *    - rho_
 *    - vx_
 *    - vy_
 *    - vz_
 *    - c_
 *    - bx_
 *    - by_
 *    - bz_
 *    - bi_
 *    - bj_
 *    - bk_
 *    - ei_
 *    - ej_
 *    - ek_
 *
 *  Accepts the following LFM-para ionosphere variables:
 *    - x2ion
 *    - y2ion
 *    - x_interp
 *    - y_interp
 *    - rho0
 *    - c0
 *    - curnorth
 *    - potnorth
 *    - SigmaH_north
 *    - SigmaP_north
 *    - fluxnorth
 *    - avE_north
 *    - cursouth
 *    - potsouth
 *    - SigmaH_south
 *    - SigmaP_south
 *    - fluxsouth
 *    - avE_south
 * 
 *  Accepts the following MIX ionospher variables:
 *    - Grid X
 *    - Grid Y
 *    - Potential North [V]
 *    - Potential South [V]
 *    - FAC North [A/m^2]
 *    - FAC South [A/m^2]
 *    - Pedersen conductance North [S]
 *    - Pedersen conductance South [S]
 *    - Average energy North [keV]
 *    - Average energy South [keV]
 *    - Number flux North [1/cm^2 s]
 *    - Number flux South [1/cm^2 s]
 *    - Neutral wind speed South [m/s]
 *    - Neutral wind speed North [m/s]
 *
 *  @seealso Data, ASCII_data & NETCDF_data
 */
template<typename dtype>
class HDF_data : public Data
{
private:
  int32 fid;

public:
  /**************************************************************************/

  /// Default constructor
  HDF_data(void){
    fid = -999;
  }

  /**************************************************************************/

  HDF_data(const char *file)  {
    fid = -999;
    if (! open(file) ){
      std::cerr << "*** Error opening " << Data::filename << std::endl;
    }
  }
  
  /**************************************************************************/
  
  /// Destructor - closes a file that's already been opened.
  ~HDF_data(void){
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
  bool open(const char *file)  {
    // copy filename
    strcpy(Data::filename, file);
    
    // Open HDF file and initialize an SD interface.
    fid = SDstart(Data::filename, DFACC_RDONLY);
    
    if (fid < 0){
      std::cerr << "*** HDF_data::open failed" << std::endl;
      strcpy(Data::filename, "\0");
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
   *  @return rank - the number of dimensions for variable (and the
   *                 length of the dimensions array).  Returns 0 if
   *                 there's a problem reading the dimension information.
   */
  int  getDimensions(const char *variable, int *dimensions)
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
    if (-1 == variableId){
      std::cerr << "*** HDF_data::getDimensions unable to open variable " << variable << std::endl;
      return 0;
    }
    
    // select the variable ID that we're going to read.
    SDselectId = SDselect(fid, variableId);
    
    // Read data name, rank of the variable, XYZ_dims, datatype and number of stored battributes
    errorId = SDgetinfo(SDselectId, dataName, &rank, dimensions_i32, &dataType, &nAttributes);
    if (errorId < 0){
      std::cerr << "*** HDF_data::getDimensions unable to read info for variable " << variable << std::endl;
      return 0;
    }
    errorId = SDendaccess(SDselectId);
    if (errorId < 0){
      std::cerr << "*** HDF_data::getDimensions unable to end information access for variable " << variable << std::endl;
      return 0;
    }
    
    // Rank should always be lass than 3
    assert( rank <= MAX_VAR_DIMS );
    
    // We should probably have either 4-byte single or 8-byte double-precision numbers.
    // data type table is available at:
    // http://idlastro.gsfc.nasa.gov/idl_html_help/HDF_Scientific_Dataset_ID_Numbers.html
    assert( (dataType == DFNT_FLOAT32) || (dataType == DFNT_DOUBLE) );
    
    // FIXME: I dont really understand nAttributes...  should probably put some check on it.
    //assert(nAttributes == 1);
    //cout << "nAttributes = " << nAttributes << std::endl;
    
    // set dimensions as integers
    for (int i = 0; i < MAX_VAR_DIMS; i++){
      dimensions[i] = dimensions_i32[i];
    }
    
    return rank;
  }

  /**************************************************************************/
  
  // Read <dtype> data from file
  /**
   *
   *  @param variable - INPUT string name of variable to get read data

   *  @param data - 1-D <dtype> data to read data from file into data
   *      in memory.  This memory should be allocated/deallocated
   *      BEFORE calling HDF_data::getData
   *
   *  @note HDF SDreaddata reads multi-dimensional data into a 1-d array like so:
   *       x-index i loops fastest
   *       y-index j loops next
   *       z-index k loops next
   *       time index t loops slowest
   *  So when accessing the data, you should index like so:
   *       (j*ni)+k*(ni*nj)+t*(ni*nj*nk)
   *
   *  @return true if success, false otherwise.
   */
  bool getData(const char *variable, void *data)
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
      std::cerr << "*** HDF_data::getData Unable to open " << variable << std::endl;
      return false;
    }
    else {
      SDselectId = SDselect(fid,varId);
      errorId = SDgetinfo(SDselectId,dataName,&rank,dimensions_i32,&dataType,&nAttributes);
      if (errorId < 0){
	std::cerr << "*** HDF_data::getData unable to read info for variable " << variable << std::endl;
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
	std::cerr << "*** HDF_data::getData unable to read data for variable " << variable << std::endl;
	return false;
      }
      
      errorId = SDendaccess(SDselectId);
      if (errorId < 0){
	std::cerr << "*** HDF_data::getData unable to end access for variable " << variable << std::endl;
	return false;
      }
    }
    
    // if we made it this far, success!
    return true;
  }

  /**************************************************************************/

  /// close access to the file (note: this should be called by the destructor)
  bool close(void){
    if ( SDend(fid) < 0 ){
      std::cerr << "*** HDF_data::close() failed" << std::endl;
      return false;
    }
    else{
      fid = -999;
      strcpy(Data::filename, "\0");
      return true;
    }
  }
};

#endif
