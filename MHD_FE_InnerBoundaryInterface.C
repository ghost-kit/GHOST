/**
 * \file MHD_FE_InnerBoundaryInterface.C 
 * \brief MHD_FE_InnerBoundaryInterface definition.
 */

#include "MHD_FE_InnerBoundaryInterface.h"

/********************************************************************//**
 * \author Peter Schmitt (schmitt at ucar.edu)
 * \since 09-2008
 * \param[in] jobDescriptionMJD   The name of the InterComm XJD file
 * \param[in] localName           The name of the local program
 * \param[in] x                   X coordinates of the MHD grid vertices
 * \param[in] y                   Y coordinates of the MHD grid vertices
 * \param[in] z                   Z coordinates of the MHD grid vertices
 * \param[in] NI  The number of shells in the i-th direction where to calculate the FAC
 * \param[in] NJ  The number of grid cells in the j-th direction
 * \param[in] NK  The number of grid cells in the k-th direction
 *
 * Constructor that initializes the LFM-MIX interface and brings up
 * the file exchange interface.
 *
 * This calls the MHDInnerBoundaryInterface base class constructor,
 * which calculates a lot of grid-related quantities that are needed
 * for future calculations (especially the field-aligned current
 * calculation), but have to be calculated only once. Among these
 * quantities are: cell volumes, cell centers, and face centers as
 * well as magnetic field at the inner MHD boundary (dipole) for the
 * parallel current calculation.
 *
 * \retval The outputs are placed in the global variables declared class-wide: 
 * - #volume - array containing calculated cell volumes
 * - #xCenter - Cell center coordinates
 * - #yCenter
 * - #zCenter
 * - #x_iFaceCenters - Face center coordinates
 * - #y_iFaceCenters etc.
 *
 * These quantities are needed because the calculation is done on a
 * staggered grid. See jpara.tex for details.
 *
 * #bxDot, #byDot, #bzDot are the components of the modified magnetic
 * field multiplying the current density to obtain ionospheric FAC.
 * (see jpara.tex for details)
 *
 * The cell volume is calculated as a triple product of vectors
 * connecting centers of opposing faces.  Cells are offset half-cell
 * size (modified cells are based on original cell centers). See
 * jpara.tex for details.
 *
 * \see MHDInnerBoundaryInterface::MHDInnerBoundaryInterface
 ************************************************************************/
MHD_FE_InnerBoundaryInterface::MHD_FE_InnerBoundaryInterface(char* jobDescriptionMJD, char* localName,
		     const doubleArray &x, const doubleArray &y, const doubleArray &z,
		     const int NI, const int NJ, const int NK)
  :
  // Base class constructor
  MHDInnerBoundaryInterface(jobDescriptionMJD, localName, x,y,z, NI,NJ,NK)
{
  // Stdout that we're using the file exchange interface...
  cout << "Initializing MHD File Exchange Interface..." << endl;
  ///////////////////////////////////////////////////////////////////////

  // Should maybe use the stdio.h function "tmpnam"  for these files?
  MIXLockFile = "MIX_status";
  MHDLockFile = "MHD_status";

  assert( setMHDBusy() );

  while( !isMIXBusy() ){
#ifdef DEBUG_MODE_ON
    std::cout << "  MHD_FE: Waiting for MIX to start...\n";
#endif
    sleep(2);
  }
#ifdef DEBUG_MODE_ON
  std::cout << "  MHD_FE: MIX started.\n";
#endif
  // Sleep to prevent any buffered I/O issues with file exchanges
  sleep(5);
} 

MHD_FE_InnerBoundaryInterface::~MHD_FE_InnerBoundaryInterface()
{
#ifdef DEBUG_MODE_ON
  cout << "  MHD_FE: Destructor called.\n"; 
#else 
  assert( remove(MHDLockFile.c_str()) == 0 );
 
  remove("MHD_scalars");
  remove("MHD_current");
  remove("MHD_density");
  remove("MHD_soundSpeed");
#endif
}

/********************************************************************//**
 * \author Peter Schmitt (schmitt at ucar.edu)
 * \since 09-2008
 *
 * \param[in] bx MHD magnetic field x-component
 * \param[in] by MHD magnetic field y-component
 * \param[in] bz MHD magnetic field z-component
 * \param[in] rho MHD density
 * \param[in] cs MHD sound speed
 *
 * Retrieve the FAC, density & sound speed in the second shell and use
 * file exchanges to send these variables to MIX.
 ************************************************************************/
