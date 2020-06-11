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

#launch LAMMPS
${LAMMPS_LOC} -mdi "-role ENGINE -name MM -method TCP -port 8021 -hostname localhost" -in lammps.in > lammps.out &

#launch driver
${DRIVER_LOC} -mdi "-role DRIVER -name driver -method TCP -port 8021" &

wait
