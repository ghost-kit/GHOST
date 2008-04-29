#include <cmath>
#include <cassert>
#include <cstring>
#include <iostream>
#include <limits>
#include <mfhdf.h> // defines NC_VAR_DIMS
#include "Data.h"
#include "ASCII.h"
#include "LFMpara.h"

/// Print usage information.
void printHelp(const char *);
/// Compute RMS error, check for infs and NaNs for variable between base_file and test_file.
bool check_variable(const char *variable, const float rms_tolerance, Data *base_file, Data *test_file);
/// Make sure that the two arrays (dim1 and dim2) have the same values for the dimensions.
bool check_dimensions(int rank_1, int *dim1, int rank_2, int *dim2);
/// Check base_data and test_data for infs and NaNs.
template<typename d_type>
bool check_valid_data(const int nElements, const d_type *base_data, const d_type *test_data);
/// Compute the RMS error for base_data - test_data
template<typename d_type>
float compute_rms_error(const char *variable, const int nElements, const d_type *base_data, const d_type *test_data);

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
                                                      // read (LFM-para ion, LFM-para mhd, RCM, TIEGCM, etc.)
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
    base_file = new ASCII((const char *) base_filename);
    test_file = new ASCII((const char *) test_filename);
    break;
  case 1:
    base_file = new LFMpara((const char *) base_filename);
    test_file = new LFMpara((const char *) test_filename);
    break;
  default:
    std::cerr << "*** Unknown dataset type " << dataset_type << "\n";
    return -1;
    break;    
  }

  ///////////////////////////////////////////////////////////////
  // verify the data:
  if (! check_variable(variable, rms_tolerance, base_file, test_file)){
    return -1;
  }
  ///////////////////////////////////////////////////////////////

  // close the files:  
  if (! base_file->close() ){
    std::cerr << "*** Error opening " << base_filename << "\n";
    return -1;
  }
  if ( ! test_file->close() ){
    std::cerr << "*** Error opening " << test_filename << "\n";
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
  int base_ijk[MAX_VAR_DIMS], test_ijk[MAX_VAR_DIMS]; // dimension of dataset
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
  if ( !check_valid_data(nElements, base_data, test_data) ){
    std::cerr << "*** Found an inf or NaN in the data for variable \"" << variable << "\"!\n";
    return false;
  }

  ////////////////////////////
  //   4. compute RMS error /
  //////////////////////////
  rms_error = compute_rms_error(variable, nElements, base_data, test_data);

  //////////////////////////////
  //   5. Check within bounds /
  ////////////////////////////
  if ( rms_error > rms_tolerance ){
    std::cerr << "*** FAIL: rms_error = " << rms_error
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

bool check_dimensions(int rank_1, int *dim1, int rank_2, int *dim2)
{
  assert(rank_1 == rank_2);
  for (int i=0; i < rank_1; i++){
    assert(dim1[i] == dim2[i]);
  }
  return true;
}

/**************************************************************************/

template<typename d_type>
bool check_valid_data(const int nElements, const d_type *base_data, const d_type *test_data)
{
  bool return_value = true;

  for (int i = 0; i < nElements; i++){
    // WARNING:   This makes use of "isinf" and "isnan", which might not be defined for some compilers.
    //            C99 standard says "isinf" and "isnan" are defined in <cmath>.
    if ( isinf(base_data[i]) ){
      return_value = false;
      std::cerr << "*** inf detected at " << i << " in " << "c_" << " in " << "base file\n"; 
    }
    if ( isnan(base_data[i]) ){
      return_value = false;
      std::cerr << "*** NaN detected at " << i << " in " << "c_" << " in " << "base file\n"; 
    }
    if ( isinf(test_data[i]) ){
      return_value = false;
      std::cerr << "*** inf detected at " << i << " in " << "c_" << " in " << "test file\n"; 
    }
    if ( isnan(test_data[i]) ){
      return_value = false;
      std::cerr << "*** NaN detected at " << i << " in " << "c_" << " in " << "test file\n"; 
    }
  }
  return return_value;
}

/**************************************************************************/

/**
 *  This computes the RMS error (L2 norm) over all points, normalized by \f$ |x_i| + |\bar{x_i}| \f$
 *
 *  @returns \f$ rtol = sqrt{\frac{1}{n}\sum_{i=1}^n \left( \frac{x_i - \bar{x_i}}{|x_i|+|\bar{x_i}|}{}\right)^2} \f$ 
 *  summed over all non-zero data points
 *
 */
template<typename d_type>
float compute_rms_error(const char *variable, const int nElements, const d_type *base_data, const d_type *test_data)
{
  // machine roundoff error
  float epsilon = std::numeric_limits<d_type>::epsilon();
  // We need to report a warning if the field is below roundoff error everywhere.
  bool isFieldZeroEverywhere = true;

  float rms_error = 0.0;
  float dx;

  for (int i = 0; i < nElements; i++){
    dx = base_data[i]-test_data[i];
    // if |dx| - epsilon <= 0, then it is below machine roundoff error and we throw out the data point.
    if ( (fabs(dx) - epsilon) > 0 ){      
      // data is non-zero
      isFieldZeroEverywhere = false;
      rms_error += dx*dx/( (fabs(base_data[i]) + fabs(test_data[i])) * (fabs(base_data[i]) + fabs(test_data[i])) );
    }
    //else if ( (fabs(dx) - epsilon) <= 0 )
      // data appears to be zero or within machine roundoff error 
  }
  if (isFieldZeroEverywhere)
    std::cerr << "WARNING:  { (base_data - test_data) < machine roundoff } at every point for variable \""
	      << variable << "\"!\n";
  rms_error /= nElements;
  rms_error = sqrt(rms_error);

  return rms_error;
}

/**************************************************************************/
