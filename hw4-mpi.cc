#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cmath>
#include <fstream>
#include "elapsed.h"

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
    timeval a;
    gettimeofday(&a, 0);
    
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
    int guardright = Nx_local + 1;
    
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
    
//     if (world_rank==0){
//         cout << "(Nx, Nx_local, Ny) = (" << Nx<<','<<Nx_local<<','<<Ny << ')' << endl;
//         cout << "T has shape (" << local_xwidth <<','<< Ny <<','<< 2 << ')' << endl;
//         cout << "(imin, imax) = (" << imin << ',' << imax << ')' << endl;
//         cout << "(guardleft, guardright) = (" << guardleft << ',' << guardright << ')' << endl;
//     }
    
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
            cout << "\rStep " << k << " of " << Nsteps << " (" << float(k)/Nsteps*100 << "% done).";
        }
        
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Send(&inner_left_edge,  Ny, MPI_FLOAT, left_rank,  world_rank, MPI_COMM_WORLD);
        MPI_Send(&inner_right_edge, Ny, MPI_FLOAT, right_rank, world_rank, MPI_COMM_WORLD);
        MPI_Recv(&outer_left_edge,  Ny, MPI_FLOAT, left_rank,  left_rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&outer_right_edge, Ny, MPI_FLOAT, right_rank, right_rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // put the received data into the extreme left and right edges of our local array
        for(int j=0; j<Ny; j++){
            T[guardleft][j][this_k] = outer_left_edge[j];
        }
        for(int j=0; j<Ny; j++){
            T[guardright][j][this_k] = outer_right_edge[j];
        }
//         cout << "getting " << outer_right_edge[3] << " from " << left_rank << endl;
        
        // now that we have both (overlap) borders, we can do math with it
        for (int i=imin; i<=imax; i++) { // don't update the borders ("guard cells")
            for (int j=1; j<Ny-1; j++) { // top and bottom boundaries shouldn't change.
                T[i][j][next_k] = T[i][j][this_k] + dt * kappa / dx / dy * (
                                        + T[i - 1][j][this_k] + T[i + 1][j][this_k]
                                        + T[i][j - 1][this_k] + T[i][j + 1][this_k]
                                        - 4 * T[i][j][this_k]
                                    );
            }
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Send(&T, Ny*(local_xwidth)*2, MPI_FLOAT, 0, world_rank, MPI_COMM_WORLD);
    if(world_rank==0){
        float final_T[Nx][Ny];
        float gathered_slice[local_xwidth][Ny][2];
        int global_i;
        for(int rank=0; rank<world_size; rank++){
            MPI_Recv(&gathered_slice, Ny*(local_xwidth)*2, MPI_FLOAT, rank, rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for(int i=imin; i<=imax; i++){
                global_i = getGlobalXCoord(i, rank, Nx_local, world_size);
                for(int j=0; j<Ny; j++){
                    final_T[global_i][j] = gathered_slice[i][j][0];
                }
            }
        }
        cout << endl;
        float sum = 0;
        ofstream outputFile;
        outputFile.open("output.csv", ios::out);
        for (int j=0; j<Ny; j++) {
            for (int i=0; i<Nx-1; i++) {
                outputFile << final_T[i][j] << ", ";
                sum += final_T[i][j];
            }
            outputFile << final_T[Nx-1][j];
            sum += final_T[Nx-1][j];
            outputFile << endl;
        }
        outputFile.close();
        
        cout << "sum is " << sum << endl;
        float mean = sum / Ny / Nx;
        cout << "mean is " << mean << endl;
        
        ofstream statsFile;
        char buffer [50];
        int stats_filename = sprintf(buffer, "stats%dx%d-%dprocs.txt", Nx, Ny, world_size);
        statsFile.open(buffer, ios::out);
        statsFile << "sum=" << sum << endl;
        statsFile << "mean=" << mean << endl;
        statsFile.close();
    }
    MPI_Barrier(MPI_COMM_WORLD);  // allow file activity on the master node to complete before finalizing. I'm not sure if this is actually important.
    if(world_rank==0){
        timeval b;
        gettimeofday(&b, 0);
        cout << "elapsed time: " << elapsed(a, b) << endl;
    }
    MPI_Finalize();
    return 0;
}
