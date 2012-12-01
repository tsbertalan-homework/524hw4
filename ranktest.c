#include <mpi.h>
#include <stdio.h>
int main(int argc, char *argv[]){
    MPI_Init(NULL, NULL);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    printf("rank %d\n", world_rank);
    printf("size %d\n", world_size);
    MPI_Finalize();
    return 0;
}
