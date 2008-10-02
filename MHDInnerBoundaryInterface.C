/**
 * \file MHDInnerBoundaryInterface.C 
 * \brief MHDInnerBoundaryInterface definition.
 */

#include "MHDInnerBoundaryInterface.h"

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
 *
 * The constructor calculates a lot of grid-related quantities that
 * are needed for future calculations (especially the field-aligned
 * current calculation), but have to be calculated only once. Among
 * these quantities are: cell volumes, cell centers, and face centers
 * as well as magnetic field at the inner MHD boundary (dipole) for
 * the parallel current calculation.
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
 ************************************************************************/
MHDInnerBoundaryInterface::MHDInnerBoundaryInterface(char* jobDescriptionMJD, char* localName,
		     const doubleArray &x, const doubleArray &y, const doubleArray &z,
		     const int NI, const int NJ, const int NK)
  :ni(NI),               // Get the grid info
   nj(NJ),               // 
   nk(NK),               // 
   nim1(ni - 1),         // 
   nip1(ni + 1),         // 
   njm1(nj - 1),         // 
   njp1(nj + 1),         // 
   nkm1(nk - 1),         // 
   nkp1(nk + 1),         // End grid info
   volume(nim1,njm1,nk), 
   xCenter(ni,nj,nkp1),
   yCenter(ni,nj,nkp1), 
   zCenter(ni,nj,nkp1),
   bxDot(nim1,njp1,nkp1),
   byDot(nim1,njp1,nkp1),
   bzDot(nim1,njp1,nkp1),
   x_iFaceCenter(ni,njm1,nk),
   y_iFaceCenter(ni,njm1,nk), 
   z_iFaceCenter(ni,njm1,nk),
   x_jFaceCenter(nim1,nj,nk), 
   y_jFaceCenter(nim1,nj,nk),
   z_jFaceCenter(nim1,nj,nk),
   x_kFaceCenter(nim1,njm1,nkp1),
   y_kFaceCenter(nim1,njm1,nkp1),
   z_kFaceCenter(nim1,njm1,nkp1),

   // This stuff is for the reverse interface

   di(nim1,njp1,nkp1),   // The following arrays are not needed for the parallel current calculation
   dj(ni,nj,nkp1),      // They are used for the potential electric field calculation
   dk(ni,njp1,nk),       // In the reverse interface. See the very bottom of this constructor

   // These nine quantities are identical to dx_i, dx_j, dx_k, etc. used in the current calculation
   // except for the fact that they are defined for the cells based on the vertices of the 
   // LFM grid, rather than centers of those cells. In other words, i_x is half-cell shifted 
   // version of dx_i. In addition, they are normalized, i.e. they are truly components of 
   // unit vectors i,j,k defining these directions in a cell
   i_x(nim1,nj,nk), i_y(nim1,nj,nk), i_z(nim1,nj,nk),
   j_x(nim1,nj,nk), j_y(nim1,nj,nk), j_z(nim1,nj,nk),
   k_x(nim1,nj,nk), k_y(nim1,nj,nk), k_z(nim1,nj,nk),

   // Physical variables for exchange with ionosphere  
   current(njp1,nkp1),
   density(njp1,nkp1),
   soundSpeed(njp1,nkp1),
   potential(2,njp1,nkp1)
{

  cout << "Initializing MHD Interface..." << endl;
  /*  Define the necessary ranges  */
  Range all, I(1,ni), Im1(1,nim1), Ip1(1,nip1), 
    J(1,nj), Jm1(1,njm1), Jp1(1,njp1), 
    K(1,nk), Kp1(1,nkp1);

  /* Set up the necessary arrays */
  volume.setBase(1);
  xCenter.setBase(1);
  yCenter.setBase(1); 
  zCenter.setBase(1);

  /* Find cell centers */
  //                        Arrays on the LHS have dimensions (I,J,Kp1) while on the RHS (I,J,K). 
  //                        Therefore, the indices on the RHS are set explicitly
  xCenter(I,J,K) = 0.125*( x(I,J,K) + x(I,J,K+1) + x(I+1,J,K+1) + x(I+1,J,K) +
		           x(I,J+1,K) + x(I,J+1,K+1) + x(I+1,J+1,K+1) + x(I+1,J+1,K) );
  yCenter(I,J,K) = 0.125*( y(I,J,K) + y(I,J,K+1) + y(I+1,J,K+1) + y(I+1,J,K) +
		           y(I,J+1,K) + y(I,J+1,K+1) + y(I+1,J+1,K+1) + y(I+1,J+1,K) );
  zCenter(I,J,K) = 0.125*( z(I,J,K) + z(I,J,K+1) + z(I+1,J,K+1) + z(I+1,J,K) +
		           z(I,J+1,K) + z(I,J+1,K+1) + z(I+1,J+1,K+1) + z(I+1,J+1,K) );

  /* Ensure periodicity */
  xCenter(all,all,nkp1) = xCenter(all,all,1);
  yCenter(all,all,nkp1) = yCenter(all,all,1);
  zCenter(all,all,nkp1) = zCenter(all,all,1);

  /* We calculate the volume of cells based on centers of the original cells. The cell volume is 
     calculated as the triple product of vectors connecting centers of corresponding faces. */
  
  /* Setup the necessary arrays */  
  x_iFaceCenter.setBase(1); 
  y_iFaceCenter.setBase(1);
  z_iFaceCenter.setBase(1); 
  x_jFaceCenter.setBase(1); 
  y_jFaceCenter.setBase(1); 
  z_jFaceCenter.setBase(1);
  x_kFaceCenter.setBase(1);
  y_kFaceCenter.setBase(1);
  z_kFaceCenter.setBase(1);

  /* Find cell-face centers. The index in k-direction spans the full range to ensure periodicity */
  //                         Arrays on the LHS and RHS are dimensioned the same way.
  //                         Therefore, indexing on the LHS is implicit
  x_iFaceCenter = 0.25*( xCenter(I,Jm1,K) + xCenter(I,Jm1,K+1) + xCenter(I,Jm1+1,K+1) + xCenter(I,Jm1+1,K) );
  y_iFaceCenter = 0.25*( yCenter(I,Jm1,K) + yCenter(I,Jm1,K+1) + yCenter(I,Jm1+1,K+1) + yCenter(I,Jm1+1,K) );
  z_iFaceCenter = 0.25*( zCenter(I,Jm1,K) + zCenter(I,Jm1,K+1) + zCenter(I,Jm1+1,K+1) + zCenter(I,Jm1+1,K) );

  x_jFaceCenter = 0.25*( xCenter(Im1,J,K) + xCenter(Im1,J,K+1) + xCenter(Im1+1,J,K+1) + xCenter(Im1+1,J,K) );
  y_jFaceCenter = 0.25*( yCenter(Im1,J,K) + yCenter(Im1,J,K+1) + yCenter(Im1+1,J,K+1) + yCenter(Im1+1,J,K) );
  z_jFaceCenter = 0.25*( zCenter(Im1,J,K) + zCenter(Im1,J,K+1) + zCenter(Im1+1,J,K+1) + zCenter(Im1+1,J,K) );

  x_kFaceCenter = 0.25*( xCenter(Im1,Jm1,Kp1) + xCenter(Im1,Jm1+1,Kp1) + xCenter(Im1+1,Jm1+1,Kp1) + xCenter(Im1+1,Jm1,Kp1) );
  y_kFaceCenter = 0.25*( yCenter(Im1,Jm1,Kp1) + yCenter(Im1,Jm1+1,Kp1) + yCenter(Im1+1,Jm1+1,Kp1) + yCenter(Im1+1,Jm1,Kp1) );
  z_kFaceCenter = 0.25*( zCenter(Im1,Jm1,Kp1) + zCenter(Im1,Jm1+1,Kp1) + zCenter(Im1+1,Jm1+1,Kp1) + zCenter(Im1+1,Jm1,Kp1) );

  /* Ensure periodicity in k */
  x_kFaceCenter(all,all,nkp1) = x_kFaceCenter(all,all,1);
  y_kFaceCenter(all,all,nkp1) = y_kFaceCenter(all,all,1);
  z_kFaceCenter(all,all,nkp1) = z_kFaceCenter(all,all,1);

  /* Calculate cell volume */
  doubleArray dx_i(nim1,njm1,nk), dx_j(nim1,njm1,nk), dx_k(nim1,njm1,nk),
              dy_i(nim1,njm1,nk), dy_j(nim1,njm1,nk), dy_k(nim1,njm1,nk),
              dz_i(nim1,njm1,nk), dz_j(nim1,njm1,nk), dz_k(nim1,njm1,nk);

  //                         Volume has Base 1, so we set Base for these arrays accordingly
  //                         Arrays on the LHS and RHS are dimensioned the same way.
  //                         Therefore, indexing on the LHS is implicit
  dx_i.setBase(1);  dy_i.setBase(1);  dz_i.setBase(1);
  dx_j.setBase(1);  dy_j.setBase(1);  dz_j.setBase(1);
  dx_k.setBase(1);  dy_k.setBase(1);  dz_k.setBase(1);

  dx_i = x_iFaceCenter(Im1+1,Jm1,K) - x_iFaceCenter(Im1,Jm1,K);
  dy_i = y_iFaceCenter(Im1+1,Jm1,K) - y_iFaceCenter(Im1,Jm1,K);
  dz_i = z_iFaceCenter(Im1+1,Jm1,K) - z_iFaceCenter(Im1,Jm1,K);

  dx_j = x_jFaceCenter(Im1,Jm1+1,K) - x_jFaceCenter(Im1,Jm1,K);
  dy_j = y_jFaceCenter(Im1,Jm1+1,K) - y_jFaceCenter(Im1,Jm1,K);
  dz_j = z_jFaceCenter(Im1,Jm1+1,K) - z_jFaceCenter(Im1,Jm1,K);

  dx_k = x_kFaceCenter(Im1,Jm1,K+1) - x_kFaceCenter(Im1,Jm1,K);
  dy_k = y_kFaceCenter(Im1,Jm1,K+1) - y_kFaceCenter(Im1,Jm1,K);
  dz_k = z_kFaceCenter(Im1,Jm1,K+1) - z_kFaceCenter(Im1,Jm1,K);

  /* Volume is a triple product */
  volume = abs(dx_i*(dy_j*dz_k - dy_k*dz_j) +
               dy_i*(dz_j*dx_k - dz_k*dx_j) +
	       dz_i*(dx_j*dy_k - dx_k*dy_j));

  /* 
     Now calculate the vector to dot the current with. The current
     density calculated by jpara should be dotted with B_ion/B_m (\vec
     B_m)/B_m in order to obtain the ionospheric current
     density. (\vec B_m)/B_m is the unit vector in the direction of
     the dipole field at the specified location calculated by separate
     functions bxNorm, byNorm, bzNorm. B_ion/B_m is calculated by the
     function bion2bmag. See jpara.tex for details.
  */
  bxDot.setBase(1);
  byDot.setBase(1);
  bzDot.setBase(1);

  /*
    The current is calculated on the staggered mesh, therefore it is defined at the i=2,i=3 
   and so on layers. For two innermost shells calculation (nip1+3), the current is defined 
   at the i=2 layer. Therefore, Im1+1 index.
  */
  doubleArray dummyX = x(Im1+1,Jp1,Kp1);  // Can't give x(Im1_1,Jp1,Kp1) to bxNorm directly. 
  doubleArray dummyY = y(Im1+1,Jp1,Kp1);  // Compiler barfs. So we use dummies.
  doubleArray dummyZ = z(Im1+1,Jp1,Kp1);

  dummyX.setBase(1);
  dummyY.setBase(1);
  dummyZ.setBase(1);

  bxDot = bxNorm(dummyX, dummyY, dummyZ)*bion2bmag(dummyX, dummyY, dummyZ);
  byDot = byNorm(dummyX, dummyY, dummyZ)*bion2bmag(dummyX, dummyY, dummyZ);
  bzDot = bzNorm(dummyX, dummyY, dummyZ)*bion2bmag(dummyX, dummyY, dummyZ);

  // Finally, we need to define di, dj, dk as well as i_x, i_y, i_z, etc. for 
  // the potential field calculation. di,dj,dk are lengths of edges of cells.
  // i_x, i_y, i_z, etc. are coordinates of vectors connecting the centers of the 
  // opposite cell faces.
  // See the reverse interface (the second overloaded version of "communicate"
  // and functions called from there

  di.setBase(1);
  dj.setBase(1);
  dk.setBase(1);

  di(Im1,Jp1,Kp1) = sqrt( pow( x(Im1+1,Jp1,Kp1) - x(Im1,Jp1,Kp1),2. ) + 
			  pow( y(Im1+1,Jp1,Kp1) - y(Im1,Jp1,Kp1),2. ) +
			  pow( z(Im1+1,Jp1,Kp1) - z(Im1,Jp1,Kp1),2. ) );
  dj(I,J,Kp1) = sqrt( pow( x(I,J+1,Kp1) - x(I,J,Kp1),2. ) + 
			  pow( y(I,J+1,Kp1) - y(I,J,Kp1),2. ) +
			  pow( z(I,J+1,Kp1) - z(I,J,Kp1),2. ) );
  dk(I,Jp1,K) = sqrt( pow( x(I,Jp1,K+1) - x(I,Jp1,K),2. ) + 
			  pow( y(I,Jp1,K+1) - y(I,Jp1,K),2. ) +
			  pow( z(I,Jp1,K+1) - z(I,Jp1,K),2. ) );

  i_x.setBase(1); i_y.setBase(1); i_z.setBase(1);
  j_x.setBase(1); j_y.setBase(1); j_z.setBase(1);
  k_x.setBase(1); k_y.setBase(1); k_z.setBase(1);
  
  i_x(Im1,J,K) = 0.25*( x(Im1+1,J,K) + x(Im1+1,J+1,K) + x(Im1+1,J,K+1) + x(Im1+1,J+1,K+1) -
			x(Im1,J,K) - x(Im1,J+1,K) - x(Im1,J,K+1) - x(Im1,J+1,K+1) );
  i_y(Im1,J,K) = 0.25*( y(Im1+1,J,K) + y(Im1+1,J+1,K) + y(Im1+1,J,K+1) + y(Im1+1,J+1,K+1) -
			y(Im1,J,K) - y(Im1,J+1,K) - y(Im1,J,K+1) - y(Im1,J+1,K+1) );
  i_z(Im1,J,K) = 0.25*( z(Im1+1,J,K) + z(Im1+1,J+1,K) + z(Im1+1,J,K+1) + z(Im1+1,J+1,K+1) -
			z(Im1,J,K) - z(Im1,J+1,K) - z(Im1,J,K+1) - z(Im1,J+1,K+1) );

  j_x(Im1,J,K) = 0.25*( x(Im1,J+1,K) + x(Im1+1,J+1,K) + x(Im1,J+1,K+1) + x(Im1+1,J+1,K+1) -
			x(Im1,J,K) - x(Im1+1,J,K) - x(Im1,J,K+1) - x(Im1+1,J,K+1) );
  j_y(Im1,J,K) = 0.25*( y(Im1,J+1,K) + y(Im1+1,J+1,K) + y(Im1,J+1,K+1) + y(Im1+1,J+1,K+1) -
			y(Im1,J,K) - y(Im1+1,J,K) - y(Im1,J,K+1) - y(Im1+1,J,K+1) );
  j_z(Im1,J,K) = 0.25*( z(Im1,J+1,K) + z(Im1+1,J+1,K) + z(Im1,J+1,K+1) + z(Im1+1,J+1,K+1) -
			z(Im1,J,K) - z(Im1+1,J,K) - z(Im1,J,K+1) - z(Im1+1,J,K+1) );

  k_x(Im1,J,K) = 0.25*( x(Im1,J,K+1) + x(Im1+1,J,K+1) + x(Im1,J+1,K+1) + x(Im1+1,J+1,K+1) - 
			x(Im1,J,K) - x(Im1+1,J,K) - x(Im1,J+1,K) - x(Im1+1,J+1,K) );
  k_y(Im1,J,K) = 0.25*( y(Im1,J,K+1) + y(Im1+1,J,K+1) + y(Im1,J+1,K+1) + y(Im1+1,J+1,K+1) - 
			y(Im1,J,K) - y(Im1+1,J,K) - y(Im1,J+1,K) - y(Im1+1,J+1,K) );
  k_z(Im1,J,K) = 0.25*( z(Im1,J,K+1) + z(Im1+1,J,K+1) + z(Im1,J+1,K+1) + z(Im1+1,J+1,K+1) - 
			z(Im1,J,K) - z(Im1+1,J,K) - z(Im1,J+1,K) - z(Im1+1,J+1,K) );

} 

