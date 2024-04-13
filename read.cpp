/*
 * @ Write and Read hdf5 file using papillon
*/

#include <sstream>
#include <iostream>
#include <mpi.h>
#include <string>
#include "PapillonNDL/ace.hpp"



int main(int argc, char* argv[])
{
  // initialize MPI
  MPI_Init(&argc, &argv);

  // check the number of arguments
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " filename" << std::endl;
    return 1;
  }

  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  int mpi_size;
  MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);


  // get the filename string from the first argument
  std::string filename = argv[1];
  std::cout << "filename: " << filename << std::endl;

  // read an ace file
  pndl::ACE acefile(filename, pndl::ACE::Type::ASCII);
  std::cout << "ACE file is read." << std::endl;
  std::string wfilename = "test.h5";
  acefile.save_hdf5(wfilename);
  std::cout << "ACE file is saved as HDF5." << std::endl;

  // again convert the hdf5 file to binary
  pndl::ACE acefile2(wfilename, pndl::ACE::Type::HDF5);
  std::cout << "HDF5 file is read." << std::endl;
  std::string wfilename2 = "test2.BIN";
  acefile2.save_binary(wfilename2);
  std::cout << "HDF5 file is saved as binary." << std::endl;

  // finalize MPI
  MPI_Finalize();

  
  return 0;
}

