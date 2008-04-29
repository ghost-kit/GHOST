#include "ASCII.h"
#include <string> 

/**************************************************************************/

ASCII::ASCII(void)
{
}

/**************************************************************************/

/// Open ASCII file for reading.
/**
 * @param file - path to the ASCII file to open.
 */
ASCII::ASCII(const char *file)
{
  open(file);
}

/**************************************************************************/

/// Destructor - closes a file that's already been opened.
ASCII::~ASCII(void)
{
  if (ins.is_open())
    close();
}

/**************************************************************************/

/// Open ASCII file for reading
/**
 * @param file - path to the ASCII file to open.
 */
bool ASCII::open(const char *file)
{
  // copy filename
  strcpy(filename, file);

  // Open input file stream
  ins.open(file);

  if (ins.fail()){
    std::cerr << "*** Fail:  Unable to open \"" << file << "\".\n";
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
int ASCII::getDimensions(const char *variable, int *dimensions)
{
  int variableIndex = atoi(variable);
  char *token;
  int nDims;
  char data_str[1024];

  int MAX_VAR_DIMS = 32;

  for (int i = 0; i < MAX_VAR_DIMS; i++){
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
  for (int i = 0; i < MAX_VAR_DIMS; i++){
    if (dimensions[i] != 0)
      nDims++;
  }

  // close and re-open to reset the file pointer...  We should be able
  // to doo this with ins.seekg(...) but I couldn't get it working :(
  close();
  open(filename);

  if (variableIndex >= nDims){
    std::cerr << "*** ERROR:  Requested variable index \"" << variableIndex
	      << "\" is out of bounds of the rank of the dataset \"" << nDims 
	      << "\".\n";
    return 0;
  }
  return nDims;
}

/**************************************************************************/

/// Read data into memory
/**
 *  @param variable - Index of variable to read in.
 *  @param data - floating-point array of data to read in 
 *
 *  @note data should be allocated by the user before calling this function.
 */
bool ASCII::getData(const char *variable, float *data)
{
  int variableIndex = atoi(variable);
  char data_str[1024];
  int i = 0;
  char *token;

  while ( !ins.eof() ){
    ins.getline(data_str, 1024);

    // make sure we have a valid string:
    if (strlen(data_str) > 0){
      // Tokenize the string
      token = strtok(data_str, " \t ");
      // find the correct token
      for ( int j=0; j < variableIndex; j++ ){
	if (token == NULL){
	  std::cerr << "*** ERROR reading data for variable \"" << variable << "\".\n";
	  return false;
	}
	token = strtok(NULL, " \t ");
      }
      // Get the data from the token.
      data[i++] = atof(token);
    }
  }

  return true;
}

/**************************************************************************/

/// End access to the file.
bool ASCII::close(void)
{
  if (ins.is_open()){    
    ins.close();
  }    

  return true;
}

/**************************************************************************/