/********************************************************************//**
 * \author Peter Schmitt (schmitt at ucar.edu)
 *
 * \param[out] eField_j   Electric field j-component at the i=1 edge
 * \param[out] eField_k   Electric field k-component at the i=1 edge
 *
 * \param[out] velocity Velocity (x,y,z components) at the center of
 * the first layer of cells
 *
 * Calculate the potential electric field & the velocity at the inner boundary
 *
 * getElectricField() calculates #eField_j and #eField_k at
 * the first shell (i=1) edges. 
 * 
 * #velocity is calculated by calling getVelocity() at the centers of
 * the first layer of cells (i=1).
 *
 * \note processImport should only be called within a derived class'
 * Import(...) method.
 ************************************************************************/
void MHDInnerBoundaryInterface::processImport(doubleArray & eField_j, doubleArray & eField_k, doubleArray & velocity)
{
  doubleArray eField_i; // This one is not needed in the calling function, so define it here
  //  doubleArray eField_i(1,njp1,nkp1); // This one is not needed in the calling function, so define it here
  getElectricField(eField_i, eField_j, eField_k);   // This fills in the values in the class-wide variable "potential"

  velocity = getVelocity(eField_i, eField_j, eField_k);

  // FOR LFM ONLY output -delta(Potential)*1.e8 instead of electric field
  eField_j = -eField_j*dj*RION*1.e6;
  eField_k = -eField_k*dk*RION*1.e6;
}

