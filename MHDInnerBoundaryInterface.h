/**
 * \file MHDInnerBoundaryInterface.h
 * \brief MHDInnerBoundaryInterface declaration
 *
 * \class MHDInnerBoundaryInterface 
 * \brief Abstract base class that defines functions for communication with the MIX
 *
 *\author Slava Merkin (vgm at bu.edu) 
 *\since 08-2005
 *
 * Abstract base class that defines functions for communication
 * between an MHD code and the Ionosphere Coupler/Solver code. 
 *
 * The virtual functions visible to the outside world are 
 *   - MHDInnerBoundaryInterface()
 *   - Export()
 *   - Import(),
 *   - sendScalars()
 * These virtual functions must be defined in a derived class.
 *
 * This code implies that the MHD grid is a logically spherical grid
 * with the symmetry axis aligned with the SM x-axis (LFM definition).
 *
 * \see MHD_IC_InnerBoundaryInterface.h
 */
#ifndef INTERFACE_H
#define INTERFACE_H


#define PPLUSPLUS  //!< Compile a P++ code rather than A++
#include <A++.h>

#ifndef PI
#define PI 3.1415926
#endif
#define RION 6.5e8         /*!<\brief radius of the ionosphere in
			     cm. The grid coordinates in the LFM are
			     given by x*RION etc.*/

#define REARTH 6.38e8  //!< Radius of the earth in cm
#define BEQUATOR 3.11e-5 /*!<\brief Earth's dipole magnetic field at
			   the equator in Tesla (Baumjohann, p.33) */

#define STRLEN 256     //!< The maximum length of a character string 

class MHDInnerBoundaryInterface {
public:

  /**\brief Class constructor */
  MHDInnerBoundaryInterface(char*, char*,
			    const doubleArray &, const doubleArray &, const doubleArray &, 
			    const int, const int, const int);
  virtual ~MHDInnerBoundaryInterface() {};

  /**\brief Export the MHD FAC, density and sound speed to the MIX code*/
  virtual void Export(const doubleArray &, const doubleArray &, const doubleArray &, 
		      const doubleArray &, const doubleArray &) = 0;

  /**\brief Import potential and calculate electric field & velocity at the inner MHD boundary*/
  virtual void Import(doubleArray &, doubleArray &, doubleArray &) = 0;

  /** \brief Broadcast scalar array (i.e. UT & kill signal) from MHD code. */
  virtual void sendScalars(const doubleArray &) = 0;

protected:
  /**\brief Calculate FAC, density & sound speed in second shell before */
  void prepareExport(const doubleArray &bx, const doubleArray &by, const doubleArray &bz, 
		     const doubleArray &rho, const doubleArray &cs);

  /**\brief Calculate the potential electric field & the velocity at the inner boundary */
  void processImport(doubleArray & eField_j, doubleArray & eField_k, doubleArray & velocity);


  /**\name MHD grid dimension information */
  //@{
  const int ni;
  const int nj;
  const int nk;
  const int nim1;
  const int nip1;
  const int njm1;
  const int njp1;
  const int nkp1;
  const int nkm1;
  //@}

  doubleArray current, density, soundSpeed, potential;

private:
  /**\name Dipole field functions */
  //@{
  /**\brief X-component of a unit vector in dipole field direction */
  doubleArray bxNorm(const doubleArray &, const doubleArray &, const doubleArray &);

  /**\brief Y-component of a unit vector in dipole field direction */
  doubleArray byNorm(const doubleArray &, const doubleArray &, const doubleArray &);

  /**\brief Z-component of a unit vector in dipole field direction */
  doubleArray bzNorm(const doubleArray &, const doubleArray &, const doubleArray &);

  /**\brief Dipole field magnitude */
  doubleArray bMag(const doubleArray &, const doubleArray &, const doubleArray &);

  /**\brief Ratio of the dipole field magnitude at magnetosphere/ionosphere */
  doubleArray bion2bmag(const doubleArray &, const doubleArray &, const doubleArray &);
  //@}

  /**\name MHD inner boundary condition functions */
  //@{
  /**\brief Get density and sound speed at the second shell vertices */
  doubleArray getVarSecondShell(const doubleArray &);

  /**\brief Calculate the field-aligned current */
  void getParallelCurrent(const doubleArray &, const doubleArray &, const doubleArray &);

  /**\brief Smooth the field-aligned current */
  void smoothParallelCurrent();

  /**\brief Calculate the electric field from the potential received
     from MIX */
  void getElectricField(doubleArray &, doubleArray &, doubleArray & );

  /**\brief Calculate the velocity from the electric field at the inner boundary */
  doubleArray getVelocity(const doubleArray &, const doubleArray &, const doubleArray &);
  //@}

  /**\name Vector manipulation functions */
  //@{
  doubleArray dot(const doubleArray &, const doubleArray &);
  doubleArray cross(const doubleArray &, const doubleArray &);
  doubleArray triple(const doubleArray &, const doubleArray &, const doubleArray &);
  //@}

  /**\name Arrays dimensioned and assigned by the class constructor
   *  and then used by other functions of the class*/
  //@{
  doubleArray volume, xCenter, yCenter, zCenter, bxDot, byDot, bzDot;
  doubleArray x_iFaceCenter, y_iFaceCenter, z_iFaceCenter;
  doubleArray x_jFaceCenter, y_jFaceCenter, z_jFaceCenter;
  doubleArray x_kFaceCenter, y_kFaceCenter, z_kFaceCenter;
  doubleArray di, dj, dk, i_x, i_y, i_z, j_x, j_y, j_z, k_x, k_y, k_z;
  //@}

};

#endif
