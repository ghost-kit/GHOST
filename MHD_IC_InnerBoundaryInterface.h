/**
 * \file MHD_IC_InnerBoundaryInterface.h
 * \brief MHD_IC_InnerBoundaryInterface declaration
 *
 * \class MHD_IC_InnerBoundaryInterface 
 * \brief Defines functions for communication (using InterComm) with the MIX
 *
 * \author Slava Merkin (vgm at bu.edu) 
 * \author Peter Schmitt (schmitt at bu.edu) 
 * \since 08-2005
 *
 * This class defines functions for communication between an MHD code
 * and the Ionosphere Coupler/Solver code using InterComm. The functions visible to
 * the outside world are Export(), Import(), sendScalars(), and
 * contructor MHD_IC_InnerBoundaryInterface(). 
 *
 * This code implies that the MHD grid is a logically spherical grid
 * with the symmetry axis aligned with the SM x-axis (LFM definition).
 *
 * \see MHDInnerBoundaryInterface
 */
#ifndef IC_INTERFACE_H
#define IC_INTERFACE_H

// LFM-MIX includes
#include "MHDInnerBoundaryInterface.h"
// 3rd-party includes
#include <IC_EndPointSet.h>

class MHD_IC_InnerBoundaryInterface : public MHDInnerBoundaryInterface {
public:

  /**\brief Class constructor */
  MHD_IC_InnerBoundaryInterface(char*, char*,
				const doubleArray &, const doubleArray &, const doubleArray &, 
				const int, const int, const int, const int);

  /**\brief Export the MHD FAC, density and sound speed to the MIX code*/
  void Export(const doubleArray &, const doubleArray &, const doubleArray &, 
	      const doubleArray &, const doubleArray &);
  
  /**\brief Import potential and calculate electric field & velocity at the inner MHD boundary*/
  void Import(doubleArray &, doubleArray &, doubleArray &);

  /** \brief Broadcast scalar array (i.e. UT & kill signal) from MHD code. */
  void sendScalars(const doubleArray &);

private:
  /**\name InterComm variables */
  //@{
  IC_EndPointSet epset; //!< InterComm end point set
  int ic_err;
  //@}
};

#endif
