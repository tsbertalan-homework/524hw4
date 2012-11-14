#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cmath>
#define SIZE 100
using namespace std;

int getGlobalXCoord(const int i, const int world_rank, const int Nx_local, const int world_size){
    int global_i = i-1+Nx_local*world_rank;
    if (global_i==-1){
        global_i = Nx_local*world_size - 1;
    }
    if (global_i==Nx_local*world_size){
        global_i = 0;
    }
    return global_i;
}

int main(int argc, char *argv[]) {
    int Nx, Ny;
    if(argc != 2) {
        Nx = 32;
    } else {
        Nx = atoi( argv[1] );
    }
    Ny = Nx;
    MPI_Init(NULL, NULL);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    printf("check print; rank %d\n", world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    
    if (SIZE%world_size != 0) {
        fprintf(stderr, "World size must be an even divisor of %d for %s\n", Nx, argv[0]);
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
    
//     int Nx = SIZE;
//     int Ny = SIZE;
    int Nx_local = Nx / world_size;
    
//     float **T;
//     T = (float **)malloc((Nx_local+2) * sizeof(float *));
//     for(int i=0; i<Nx_local+2; i++){
//         T[i] = (float *)malloc(Ny * sizeof(float));
//     }
    float pi = 3.14159;
    float t0 = 0;
    float kappa = 0.4;
    float tmax = 0.5 * pow(pi, 2) / kappa;
    float x0 = 0;
    float y0 = x0;
    float xmax = pi;
    float ymax = xmax;
    float dx = (xmax - x0) / Nx;
    float dy = (ymax - y0) / Ny;
    float dt = dx * dy / 4 / kappa;
    int Nsteps = std::ceil((tmax - t0) / dt);
    float T[Nx_local+2][Ny][2]];
    float inner_left_edge[Ny];
    float inner_right_edge[Ny];
    
    // Initialize local arrays
    for(int i=1; i<Nx_local+1; i++){
        for(int j=0; j<Ny; j++){
            T[i][j][k] = 0.0;
        }
    }
    for(int i=0; i<Nx_local+2; i++){
        for(int k=0; k<2; k++){
            T[i][0][k] = pow(cos(getGlobalXCoord(i,world_rank,Nx_local,world_size)), 2);
            T[i][Ny-1][k] = pow(sin(getGlobalXCoord(i,world_rank,Nx_local,world_size)), 2);
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);  // wait until all nodes have done their initial setup
    for (int k=0; k<Nsteps; k++) {
        //set sending arrays to the values of the left and right edges.
        for(int j=0; j<Ny; j++){
            inner_left_edge[j] = T[1][j][0];
        }
        for(int j=0; j<Ny; j++){
            inner_right_edge[j] = T[Nx_local-2][j];
        }
        int next_k = k%2;  // use this to decide whether which is the 'current' work array...
        int this_k = (k+1)%2; // ...and which is the 'next' work array
        if(world_rank==0){
            cout << "\rStep " << k << " of " << Nsteps << " (" << float(k)/Nsteps*100 << "\% done).";
        }
        
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Send(&left_edge,  Ny, MPI_FLOAT, left_rank,  0, MPI_COMM_WORLD);
        MPI_Send(&right_edge, Ny, MPI_FLOAT, right_rank, 0, MPI_COMM_WORLD);
    //     printf("%d sent to %d\n", world_rank, right_rank);
        
        float outer_left_edge[Ny];
        float outer_right_edge[Ny];
        MPI_Recv(&outer_left_edge,  Ny, MPI_FLOAT, left_rank,  0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&outer_right_edge, Ny, MPI_FLOAT, right_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    //     printf("saving left in %d\n", world_rank);
        for(int j=0; j<Ny; j++){
            T[0][j] = outer_left_edge[j];
        }
    //     printf("saving right in %d\n", world_rank);
        for(int j=0; j<Ny; j++){
            T[Nx_local-2][j] = outer_right_edge[j];
        }
        MPI_Barrier(MPI_COMM_WORLD);
//         printf("done saving in %d\n", world_rank);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Send(&T, Ny*(Nx_local+2), MPI_FLOAT, 0, world_rank, MPI_COMM_WORLD);
    if(world_rank==0){
        printf("gathering...\n");
//         float **received_data;
//         received_data = (float **)malloc((Nx_local+2) * sizeof(float *));
//         for(int i=0; i<Nx_local+2; i++){
//             received_data[i] = (float *)malloc(Ny * sizeof(float));
//         }
        float received_data[Nx_local+2][Ny];
        for(int i=0; i<world_size; i++){
            MPI_Recv(&received_data, Ny*(Nx_local*2), MPI_FLOAT, i, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("a middlish value for i=%d is %f\n", i, received_data[3][4]);
        }
//         for(int i=0; i<Nx_local+2; i++){
//             free(received_data[i]);
//         }
//         free(received_data);
    }
    MPI_Barrier(MPI_COMM_WORLD);
//     for(int i=0; i<Nx_local+2; i++){
//         free(T[i]);
//     }
//     free(T);
    MPI_Finalize();
    return 0;
}
