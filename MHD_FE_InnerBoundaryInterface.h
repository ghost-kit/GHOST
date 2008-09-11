/**
 * \file MHD_FE_InnerBoundaryInterface.h
 * \brief MHD_FE_InnerBoundaryInterface declaration
 *
 * \class MHD_FE_InnerBoundaryInterface 
 * \brief Defines functions for communication with MIX using File Exchanges
 *
 * \author Peter Schmitt (schmitt at bu.edu) 
 * \since 09-2008
 *
 * This class defines functions for communication between an MHD code
 * (i.e. LFM) and the Ionosphere Coupler/Solver (MIX) code using file
 * exchanges.  Communication occurs through a series of I/O to the
 * following files:
 *
 *   - MHD_status
 *   - MHD_scalars
 *   - MHD_current
 *   - MHD_density
 *   - MHD_soundSpeed
 *
 * If your computing environment supports it, you should use the
 * InterComm version of this class (MHD_IC_InnerBoundaryInterface).
 * The InterComm version communicates with MIX much more efficiently.
 *
 * This code implies that the MHD grid is a logically spherical grid
 * with the symmetry axis aligned with the SM x-axis (LFM definition).
 *
 * \see MHDInnerBoundaryInterface, MHD_IC_InnerBoundaryInterface, MHD_Interface, MHD_FE_Interface
 */
#ifndef FE_INTERFACE_H
#define FE_INTERFACE_H

#include "MHDInnerBoundaryInterface.h"

class MHD_FE_InnerBoundaryInterface : public MHDInnerBoundaryInterface {
public:

  /**\brief Class constructor */
  MHD_FE_InnerBoundaryInterface(char*, char*,
				const doubleArray &, const doubleArray &, const doubleArray &, 
				const int, const int, const int);

  /**\brief Export the MHD FAC, density and sound speed to the MIX code*/
  void Export(const doubleArray &, const doubleArray &, const doubleArray &, 
	      const doubleArray &, const doubleArray &);
  
  /**\brief Import potential and calculate electric field & velocity at the inner MHD boundary*/
  void Import(doubleArray &, doubleArray &, doubleArray &);

  /** \brief Broadcast scalar array (i.e. UT & kill signal) from MHD code. */
  void sendScalars(const doubleArray &);

private:
  /**\name File exchange members */
  //@{

  /**\brief Returns true if MIX is busy */
  bool isMIXBusy()  { return ( strcmp( "WORKING", readMIXLockFile().c_str() ) == 0 ); }
  /**\brief Returns true if MIX is ready and waiting for the MHD */
  bool isMIXReady() { return ( strcmp( "WAITING", readMIXLockFile().c_str() ) == 0 ); }

  /**\brief Set the MHD lock file to "WORKING" (aka busy)  */
  bool setMHDBusy(void)   { return writeMHDLockFile("WORKING"); }
  /**\brief Set the MHD lock file to "WAITING" (aka ready) */
  bool setMHDReady(void)  { return writeMHDLockFile("WAITING"); }

  /**\brief  Write #status to the MHD lock file */
  bool  writeMHDLockFile(const string &status);
  /**\brief  Read the MIX lock file to check its status */
  string readMIXLockFile(void);

  /**\brief  Read a 3d array from file into #data*/
  void read3dData(const string &filename, const doubleArray &data,  
		  const int &ni, const int &nj, const int &nk);
  /**\brief  Write #data to file */
  void write3dData(const string &filename, const doubleArray &data,  
		   const int &ni, const int &nj, const int &nk);

  /**\brief filename of MIX lock file. */
  string MIXLockFile;
  /**\brief filename of MHD lock file. */
  string MHDLockFile;
  //@}

};

#endif
