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

/// Compute norm, check for infs and NaNs for variable between base_file and test_file.
template <typename d_type>
bool check_variable(const int &norm_type, const char *variable, 
		    Data *base_file, const int & base_step, 
		    Data *test_file, const int &test_step);

/// CalculatNorm Application
/**
 *  Computes a norm of <VARIABLE> for timestep <BASE_STEP> and
 *  <TEST_STEP> ofthe data files <BASE_FILE> & <TEST_FILE>.
 *
 *  Norm options:
 *    - Max norm
 *    - L2 norm (RMS error)
 *        http://en.wikipedia.org/wiki/Root_mean_square
 *
 *        The RMS error is "normalized" in the sense that we compute
 *             ( (x_i - y_i)/( |x_i| + |y_i| ) )^2
 *        pointwise rather than (x_i - y_i)^2 for the RMS error.
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
 *      3. check for infs and NaNs
 *      4. compute norm
 ************************************
 *
 * FIXME:  There's some overlap/duplicate code here with RMSerror.C... 
 */
int main(int argc, char **argv)
{
  char base_filename[2048], test_filename[2048];      // INPUT filenames of data that will be read
  int dataset_type;                                   // INPUT integer to keep track of what data is being
                                                      // read (ASCII=0, HDF=1, NETCDF=2, etc.)
  int norm_type;                                      // 1=max norm, 2=Normalized RMS error
  Data *base_file, *test_file;                        // data that will be read
  int base_step, test_step;
  char variable[128];                                 // variable name to read
  bool isFloat = false;
  bool isDouble = false;
  
  // Initialize data
  base_file = NULL;
  test_file = NULL;
  /////////////////////////////
  // SET COMMAND-LINE ARGS: //
  ///////////////////////////////////////////////////////////////
  // Check arguments
  if ( argc < 8 ){
    printHelp(argv[0]);
    exit(0);
  }
  dataset_type = atoi(argv[1]);
  norm_type = atoi(argv[2]);
  strcpy(base_filename, argv[3]);
  base_step = atoi(argv[4]);
  strcpy(test_filename, argv[5]);
  test_step = atoi(argv[6]);
  strcpy(variable, argv[7]);  
  ///////////////////////////////////////////////////////////////

  //////////////
  // DO IT!!! /
  ////////////

  // Create "data" object and open the file:
  switch(dataset_type) {
  case 0: 
    base_file = new ASCII_data<float>((const char *) base_filename);
    test_file = new ASCII_data<float>((const char *) test_filename);
    isFloat = true;
    break;
  case 1:
    base_file = new HDF_data<float>((const char *) base_filename);
    test_file = new HDF_data<float>((const char *) test_filename);
    isFloat = true;
    break;
  case 2:
    base_file = new HDF_data<double>((const char *) base_filename);
    test_file = new HDF_data<double>((const char *) test_filename);
    isDouble = true;
    break;
  default:
    std::cerr << "*** Unknown dataset type " << dataset_type << "\n";
    return -1;
  }

  ///////////////////////////////////////////////////////////////
  // verify the data:
  if (isFloat){
    if (! check_variable<float>(norm_type, variable, base_file, base_step, test_file, test_step)){
      return -1;
    }
  }else if (isDouble){
    if (! check_variable<double>(norm_type, variable, base_file, base_step, test_file, test_step)){
      return -1;
    }
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
	    << "\t RMSdifference <FILE_TYPE> <NORM> <BASE_FILE> <BASE_STEP> <TEST_FILE> <TEST_STEP> <VARIABLE>\n"
	    << "\n"
	    << "\t FILE_TYPE - Integer number corresponding to file type\n"
	    << "\t\t 0  -  whitespace-separated ASCII text file with n columns; Specify column number in \"VARIABLE\".\n"
	    << "\t\t 1  -  HDF (i.e. LFM-para and MIX files)\n"
	    << "\t\t 2  -  NETCDF (i.e. TIEGCM files)\n"
	    << "\t NORM\n"
	    << "\t\t 1  -  Max norm\n"
	    << "\t\t 2  -  L2 (Normalized RMS difference)\n"
	    << "\t BASE_FILE - path to base \"gold standard\" data file\n"
	    << "\t BASE_STEP - timestep of base file\n"
	    << "\t TEST_FILE - path to test data file\n"
	    << "\t TEST_STEP - timestep of test file\n"
	    << "\t VARIABLE - variable to check\n"
	    << "\n"
	    << "Computes the norm difference of <VARIABLE> from <BASE_FILE> - <TEST_FILE>.\n"
	    << "\n";
}

/**************************************************************************/

template <typename d_type>
bool check_variable(const int &norm_type, const char *variable, 
		    Data *base_file, const int &base_step, 
		    Data *test_file, const int &test_step)
{
  int base_rank, test_rank;                           // rank (# of dimensions) of dataset variables
  int base_ijk[MAX_VAR_DIMS], test_ijk[MAX_VAR_DIMS]; // dimension of dataset
  int nElements;                                      // ni * nj * nk
  d_type *base_data, *test_data;                       // variables to hold floating-point data that's read from file.
  d_type norm;                                    // rms error that we're going to compute.  

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

  if ( base_rank < 4 ){
    std::cerr << "*** Error: RMSdifference needs four-dimensional data, but rank(base_data) = " << base_rank << std::endl;
  }
  if ( test_rank < 4 ){
    std::cerr << "*** Error: RMSdifference needs four-dimensional data, but rank(test_data) = " << test_rank << std::endl;
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
    base_data = new d_type[nElements];
  }
  else{
    std::cerr << "*** base_data already defined for variable \"" << variable << "\"!\n";
  }
  if(test_data == NULL){
    test_data = new d_type[nElements];
  }
  else{
    std::cerr << "*** test_data already defined for variable \"" << variable << "\"!\n";
  }

  ////////////////////
  //   2. read data / 
  //////////////////
  if ( !base_file->getData(variable, base_data) ){
    std::cerr << "*** Error reading data from \"" << base_file->getFilename() 
	      << "\" for variable \"" << variable << "\"!\n";
    return false;
  }
  if ( !test_file->getData(variable, test_data) ){
    std::cerr << "*** Error reading data from \"" << test_file->getFilename() 
	      << "\" for variable \"" << variable << "\"!\n";
    return false;
  }  

  ////////////////////////////
  //   3. compute norm  error /
  //////////////////////////
  switch(norm_type){
  case 1:
    norm = compute_max_difference(variable, base_rank, base_ijk, base_data, base_step, test_data,test_step);
    break;
  case 2:
    norm = compute_rms_difference(variable, base_rank, base_ijk, base_data, base_step, test_data,test_step);
    break;
  default:
    std::cerr << "*** Error computing norm_type=\"" << norm_type 
	      << "\".  See usage message for valid norm_types." << std::endl;
    break;
  }

  /////////////////////////////
  //   4. Free up the memory /
  ///////////////////////////
  delete[] base_data;
  delete[] test_data;

  ////////////////////////
  //   5. Output status /
  //////////////////////
#ifdef VERBOSE
  std::cout << "RMS error for variable "<< variable << "\" in "
	    << "\"" << base_file->getFilename() << "\" to "
	    << "\"" << test_file->getFilename() << "\":  "
	    << norm << std::endl;
#else
    std::cout << norm << "\n";
#endif
  return true;
}

/**************************************************************************/
