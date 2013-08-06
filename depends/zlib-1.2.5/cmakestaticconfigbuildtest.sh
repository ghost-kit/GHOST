cd buildstatic
cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=/usr/local/zlib1.2.5/static -DBUILD_SHARED_LIBS:BOOL=OFF -DBUILD_TESTING:BOOL=ON -DZLIB_BUILD_EXAMPLES:BOOL=ON ..
ctest -D Experimental
cd ..
