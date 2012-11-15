CFLAGS  = -g #-Wall -Pedantic
FUNCTIONS=elapsed.cc saveStats.cc

all: serial omp mpi


opt-test: serial-nonopt serial-opt1 serial-opt2 serial-opt3
serial-nonopt:
	g++ ${CFLAGS} -o heat_serial_O0 heat_serial.cc ${FUNCTIONS}
serial-opt1:
	g++ -O1 ${CFLAGS} -o heat_serial_O1 heat_serial.cc ${FUNCTIONS}
serial-opt2:
	g++ -O2 ${CFLAGS} -o heat_serial_O2 heat_serial.cc ${FUNCTIONS}
serial-opt3:
	g++ -O3 ${CFLAGS} -o heat_serial_O3 heat_serial.cc ${FUNCTIONS}

serial:
	g++ -O1 ${CFLAGS} -o heat_serial heat_serial.cc ${FUNCTIONS}
omp:
	g++ -O1 -fopenmp ${CFLAGS} -o heat_omp heat_omp.cc ${FUNCTIONS}
mpi:
	mpic++ -O1 ${CFLAGS} -o heat_mpi heat_mpi.cc ${FUNCTIONS} -lm
	# It's possible to put '-limf' between 'mpic++' and '${CLFAGS}' to avoid
	# 'feupdateenv is not implemented and will always fail' http://tinyurl.com/feupdateenv
	# but this doesn't work on my own machines, just Adroit.
clean:
	rm -f heat_serial heat_omp heat_mpi heat_serial_O*
