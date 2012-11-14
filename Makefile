CFLAGS  = -g #-Wall -Pedantic
LD		= /usr/bin/mpicc
LDFLAGS = -g
CXX = g++

all: serial omp

serial:
	${CXX} -o hw4.exe hw4.cpp
omp:
	${CXX} -fopenmp -o hw4-OpenMP.exe hw4.cpp
clean:
	rm -f hw4.o hw4.exe hw4-OpenMP.exe
