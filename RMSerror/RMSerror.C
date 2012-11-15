#include <cmath>
#include <cassert>
#include <cstring>
// Some machines need iostream.h while others need iostream...
// If you get errors with std::cout, try swapping these:
// #include <iostream.h>
#include <iostream>
#include <limits>
#include <mfhdf.h> // defines NC_VAR_DIMS

#include "Data.h"
#include "ASCII_data.h"
#include "HDF_data.h"

#include "helpers.h"

/// Print usage information.
void printHelp(const char *);
/// Compute RMS error, check for infs and NaNs for variable between base_file and test_file.
bool check_variable(const char *variable, const float rms_tolerance, Data *base_file, Data *test_file);

#ifndef H4_MAX_VAR_DIMS
#define H4_MAX_VAR_DIMS MAX_VAR_DIMS
#endif

/// RMSerror Application
/**
 *  Computes a normalized RMS error of <VARIABLE> for the data files <BASE_FILE> - <TEST_FILE>. 
 *
 *  http://en.wikipedia.org/wiki/Root_mean_square
 *
 *  The RMS error is "normalized" in the sense that we compute
 *       ( (x_i - y_i)/( |x_i| + |y_i| ) )^2
 *  pointwise rather than (x_i - y_i)^2 for the RMS error.
 *
 *  Psuedo-code for this application:
 *
 ************************************
 *    Read <VARIABLE> data from <BASE_FILE>
 *    Read <VARIABLE> data from <TEST_FILE>
 * 
 *    verification:
 *    for each variable in the file
 *      1. compare dimensions
 *      2. read data  
 *      3. Check for infs and NaNs
 *      4. compute RMS error
 *      5. Check within bounds
 ************************************
 */
int main(int argc, char **argv)
{
  char base_filename[2048], test_filename[2048];      // INPUT filenames of data that will be read
  int dataset_type;                                   // INPUT integer to keep track of what data is being
                                                      // read (ASCII=0, HDF=1, NETCDF=2, etc.)
  float rms_tolerance;                                // INPUT tolerance that rms_error should be within.
  char variable[128];                                 // variable name to read
  Data *base_file, *test_file;                        // data that will be read
  
  // Initialize data
  base_file = NULL;
  test_file = NULL;
  /////////////////////////////
  // SET COMMAND-LINE ARGS: //
  ///////////////////////////////////////////////////////////////
  // Check arguments
  if ( argc < 6 ){
    printHelp(argv[0]);
    exit(0);
  }
  dataset_type = atoi(argv[1]);
  strcpy(base_filename, argv[2]);
  strcpy(test_filename, argv[3]);
  strcpy(variable, argv[4]);  
  rms_tolerance = atof(argv[5]);
  ///////////////////////////////////////////////////////////////

  //////////////
  // DO IT!!! /
  ////////////

  // Create "data" object and open the file:
  switch(dataset_type) {
  case 0: 
    base_file = new ASCII_data<float>((const char *) base_filename);
    test_file = new ASCII_data<float>((const char *) test_filename);
    break;
  case 1:
    base_file = new HDF_data<float>((const char *) base_filename);
    test_file = new HDF_data<float>((const char *) test_filename);
    break;
  case 2:
    base_file = new HDF_data<double>((const char *) base_filename);
    test_file = new HDF_data<double>((const char *) test_filename);
    break;
  default:
    std::cerr << "*** Unknown dataset type " << dataset_type << "\n";
    return -1;
  }

  ///////////////////////////////////////////////////////////////
  // verify the data:
  if (! check_variable(variable, rms_tolerance, base_file, test_file)){
    return -1;
  }
  ///////////////////////////////////////////////////////////////

  // close the files:  
  if (! base_file->close() ){
    std::cerr << "*** Error closing " << base_filename << "\n";
    return -1;
  }
  if ( ! test_file->close() ){
    std::cerr << "*** Error closing " << test_filename << "\n";
    return -1;
  }

  delete base_file;

  return 0;
}

/**************************************************************************/

