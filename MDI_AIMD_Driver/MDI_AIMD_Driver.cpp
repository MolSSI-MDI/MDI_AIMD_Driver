#include <iostream>
#include <mpi.h>
#include <string.h>
extern "C" {
#include "mdi.h"
}

using namespace std;

int main(int argc, char **argv) {

  // Initialize the MPI environment
  MPI_Comm world_comm;
  MPI_Init(&argc, &argv);

  // Read through all the command line options
  int iarg = 1;
  bool initialized_mdi = false;
  while ( iarg < argc ) {

    if ( strcmp(argv[iarg],"-mdi") == 0 ) {

      // Ensure that the argument to the -mdi option was provided
      if ( argc-iarg < 2 ) {
	throw runtime_error("The -mdi argument was not provided.");
      }

      // Initialize the MDI Library
      world_comm = MPI_COMM_WORLD;
      int ret = MDI_Init(argv[iarg+1], &world_comm);
      if ( ret != 0 ) {
	throw runtime_error("The MDI library was not initialized correctly.");
      }
      initialized_mdi = true;
      iarg += 2;

    }
    else {
      throw runtime_error("Unrecognized option.");
    }

  }
  if ( not initialized_mdi ) {
    throw runtime_error("The -mdi command line option was not provided.");
  }

  // Connect to the engines
  MDI_Comm mm_comm = MDI_NULL_COMM;
  MDI_Comm qm_comm = MDI_NULL_COMM;
  int nengines = 2;
  for (int iengine=0; iengine < nengines; iengine++) {
    MDI_Comm comm = MDI_Accept_Communicator();
 
    // Determine the name of this engine
    char* engine_name = new char[MDI_NAME_LENGTH];
    MDI_Send_Command("<NAME", comm);
    MDI_Recv(engine_name, MDI_NAME_LENGTH, MDI_CHAR, comm);
 
    cout << "Engine name: " << engine_name << endl;
 
    if ( strcmp(engine_name, "MM") == 0 ) {
      if ( mm_comm != MDI_NULL_COMM ) {
	throw runtime_error("Accepted a communicator from a second MM engine.");
      }
      mm_comm = comm;
    }
    else if ( strcmp(engine_name, "QM") == 0 ) {
      if ( qm_comm != MDI_NULL_COMM ) {
	throw runtime_error("Accepted a communicator from a second QM engine.");
      }
      qm_comm = comm;
    }
    else {
      throw runtime_error("Unrecognized engine name.");
    }
 
    delete[] engine_name;
  }

  // Perform the simulation
  int niterations = 10;  // Number of MD iterations
  int natoms;
  double qm_energy;
  double mm_energy;
 
  // Receive the number of atoms from the MM engine
  MDI_Send_Command("<NATOMS", mm_comm);
  MDI_Recv(&natoms, 1, MDI_INT, mm_comm);
 
  // Allocate the arrays for the coordinates and forces
  double coords[3*natoms];
  double forces[3*natoms];

  // Have the MD engine initialize a new MD simulation
  MDI_Send_Command("MD_INIT", mm_comm);
 
  // Perform each iteration of the simulation
  for (int iiteration = 0; iiteration < niterations; iiteration++) {

    // Receive the coordinates from the MM engine
    MDI_Send_Command("<COORDS", mm_comm);
    MDI_Recv(&coords, 3*natoms, MDI_DOUBLE, mm_comm);
 
    // Send the coordinates to the QM engine
    MDI_Send_Command(">COORDS", qm_comm);
    MDI_Send(&coords, 3*natoms, MDI_DOUBLE, qm_comm);
 
    // Have the QM engine perform an SCF calculation
    MDI_Send_Command("SCF", qm_comm);
 
    // Get the QM energy
    MDI_Send_Command("<ENERGY", qm_comm);
    MDI_Recv(&qm_energy, 1, MDI_DOUBLE, qm_comm);
 
    // Get the MM energy
    MDI_Send_Command("<ENERGY", mm_comm);
    MDI_Recv(&mm_energy, 1, MDI_DOUBLE, mm_comm);
 
    // Receive the forces from the QM engine
    MDI_Send_Command("<FORCES", qm_comm);
    MDI_Recv(&forces, 3*natoms, MDI_DOUBLE, qm_comm);
 
    // Send the forces to the MM engine
    MDI_Send_Command(">FORCES", mm_comm);
    MDI_Send(&forces, 3*natoms, MDI_DOUBLE, mm_comm);
 
    // Do an MD timestep
    MDI_Send_Command("ATOM_STEP", mm_comm);
 
    cout << "timestep: " << iiteration << " " << mm_energy << " " << qm_energy << endl;
  }

  // Send the "EXIT" command to each of the engines
  MDI_Send_Command("EXIT", mm_comm);
  MDI_Send_Command("EXIT", qm_comm);

  // Synchronize all MPI ranks
  MPI_Barrier(world_comm);

  return 0;
}
