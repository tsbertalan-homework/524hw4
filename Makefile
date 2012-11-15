CFLAGS  = -g #-Wall -Pedantic

all: serial omp mpi

serial:
	g++ ${CFLAGS} -o heat_serial heat_serial.cc elapsed.cc saveStats.cc
omp:
	g++ -fopenmp ${CFLAGS} -o heat_omp heat_omp.cc elapsed.cc saveStats.cc
mpi:
	mpic++ ${CFLAGS} -o heat_mpi heat_mpi.cc elapsed.cc saveStats.cc -lm
clean:
	rm -f heat_serial heat_omp heat_mpi
