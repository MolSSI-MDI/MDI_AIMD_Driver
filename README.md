MDI AIMD Driver
=====================================

## Overview

This driver enables users to perform ab initio molecular dynamics (AIMD) simulations using the MolSSI Driver Interface (MDI).
It uses two engines: a QM engine to compute atomic forces, and an MM engine to perform time integration.

## Installation

```
git clone git@github.com:MolSSI-MDI/MDI_AIMD_Driver.git
cd MDI_AIMD_Driver
mkdir build
cd build
cmake ..
make
```

## Getting started with the examples

The repository includes example scripts to run using Quantum ESPRESSO (QE) as the QM code and LAMMPS as the MM code.
Both of these codes must be compiled in a manner that supports MDI prior to running the example scripts.
Instructions for compiling specific codes with MDI support can be found <a href="https://molssi-mdi.github.io/MDI_Library/html/mdi_ecosystem.html">here</a>.

It is also necessary to provide the example scripts with the location of each of the driver and engine executables.
In the `MDI_AIMD_Driver/tests/locations` directory are three files, named `MDI_AIMD_Driver`, `LAMMPS`, and `QE`.
Each of these files should be modified to contain the absolute path to the corresponding executable.
The files should not contain any additional whitespace and should not terminate in a newline.

Example scripts for running an AIMD simulation on a small box of water are provided in the `MDI_AIMD_Driver/tests/water` directory.
The scripts are named `tcp.sh` and `mpi.sh`, and will run the AIMD simulation with TCP-based or MPI-based MDI communication, respectively.
The scripts should be executed within the `MDI_AIMD_Driver/tests/water` directory.
The output files for the simulation are located in `MDI_AIMD_Driver/tests/water/work`.
In the case of the `mpi.sh` executable, the driver's output is located in `MDI_AIMD_Driver/tests/water/work/driver.out`.


## General usage instructions

This driver assumes that the QM and MM engines are initialized with the same number of atoms, which **must** be listed in the same order in their respective input files.
For example, if you wanted to run an AIMD simulation on a single water molecule, both engines must be launched using input files that specify their system as consisting of one oxygen atom and two hydrogen atoms, and the atoms must listed in the same order in the QM and MM input files (*i.e.* O, H, H).
It is not necessary that the initial nuclear coordinates of the QM and MM engines be the same; the driver will replace the initial nuclear coordinates of the QM engine with the initial nuclear coordinates from the MM engine.
If you are running a calculation with periodic boundary conditions, the QM and MM engines must be launched with the same periodic cell dimensions.
