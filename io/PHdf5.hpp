#ifndef PHDF5_HPP__
#define PHDF5_HPP__

#include "Hdf5.hpp"

class PHdf5 : public Hdf5 {

 public:

  PHdf5(int superDomainSize);

  bool readVariable( const string& variableName, 
		     const string& group,
		     const array_info_t& info,
		     void* data );
  
  bool writeVariable( const string& variableName, 
		      const string& group,
		      const array_info_t& info,
		      const void* data );

  void getBcastArrayInfo( const string& group,
			  array_info_t& info );
  
  void putArrayInfo( const string& group,
		     const array_info_t& info );

  bool verifyShape( const string& variableName,
		    const string& group,
		    const array_info_t& info );

 protected:

#ifdef HAS_PHDF5
  bool open(const string& filename, const hid_t& accessMode );

  MPI_Comm comm;
  void setupComm();

  bool collectiveRead, collectiveWrite;
#endif
};

#endif
