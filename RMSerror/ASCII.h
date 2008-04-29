#ifndef __ASCII_H__
#define __ASCII_H__

#include "Data.h"
#include <fstream>

/// Manages reading data for whitespace-separated ASCII files with an arbitrary number of columns
/**
 *  This class can handle reading ASCII files with an arbitrary number
 *  of columns.  To specify a particular column, set the
 *  character-array "variable" to a string containing the column
 *  number that you wish to read in.  You must do this as a string, as
 *  an artifact of the class structure that allows for a Data object
 *  to be created for reading LFM HDF data.
 *
 *  @seealso Data, LFMpara
 */
class ASCII : public Data
{
public:
  ASCII(void);
  ASCII(const char *file);
  ~ASCII(void);
  bool open(const char *file);
  int  getDimensions(const char *variable, int *dimensions);
  bool getData(const char *variable, float *data);
  bool close(void);

private:
  // input filestream to handle data.
  std::ifstream ins;
};

#endif
