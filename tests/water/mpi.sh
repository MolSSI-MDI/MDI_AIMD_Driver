#location of required codes
DRIVER_LOC=$(cat ../locations/MDI_AIMD_Driver)
LAMMPS_LOC=$(cat ../locations/LAMMPS)

#remove old files
if [ -d work ]; then 
  rm -r work
fi

#create work directory
cp -r data work
cd work

#set the number of threads
export OMP_NUM_THREADS=1

#launch the codes
mpiexec -n 1 ${DRIVER_LOC} -mdi "-role DRIVER -name driver -method MPI -out driver.out" : \
    -n 1 ${LAMMPS_LOC} -mdi "-role ENGINE -name MM -method MPI -out lammps.out" -in lammps.in

wait
