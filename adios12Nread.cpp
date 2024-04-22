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
#include <ios>


std::vector<std::string> splitString(const std::string& str);
void readAdios2(const std::string &filename, const std::string &xsfilename, MPI_Comm comm, int rank);
//void BCastACE(ACE& ace, MPI_Comm comm, int rank);



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
  readAdios2(filename, xsfilename, MPI_COMM_WORLD, rank);
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

void readAdios2(const std::string &filename, const std::string &xsfilename, MPI_Comm comm, int rank)
{
  std::ifstream file(filename);
  if (!file.is_open())
  {
    std::cerr << "Could not open file: " << filename << std::endl;
    return;
  }

    // !why can't I do this only in rank 0?
    adios2::ADIOS adios;
    adios2::IO io = adios.DeclareIO("readIO");
    adios2::Engine bpReader = io.Open(xsfilename, adios2::Mode::Read);
    bpReader.BeginStep();


  // a map to store the nuclide name and the ACE object
  std::unordered_map<std::string, pndl::ACE> aceMap;
  




  std::string line;
  std::vector<char> buffer;
  //std::stringstream ss(std::ios::binary | std::ios::out | std::ios::in);

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
    
    if (rank == 0){
        aceMap.emplace(dirs[2], pndl::ACE(io, bpReader, dirs[1], dirs[2])); 

        // serialize the ACE object
        aceMap[dirs[2]].serialize(buffer);
        //std::cout << "Rank: " << rank << " serialized ACE object." << std::endl;
        long unsigned int datasize = buffer.size();


        // broadcast the size of the serialized ACE object
        MPI_Bcast(&datasize, 1, MPI_UNSIGNED_LONG, 0, comm);


        // broadcast the serialized ACE object
        MPI_Bcast(buffer.data(), datasize, MPI_CHAR, 0, comm);
    }
    else{
        unsigned long int datasize;
        MPI_Bcast(&datasize, 1, MPI_UNSIGNED_LONG, 0, comm);

        buffer.reserve(datasize);

        // broadcast the serialized ACE object
        MPI_Bcast(buffer.data(), datasize, MPI_CHAR, 0, comm);
        //std::cout << "Rank: " << rank << " received serialized ACE object." << std::endl;


        // deserialize the ACE object
        pndl::ACE ace;
        ace.deserialize(buffer);

        // add the ACE object to the map
        aceMap.emplace(dirs[2], ace);
    }

    MPI_Barrier(comm);

    
    const auto it = aceMap.find(dirs[2]);
    if (it == aceMap.end())
    {
      std::cerr << "Nuclide not found in the map of rank: " << rank << "\n";
    }
    else
    {
      std::stringstream ss;
      ss << "Nuclide read with zaid: " << it->second.zaid_id() << " and mat " << it->second.mat() << " and temp: " << it->second.temperature() << " at rank: " << rank << "\n";
      ss << "Size of xss: " << it->second.xss_size() << "\n";
      std::cout << ss.str();
    }
    
    buffer.clear();

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
