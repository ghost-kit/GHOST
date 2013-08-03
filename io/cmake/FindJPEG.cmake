# - Find JPEG
# Find the native JPEG includes and library
# This module defines
# JPEG_INCLUDE_DIR, where to find jpeglib.h, etc.
# JPEG_LIBRARIES, the libraries needed to use JPEG.
# JPEG_FOUND, If false, do not try to use JPEG.
# also defined, but not for general use are
# JPEG_LIBRARY, where to find the JPEG library.

#=============================================================================
#CMake - Cross Platform Makefile Generator
#Copyright 2000-2011 Kitware, Inc., Insight Software Consortium
#All rights reserved.

#Redistribution and use in source and binary forms, with or without
#modification, are permitted provided that the following conditions
#are met:

#* Redistributions of source code must retain the above copyright
#  notice, this list of conditions and the following disclaimer.

#* Redistributions in binary form must reproduce the above copyright
#  notice, this list of conditions and the following disclaimer in the
#  documentation and/or other materials provided with the distribution.

#* Neither the names of Kitware, Inc., the Insight Software Consortium,
#  nor the names of their contributors may be used to endorse or promote
#  products derived from this software without specific prior written
#  permission.

#THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
#"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
#LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
#A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
#HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
#SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
#LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
#DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
#THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
#OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#------------------------------------------------------------------------------

#The above copyright and license notice applies to distributions of
#CMake in source and binary form. Some source files contain additional
#notices of original copyright by their contributors; see each source
#for details. Third-party software packages supplied with CMake under
#compatible licenses provide their own copyright notices documented in
#corresponding subdirectories.

#------------------------------------------------------------------------------

#CMake was initially developed by Kitware with the following sponsorship:

# * National Library of Medicine at the National Institutes of Health
#   as part of the Insight Segmentation and Registration Toolkit (ITK).

# * US National Labs (Los Alamos, Livermore, Sandia) ASC Parallel
#   Visualization Initiative.

# * National Alliance for Medical Image Computing (NAMIC) is funded by the
#   National Institutes of Health through the NIH Roadmap for Medical Research,
#   Grant U54 EB005149.

# * Kitware, Inc.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
# License text for the above reference.)

find_path(JPEG_INCLUDE_DIR jpeglib.h)

set(JPEG_NAMES ${JPEG_NAMES} ${CMAKE_STATIC_LIBRARY_PREFIX}jpeg${CMAKE_STATIC_LIBRARY_SUFFIX})
find_library(JPEG_LIBRARY NAMES ${JPEG_NAMES} )

# handle the QUIETLY and REQUIRED arguments and set JPEG_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(JPEG DEFAULT_MSG JPEG_LIBRARY JPEG_INCLUDE_DIR)

if(JPEG_FOUND)
  set(JPEG_LIBRARIES ${JPEG_LIBRARY})
endif()

# Deprecated declarations.
set (NATIVE_JPEG_INCLUDE_PATH ${JPEG_INCLUDE_DIR} )
if(JPEG_LIBRARY)
  get_filename_component (NATIVE_JPEG_LIB_PATH ${JPEG_LIBRARY} PATH)
endif()

mark_as_advanced(JPEG_LIBRARY JPEG_INCLUDE_DIR )
