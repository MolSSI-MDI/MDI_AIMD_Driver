#location of required codes
DRIVER_LOC=$(cat ../locations/MDI_AIMD_Driver)
LAMMPS_LOC=$(cat ../locations/LAMMPS)
QE_LOC=$(cat ../locations/QE)

#remove old files
if [ -d work ]; then 
  rm -r work
fi

#create work directory
cp -r data work
cd work

#set the number of threads
export OMP_NUM_THREADS=1

#launch Psi4
(cd /home/tbarnes/mdi/plugin_test/mdi_engine; psi4 -n 16 -i input.dat) &
#export PYTHONPATH=$PYTHONPATH:/home/tbarnes/mdi/plugin_test/mdi_engine
#psi4 -n 16 -i /home/tbarnes/mdi/plugin_test/mdi_engine/input.dat &

#launch LAMMPS
${LAMMPS_LOC} -mdi "-role ENGINE -name MM -method TCP -port 8021 -hostname localhost" -in lammps.in > lammps.out &

#launch driver
${DRIVER_LOC} -mdi "-role DRIVER -name driver -method TCP -port 8021" &

wait
