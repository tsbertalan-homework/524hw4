#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cmath>
#include <fstream>
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
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    int Nx_local = Nx / world_size;
    int local_xwidth = Nx_local + 2;
    int imin = 1;
    int imax = Nx_local;
    int guardleft = 0;
    int guardright = Nx_local+1;
    if (world_rank==0){
        cout << "(Nx, Nx_local, Ny) = (" << Nx<<','<<Nx_local<<','<<Ny << ')' << endl;
    }
    
    if (Nx%world_size != 0) {
        fprintf(stderr, "World size must be an even divisor of %d for %s\n", Nx, argv[0]);
        MPI_Abort(MPI_COMM_WORLD, 1); 
    }

    // Decide on neighbors' ids, considering whether this is the first or last node.
    int left_rank = world_rank - 1;
    int right_rank = world_rank + 1;
    if(world_rank==0){
        left_rank = world_size - 1;
    }
    if(world_rank==world_size-1){
        right_rank = 0;
    }
    
    
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
    float T[local_xwidth][Ny][2];
    float inner_left_edge[Ny];  // holds our own left boundary, to be sent
    float inner_right_edge[Ny];
    float outer_left_edge[Ny];  // holds left neighbor's boundary, after we receive it
    float outer_right_edge[Ny];
    
    // Initialize local arrays
    for(int i=guardleft; i<=guardright; i++){
        for(int j=0; j<Ny; j++){
            for(int k=0; k<2; k++){
                T[i][j][k] = 0.0;
            }
        }
    }
    for(int i=guardleft; i<=guardright; i++){
        for(int k=0; k<2; k++){
            float x = (float) getGlobalXCoord(i,world_rank,Nx_local,world_size);
            T[i][0][k] = pow(cos((float) getGlobalXCoord(i,world_rank,Nx_local,world_size) * dx), 2);
            T[i][Ny-1][k] = pow(sin((float) getGlobalXCoord(i,world_rank,Nx_local,world_size) * dx), 2);
        }
    }
    
    MPI_Barrier(MPI_COMM_WORLD);  // wait until all nodes have done their initial setup
    for (int k=0; k<Nsteps; k++) {
        int this_k = k%2;  // use this to decide whether which is the 'current' work array...
        int next_k = (k+1)%2; // ...and which is the 'next' work array
        
        //set sending arrays to the values of the left and right edges.
        for(int j=0; j<Ny; j++){
            inner_left_edge[j] = T[imin][j][this_k];
        }
        for(int j=0; j<Ny; j++){
            inner_right_edge[j] = T[imax][j][this_k];
        }
        if(world_rank==0){
            cout << "\rStep " << k << " of " << Nsteps << " (" << float(k)/Nsteps*100 << "\% done).";
        }
        
        MPI_Barrier(MPI_COMM_WORLD);
//         cout << world_rank << ": a middlish T value is " << T[1][2][this_k] << endl;
//         cout << world_rank << ": a middlish s value is " << inner_left_edge[2] << endl;
        MPI_Send(&inner_left_edge,  Ny, MPI_FLOAT, left_rank,  0, MPI_COMM_WORLD);
        MPI_Send(&inner_right_edge, Ny, MPI_FLOAT, right_rank, 0, MPI_COMM_WORLD);
        MPI_Recv(&outer_left_edge,  Ny, MPI_FLOAT, left_rank,  0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&outer_right_edge, Ny, MPI_FLOAT, right_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//         cout << world_rank << ": a middlish r value is " << outer_left_edge[2] << endl;
        
        // put the received data into the extreme left and right edges of our local array
//         cout << "saving left edge" << endl;
        for(int j=0; j<Ny; j++){
            T[guardleft][j][this_k] = outer_left_edge[j];
        }
        for(int j=0; j<Ny; j++){
            T[guardright][j][this_k] = outer_right_edge[j];
        }
        
        // now that we have both (overlap) borders, we can do math with it
        for (int i=imin; i<=imax; i++) { // don't update the borders ("guard cells")
            for (int j=1; j<Ny-1; j++) { // top and bottom boundaries shouldn't change.
                T[i][j][next_k] = T[i][j][this_k] + dt * kappa / dx / dy * (
                                        + T[i - 1][j][this_k] + T[i + 1][j][this_k]
                                        + T[i][j - 1][this_k] + T[i][j + 1][this_k]
                                        - 4 * T[i][j][this_k]
                                    );
//                 cout << world_rank << ": a middlish value at (" <<i<<','<<j<< ") is " << T[i][j][next_k] << endl;
            }
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Send(&T, Ny*(local_xwidth)*2, MPI_FLOAT, 0, world_rank, MPI_COMM_WORLD);
    if(world_rank==0){
        printf("gathering...\n");
//         float **gathered_slice;
//         gathered_slice = (float **)malloc((local_xwidth) * sizeof(float *));
//         for(int i=0; i<local_xwidth; i++){
//             gathered_slice[i] = (float *)malloc(Ny * sizeof(float));
//         }
        float final_T[Nx][Ny];
        float gathered_slice[local_xwidth][Ny][2];
        int global_i;
        for(int rank=0; rank<world_size; rank++){
            MPI_Recv(&gathered_slice, Ny*(local_xwidth)*2, MPI_FLOAT, rank, rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("a middlish value for i=%d is %f\n", rank, gathered_slice[2][2][1]);
            for(int i=imin; i<=imax; i++){
                global_i = getGlobalXCoord(0, rank, Nx_local, world_size);
                for(int j=0; j<Ny; j++){
                    final_T[global_i][j] = gathered_slice[i][j][1];
                }
            }
        }
//         for(int i=0; i<Nx; i++){
//             cout << "row" <<i<< '\n';
//             for(int j=0; j<Ny; j++){
//                 cout << final_T[i][j] << ' ';
//             }
//             cout << "\n\n";
//         }
//         for(int i=0; i<local_xwidth; i++){
//             free(gathered_slice[i]);
//         }
//         free(gathered_slice);
        ofstream outputFile;
        outputFile.open("output.csv", ios::out);
        for (int i=0; i<Nx; i++) {
            for (int j=0; j<Ny-1; j++) {
                outputFile << final_T[i][j] << ", ";
            }
            outputFile << final_T[i][Ny-1];
            outputFile << endl;
        }
        outputFile.close();
    }
    MPI_Barrier(MPI_COMM_WORLD);
//     for(int i=0; i<local_xwidth; i++){
//         free(T[i]);
//     }
//     free(T);
    MPI_Finalize();
    return 0;
}
