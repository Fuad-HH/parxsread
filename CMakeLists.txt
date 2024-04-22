cmake_minimum_required(VERSION 3.20)
project(parxsread)

include(CMakePackageConfigHelpers)
include(FetchContent)

option(PARXSREAD_USE_MPI "Compile Abeille with MPI for distributed memory parallelism" ON)

# get papillon on the system
find_package(PapillonNDL REQUIRED)
find_package(ADIOS2 REQUIRED)
find_package(HighFive REQUIRED)

# get the source files: only one source file read.cpp
add_executable(parxsread read.cpp)
add_executable(readfromadios2 readfromadios2.cpp)
add_executable(readfromh5 readfromh5.cpp)
add_executable(adios12Nread adios12Nread.cpp)

# link the libraries: Papillon and HighFive
target_link_libraries(parxsread PRIVATE PapillonNDL::PapillonNDL HighFive::HighFive adios2::cxx11_mpi MPI::MPI_C)
target_link_libraries(readfromadios2 PRIVATE PapillonNDL::PapillonNDL adios2::cxx11_mpi MPI::MPI_C)
target_link_libraries(readfromh5 PRIVATE PapillonNDL::PapillonNDL HighFive::HighFive)
target_link_libraries(adios12Nread PRIVATE PapillonNDL::PapillonNDL adios2::cxx11_mpi MPI::MPI_C)

# Find MPI if desired
if(PARXSREAD_USE_MPI)
  find_package(MPI REQUIRED)
  if(MPI_CXX_FOUND)
    target_link_libraries(parxsread PUBLIC MPI::MPI_CXX)
    target_link_libraries(readfromadios2 PUBLIC MPI::MPI_CXX)
    target_link_libraries(adios12Nread PUBLIC MPI::MPI_CXX)
    target_compile_definitions(parxsread PUBLIC PARXSREAD_USE_MPI)
    target_compile_definitions(readfromadios2 PUBLIC PARXSREAD_USE_MPI)
    target_compile_definitions(adios12Nread PUBLIC PARXSREAD_USE_MPI)
  endif()
endif()