void MHD_FE_InnerBoundaryInterface::Export(const doubleArray & bx, const doubleArray & by, const doubleArray & bz, 
		   const doubleArray &rho, const doubleArray &cs)
{
  ///////////////////////////////////////////////////////////////////////

  // Calculate the FAC, density and sound speed in the second-shell
  prepareExport(bx,by,bz, rho, cs);

  ///////////////////////////////////////////////////////////////////////
  // Export the data to MIX using file exchanges.
  
  write3dData("MHD_current", current, njp1, nkp1, 0);

  write3dData("MHD_density", density, njp1, nkp1, 0);

  write3dData("MHD_soundSpeed", soundSpeed, njp1, nkp1, 0);

  ///////////////////////////////////////////////////////////////////////

#ifdef DEBUG_MODE_ON
  cout << "  MHD_FE: MHD Exported current, density & soundSpeed to MIX.\n" << flush;
#endif
  // Set status to "WAITING"
  assert( setMHDReady() );

  // Wait until MIX is finished "WORKING" 
  while ( ! isMIXReady() ){ 
#ifdef DEBUG_MODE_ON
    cout << "  MHD_FE: Waiting until MIX is done importing current, density & soundSpeed...\n" << flush;
#endif
    sleep(2);
  }
#ifdef DEBUG_MODE_ON
  cout << "  MHD_FE: MIX receieved current, density & soundSpeed.  Moving on.  \n";
#endif
  // Now that MIX is "WAITING", change MHD status to "WORKING"
  assert( setMHDBusy() );  

  // Sleep to prevent any buffered I/O issues with file exchanges
  sleep(5);
}

/********************************************************************//**
 * \author Peter Schmitt (schmitt at ucar.edu)
 * \since 09-2008
 *
 * \param[out] eField_j   Electric field j-component at the i=1 edge
 * \param[out] eField_k   Electric field k-component at the i=1 edge
 *
 * \param[out] velocity Velocity (x,y,z components) at the center of
 * the first layer of cells
 *
 * The function first uses file exchanges to get the ionospheric
 * potential in the first two shell from the MIX code. Then
 * processImport calculates #eField_j and #eField_k at the first shell
 * (i=1) edges and #velocity is calculated at the centers of the first
 * layer of cells (i=1).
 ************************************************************************/
void MHD_FE_InnerBoundaryInterface::Import(doubleArray & eField_j, doubleArray & eField_k, doubleArray & velocity)
{
  // Wait until MIX is finished "WORKING"
  while ( ! isMIXReady() ){
#ifdef DEBUG_MODE_ON
    std::cout << "  MHD_FE: Waiting for MIX to export potential...\n" << flush;
#endif
    sleep(2);
  }
#ifdef DEBUG_MODE_ON
  std::cout << "  MHD_FE: MIX has exported potential! Read it in.\n" << flush;
#endif
  ///////////////////////////////////////////////////////////////////////
  // Import (read the data from file. . . )

  read3dData("MIX_potential", potential, 2, njp1, nkp1);
#ifdef DEBUG_MODE_ON
  write3dData("MHD_potential", potential, 2, njp1, nkp1);
#endif

  ///////////////////////////////////////////////////////////////////////

  // MHD is done reading.  Tell MIX to continue:
#ifdef DEBUG_MODE_ON
  std::cout << "  MHD_FE: Potential read in by MHD.\n";
#endif

  // Set MHD status to "WAITING"
  assert( setMHDReady() );

  // Do not return until MIX knows that we're done working.
  while ( ! isMIXBusy() ){
#ifdef DEBUG_MODE_ON
    std:: cout << "  MHD_FE: Waiting for MIX to acknowledge that we've received the potential...\n";
#endif
    sleep(2);
  }

#ifdef DEBUG_MODE_ON
  std::cout << "  MHD_FE: MIX acknowledgment; Moving on.\n";
#endif

  // MIX has moved on.  Set MHD to busy
  assert( setMHDBusy() );

  ///////////////////////////////////////////////////////////////////////
  // Now do the dirty work: calculate the potential electric field
  // and the velocity at the inner boundary
  processImport(eField_j, eField_k, velocity);
}


/********************************************************************//**
 * \author Peter Schmitt (schmitt at ucar.edu)
 * \since 09-2008
 *
 * \param[in] scalars to send to MIX.
 *
 * Perform the following steps:
 *   # Write scalars to file
 *   # Set status to "WAITING"
 *   # Wait until MIX is finished "WORKING" 
 *   # When MIX is "WAITING", change status to "WORKING"
 ************************************************************************/
void MHD_FE_InnerBoundaryInterface::sendScalars(const doubleArray & scalars) 
{ 
  ///////////////////////////////////////////////////////////////////////

  // open "MHD_scalars" for writing
  std::ofstream outs("MHD_scalars");

  // Write scalars to file
  for ( int i = 0; i < scalars.elementCount(); i++ ){
    outs << scalars(i) << "\n";
  }

  // Make sure to flush the IO to disk.
  outs.flush();

  // close "MHD_scalars" file handle
  outs.close();

  ///////////////////////////////////////////////////////////////////////

#ifdef DEBUG_MODE_ON
  cout << "  MHD_FE: MHD Scalars sent to MIX.\n" << flush;
#endif

  // Set status to "WAITING"
  assert( setMHDReady() );

  // Wait until MIX is finished "WORKING" 
  while ( ! isMIXReady() ){ 
#ifdef DEBUG_MODE_ON
    cout << "  MHD_FE: Waiting until MIX is done reading scalars...\n" << flush;
#endif
    sleep(2);
  }
#ifdef DEBUG_MODE_ON
  cout << "  MHD_FE: MIX receieved the scalars.  Moving on.  \n";
#endif

  // Now that MIX is "WAITING", change MHD status to "WORKING"
  assert( setMHDBusy() );  

  // Sleep to prevent any buffered I/O issues with file exchanges
  sleep(5);
}

