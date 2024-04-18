/*
 * @ Write and Read hdf5 file using papillon
*/

#include <sstream>
#include <iostream>
//#include <mpi.h>
#include <adios2.h>
#include <string>
#include <vector>
#include <sstream>
#include "PapillonNDL/ace.hpp"

void readxsdir(const std::string& filename);
std::vector<std::string> splitString(const std::string& str);



int main(int argc, char* argv[])
{
  // initialize MPI
  //MPI_Init(&argc, &argv);

  // check the number of arguments
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " filename" << std::endl;
    return 1;
  }


  // get the filename string from the first argument
  std::string filename = argv[1];
  std::cout << "filename: " << filename << std::endl;

  // read an ace file
  pndl::ACE acefile(filename, pndl::ACE::Type::ASCII);
  std::cout << "ACE file is read." << std::endl;
  std::string wfilename = "test.h5";
  acefile.save_hdf5(wfilename);
  std::cout << "ACE file is saved as HDF5." << std::endl;



  readxsdir("/lore/hasanm4/nuclearData/Lib80x/xsdir");
  
  return 0;
}



void readxsdir(const std::string &filename)
{
  std::ifstream file(filename);
  if (!file.is_open())
  {
    std::cerr << "Could not open file: " << filename << std::endl;
    return;
  }

  adios2::ADIOS adios;
  adios2::IO io = adios.DeclareIO("writeIO");
  adios2::Engine bpWriter = io.Open("ace.bp", adios2::Mode::Write);

  adios2::Group group = io.InquireGroup('/');

  bpWriter.BeginStep();

  std::string line;
  while (std::getline(file, line))
  {
    if (line.find("ptable") != std::string::npos)
    {
      continue; // skip this line
    }
    std::istringstream iss(line);
    std::string zaid_text, col2, loc;
    if (!(iss >> zaid_text >> col2 >> loc))
    {
      std::cerr << "Error in reading columns." << std::endl;
      break; // error in reading columns
    }
    // std::cout << "Column 1: " << zaid_text << ", Column 3: " << loc << std::endl;
    auto dirs = splitString(loc); // dirs = {Lib80x, molecule, zaid.temperature}

    // read the ace file
    std::string acefileloc = filename.substr(0, filename.find_last_of('/')) + "/" + loc; // root file name + / + loc
    pndl::ACE acefile(acefileloc, pndl::ACE::Type::ASCII);
    acefile.save_adios2(io, bpWriter, dirs[1], dirs[2], group);
  }

  bpWriter.EndStep();
  bpWriter.Close();

  file.close();
}



std::vector<std::string> splitString(const std::string& str) {
    std::vector<std::string> tokens(3);
    std::string token;
    std::stringstream ss(str);
    char delimiter = '/';

    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}