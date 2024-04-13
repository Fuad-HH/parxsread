cmake_minimum_required(VERSION 3.20)
project(parxsread)

include(CMakePackageConfigHelpers)
include(FetchContent)

option(PARXSREAD_USE_MPI "Compile Abeille with MPI for distributed memory parallelism" ON)

#===============================================================================
# Get HighFive
message(STATUS "Downloading HighFive")
set(HIGHFIVE_USE_BOOST OFF)
set(HIGHFIVE_EXAMPLES OFF)
set(HIGHFIVE_BUILD_DOCS OFF)
set(HIGHFIVE_STATIC_HDF5 ON)
set(HighFive_FIND_QUIETLY ON)
FetchContent_Declare(HighFive
  GIT_REPOSITORY https://github.com/BlueBrain/HighFive.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(HighFive)

#===============================================================================

# get papillon on the system
find_package(PapillonNDL REQUIRED)

# get the source files: only one source file read.cpp
add_executable(parxsread read.cpp)

# link the libraries: Papillon and HighFive
target_link_libraries(parxsread PUBLIC PapillonNDL::PapillonNDL HighFive)

# Find MPI if desired
if(PARXSREAD_USE_MPI)
  find_package(MPI REQUIRED)
  if(MPI_CXX_FOUND)
    target_link_libraries(parxsread PUBLIC MPI::MPI_CXX)
    target_compile_definitions(parxsread PUBLIC PARXSREAD_USE_MPI)
  endif()
endif()
