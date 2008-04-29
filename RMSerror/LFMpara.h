#ifndef __LFM_PARA_H__
#define __LFM_PARA_H__

#include <mfhdf.h>
#include "Data.h"

/// Manages reading dimension and data information for LFM-para MHD (magneto-hydro dynamics) & ION (ionosphere) data files. 
/**
 *  Manages reading dimension and data information for LFM-para MHD (magneto-hydro dynamics) & ION (ionosphere) data files. 
 *
 *  Can handle the following magnetosphere variables:
 *   
 *    - X_grid
 *    - Y_grid
 *    - Z_grid
 *    - rho_
 *    - vx_
 *    - vy_
 *    - vz_
 *    - c_
 *    - bx_
 *    - by_
 *    - bz_
 *    - bi_
 *    - bj_
 *    - bk_
 *    - ei_
 *    - ej_
 *    - ek_
 *
 *  Can handle the following ionosphere variables:
 *    - x2ion
 *    - y2ion
 *    - x_interp
 *    - y_interp
 *    - rho0
 *    - c0
 *    - curnorth
 *    - potnorth
 *    - SigmaH_north
 *    - SigmaP_north
 *    - fluxnorth
 *    - avE_north
 *    - cursouth
 *    - potsouth
 *    - SigmaH_south
 *    - SigmaP_south
 *    - fluxsouth
 *    - avE_south
 */
class LFMpara : public Data
{
public:
  LFMpara(void);
  LFMpara(const char *file);
  ~LFMpara(void);
  bool open(const char *file);
  int  getDimensions(const char *variable, int *dimensions);
  bool getData(const char *variable, float *data);
  bool close(void);

private:
  int32 fid;
};

#endif