/********************************************************************//**
 * \author Peter Schmitt (schmitt at ucar.edu)
 *
 * \param[in] bx MHD magnetic field x-component
 * \param[in] by MHD magnetic field y-component
 * \param[in] bz MHD magnetic field z-component
 * \param[in] rho MHD density
 * \param[in] cs MHD sound speed
 *
 * Calculate the FAC, density and sound speed in the second-shell
 *
 * This function calls getParallelCurrent() and
 * smoothParallelCurrent() to get the FAC, then getVarSecondShell()
 * gives the density and the sound speed.
 *
 * \note prepareExport should only be called within a derived class'
 * Export(...) method.
 ************************************************************************/
void MHDInnerBoundaryInterface::prepareExport(const doubleArray &bx, const doubleArray &by, const doubleArray &bz, 
					      const doubleArray &rho, const doubleArray &cs)
{
  // These two functions fill in values in the class-wide variable "current"
  getParallelCurrent(bx,by,bz);
  smoothParallelCurrent();

  density = getVarSecondShell(rho);
  soundSpeed = getVarSecondShell(cs);
}

/********************************************************************//**
 * \author Slava Merkin (vgm at bu.edu)
 * \since 05-2005 
 * \param[in] Bx     Magnetic field x-component in Gs
 * \param[in] By     Magnetic field y-component in Gs
 * \param[in] Bz     Magnetic field z-component in Gs
 *
 * \retval current Fills in the values in the class-wide array #current.
 *
 * See jpara.tex for details of this calculation.  It assumes a
 * logically spherical grid in the LFM sense, so that i is in the
 * radial direction, j is in the polar direction (in the LFM the polar
 * axis coincides with the SM x-axis), and k - azimuthal direction The
 * grid is periodic in k.
 ************************************************************************/
