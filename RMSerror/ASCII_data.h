#ifndef __ASCII_DATA_H__
#define __ASCII_DATA_H__

//std
#include <fstream>
#include <string> 
#include <sstream>
// custom
#include "Data.h"


/// Manages reading data for whitespace-separated ASCII files with an arbitrary number of columns
/**
 *  This class can handle reading ASCII files with an arbitrary number
 *  of columns.  To specify a particular column, set the
 *  character-array "variable" to a string containing the column
 *  number that you wish to read in.  You must do this as a string, as
 *  an artifact of the class structure that allows for a Data object
 *  to be created for reading LFM HDF data.
 *
 *  All columns are treated as one-dimensional data.
 *
 *  This class exists because some of the MIX tests create white-space delimited ASCII data files
 * 
 *  @seealso Data, HDF_data & NETCDF_data
 */
template <typename dtype>
class ASCII_data : public Data
{
  // Note:  Templates are determined at run-time, 
  // so ASCII_data implementation needs to go in header file.
private:
  // input filestream to handle data.
  std::ifstream ins;
public:

  /**************************************************************************/

  ASCII_data(void) {};
  
  /**************************************************************************/

  /// Open ASCII file for reading.
  /**
   * @param file - path to the ASCII file to open.
   */
  ASCII_data(const char *file){
    open(file);
  } 

  /**************************************************************************/

  /// Destructor - closes a file that's already been opened.
  ~ASCII_data(void){
    if (ins.is_open())
      close();
  }  

  /**************************************************************************/

  /// Open ASCII file for reading
  /**
   * @param file - path to the ASCII file to open.
   */
  bool open(const char *file){
    // copy filename
    strcpy(Data::filename, file);
    
    // Open input file stream
    ins.open(file);
    
    if (ins.fail()){
      std::cerr << "*** Fail:  Unable to open \"" << file << "\"" << std::endl;
      return false;
    }
    
    return true;
  }
  
  /**************************************************************************/

  /// Get the dimensions of the dataset
  /**
   *  @param variable - Index of variable to read in.
   *  @param dimensions - an integer array containing the number of elements in each dimension.
   *
   *  @returns rank - the number of dimensions
   */
  int  getDimensions(const char *variable, int *dimensions){
    int variableIndex = atoi(variable);
    char *token;
    int nDims;
    char data_str[1024];
    
    int MAX_VARIABLE_DIMS = 32;
    
    for (int i = 0; i < MAX_VARIABLE_DIMS; i++){
      dimensions[i] = 0;
    }
    
    while ( !ins.eof() ){
      ins.getline(data_str, 1024);
      
      nDims = 0;
      
      // check whitespace-separated data.
      token = strtok(data_str, " \t ");
      while (token != NULL){
	token = strtok(NULL, " \t ");
	// Update each variable
	dimensions[nDims++] += 1;
      }
    }
    
    // Get rank
    for (int i = 0; i < MAX_VARIABLE_DIMS; i++){
      if (dimensions[i] != 0)
	nDims++;
    }
    
    // close and re-open to reset the file pointer...  We should be able
    // to doo this with ins.seekg(...) but I couldn't get it working :(
    close();
    open(Data::filename);
    
    if (variableIndex >= nDims){
      std::cerr << "*** ERROR:  Requested variable index \"" << variableIndex
		<< "\" is out of bounds of the rank of the dataset \"" << nDims 
		<< "\"." << std::endl;
      return 0;
    }
    // FIXME: This only handles 1-dimensional ASCII data
    //return nDims;
    return 1;
  }
  
  /**************************************************************************/

  /// Read data into memory
  /**
   *  @param variable - Index of variable to read in.
   *  @param data - floating-point array of data to read in 
   *
   *  @note data should be allocated by the user before calling this function.
   */
  bool getData(const char *variable, void *data){
    int variableIndex = atoi(variable);
    char data_str[1024];
    int i = 0;
    char *token;

    std::stringstream ss;
    dtype *pdata = (dtype *)data;
    //dtype *pdata = static_cast<dtype*>(data);

    while ( !ins.eof() ){
      ins.getline(data_str, 1024);
      
      // make sure we have a valid string:
      if (strlen(data_str) > 0){
	// Tokenize the string
	token = strtok(data_str, " \t ");
	// find the correct token
	for ( int j=0; j < variableIndex; j++ ){
	  if (token == NULL){
	    std::cerr << "*** ERROR reading data for variable \"" << variable << "\"" << std::endl;
	    return false;
	  }
	  token = strtok(NULL, " \t ");
	}
	// Get the data from the token using stringstream to convert type (string to dtype)
	ss<<token;
	ss>>pdata[i++];
	ss.clear();
	//std::cout << static_cast<dtype*>(data)[i-1] << " ";
      }
    }
    
    return true;
  }
  
  /**************************************************************************/

  /// End access to the file.
  bool close(void){
    if (ins.is_open()){    
      ins.close();
    }    
    
    return true;
  }
  
};

#endif
