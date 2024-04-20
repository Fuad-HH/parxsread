/*
 * @ Read from adios2 format of the ACE Library
*/

#include <sstream>
#include <iostream>
//#include <utility>
#include <mpi.h>
#include <adios2.h>
#include <string>
#include <vector>
#include <sstream>
#include "PapillonNDL/ace.hpp"
#include <unordered_map>
#include <chrono>


std::vector<std::string> splitString(const std::string& str);
void readAdios2(const std::string &filename, const std::string &xsfilename, MPI_Comm comm);
void readlikeabeille(const std::string &dirname);



int main(int argc, char* argv[])
{
  // initialize MPI
  MPI_Init(&argc, &argv);
  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // check the number of arguments
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " filename containing the list of nuclides to read(ACE xsdir format)" << std::endl;
    return 1;
  }

  std::string xsfilename = "ace.bp"; // the adios2 file to read from

  // get the filename string from the first argument
  std::string filename = argv[1]; // for now /lore/hasanm4/nuclearData/Lib80x/xsdir

  if (rank == 0){
  std::cout << "Cross sections reading from list: " << filename << std::endl;
  std::cout << "And the adios2 file: " << xsfilename << std::endl;
  }

  const auto start = std::chrono::steady_clock::now();
  readAdios2(filename, xsfilename, MPI_COMM_WORLD);
  //readlikeabeille(filename);
  MPI_Barrier(MPI_COMM_WORLD);
  const auto end = std::chrono::steady_clock::now();

  const std::chrono::duration<double> elapsed_seconds = end - start;

  if (rank == 0){
  std::cout << "Done reading all the nuclides in " << elapsed_seconds << " seconds." << std::endl;
  }



  MPI_Finalize();
  return 0;
}

void readAdios2(const std::string &filename, const std::string &xsfilename, MPI_Comm comm)
{
  std::ifstream file(filename);
  if (!file.is_open())
  {
    std::cerr << "Could not open file: " << filename << std::endl;
    return;
  }

  adios2::ADIOS adios;
  adios2::IO io = adios.DeclareIO("readIO");
  adios2::Engine bpReader = io.Open(xsfilename, adios2::Mode::Read);

  // a map to store the nuclide name and the ACE object
  std::unordered_map<std::string, pndl::ACE> aceMap;
  


  bpReader.BeginStep();

  std::string line;
  while (std::getline(file, line))
  {
    if (line.find("ptable") != std::string::npos)
    {
      continue; // skip this line that contains ptable
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

    // read the adios2 ace file
    std::string groupname = dirs[1]+"/"+dirs[2]+"/";
    
    aceMap.emplace(dirs[2], pndl::ACE(io, bpReader, dirs[1], dirs[2])); // TODO : check if this works

    /*
    const auto it = aceMap.find(dirs[2]);
    if (it == aceMap.end())
    {
      std::cerr << "Nuclide not found in the map." << std::endl;
    }
    else
    {
      std::cout << "Nuclide read with zaid: " << it->second.zaid_id() << std::endl;
    }
    */

  }

  bpReader.EndStep();
  bpReader.Close();

  file.close();
}


std::vector<std::string> splitString(const std::string& str) {
    std::vector<std::string> tokens;
    std::string token;
    std::stringstream ss(str);
    char delimiter = '/';

    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

void readlikeabeille(const std::string &dirname)
{
  std::string filename = dirname + "/xsdir";
  std::ifstream file(filename);
  if (!file.is_open())
  {
    std::cerr << "Could not open file: " << filename << std::endl;
    return;
  }

  std::cout << "Rreading ACE library using the list : " << filename << std::endl;
  // a map to store the nuclide name and the ACE object
  std::unordered_map<std::string, pndl::ACE> aceMap;


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
    std::vector<std::string> dirs = splitString(loc); // dirs = {Lib80x, molecule, zaid.temperature}


    // read the ace file
    std::string acefileloc = dirname + "/" + loc; // root file name + / + loc
    //std::cout << "ACE file at : " << acefileloc << std::endl;
    aceMap.emplace(dirs[1]+"/"+dirs[2], pndl::ACE(acefileloc, pndl::ACE::Type::ASCII));
    pndl::ACE acefile(acefileloc, pndl::ACE::Type::ASCII);
  }

  //h5file.close();

  file.close();
}