void MHDInnerBoundaryInterface::getParallelCurrent(const doubleArray &Bx, 
						   const doubleArray &By, 
						   const doubleArray &Bz)
{
  /*  Define the necessary ranges  */
  Range all, I(1,ni), Im1(1,nim1), Ip1(1,nip1), J(1,nj), Jm1(1,njm1), Jp1(1,njp1), K(1,nk), Kp1(1,nkp1), Km1(1,nkm1);

  /*  ----  Calculate the line integrals of b between cell centers  ----  */
  doubleArray binti(nim1,nj,nkp1), bintj(ni,njm1,nkp1), bintk(ni,nj,nk);
  binti.setBase(1); bintj.setBase(1); bintk.setBase(1);

  //                      Magnetic field is defined at centers of cells Bx(ni,nj,nk).
  //                      Arrays on the LHS have dimensions (Im1,J,Kp1) while on the RHS (Im1,J,K)
  //                                                        (I,Jm1,Kp1)                  (I,Jm1,K)
  //                                                        (I,J,K)                    (I,J,Km1)
  //                      Therefore, explicit indexing on the LHS.
  
  binti(Im1,J,K) = 0.5*( (Bx(Im1,J,K) + Bx(Im1+1,J,K))*(xCenter(Im1+1,J,K) - xCenter(Im1,J,K)) +
	                 (By(Im1,J,K) + By(Im1+1,J,K))*(yCenter(Im1+1,J,K) - yCenter(Im1,J,K)) +
     		         (Bz(Im1,J,K) + Bz(Im1+1,J,K))*(zCenter(Im1+1,J,K) - zCenter(Im1,J,K)) );
  binti(Im1,J,nkp1) = binti(Im1,J,1);

  bintj(I,Jm1,K) = 0.5*( (Bx(I,Jm1,K) + Bx(I,Jm1+1,K))*(xCenter(I,Jm1+1,K) - xCenter(I,Jm1,K)) +
		         (By(I,Jm1,K) + By(I,Jm1+1,K))*(yCenter(I,Jm1+1,K) - yCenter(I,Jm1,K)) +
		         (Bz(I,Jm1,K) + Bz(I,Jm1+1,K))*(zCenter(I,Jm1+1,K) - zCenter(I,Jm1,K)) );
  bintj(I,Jm1,nkp1) = bintj(I,Jm1,1);

  bintk(I,J,Km1) = 0.5*( (Bx(I,J,Km1) + Bx(I,J,Km1+1))*(xCenter(I,J,Km1+1) - xCenter(I,J,Km1)) +
		         (By(I,J,Km1) + By(I,J,Km1+1))*(yCenter(I,J,Km1+1) - yCenter(I,J,Km1)) +
		         (Bz(I,J,Km1) + Bz(I,J,Km1+1))*(zCenter(I,J,Km1+1) - zCenter(I,J,Km1)) );
  bintk(I,J,nk)  = 0.5*( (Bx(I,J,nk) + Bx(I,J,1))*(xCenter(I,J,1) - xCenter(I,J,nk)) +
		         (By(I,J,nk) + By(I,J,1))*(yCenter(I,J,1) - yCenter(I,J,nk)) +
		         (Bz(I,J,nk) + Bz(I,J,1))*(zCenter(I,J,1) - zCenter(I,J,nk)) );

  /*  -----------------------------------------------------------------  */

  /*  ------------------- Find area integrals of j  -------------------  */
  doubleArray ji(ni,njm1,nk), jj(nim1,nj,nk), jk(nim1,njm1,nkp1);
  ji.setBase(1); jj.setBase(1); jk.setBase(1);

  jk = binti(Im1,Jm1,Kp1) + bintj(Im1+1,Jm1,Kp1) - binti(Im1,Jm1+1,Kp1) - bintj(Im1,Jm1,Kp1);

  ji = bintj(I,Jm1,K) + bintk(I,Jm1+1,K) - bintj(I,Jm1,K+1) - bintk(I,Jm1,K);

  jj = bintk(Im1,J,K) + binti(Im1,J,K+1) - bintk(Im1+1,J,K) - binti(Im1,J,K);
  /*  -----------------------------------------------------------------  */

  /*  -------------- Find the value of j over cell volume  -----------------------  */
  doubleArray jx(nim1,njp1,nkp1), jy(nim1,njp1,nkp1), jz(nim1,njp1,nkp1);
  jx.setBase(1); jy.setBase(1); jz.setBase(1);

  // These loops represent Eq. (8) in jpara.tex
  // Do the i-Faces
  jx(Im1,Jm1+1,K+1) = x_iFaceCenter(Im1+1,Jm1,K)*ji(Im1+1,Jm1,K) - 
                      x_iFaceCenter(Im1  ,Jm1,K)*ji(Im1  ,Jm1,K);
  jy(Im1,Jm1+1,K+1) = y_iFaceCenter(Im1+1,Jm1,K)*ji(Im1+1,Jm1,K) - 
                      y_iFaceCenter(Im1  ,Jm1,K)*ji(Im1  ,Jm1,K);
  jz(Im1,Jm1+1,K+1) = z_iFaceCenter(Im1+1,Jm1,K)*ji(Im1+1,Jm1,K) - 
                      z_iFaceCenter(Im1  ,Jm1,K)*ji(Im1  ,Jm1,K);
				
  // Do the j-Faces
  jx(Im1,Jm1+1,K+1) = x_jFaceCenter(Im1,Jm1+1,K)*jj(Im1,Jm1+1,K) - 
                      x_jFaceCenter(Im1,Jm1  ,K)*jj(Im1,Jm1, K) + 
                      jx(Im1,Jm1+1,K+1);
  jy(Im1,Jm1+1,K+1) = y_jFaceCenter(Im1,Jm1+1,K)*jj(Im1,Jm1+1,K) - 
                      y_jFaceCenter(Im1,Jm1  ,K)*jj(Im1,Jm1  ,K) +
                      jy(Im1,Jm1+1,K+1);
  jz(Im1,Jm1+1,K+1) = z_jFaceCenter(Im1,Jm1+1,K)*jj(Im1,Jm1+1,K) - 
                      z_jFaceCenter(Im1,Jm1  ,K)*jj(Im1,Jm1  ,K) +
                      jz(Im1,Jm1+1,K+1);
  // Do the k-Faces
  jx(Im1,Jm1+1,K+1) = x_kFaceCenter(Im1,Jm1,K+1)*jk(Im1,Jm1,K+1) - 
                      x_kFaceCenter(Im1,Jm1,K  )*jk(Im1,Jm1,K  ) +
                      jx(Im1,Jm1+1,K+1);
  jy(Im1,Jm1+1,K+1) = y_kFaceCenter(Im1,Jm1,K+1)*jk(Im1,Jm1,K+1) - 
                      y_kFaceCenter(Im1,Jm1,K  )*jk(Im1,Jm1,K  ) +
                      jy(Im1,Jm1+1,K+1);
  jz(Im1,Jm1+1,K+1) = z_kFaceCenter(Im1,Jm1,K+1)*jk(Im1,Jm1,K+1) - 
                      z_kFaceCenter(Im1,Jm1,K  )*jk(Im1,Jm1,K  ) +
                      jz(Im1,Jm1+1,K+1);
  /* 
     These loops are commented out here. They are different from the above in that 
     x(i+1)*I(i+1) - x(i)*I(i) is approximated as <I>*( x(i+1) - x(i) ). This is the way
     it's done in the legacy LFM code. Both ways give the same answer.

  // Do the j-Faces
  jx(Im1,Jm1+1,K+1) = 0.5*(ji(Im1+1,Jm1,K) + ji(Im1,Jm1,K))*(x_iFaceCenter(Im1+1,Jm1,K) - x_iFaceCenter(Im1,Jm1,K));
  jy(Im1,Jm1+1,K+1) = 0.5*(ji(Im1+1,Jm1,K) + ji(Im1,Jm1,K))*(y_iFaceCenter(Im1+1,Jm1,K) - y_iFaceCenter(Im1,Jm1,K));
  jz(Im1,Jm1+1,K+1) = 0.5*(ji(Im1+1,Jm1,K) + ji(Im1,Jm1,K))*(z_iFaceCenter(Im1+1,Jm1,K) - z_iFaceCenter(Im1,Jm1,K));
				
  // Do the j-Faces
  jx(Im1,Jm1+1,K+1) = 0.5*(jj(Im1,Jm1+1,K) + jj(Im1,Jm1,K))*(x_jFaceCenter(Im1,Jm1+1,K) - x_jFaceCenter(Im1,Jm1,K))
                    + jx(Im1,Jm1+1,K+1);;
  jy(Im1,Jm1+1,K+1) = 0.5*(jj(Im1,Jm1+1,K) + jj(Im1,Jm1,K))*(y_jFaceCenter(Im1,Jm1+1,K) - y_jFaceCenter(Im1,Jm1,K))
                    + jy(Im1,Jm1+1,K+1);
  jz(Im1,Jm1+1,K+1) = 0.5*(jj(Im1,Jm1+1,K) + jj(Im1,Jm1,K))*(z_jFaceCenter(Im1,Jm1+1,K) - z_jFaceCenter(Im1,Jm1,K))
                    + jz(Im1,Jm1+1,K+1);
  // Do the k-Faces
  jx(Im1,Jm1+1,K+1) = 0.5*(jk(Im1,Jm1,K+1) + jk(Im1,Jm1,K))*(x_kFaceCenter(Im1,Jm1,K+1) - x_kFaceCenter(Im1,Jm1,K))
                    + jx(Im1,Jm1+1,K+1);
  jy(Im1,Jm1+1,K+1) = 0.5*(jk(Im1,Jm1,K+1) + jk(Im1,Jm1,K))*(y_kFaceCenter(Im1,Jm1,K+1) - y_kFaceCenter(Im1,Jm1,K))
                    + jy(Im1,Jm1+1,K+1);
  jz(Im1,Jm1+1,K+1) = 0.5*(jk(Im1,Jm1,K+1) + jk(Im1,Jm1,K))*(z_kFaceCenter(Im1,Jm1,K+1) - z_kFaceCenter(Im1,Jm1,K))
                    + jz(Im1,Jm1+1,K+1);
  */
  // Normalize to volume
  jx(Im1,Jm1+1,K+1) = jx(Im1,Jm1+1,K+1)/volume(Im1,Jm1,K);
  jy(Im1,Jm1+1,K+1) = jy(Im1,Jm1+1,K+1)/volume(Im1,Jm1,K);
  jz(Im1,Jm1+1,K+1) = jz(Im1,Jm1+1,K+1)/volume(Im1,Jm1,K);

  // Set the current at the j boundaries to zero
  jx(Im1,1,Kp1) = 0.;
  jy(Im1,1,Kp1) = 0.; 
  jz(Im1,1,Kp1) = 0.;
  jx(Im1,njp1,Kp1) = 0.; 
  jy(Im1,njp1,Kp1) = 0.; 
  jz(Im1,njp1,Kp1) = 0.;

  // Ensure periodicity at the k boundaries
  jx(Im1,Jp1,1) = jx(Im1,Jp1,nkp1); 
  jy(Im1,Jp1,1) = jy(Im1,Jp1,nkp1); 
  jz(Im1,Jp1,1) = jz(Im1,Jp1,nkp1);

  /*  
      Now dot the j's with the right constant vector to get
      ionospheric field aligned current.  
      A++/P++ does not understand an assignmet like this: 
      B=A(1,*,*) where B(M,K) and A(N,M,K),
      because the orientation of arrays in the index space matters.
      Thus, we have to use a loop to assign the current values.
  */

  // USE RESHAPE BELOW EVENTUALLY

  // Current has base 0, so watch indexing below
  for (int j = 1; j < njp1+1; j++) {
    for (int k = 1; k < nkp1+1; k++) {
      current(j-1,k-1) = bxDot(1,j,k)*jx(1,j,k) +
	byDot(1,j,k)*jy(1,j,k) +
	bzDot(1,j,k)*jz(1,j,k);
   }
 }


//   doubleArray current(1,njp1,nkp1);
//   current.setBase(1);
//   current = bxDot*jx + byDot*jy + bzDot*jz;
//   current.reshape(njp1,nkp1);
 
 /*  
      The current is in [Gs/ionosphere radius] inits.
      Convert to MKS:
         [Gs]       1.e-4 [T]
      ---------- = ----------------
      [6.5e8 cm]    6.5e8 1.e-2 [m]
      and divide by mu_0 = 4PI/1.e7 [H/m] to get the MKS current density:
      Conversion factor = 10^5/4pi/6.5e8  
*/

  current = 1.e5/(4*PI)/RION*current;

#ifdef DEBUG
  Range io_pe(0,0); Partitioning_Type io(io_pe);
  current.partition(io);
  xCenter.partition(io);
  yCenter.partition(io);
  zCenter.partition(io);
  ofstream file("current.dat",ios::out);
  file << "object 1 class array type float rank 0 items 3120 data follows\n";
  for (int k = 1; k < nkp1+1; k++) {
    for (int j = 1; j < nj+1; j++) {
      file << current(j-1,k-1)<<"\n";
    }
  }
  file << "attribute \"dep\" string \"positions\"\n";
  file << "#\n";
  file <<"object 2 class array type float rank 1 shape 3 items 3120 data follows\n";
  for (int k = 1; k < nkp1+1; k++) {
    for (int j = 1; j < nj+1; j++) {
      file <<  xCenter(1,j,k) << "\t" << yCenter(1,j,k)<< "\t" << zCenter(1,j,k)<<"\n" ;
    }
  }
  file << "\nattribute \"dep\" string \"positions\"\n#\n";
  file << "object 3 class gridconnections counts 65 48\n";
  file << "attribute \"element type\" string \"quads\"\n";
  file << "attribute \"dep\" string \"connections\"\n";
  file << "attribute \"ref\" string \"positions\"\n#\n";
  file << "object \"default\" class field\n";
  file << "component \"data\" value 1\n";
  file << "component \"positions\" value 2\n";
  file << "component \"connections\" value 3\n";
  file << "#\nend\n";
  file.close();
  exit(1);
#endif //DEBUG
}

