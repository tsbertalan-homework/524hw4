CFLAGS  = -g #-Wall -Pedantic

all: serial omp mpi

serial:
	g++ ${CFLAGS} -o hw4-serial.exe hw4.cc elapsed.cc saveStats.cc
omp:
	g++ -fopenmp ${CFLAGS} -o hw4-OpenMP.exe hw4.cc elapsed.cc saveStats.cc
mpi:
	mpic++ ${CFLAGS} -o hw4-mpi.exe hw4-mpi.cc elapsed.cc saveStats.cc -lm
clean:
	rm -f hw4.o hw4-serial.exe hw4-OpenMP.exe hw4-OpenMP.exe
