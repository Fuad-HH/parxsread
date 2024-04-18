#gexport PapillonNDL_DIR=/users/hasanm4/sources/PCPEproject/install/papillon/lib64/cmake/PapillonNDL
cmake -B build -S . \
    -DPARXSREAD_USE_MPI=OFF \
    -DPapillonNDL_ROOT=/users/hasanm4/sources/PCPEproject/install/papillon \
    -DHighFive_ROOT=/users/hasanm4/sources/PCPEproject/HighFive/installh5
