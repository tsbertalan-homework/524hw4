#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#define SIZE 100

int main(int argc, char** argv) {
    MPI_Init(NULL, NULL);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    
    if (SIZE%world_size != 0) {
        fprintf(stderr, "World size must be an even divisor of %d for %s\n", SIZE, argv[0]);
        MPI_Abort(MPI_COMM_WORLD, 1); 
    }

    int left_rank = world_rank - 1;
    int right_rank = world_rank + 1;
    if(world_rank==0){
        left_rank = world_size - 1;
    }
    if(world_rank==world_size-1){
        right_rank = 0;
    }
    
    int Nx = SIZE;
    int Ny = SIZE;
    int Nx_local = Nx / world_size;
    
    float **local_data;
    local_data = (float **)malloc((Nx_local+2) * sizeof(float *));
    for(int i=0; i<Nx_local+2; i++){
        local_data[i] = (float *)malloc(Ny * sizeof(float));
    }
    
    float left_edge[Ny];
    float right_edge[Ny];
    for(int i=1; i<Nx_local-1; i++){
        for(int j=0; j<Ny; j++){
            local_data[i][j] = (float) world_rank;

        }
    }
    for(int j=0; j<Ny; j++){
        left_edge[j] = local_data[1][j];
    }
    for(int j=0; j<Ny; j++){
        right_edge[j] = local_data[Nx_local-2][j];
    }
//     printf("%d sending to %d\n", world_rank, left_rank);
    MPI_Send(&left_edge,  Ny, MPI_FLOAT, left_rank,  0, MPI_COMM_WORLD);
//     printf("%d sent to %d\n", world_rank, left_rank);
//     printf("%d sending to %d\n", world_rank, right_rank);
    MPI_Send(&right_edge, Ny, MPI_FLOAT, right_rank, 0, MPI_COMM_WORLD);
//     printf("%d sent to %d\n", world_rank, right_rank);
    
    float outer_left_edge[Ny];
    float outer_right_edge[Ny];
    MPI_Recv(&outer_left_edge,  Ny, MPI_FLOAT, left_rank,  0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&outer_right_edge, Ny, MPI_FLOAT, right_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//     printf("saving left in %d\n", world_rank);
    for(int j=0; j<Ny; j++){
        local_data[0][j] = outer_left_edge[j];
    }
//     printf("saving right in %d\n", world_rank);
    for(int j=0; j<Ny; j++){
        local_data[Nx_local-2][j] = outer_right_edge[j];
    }
    printf("done saving in %d\n", world_rank);
    
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Send(&(local_data[0][0]), Ny*(Nx_local+2), MPI_FLOAT, 0, world_rank, MPI_COMM_WORLD);
    if(world_rank==0){
        printf("gathering...\n");
        float **received_data;
        received_data = (float **)malloc((Nx_local+2) * sizeof(float *));
        for(int i=0; i<Nx_local+2; i++){
            received_data[i] = (float *)malloc(Ny * sizeof(float));
        }
        for(int i=0; i<world_size; i++){
            MPI_Recv(&(received_data[0][0]), Ny*(Nx_local*2), MPI_FLOAT, i, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        for(int i=0; i<Nx_local+2; i++){
            free(received_data[i]);
        }
        free(received_data);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    for(int i=0; i<Nx_local+2; i++){
        free(local_data[i]);
    }
    free(local_data);
    MPI_Finalize();
    return 0;
}
