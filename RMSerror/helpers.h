/**************************************************************************/

/// Make sure that the two arrays (dim1 and dim2) have the same values for the dimensions.
bool check_dimensions(int rank_1, int *dim1, int rank_2, int *dim2)
{
  assert(rank_1 == rank_2);
  for (int i=0; i < rank_1; i++){
    assert(dim1[i] == dim2[i]);
  }
  return true;
}

/**************************************************************************/

/// Use rank & dims to set nt,ni,nj,nk.
void set_dimensions(const int &rank, const int *dims, 
		    int &nt, int &ni, int &nj, int &nk)
{
  switch(rank){
  case 1:    nt = 0;          ni = dims[0];    nj = 1;          nk = 1;          break;
  case 2:    nt = 0;          ni = dims[0];    nj = dims[1];    nk = 1;          break;
  case 3:    nt = 0;          ni = dims[0];    nj = dims[1];    nk = dims[2];    break;
  case 4:    nt = dims[0];    nk = dims[1];    nj = dims[2];    ni = dims[3];    break;
    // 4-d:       time             level            lat              lon
  case 0:    nt = 0;          ni = 0;          nj = 0;          nk = 0;
  default:
    std::cerr << "*** Error:  rank " << rank << " misunderstood.  We require rank <= 4" << std::endl;
  }
}
/**************************************************************************/

/// Check base_data and test_data for infs and NaNs.
template<typename d_type>
bool check_valid_data(const int &rank, const int *dims,
		      const d_type *base_data, const int &base_step, 
		      const d_type *test_data, const int &test_step)
{
  bool return_value = true;

  int nt, ni,nj,nk;
  set_dimensions(rank, dims, nt,ni,nj,nk);

  int base_index, test_index;

  for (int k=0; k < nk; k++){
    for (int j=0; j < nj; j++){
      for (int i=0; i < ni; i++){
	// Time loops on the slowest index:
	base_index = i + (j*ni)+k*(ni*nj)+base_step*(ni*nj*nk);
	test_index = i + (j*ni)+k*(ni*nj)+test_step*(ni*nj*nk);

	// WARNING:   This makes use of "isinf" and "isnan", which might not be defined for some compilers.
	//            C99 standard says "isinf" and "isnan" are defined in <cmath>.
	if ( isinf(base_data[base_index]) ){
	  return_value = false;
	  std::cerr << "*** inf detected at " 
		    << "(" <<i<<","<<j<<","<<k<<")"
		    << " in " << "variable" << " in " << "base file\n"; 
	}
	if ( isnan(base_data[base_index]) ){
	  return_value = false;
	  std::cerr << "*** NaN detected at "
		    << "(" <<i<<","<<j<<","<<k<<")"
		    << " in " << "variable" << " in " << "base file\n"; 

	}
	if ( isinf(test_data[test_index]) ){
	  return_value = false;
	  std::cerr << "*** inf detected at "
		    << "(" <<i<<","<<j<<","<<k<<")"
		    << " in " << "variable" << " in " << "base file\n"; 

	}
	if ( isnan(test_data[test_index]) ){
	  return_value = false;
	  std::cerr << "*** NaN detected at "
		    << "(" <<i<<","<<j<<","<<k<<")"
		    << " in " << "variable" << " in " << "base file\n"; 
	}
      }
    }
  }
  return return_value;
}

/**************************************************************************/

template<typename d_type>
bool global_check_valid_data(const int &rank, const int *dims, const d_type *base_data, const d_type *test_data)
{
  bool returnValue = true;

  if (rank == 4){
    for (int i=0; i < dims[0]; i++){
      if (!check_valid_data(rank, dims, base_data, i, test_data, i))
	return false;
    }
  }
  else{
    if (!check_valid_data(rank, dims, base_data, 0, test_data, 0))
      return false;
  }

  return true;
}

/**************************************************************************/