/********************************************************************//**
 * \author Slava Merkin (vgm at bu.edu)
 * \since 05-2005 
 *
 * \param[in] x Coordinates of points where to do the calculation in
 * ionospheric radii
 *
 * \param[in] y
 * \param[in] z
 *
 * \return The x-component of the unit vector in the direction of the
 * dipole field at the specified locations
 ************************************************************************/
doubleArray MHDInnerBoundaryInterface::bxNorm(const doubleArray &x, const doubleArray &y, const doubleArray &z)  {
  doubleArray dummy(x);  // This array will hold the output of the function.
                         // Dimension it the same way, the coordinates are dimensioned
  doubleArray r = sqrt(x*x + y*y + z*z);
  doubleArray l = sqrt(1. + 3.*z*z/(r*r)); // This is the norm of the magnetic field 
                                           // (without the magnetic moment)

  dummy = -3.*x*z/(l*r*r);
  return dummy;
}

/********************************************************************//**
 * \author Slava Merkin (vgm at bu.edu)
 * \since 05-2005 
 *
 * \param[in] x Coordinates of points where to do the calculation in
 * ionospheric radii
 *
 * \param[in] y
 * \param[in] z
 *
 * \return The y-component of the unit vector in the direction of the
 * dipole field at the specified locations
 ************************************************************************/