void printHelp(const char * exe_name)
{
  std::cout << exe_name << "\n"
	    << "Usage: \n"
	    << "\t RMSerror <FILE_TYPE_INTEGER> <BASE_FILE> <TEST_FILE> <VARIABLE> <TOLERANCE>\n"
	    << "\n"
	    << "\t FILE_TYPE_INTEGER - Integer number corresponding to file type\n"
	    << "\t\t 0  -  whitespace-separated ASCII text file with n columns; Specify column number in \"VARIABLE\".\n"
	    << "\t\t 1  -  HDF (i.e. LFM-para and MIX files)\n"
	    << "\t\t 2  -  NETCDF (i.e. TIEGCM files)\n"
	    << "\t BASE_FILE - path to base \"gold standard\" data file\n"
	    << "\t TEST_FILE - path to test data file\n"
	    << "\t VARIABLE - variable to check\n"
	    << "\t TOLERANCE - acceptable tolerance that the RMS error should be within\n"
	    << "\n"
	    << "Computes the RMS error of <VARIABLE> from <BASE_FILE> - <TEST_FILE>.\n"
	    << "\n";
}

/**************************************************************************/

bool check_variable(const char *variable, const float rms_tolerance, Data *base_file, Data *test_file)
{
  int base_rank, test_rank;                           // rank (# of dimensions) of dataset variables
  int base_ijk[H4_MAX_VAR_DIMS], test_ijk[H4_MAX_VAR_DIMS]; // dimension of dataset
  int nElements;                                      // ni * nj * nk
  float *base_data, *test_data;                       // variables to hold floating-point data that's read from file.
  float rms_error;                                    // rms error that we're going to compute.  

  // initialize variables
  base_data = NULL;
  test_data = NULL;

  /////////////////////////////
  //   1. compare dimensions /
  ///////////////////////////
  base_rank = base_file->getDimensions(variable, base_ijk);
  test_rank = test_file->getDimensions(variable, test_ijk);

  if ( base_rank == 0 ){
    std::cerr << "*** Error reading dimension for base_rank for variable \"" << variable << "\"\n";
    return false;
  }
  if ( test_rank == 0 ){
    std::cerr << "*** Error reading dimension for base_rank for variable \"" << variable << "\"\n";
    return false;
  }

  if ( !check_dimensions(base_rank, base_ijk, test_rank, test_ijk) ){
    std::cerr << "*** Dimensions differ for variable \"" << variable << "\"!\n";
    return false;
  }

  ///////////////////////
  //   Allocate Memory /
  /////////////////////
  nElements = 1;
  for (int i = 0; i < base_rank; i++){
    nElements *= base_ijk[i];
  }

  if (base_data == NULL){
    base_data = new float[nElements];
  }
  else{
    std::cerr << "*** base_data already defined for variable \"" << variable << "\"!\n";
  }
  if(test_data == NULL){
    test_data = new float[nElements];
  }
  else{
    std::cerr << "*** test_data already defined for variable \"" << variable << "\"!\n";
  }

  ////////////////////
  //   2. read data / 
  //////////////////
  if ( !base_file->getData(variable, base_data) ){
    std::cerr << "*** Error reading data from \"" << base_file->getFilename() << "\" for variable \"" << variable << "\"!\n";
    return false;
  }
  if ( !test_file->getData(variable, test_data) ){
    std::cerr << "*** Error reading data from \"" << test_file->getFilename()  << "\" for variable \"" << variable << "\"!\n";
    return false;
  }
  
  //////////////////////////////////
  //   3. Check for infs and NaNs /
  ////////////////////////////////
  if ( !global_check_valid_data(base_rank, base_ijk, base_data, test_data) ){
    std::cerr << "*** Found an inf or NaN in the data for variable \"" << variable << "\"!\n";
    return false;
  }
    
  ////////////////////////////
  //   4. compute RMS error /
  //////////////////////////
  rms_error = compute_global_rms_difference(variable, base_rank, base_ijk, base_data, test_data);

  //////////////////////////////
  //   5. Check within bounds /
  ////////////////////////////
  if ( rms_error > rms_tolerance ){
    std::cerr << "WARNING: rms_error = " << rms_error
	      << " exceeds threshold of " << rms_tolerance
	      << " for variable \"" << variable << "\".\n";
    assert(rms_error < rms_tolerance);
    return false;
  }    

  ////////////////////////////
  //  6. Free up the memory /
  //////////////////////////
  delete[] base_data;
  delete[] test_data;

  ///////////////////////
  //  7. Output status /
  /////////////////////
#ifdef VERBOSE
  std::cout << "RMS error for variable "<< variable << "\" in "
	    << "\"" << base_file->getFilename() << "\" to "
	    << "\"" << test_file->getFilename() << "\":  "
	    << rms_error << " < tolerance "
	    << rms_tolerance << ".  "
	    << "OK!\n";
#else
    std::cout << rms_error << "\n";
#endif
  return true;
}

/**************************************************************************/
