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

# link the libraries: Papillon and HighFive
target_link_libraries(parxsread PRIVATE PapillonNDL::PapillonNDL HighFive::HighFive adios2::cxx11_mpi MPI::MPI_C)

# Find MPI if desired
if(PARXSREAD_USE_MPI)
  find_package(MPI REQUIRED)
  if(MPI_CXX_FOUND)
    target_link_libraries(parxsread PUBLIC MPI::MPI_CXX)
    target_compile_definitions(parxsread PUBLIC PARXSREAD_USE_MPI)
  endif()
endif()