doubleArray MHDInnerBoundaryInterface::byNorm(const doubleArray &x, const doubleArray &y, const doubleArray &z)  {
  doubleArray dummy(x);  // This array will hold the output of the function.
                         // Dimension it the same way, the coordinates are dimensioned
  doubleArray r = sqrt(x*x + y*y + z*z);
  doubleArray l = sqrt(1. + 3.*z*z/(r*r)); // This is the norm of the magnetic field 
                                           // (without the magnetic moment)

  dummy = -3.*y*z/(l*r*r);
  return dummy;
}

/********************************************************************//**
 * \author Slava Merkin (vgm at bu.edu)
 * \since 05-2005 
 *
 * \param[in] x Coordinates of points where to do the calculation in
 * ionospheric radii
 *
 * \param[in] y
 * \param[in] z
 *
 * \return The z-component of the unit vector in the direction of the
 * dipole field at the specified locations
 ************************************************************************/
doubleArray MHDInnerBoundaryInterface::bzNorm(const doubleArray &x, const doubleArray &y, const doubleArray &z)  {
  doubleArray dummy(x);  // This array will hold the output of the function.
                         // Dimension it the same way, the coordinates are dimensioned
  doubleArray r = sqrt(x*x + y*y + z*z);
  doubleArray l = sqrt(1. + 3.*z*z/(r*r)); // This is the norm of the magnetic field 
                                           // (without the magnetic moment)

  dummy = ( -2. + 3.*(x*x + y*y)/(r*r) )/l;
  return dummy;
}

/********************************************************************//**
 * \author Slava Merkin (vgm at bu.edu)
 * \since 05-2005 
 *
 * \param[in] x Coordinates of points where to do the calculation in
 * ionospheric radii
 *
 * \param[in] y
 * \param[in] z
 *
 * \return The magnitude of the dipole magnetic field in T at the
 * specified locations
 ************************************************************************/
doubleArray MHDInnerBoundaryInterface::bMag(const doubleArray & x, const doubleArray & y, const doubleArray & z)
{
  // Dipole magnetic field in Tesla
  doubleArray dummy(x);
  doubleArray r = sqrt(x*x + y*y + z*z);   // in Rion
  dummy = BEQUATOR*pow(REARTH/(r*RION),3.)*sqrt(1. + 3.*z*z/(r*r));
  return dummy;
}

/********************************************************************//**
 * \author Slava Merkin (vgm at bu.edu)
 * \since 05-2005 
 *
 * \param[in] x Coordinates of points where to do the calculation in
 * ionospheric radii
 *
 * \param[in] y
 * \param[in] z
 *
 * \return The ratio of the dipole magnetic field at the point (x,y,z)
 * to its conjugate lying on the same field line in the ionosphere.
 ************************************************************************/
doubleArray MHDInnerBoundaryInterface::bion2bmag(const doubleArray &x, const doubleArray &y, const doubleArray &z)  {
  doubleArray dummy(x);  // This array will hold the output of the function.
                         // Dimension it the same way, the coordinates are dimensioned
  doubleArray r = sqrt(x*x + y*y + z*z);
  doubleArray l = sqrt(1. + 3.*z*z/(r*r));
  
  dummy = r*r*r*sqrt(1. + 3.*( 1. - (1. - z*z/(r*r))/r ) )/l;
  return dummy;
}

/********************************************************************//**
 * \author Slava Merkin (vgm at bu.edu)
 * \since 05-2005 
 *
 * \param[in] none The function works on the #current array which is
 * declared class-wide
 *
 * The function is used to smooth the field-aligned current coming out
 * of getParallelCurrent(). The smoothing is done using the standard
 * 9-point stencil:
 *
 * 1---2---3
 * |   |   |
 * 4---5---6
 * |   |   |
 * 7---8---9
 *
 * The smoothed current at the center (point 5) is calculated as the sum of 
 * 1) 1/4 I5
 * 2) 1/2 average(i2,i4,i6,i8)
 * 3) 1/4 average(i1,i3,i7,i9)
 *
 ************************************************************************/
void MHDInnerBoundaryInterface::smoothParallelCurrent()  {
  Range J(1,nj-1), K(1,nk-2);
  doubleArray dummy(current);  // This array will hold the output of the function.
                               // Dimension it the same way
  dummy = current;
  dummy(J,K) = 0.25*current(J,K) 
                 + 0.125*( current(J+1,K) + current(J,K+1) + current(J-1,K) + current(J,K-1))
                 + 0.0625*( current(J-1,K-1) + current(J-1,K+1) + current(J+1,K-1) +current(J+1,K+1));



  // EXPERIMENTAL!!!

  // Let's fix the current at the nose/tail (cf. getElectricField)
  int nZero = 2;

  Range all, indexZeroNose(0,nZero-1), indexZeroTail(nj-nZero+1, nj);
  dummy(indexZeroNose,all) = dummy(indexZeroTail,all) = 0.;

  // Also at the sides
  Range indexZeroSide1a(0,nZero),indexZeroSide1b(nk-nZero,nk),
    indexZeroSide2(nk/2-nZero,nk/2+nZero);
  dummy(all,indexZeroSide1a) = 0.;
  dummy(all,indexZeroSide1b) = 0.;
  dummy(all,indexZeroSide2) = 0.;

  current = dummy;
}

/********************************************************************//**
 * \author Slava Merkin (vgm at bu.edu)
 * \since 05-2005 
 *
 * \param[in] var   The variable to process.
 *
 * \return          The input variable defined at the vertices of the 2nd shell
 *
 * Get the density or sound speed (anything else?) at the vertices of
 * the second MHD shell. The LFM density and sound speed are
 * cell-centered. To prepare them to be sent to the ionospheric code,
 * they should be defined at the vertices of the second shell just
 * like the FAC coming out of getParallelCurrent(). An 8-point
 * averaging is used.
 ************************************************************************/
