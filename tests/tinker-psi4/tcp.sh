#location of required codes
DRIVER_LOC=$(cat ../locations/MDI_AIMD_Driver)
TINKER_LOC=$(cat ../locations/Tinker)
#PSI4_LOC=$(cat ../locations/Psi4)

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
psi4 --mdi "-role ENGINE -name QM -method TCP -port 8021 -hostname localhost" -i psi4.in &

#launch Tinker
cp ${TINKER_LOC%/*}/../params/amoebabio18.prm .
${TINKER_LOC} tinker.in amoebabio18.prm -k tinker.key -mdi "-role ENGINE -name MM -method TCP -port 8021 -hostname localhost" 1000 1.0 10.0 2 298.0 778.0 > tinker.out &

#launch driver
${DRIVER_LOC} -mdi "-role DRIVER -name driver -method TCP -port 8021" &

wait
