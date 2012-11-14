#!/usr/bin/env sh
make mpi
FILENAME=time_results-OpenMP.csv
if [ $# -ne 1 ]
then
    echo "USAGE: `basename $0` [\"list of nx values\"]"
    echo "multiples of 8 are recommended."
    NX_LIST="32 64 128 196"
else
    NX_LIST=$1
fi
echo "proceeding with this list: $NX_LIST"
for NX in $NX_LIST
do
    echo ""
    echo "NX=$NX"
    for N_PROCS in 16 8 4
    do
        echo "N_PROCS=$N_PROCS"
        export OMP_NUM_THREADS=$N_PROCS
        COMMAND="./hw4-OpenMP.exe $NX"
        /usr/bin/time -f "$NX $N_PROCS %e" --output=time_output $COMMAND
        echo ""
        cat time_output | tee -a $FILENAME
    done
done
echo " " # | tee -a $FILENAME
echo "DONE" #| tee -a $FILENAME