doubleArray MHDInnerBoundaryInterface::getVarSecondShell(const doubleArray & var)
{
  doubleArray dummy(1,njp1,nkp1);
  dummy.setBase(1);
  Range J(2,nj), K(2,nk), all;

  dummy(1,J,K) = 0.125*( var(1,J,K) + var(1,J,K-1) + var(1,J-1,K) + var(1,J-1,K-1)
			 + var(2,J,K) + var(2,J,K-1) + var(2,J-1,K) + var(2,J-1,K-1) );
  dummy(1,1,all) = dummy(1,2,all);
  dummy(1,njp1,all) = dummy(1,nj,all);

  dummy(1,all,1) = dummy(1,all,2);           // This is not really necessary because
  dummy(1,all,nkp1) = dummy(1,all,nk);     // only dummy(all,2:nk) participate in mapping to ionosphere

  // DO RESHAPE HERE AS  COMMENTED OUT
  //  dummy.reshape(njp1,nkp1);

  doubleArray dummy1(njp1,nkp1);
  dummy1.setBase(1);
  for (int j = 1; j < njp1+1; j++) {
    for (int k = 1; k < nkp1+1; k++) {
      dummy1(j,k) = dummy(1,j,k);
   }
 }
  dummy1.setBase(0);
  return dummy1;
}

/********************************************************************//**
 * \author Slava Merkin (vgm at bu.edu)
 * \since 05-2005 
 *
 * \param[out] eField_i Electric field i-component at the edge in the
 * i-th direction of the i=1 layer of cells
 *
 * \param[out] eField_j Electric field j-component at the i=1 edge
 * \param[out] eField_k Electric field k-component at the i=1 edge
 *
 * The function takes the class-wide variable #potential and
 * calculates electric field along the edges of the first layer of
 * cells.
 ************************************************************************/
void MHDInnerBoundaryInterface::getElectricField(doubleArray & eField_i, 
				 doubleArray & eField_j, 
				 doubleArray & eField_k)
{
  Range J(1,nj), Jp1(1,njp1), J2nj(2,nj), K(1,nk), Kp1(1,nkp1), ONE(1,1), TWO(1,2);

  // Electric field along the edges of cells in the first layer
  eField_i.redim(1,njp1,nkp1);
  eField_j.redim(2,nj,nkp1);
  eField_k.redim(2,njp1,nk);

  eField_i.setBase(1);
  eField_j.setBase(1);
  eField_k.setBase(1);

  // Note, potential has base=0. Other arrays have bases 1.
  eField_i(ONE,Jp1,Kp1) = - ( potential(1,Jp1-1,Kp1-1) - potential(0,Jp1-1,Kp1-1) )/di/RION*1.e2;  // in [V/m]
  eField_j(TWO,J,Kp1) = - ( potential(TWO-1,J ,Kp1-1) - potential(TWO-1,J-1,Kp1-1) )/dj/RION*1.e2;  // in [V/m]

  // We know that dk is 0 for j=1 and j=njp1. gcc compiler supports
  // NaNs, so that we can get away by applying the following code at all
  // points (this results in getting NaN values at the nose and
  // tail. Just to be on the safe side, however, to avoid possible
  // compiler issues in the future, let's not get to the point of
  // getting NaNs and do it more carefully: Apply the code below to
  // points from j=2 to j=nj, and then set electric field to zero at
  // j=1 and j=njp1.
  eField_k(TWO,J2nj,K) = - ( potential(TWO-1,J2nj-1,K) - potential(TWO-1,J2nj-1,K-1) )/dk(TWO,J2nj,K)/RION*1.e2;  // in [V/m]

  // Now set el. field to 0 at nZero edges at the nose and tail
  int nZero = 2;
  Range indexZeroNose(1,nZero), indexZeroTail(njp1-nZero+1, njp1);
  eField_i(ONE,indexZeroNose,Kp1) = eField_i(ONE,indexZeroTail,Kp1) = 0.;
  //  eField_j(TWO,indexZeroNose,Kp1) = eField_j(TWO,indexZeroTail,Kp1) = 0.;
  // The above is not correct because eField_j goes to nj, not to njp1. Therefore, indexZeroTail-1!
  eField_j(TWO,indexZeroNose,Kp1) = eField_j(TWO,indexZeroTail-1,Kp1) = 0.;
  eField_k(TWO,indexZeroNose,K) = eField_k(TWO,indexZeroTail,K) = 0.;
}

/********************************************************************//**
 * \author Slava Merkin (vgm at bu.edu)
 * \since 05-2005 
 *
 * \param[in] eField_i Electric field i-component at the edge in the
 * i-th direction of the i=1 layer of cells
 *
 * \param[in] eField_j Electric field j-component at the i=1 edge
 * \param[in] eField_k Electric field k-component at the i=1 edge
 *
 * The function takes the class-wide variable #potential and
 * calculates electric field along the edges of the first layer of
 * cells.
 ************************************************************************/
