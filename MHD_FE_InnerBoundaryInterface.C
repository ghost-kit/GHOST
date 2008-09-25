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
 * \param[in] NO2  n_order/2, corresponds to guard cell distribution, required for file exchange interface. Unused for InterComm interface.
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
		     const int NI, const int NJ, const int NK, const int NO2)
  :
  // Base class constructor
  MHDInnerBoundaryInterface(jobDescriptionMJD, localName, x,y,z, NI,NJ,NK, NO2)
{
  cout << "Initializing MHD File Exchange Interface..." << endl;

  ///////////////////////////////////////////////////////////////////////

  //FIXME: Should use the stdio.h function "tmpnam"  for these files?
  MIXLockFile = "MIX_status";
  MHDLockFile = "MHD_status";

  // Set MHD data to "OLD"
  assert( setMHDOld() );
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
#ifdef DEBUG_MODE_ON
  cout << "DEBUG:  process " << Communication_Manager::My_Process_Number << " "
       << "MHD_FE_InnerBoundaryInterface::Export(...)\n";
#endif

  ///////////////////////////////////////////////////////////////////////

  // Calculate the FAC, density and sound speed in the second-shell
  prepareExport(bx,by,bz, rho, cs);

  // Wait until MIX has set the MHDstatus to "old".
  while ( ! isMHDOld() ){
#ifdef DEBUG_MODE_ON
    std::cout << "  MIX_FE: Waiting for MIX to acknowledge that its ready for data Export...\n" << flush;
#endif
    sleep(4);
  }


  ///////////////////////////////////////////////////////////////////////
  // Export the data to MIX using file exchanges.

  /* ************  P++ trickery to gather data to head node ************ */
  // doing I/O on task 0
  Range io_pe(0,0); 

  // Strip guard cells to make indexing easy for dump
  Partitioning_Type Guard(io_pe);
  Guard.partitionAlongAxis(0,TRUE, 0);
  Guard.partitionAlongAxis(1,FALSE,0);

  // Dummy Variable used for I/O:
  doubleArray D_VAR(njp1,nkp1,Guard);

  Range Jp1(0, nj);
  Range Kp1(0, nk);

  /* ***************************** current ***************************** */
  // P++ trickery analagous to MPI_GATHER of current to head node:
  Communication_Manager::Sync();
  D_VAR(Jp1, Kp1) = current(Jp1, Kp1);
  // File I/O on head node:
  write3dData("MHD_current", D_VAR.getDataPointer(), njp1, nkp1, 0);

  /* ***************************** density ***************************** */
  // P++ trickery analagous to MPI_GATHER of density to head node:
  Communication_Manager::Sync();
  D_VAR(Jp1, Kp1) = density(Jp1, Kp1);
  // File I/O on head node:
  write3dData("MHD_density", D_VAR.getDataPointer(), njp1, nkp1, 0);

  /* *************************** soundSpeed **************************** */
  // P++ trickery analagous to MPI_GATHER of sound speed to head node:
  Communication_Manager::Sync();
  D_VAR(Jp1, Kp1) = soundSpeed(Jp1, Kp1);
  // File I/O on head node:
  write3dData("MHD_soundSpeed", D_VAR.getDataPointer(), njp1, nkp1, 0);

  ///////////////////////////////////////////////////////////////////////

#ifdef DEBUG_MODE_ON
  cout << "  MHD_FE: MHD Exported current, density & soundSpeed to MIX.\n" << flush;
#endif

  // Set MHD status to "new" since we've just sent the scalars.
  assert( setMHDNew() );
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
#ifdef DEBUG_MODE_ON
  cout << "DEBUG:  process " << Communication_Manager::My_Process_Number << " "
       << "MHD_FE_InnerBoundaryInterface::Import(...)\n";
#endif

  while ( ! isMIXNew() ){
#ifdef DEBUG_MODE_ON
    std::cout << "  MHD_FE: Waiting for MIX to export potential...\n" << flush;
#endif
    sleep(4);
  }

#ifdef DEBUG_MODE_ON
  std::cout << "  MHD_FE: MIX has exported potential! Read it in.\n" << flush;
#endif
  ///////////////////////////////////////////////////////////////////////
  // Import (read the data from file. . . )

  /* ************  P++ trickery to gather data to head node ************ */
  // doing I/O on task 0
  Range io_pe(0,0); 

  // Strip guard cells to make indexing easy for dump
  Partitioning_Type Guard(io_pe);
  Guard.partitionAlongAxis(0,TRUE, 0);
  Guard.partitionAlongAxis(1,TRUE, 0);
  Guard.partitionAlongAxis(2,FALSE,0);

  // Dummy Variable used for I/O:
  doubleArray D_VAR(2, njp1,nkp1,Guard);

  Range Ip1(0, 1);
  Range Jp1(0, nj);
  Range Kp1(0, nk);

  /* **************************** potential **************************** */
  Communication_Manager::Sync();
  // File I/O on head node:
  read3dData("MIX_potential", D_VAR.getDataPointer(), 2, njp1, nkp1);  

  // P++ trickery analagous to MPI_SCATTER of current to head node:
  potential(Ip1, Jp1, Kp1) = D_VAR(Ip1, Jp1, Kp1);

#ifdef DEBUG_MODE_ON
  write3dData("MHD_potential", potential.getDataPointer(), 2, njp1, nkp1);
#endif

  ///////////////////////////////////////////////////////////////////////

  // MHD is done reading.  Tell MIX to continue:
#ifdef DEBUG_MODE_ON
  std::cout << "  MHD_FE: Potential read in by MHD.\n";
#endif

  // MHD is done reading the "new" variables.  Set MIX status to "old"
  assert( setMIXOld() );

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
 *   # Wait until we know the scalars are old
 *   # Write scalars to file
 *   # Set status to "new"
 ************************************************************************/
void MHD_FE_InnerBoundaryInterface::sendScalars(const doubleArray & scalars) 
{ 
#ifdef DEBUG_MODE_ON
  cout << "DEBUG:  process " << Communication_Manager::My_Process_Number << " "
       << "MHD_FE_InnerBoundaryInterface::sendScalars(...)\n";
#endif

  // Wait until MIX has set the MHDstatus to "old".
  while ( ! isMHDOld() ){
#ifdef DEBUG_MODE_ON
    std::cout << "  MIX_FE: Waiting for MIX to acknowledge that its ready for scalars...\n" << flush;
#endif
    sleep(4);
  }
  

  ///////////////////////////////////////////////////////////////////////

  // FIXME: This should only be done on the head node (I/O processor)
  // but the LFM-MIX binary seems to get confused when reading scalars(i)...

  // open "MHD_scalars" for writing
  std::ofstream outs("MHD_scalars");
  
  if (outs.bad()){
    std::cerr << "*** Trouble Writting " << "MHD_scalars" << " on processor "
	      << Communication_Manager::My_Process_Number  << "\n";
  }

  // Write scalars to file
  for ( int i = 0; i < scalars.elementCount(); i++ ){
    outs << scalars(i) << "\n";
  }
  
  // Make sure to flush the IO to disk.
  outs.flush();
  
  // close "MHD_scalars" file handle
  outs.close();

  // Barrier to prevent race condition
  Communication_Manager::Sync();

  ///////////////////////////////////////////////////////////////////////

  // Set MHD status to "new" since we've just sent the scalars.
  assert( setMHDNew() );
}

/************************************************************************
 * Private member functions
 ***********************************************************************/

/********************************************************************//**
* \author Peter Schmitt (schmitt at ucar.edu)
* \since 09-2008
*
* \param[in] filename - path to lockfile that we'll write.
* \param[in] status - string that sets the MIX lock file status
* (e.g. "OLD" or "NEW").
*
* \param[out] bool - returns true if succesful.
*
* Write to code status to the SACII text file MIXLockFile
*
* \FIXME Should probably handle errors within this function rather
* than return a bool....
************************************************************************/
bool MHD_FE_InnerBoundaryInterface::writeLockFile(const string &filename, const string &status)
{
#ifdef DEBUG_MODE_ON
  cout << "DEBUG:  process " << Communication_Manager::My_Process_Number << " "
       << "MHD_FE_InnerBoundaryInterface::writeLockFile(...)\n";
#endif
  
  bool return_value = true;

  if ( Communication_Manager::My_Process_Number == 0 ){
    // open the file for writing
    std::ofstream outs(filename.c_str());
    if (outs.bad()){
      std::cerr << "*** Trouble Writting " << filename <<  " " << "on processor "
		<< Communication_Manager::My_Process_Number  << "\n";
      return_value = false;
    }
    
    // write status
    outs << status << "\n";
    
    // Make sure to flush the IO to disk.
    outs.flush();
    
    // close the file
    outs.close();
  }
    
  // Barrier to prevent race condition
  Communication_Manager::Sync();

  //FIXME    
  //FIXME    
  //FIXME    Need to be careful about return value...
  //FIXME    Should broadcast it from the  head node and/or
  //FIXME    call something like MPI_Abort() on error...
  //FIXME
  //FIXME

  return return_value;
}

/********************************************************************//**
* \author Peter Schmitt (schmitt at ucar.edu)
* \since 09-2008
*
* \param[in] filename - path to lockfile that we'll read.
* \param[out] MIXLockFile text file contents.  Should be "OLD" or "NEW".
*
* Returns the contents of the ASCII file MIXLockFile.
*
* \FIXME This the MIX lock file is read by EVERY processor!
*        Should probably just read this on the head node & broadcast it out...
************************************************************************/
string MHD_FE_InnerBoundaryInterface::readLockFile(const string &filename)
{
  // open the file for reading
  std::ifstream ins(filename.c_str());
  if (ins.bad()){
    std::cerr << "*** Trouble reading " << filename << "\n";
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
 * \note This function only reads the data in on the head node (process 0)
 *       You need to worry about gather/scatter operations outside this function. 
 *
 * \FIXME #data could be a template so we can handle any data type?
 ************************************************************************/
void MHD_FE_InnerBoundaryInterface::read3dData(const string &filename, double *data,
					       const int &ni, const int &nj, const int &nk)
{
#ifdef DEBUG_MODE_ON
  cout << "DEBUG:  process " << Communication_Manager::My_Process_Number << " "
       << "MHD_FE_InnerBoundaryInterface::read3dData(...)\n";
#endif

  // We should only write the file once on the head node
  if ( Communication_Manager::My_Process_Number == 0 ){  
    // open the file for writing
    std::ifstream ins(filename.c_str());
    if (ins.bad()){
      std::cerr << "*** Trouble reading " << filename << "\n";

      // FIXME: Need a more graceful way of exiting on all nodes, not just the I/O processor...
      Optimization_Manager::Exit_Virtual_Machine();
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
	  // ins >> data(i,j,k);
	  ins >> data[i + j*(ni) + k*(ni*nj)];
	}
      }
    }
    
    // close the file
    ins.close();
  }

  // Barrier to prevent race condition
  Communication_Manager::Sync();
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
 * \note This function only reads the data in on the head node (process 0)
 *       You need to worry about gather/scatter operations outside this function. 
 *
 * \FIXME #data could be a template so we can handle any data type?
 ************************************************************************/
void MHD_FE_InnerBoundaryInterface::write3dData(const string &filename, const double *data,  
						const int &ni, const int &nj, const int &nk)
{  
#ifdef DEBUG_MODE_ON
  cout << "DEBUG:  process " << Communication_Manager::My_Process_Number << " "
       << "MHD_FE_InnerBoundaryInterface::write3dData(...)\n";
#endif

  // We should only write the file once on the head node
  if ( Communication_Manager::My_Process_Number == 0 ){  
    // open the file for writing
    std::ofstream outs(filename.c_str());
    if (outs.bad()){
      std::cerr << "*** Trouble Writting " << filename << "\n";    

      // FIXME: Need a more graceful way of exiting on all nodes, not just the I/O processor...
      Optimization_Manager::Exit_Virtual_Machine();
    }
    
    // write data
    outs << ni << "\t" << nj << "\t" << nk << "\n";
    outs.precision(8);
    for (int k = 0; k < max(nk, 1); k++){
      for (int j = 0; j < max(nj, 1); j++){
	for (int i = 0; i < max(ni, 1); i++){
	  //outs << data[k + (j*nk) + i*(nj*nk)] << "\t";
	  outs << data[i + j*(ni) + k*(ni*nj)] << "\t";
	  //outs << data(i,j,k) << "\t";
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
  
  // Barrier to prevent race condition
  Communication_Manager::Sync();
}
