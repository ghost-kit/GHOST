#! /bin/bash
currentDir=`pwd`
echo "Build_Directory: " $currentDir

if [ $1 -eq 41 ] 
then
	cmake $2 -DNUM_CORES=$3 -DENABLE_GKbuildDep=true -DCMAKE_BUILD_TYPE=release -DParaView_URL=http://www.paraview.org/files/v4.1/ParaView-v4.1.0-source.tar.gz -DParaView_URL_MD5=1bcdc7abfccee8f0a59db8ae916bfac2
	make
elif [ $1 -eq 42 ] 
then
	cmake $2 -DNUM_CORES=$3 -DENABLE_GKbuildDep=true -DCMAKE_BUILD_TYPE=release -DParaView_URL=http://www.paraview.org/files/v4.2/ParaView-v4.2.0-source.tar.gz -DParaView_URL_MD5=77cf0e3804eb7bb91d2d94b10bd470f4
	make
elif [ $1 -eq 43 ] 
then
	cmake $2 -DNUM_CORES=$3 -DENABLE_GKbuildDep=true -DCMAKE_BUILD_TYPE=release -DParaView_URL=http://www.paraview.org/files/v4.3/ParaView-v4.3.1-source.tar.gz -DParaView_URL_MD5=d03d3ab504037edd21306413dff64293
	make
elif [ $1 -eq 44 ] 
then
	cmake $2 -DNUM_CORES=$3 -DENABLE_GKbuildDep=true -DCMAKE_BUILD_TYPE=release -DParaView_URL=http://www.paraview.org/files/v4.4/ParaView-v4.4.0-source.tar.gz -DParaView_URL_MD5=fa1569857dd680ebb4d7ff89c2227378
	make
elif [ $1 -eq 50 ] 
then
	cmake $2 -DNUM_CORES=$3 -DENABLE_GKbuildDep=true -DCMAKE_BUILD_TYPE=release -DParaView_URL=http://www.paraview.org/files/v5.0/ParaView-v5.0.0-source.tar.gz -DParaView_URL_MD5=4598f0b421460c8bbc635c9a1c3bdbee
	make
else
 	echo "USAGE: $0 {PVversion} {GHOST_src_dir} {NUM_CORES}"
fi

	
