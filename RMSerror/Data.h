#ifndef __DATA_H__
#define __DATA_H__

/// Abstract base class for data file readers
/**
 *  Here's how you should use this class in some code:
 *
 *  Data base_file = new <file_type>;
 *
 *  Where <file_type> is a derived class to read the data, such as:
 *    - LFMpara
 *
 *  All of the variables that can be set should be listed in the
 *  header of the derived class, such as:
 *    - LFMpara.h
 */
class Data
{
public:
  /// Open a datafile
  virtual bool open(const char *file) = 0;
  /// Get dimensions of the dataset
  virtual int getDimensions(const char *variable, int *dimensions) = 0;
  /// Read the data from file.  Assumes that data is already allocated.
  virtual bool getData(const char *variable, float *data) = 0;
  /// Close access to file
  virtual bool close(void) = 0;

  /// Get the data filename
  const char * getFilename(void) { return (const char*) filename; };
protected:
  char filename[2048];
};

#endif