/************************************************************************
 * Private member functions
 ***********************************************************************/

/********************************************************************//**
* \author Peter Schmitt (schmitt at ucar.edu)
* \since 09-2008
*
* \param[in] status - string that sets the MIX lock file status
* (e.g. "WAITING" or "WORKING").
*
* \param[out] bool - returns true if succesful.
*
* Write to code status to the SACII text file MIXLockFile
*
* \FIXME Should probably handle errors within this function rather
* than return a bool....
************************************************************************/
bool MHD_FE_InnerBoundaryInterface::writeMHDLockFile(const string &status)
{
  // open the file for writing
  std::ofstream outs(MHDLockFile.c_str());
  if (outs.bad()){
    std::cerr << "*** Trouble Writting " << MHDLockFile << "\n";
    return false;
  }

  // write status
  outs << status << "\n";

  // Make sure to flush the IO to disk.
  outs.flush();

  // close the file
  outs.close();

  return true;
}

/********************************************************************//**
* \author Peter Schmitt (schmitt at ucar.edu)
* \since 09-2008
*
* \param[out] MIXLockFile text file contents.  Should be "WORKING" or "WAITING".
*
* Returns the contents of the ASCII file MIXLockFile.
************************************************************************/
string MHD_FE_InnerBoundaryInterface::readMIXLockFile(void)
{
  // open the file for reading
  std::ifstream ins(MIXLockFile.c_str());
  if (ins.bad()){
    std::cerr << "*** Trouble reading " << MIXLockFile << "\n";
    // FIXME:  Error handling?
    return false;
  }

  // read status
  string statusString;
  ins >> statusString;

  // close the file
  ins.close();

  return statusString;
}

/********************************************************************//**
 * \author Peter Schmitt (schmitt at ucar.edu)
 * \since 09-2008
 *
 * Read 3d data from #filename into #data
 *
 * \param[in] filename - Points to the file to read data from
 * \param[out] data - 1, 2 or 3d array containing data that will be written to #filename
 * \param[in] ni - number of elements in i direction
 * \param[in] nj - number of elements in j direction (nj and nk should be 0 for 1d data)
 * \param[in] nk - number of elements in k direction (should be 0 for 2d data)
 *
 * \note The file format defined here needs to match that defined in
 * MHD_FE_Interface::write3dData
 *
 * \FIXME #data could be a template so we can handle any dimension data?
 ************************************************************************/
void MHD_FE_InnerBoundaryInterface::read3dData(const string &filename, const doubleArray &data,
					       const int &ni, const int &nj, const int &nk)
{
  // open the file for writing
  std::ifstream ins(filename.c_str());
  if (ins.bad()){
    std::cerr << "*** Trouble Writting " << filename << "\n";
  }

  // read data
  int ni_in, nj_in, nk_in;

  ins >> ni_in >> nj_in >> nk_in;
  assert( ni_in == ni );
  assert( nj_in == nj );
  assert( nk_in == nk );

  for (int k = 0; k < max(nk, 1); k++){
    for (int j = 0; j < max(nj, 1); j++){
      for (int i = 0; i < max(ni, 1); i++){
	ins >> data(i,j,k);
      }
    }
  }

  // close the file
  ins.close();
}

/********************************************************************//**
 * \author Peter Schmitt (schmitt at ucar.edu)
 * \since 09-2008
 *
 * Write 3d array #data to #filename
 *
 * \param[in] filename - filename to write data to
 * \param[in] data - 1, 2 or 3d array containing data that will be written to #filename
 * \param[in] ni - number of elements in i direction
 * \param[in] nj - number of elements in j direction (nj and nk should be 0 for 1d data)
 * \param[in] nk - number of elements in k direction (should be 0 for 2d data)
 *
 * \note The file format defined here needs to match that defined in
 * MHD_FE_Interface::read3dData
 *
 * \FIXME #data could be a template so we can handle any dimension data?
 ************************************************************************/
void MHD_FE_InnerBoundaryInterface::write3dData(const string &filename, const doubleArray &data,  
						const int &ni, const int &nj, const int &nk)
{
  // open the file for writing
  std::ofstream outs(filename.c_str());
  if (outs.bad()){
    std::cerr << "*** Trouble Writting " << filename << "\n";    
  }

  // write data
  outs << ni << "\t" << nj << "\t" << nk << "\n";
  for (int k = 0; k < max(nk, 1); k++){
    for (int j = 0; j < max(nj, 1); j++){
      for (int i = 0; i < max(ni, 1); i++){
	outs << data(i,j,k) << "\t";
      }
      outs << "\n";
    }
    outs << "\n";
  }

  // Make sure to flush the IO to disk.
  outs.flush();

  // close the file
  outs.close();
}