doubleArray MHDInnerBoundaryInterface::getVelocity(const doubleArray & eField_i, 
				   const doubleArray & eField_j,
				   const doubleArray & eField_k)
{
  Range J(1,nj), K(1,nk), all;
  doubleArray eFieldCenter_i(1,nj,nk), eFieldCenter_j(1,nj,nk), eFieldCenter_k(1,nj,nk);

  eFieldCenter_i.setBase(1);
  eFieldCenter_j.setBase(1);
  eFieldCenter_k.setBase(1);

  eFieldCenter_i(1,J,K) = 0.25*( eField_i(1,J,K) + eField_i(1,J+1,K) + 
			       eField_i(1,J,K+1) + eField_i(1,J+1,K+1) );
  eFieldCenter_j(1,J,K) = 0.25*( eField_j(1,J,K) + eField_j(1,J,K+1) +
				 eField_j(2,J,K) + eField_j(2,J,K+1) );
  eFieldCenter_k(1,J,K) = 0.25*( eField_k(1,J,K) + eField_k(1,J+1,K) +
				 eField_k(2,J,K) + eField_k(2,J+1,K) );

  // Here is how the (x,y,z) components of the electric field are recovered from (i,j,k) components:
  // We have a system of linear equations:
  //                                e_i = e_x*i_x + e_y*i_y + e_z*i_z
  //                                e_j = e_x*j_x + e_y*j_y + e_z*j_z                (1)
  //                                e_k = e_x*k_x + e_y*k_y + e_z*k_z
  //
  // where e_i, e_j, e_k - components of the electric field in the (i,j,k) basis, while
  // e_x,e_y,e_z - el. field in (x,y,z) basis. Correspondingly, i_x,i_y,i_z etc. are components 
  // of the (i,j,k) in (x,y,z) basis.
  //
  // The above system has a unique solution if for every cell no 5 vertices are coplanar, i.e.
  // the cells are just distorted cubes.
  // The solution is given by:
  //                                   e_x = ( e dot (ijk_y cross ijk_z) )/ det T
  //                                   e_y = ( ijk_x dot (e cross ijk_z) )/ det T
  //                                    e_z = (ijk_x dot (ijk_y cross e) )/ det T
  //
  // where T is the matrix determining (1) and det T - its determinant
  // ijk_x = (i_x, j_x, k_x)
  // ijk_y = (i_y, j_y, k_y)
  // ijk_z = (i_z, j_z, k_z)
  // e = (e_i, e_j, e_k)

  doubleArray e_ijk(1,nj,nk,3), ijk_x(1,nj,nk,3), ijk_y(1,nj,nk,3), ijk_z(1,nj,nk,3);

  e_ijk.setBase(1);  
  ijk_x.setBase(1);
  ijk_y.setBase(1);
  ijk_z.setBase(1);

  e_ijk(all,all,all,1) = eFieldCenter_i;
  e_ijk(all,all,all,2) = eFieldCenter_j;
  e_ijk(all,all,all,3) = eFieldCenter_k;

  ijk_x(all,all,all,1) = i_x/sqrt(i_x*i_x + i_y*i_y + i_z*i_z);
  ijk_x(all,all,all,2) = j_x/sqrt(j_x*j_x + j_y*j_y + j_z*j_z);
  ijk_x(all,all,all,3) = k_x/sqrt(k_x*k_x + k_y*k_y + k_z*k_z);

  ijk_y(all,all,all,1) = i_y/sqrt(i_x*i_x + i_y*i_y + i_z*i_z);
  ijk_y(all,all,all,2) = j_y/sqrt(j_x*j_x + j_y*j_y + j_z*j_z);
  ijk_y(all,all,all,3) = k_y/sqrt(k_x*k_x + k_y*k_y + k_z*k_z);

  ijk_z(all,all,all,1) = i_z/sqrt(i_x*i_x + i_y*i_y + i_z*i_z);
  ijk_z(all,all,all,2) = j_z/sqrt(j_x*j_x + j_y*j_y + j_z*j_z);
  ijk_z(all,all,all,3) = k_z/sqrt(k_x*k_x + k_y*k_y + k_z*k_z);

  doubleArray eFieldCenter(1,nj,nk,3);
  eFieldCenter.setBase(1);

  eFieldCenter(all,all,all,1) = triple(e_ijk, ijk_y, ijk_z)/triple(ijk_x,ijk_y,ijk_z);
  eFieldCenter(all,all,all,2) = triple(ijk_x,e_ijk,ijk_z)/triple(ijk_x,ijk_y,ijk_z);
  eFieldCenter(all,all,all,3) = triple(ijk_x,ijk_y,e_ijk)/triple(ijk_x,ijk_y,ijk_z);

  // Now magnetic field

  // create some aliases
  // note, that xyzCenter are dimensioned (I,J,Kp1), where the point nkp1 coincides with 1
  // this setup is needed for the jpara calculation, but is redundant here
  // therefore the below arrrays are dimensioned (1,J,K)
  const doubleArray & x = xCenter(1,J,K);
  const doubleArray & y = yCenter(1,J,K);
  const doubleArray & z = zCenter(1,J,K);

  doubleArray bDipole(1,nj,nk,3);
  bDipole.setBase(1);

  bDipole(all,all,all,1) = bMag(x,y,z)*bxNorm(x,y,z);   // in [Tesla]
  bDipole(all,all,all,2) = bMag(x,y,z)*byNorm(x,y,z);   // in [Tesla]
  bDipole(all,all,all,3) = bMag(x,y,z)*bzNorm(x,y,z);   // in [Tesla]

  // Finally return velocity in cm/s
  doubleArray velocity(1,nj,nk,3);
  velocity.setBase(1);

  // Here bDipole is given in [Tesla], eFieldCenter - in [V/m] and we want velocity in [cm/s]
  // the conversion factor is 10^2[cm/m]
  velocity = cross(eFieldCenter,bDipole);   // The cross product has the 4 dimensions

  // P++ barfs if we multiply 4-dimensional array velocity
  // by the 3-dimensional array bMag (not as smart as IDL)
  for (int axis = 1; axis < 4; axis++) {
    velocity(all,all,all,axis) = 1.e2*velocity(all,all,all,axis)/bMag(x,y,z)/bMag(x,y,z); 
  }
  return velocity;
}

/********************************************************************//**
 * \author Slava Merkin (vgm at bu.edu)
 * \since 05-2005 
 *
 * \param[in] a Vector
 * \param[in] b Vector
 * \param[in] c Vector
 *
 * \return The triple product of input parameters
 *
 * \attention Accept only three-dimensional arrays
 ************************************************************************/
doubleArray MHDInnerBoundaryInterface::triple(const doubleArray & a, const doubleArray & b, const doubleArray & c)
{
  doubleArray dummy = cross(b,c);
  return dot( a, dummy );
}

/********************************************************************//**
 * \author Slava Merkin (vgm at bu.edu)
 * \since 05-2005 
 *
 * \param[in] a Vector
 * \param[in] b Vector
 *
 * \return The cross product of input parameters
 ************************************************************************/
doubleArray MHDInnerBoundaryInterface::cross(const doubleArray & a, const doubleArray & b)
{
  doubleArray c(a);
  Range all;
  int aBase = a.getBase();   // This is done so that we don't have to care about 
  int bBase = b.getBase();   // possibly different bases in the parameter arrays
  c(all,all,all,aBase+0) = a(all,all,all,aBase+1)*b(all,all,all,bBase+2) - a(all,all,all,aBase+2)*b(all,all,all,bBase+1);
  c(all,all,all,aBase+1) = a(all,all,all,aBase+2)*b(all,all,all,bBase+0) - a(all,all,all,aBase+0)*b(all,all,all,bBase+2);
  c(all,all,all,aBase+2) = a(all,all,all,aBase+0)*b(all,all,all,bBase+1) - a(all,all,all,aBase+1)*b(all,all,all,bBase+0);

  return c;
}

/********************************************************************//**
 * \author Slava Merkin (vgm at bu.edu)
 * \since 05-2005 
 *
 * \param[in] a Vector
 * \param[in] b Vector
 *
 * \return The dot product of input parameters
 ************************************************************************/
doubleArray MHDInnerBoundaryInterface::dot(const doubleArray & a, const doubleArray &b)
{
  Range all;
  int aBase = a.getBase();   // This is done so that we don't have to care about 
  int bBase = b.getBase();   // possibly different bases in the parameter arrays
  return a(all,all,all,aBase+0)*b(all,all,all,bBase+0) + 
    a(all,all,all,aBase+1)*b(all,all,all,bBase+1) + 
    a(all,all,all,aBase+2)*b(all,all,all,bBase+2) ;
}
