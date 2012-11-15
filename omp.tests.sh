#!/usr/bin/env sh
echo "######## OpenMP ########"
FILENAME=time_results-OpenMP.txt
if [ $# -lt 2 ]
then
    echo "USAGE: `basename $0` [\"list of nx values\"] [\"list of n_threads values\"]"
    WRONG_PARAMS=65
    exit $WRONG_PARAMS
fi
NX_LIST=$1
N_THREADS_LIST=$2
for NX in $NX_LIST
do
    echo "NX=$NX"
    for N_THREADS in $N_THREADS_LIST
    do
        echo "N_THREADS=$N_THREADS"
        export OMP_NUM_THREADS=$N_THREADS
        COMMAND="./heat_omp $NX $N_THREADS"
        /usr/bin/time -f "$NX $N_THREADS %e" --output=time_output $COMMAND
        cat time_output | tee -a $FILENAME
    done
done

