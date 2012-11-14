#!/usr/bin/env sh
make mpi
for NX in 32 64 128 256
do
    for N_PROCS in 1 2 4 8
    do
        echo "For $N_PROCS processors:" | tee -a time_results.txt
        COMMAND="mpirun -np $N_PROCS hw4-mpi.exe $NX"
        /usr/bin/time -f "%C %E" --output=time_output $COMMAND
        cat time_output | tee -a time_results.txt
    done
done
