CFLAGS  = -g #-Wall -Pedantic
CXX = g++

all: serial omp mpi

serial:
	${CXX} -o hw4.exe hw4.cpp
omp:
	${CXX} -fopenmp -o hw4-OpenMP.exe hw4.cc
mpi:
	mpic++ -g -o hw4-mpi.exe hw4-mpi.cc -lm
clean:
	rm -f hw4.o hw4.exe hw4-OpenMP.exe
