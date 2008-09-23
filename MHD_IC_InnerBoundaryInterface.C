/**
 * \file MHD_IC_InnerBoundaryInterface.C 
 * \brief MHD_IC_InnerBoundaryInterface definition.
 */

#include "MHD_IC_InnerBoundaryInterface.h"

/********************************************************************//**
 * \author Slava Merkin (vgm at bu.edu)
 * \since 05-2005 
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
 * the InterComm interface.
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
MHD_IC_InnerBoundaryInterface::MHD_IC_InnerBoundaryInterface(char* jobDescriptionMJD, char* localName,
		     const doubleArray &x, const doubleArray &y, const doubleArray &z,
		     const int NI, const int NJ, const int NK, const int NO2)
  :
  // Base class constructor
  MHDInnerBoundaryInterface(jobDescriptionMJD, localName, x,y,z, NI,NJ,NK, NO2),
  // bring up the InterComm interface
  epset(jobDescriptionMJD,localName,ic_err)
{
  cout << "MHD_IC_InnerBoundaryInterface is here..." << endl;
  
  /* Set up InterComm arrays */
  epset.registerArray("current",current,ic_err);
#ifdef DEBUG_MODE_ON
  epset.printErrorMessage("     Current registered. Status",ic_err);
#endif
 
  epset.registerArray("density",density,ic_err); 
#ifdef DEBUG_MODE_ON
  epset.printErrorMessage("     Density registered. Status",ic_err);
#endif

  epset.registerArray("sound speed",soundSpeed,ic_err);
#ifdef DEBUG_MODE_ON
  epset.printErrorMessage("     Sound speed registered. Status",ic_err);
#endif

  epset.registerArray("potential",potential,ic_err);
#ifdef DEBUG_MODE_ON
  epset.printErrorMessage("     Potential registered. Status",ic_err);
#endif

  epset.commitArrays(ic_err);
#ifdef DEBUG_MODE_ON
  epset.printErrorMessage("     Arrays commited. Status",ic_err);  
#endif
} 

/********************************************************************//**
 * \author Slava Merkin (vgm at bu.edu)
 * \since 05-2005 
 *
 * \param[in] bx MHD magnetic field x-component
 * \param[in] by MHD magnetic field y-component
 * \param[in] bz MHD magnetic field z-component
 * \param[in] rho MHD density
 * \param[in] cs MHD sound speed
 *
 * The function is called from the main level program (MHD code). It
 * calls prepareExport to get the FAC, density and sound speed in the
 * second shell. After that InterComm exportArray() functions are
 * called to export current, density and sound speed (precisely in
 * this order) to the MIX code.
 ************************************************************************/
void MHD_IC_InnerBoundaryInterface::Export(const doubleArray & bx, const doubleArray & by, const doubleArray & bz, 
		   const doubleArray &rho, const doubleArray &cs)
{
  // Calculate the FAC, density and sound speed in the second-shell
  prepareExport(bx,by,bz, rho, cs);

  // Export the data to MIX using InterComm
  epset.exportArray("current",ic_err);
#ifdef DEBUG_MODE_ON
  epset.printErrorMessage("     Current sent. Status",ic_err);
#endif

  epset.exportArray("density",ic_err);
#ifdef DEBUG_MODE_ON
  epset.printErrorMessage("     Density sent. Status",ic_err);
#endif

  epset.exportArray("sound speed",ic_err);
#ifdef DEBUG_MODE_ON
  epset.printErrorMessage("     Sound speed sent. Status",ic_err);
#endif
}

/********************************************************************//**
 * \author Slava Merkin (vgm at bu.edu)
 * \since 05-2005 
 *
 * \param[out] eField_j   Electric field j-component at the i=1 edge
 * \param[out] eField_k   Electric field k-component at the i=1 edge
 *
 * \param[out] velocity Velocity (x,y,z components) at the center of
 * the first layer of cells
 *
 * The function first calls InterComm importArray() to get the
 * ionospheric potential in the first two shell from the MIX
 * code. Then processImport calculates #eField_j and #eField_k at the
 * first shell (i=1) edges and #velocity is calculated at the centers
 * of the first layer of cells (i=1).
 ************************************************************************/
void MHD_IC_InnerBoundaryInterface::Import(doubleArray & eField_j, doubleArray & eField_k, doubleArray & velocity)
{
  epset.importArray("potential",ic_err);
#ifdef DEBUG_MODE_ON
  epset.printErrorMessage("     Potential for the two first shells received. Status",ic_err);
  cout << "Done"<<endl;
#endif
  // Now do the dirty work: calculate the potential electric field
  // and the velocity at the inner boundary

  processImport(eField_j, eField_k, velocity);
}


/********************************************************************//**
 * \author Slava Merkin (vgm at bu.edu)
 * \since 06-2007
 * \return   The array of scalars to be sent to the MIX code
 *
 * Use this function to broadcast an array of scalars to the MIX code.
 ************************************************************************/
void MHD_IC_InnerBoundaryInterface::sendScalars(const doubleArray & scalars) { 
  int nelems = scalars.elementCount();
  epset.bcastLocalArray("scalars",scalars,nelems,ic_err);
#ifdef DEBUG_MODE_ON
  epset.printErrorMessage("Scalars sent. Status: ",ic_err);
#endif
}