/**
 *  This computes the RMS error (L2 norm) over all points for particular timesteps (base_step and test_step).
 *  normalized by \f$ |x_i| + |\bar{x_i}| \f$
 *
 *  @returns \f$ rtol = sqrt{\frac{1}{n}\sum_{i=1}^n \left( \frac{x_i - \bar{x_i}}{|x_i|+|\bar{x_i}|}{}\right)^2} \f$ 
 *  summed over all non-zero data points
 *
 */
template<typename d_type>
d_type compute_rms_difference(const char *variable, const int &rank, const int *dims,
			      const d_type *base_data, const int &base_step,
			      const d_type *test_data, const int &test_step)
{
  // We need to report a warning if the field is below roundoff error everywhere.
  bool isFieldZeroEverywhere = true;

  d_type rms_error = 0.0;
  d_type dx;

  int nt,ni,nj,nk;
  set_dimensions(rank, dims, nt,ni,nj,nk);

  int base_index, test_index;

  int nElements = 0;
  for (int k=0; k < nk; k++){
    for (int j=0; j < nj; j++){
      for (int i=0; i < ni; i++){
	nElements++;
	// Time loops on the slowest index:
	base_index = i + (j*ni)+k*(ni*nj)+base_step*(ni*nj*nk);
	test_index = i + (j*ni)+k*(ni*nj)+test_step*(ni*nj*nk);
	
	dx = base_data[base_index]-test_data[test_index];
	// Do not divide by zero!
	if ( (base_data[base_index] != 0) && (test_data[test_index] != 0) ){
	  // data is non-zero
	  isFieldZeroEverywhere = false;
	  rms_error += dx*dx/( (fabs(base_data[base_index]) + fabs(test_data[test_index])) * 
			       (fabs(base_data[base_index]) + fabs(test_data[test_index])) );
	}
      }
    }
  }
  if (isFieldZeroEverywhere)
    std::cerr << "WARNING:  { (base_data - test_data) < machine roundoff } at every point for variable \""
	      << variable << "\"!\n";
  rms_error /= nElements;
  rms_error = sqrt(rms_error);

  return rms_error;
}

/**************************************************************************/

template<typename d_type>
d_type compute_global_rms_difference(const char *variable, const int &rank, const int *dims,
				     const d_type *base_data, const d_type *test_data)
{
  
  d_type totalRMS;
  d_type curRMS;

  // If 4-dimensional (time-stepping), compute global RMS (for all time).
  if (rank == 4){
    int n_ijk = dims[1] * dims[2] * dims[3];
    for (int i=0; i < dims[0]; i++){
      curRMS = compute_rms_difference(variable, rank, dims, 
				      base_data, i,
				      test_data, i);
      totalRMS += curRMS*curRMS * n_ijk;
    }
    totalRMS /= dims[0] * dims[0];
    totalRMS = sqrt(totalRMS);
  }
  else{
    totalRMS = compute_rms_difference(variable, rank, dims, 
				      base_data, 0,
				      test_data, 0);
  }

  return totalRMS;
}

/**************************************************************************/

template<typename d_type>
d_type compute_max_difference(const char *variable, const int &rank, const int *dims,
			      const d_type *base_data, const int &base_step,
			      const d_type *test_data, const int &test_step)
{
  d_type cur_error = 0.0;
  d_type max_error = 0.0;
  d_type dx;

  int nt,ni,nj,nk;
  set_dimensions(rank, dims, nt,ni,nj,nk);

  int base_index, test_index;

  int nElements = 0;
  for (int k=0; k < nk; k++){
    for (int j=0; j < nj; j++){
      for (int i=0; i < ni; i++){
	nElements++;
	// Time loops on the slowest index:
	base_index = i + (j*ni)+k*(ni*nj)+base_step*(ni*nj*nk);
	test_index = i + (j*ni)+k*(ni*nj)+test_step*(ni*nj*nk);

	cur_error = std::abs(base_data[base_index]-test_data[test_index]);

	if (cur_error > max_error){
	  max_error = cur_error;
	}
      }
    }
  }

  return max_error;
}


/**************************************************************************/
