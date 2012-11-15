if [ $# -ne 1 ]
then
    echo "USAGE: `basename $0` <n_threads>"
    WRONG_PARAMS=65
    exit $WRONG_PARAMS
fi
n_threads=$1
if [ $n_threads -eq 1 ]
then
	programs="serial omp mpi"
else
	programs="omp mpi"
fi
nx="64 128 256 512 1024"
for program in $programs
do
    sh $program.tests.sh "$nx" "$n_threads"
done
