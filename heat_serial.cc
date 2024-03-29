#include <math.h>
#include <cmath>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "elapsed.h"
#include "saveStats.h"

#define SIZE 96

using namespace std;

int main(int argc, char *argv[]) {
    timeval a;
    gettimeofday(&a, 0);
    int Nx, Ny;
    if(argc != 2) {
        cerr << "USAGE: " << argv[0] << " <nx>" << endl;
        int WRONG_ARGS = 65;
        exit(WRONG_ARGS);
    } else {
        Nx = atoi( argv[1] );
    }
    Ny = Nx;
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
    int Nsteps = (int)std::ceil((tmax - t0) / dt);
    float T[Nx][Ny][2];

    // Initially, zero everywhere ...
    for (int i=0; i<Nx; i++) {
        for (int j=0; j<Ny; j++) {
            for (int k=0; k<2; k++) {
                T[i][j][k] = 0.0;
            }
        }
    }
    // Except for the top and bottom rows.
    for (int i=0; i<Nx; i++) {
        for (int k=0; k<2; k++) {
            T[i][  0 ][k] = pow(cos(i * dx), 2);
            T[i][Ny-1][k] = pow(sin(i * dx), 2);
        }
    }
    int show_progress = 0;
    if(getenv("HEAT_PROGRESS")!=NULL){
        show_progress = atoi(getenv("HEAT_PROGRESS"));
    }
    for (int k=0; k<Nsteps; k++) {
        int next_k = k%2;  // use this to decide whether which is the 'current' work array...
        int this_k = (k+1)%2; // ...and which is the 'next' work array
        if(show_progress){
            cout << "\rStep " << k << " of " << Nsteps-1 << " (" << float(k)/(Nsteps-1)*100 << "% done).        ";
        }
        for (int i=0; i<Nx-1; i++) { // from 1 to Nx-1 because the periodic areas will be handled later
            for (int j=1; j<Ny-1; j++) { // from 1 to Ny-1 because top and bottom boundaries shouldn't change.
                T[i][ j][next_k] = T[i][j][this_k] + dt * kappa / dx / dy * (
                                        + T[i - 1][j][this_k] + T[i + 1][j][this_k]
                                        + T[i][j - 1][this_k] + T[i][j + 1][this_k]
                                        - 4 * T[i][j][this_k]
                                    );
            }
        }
        for (int j=0; j<Ny; j++) {
            T[0][j][next_k] = T[0][j][this_k] + dt * kappa / dx / dy * (
                                        + T[Nx - 1][j][this_k] + T[1][j][this_k]
                                        + T[0][j - 1][this_k] + T[0][j + 1][this_k]
                                        - 4 * T[0][j][this_k]
                                    ); // left edge
            T[Nx-1][j][next_k] = T[0][j][next_k];  // right edge - periodic (equals left edge)
        }
    }
    if(show_progress){
        cout << endl;
    }
    float sum = 0;
    ofstream outputFile;
    outputFile.open("output.csv", ios::out);
    // outputFile.write((char *) T, sizeof T);
    for (int j=0; j<Ny; j++) {
        for (int i=0; i<Nx-1; i++) {
            outputFile << T[i][j][1] << ", ";
            sum += T[i][j][1];
        }
        outputFile << T[Nx-1][j][1];
        sum += T[Nx-1][j][1];
        outputFile << endl;
    }
    outputFile.close();

    timeval b;
    gettimeofday(&b, 0);
    double elapsed_time = elapsed(a, b);
    int world_size = 1;
    saveStats(elapsed_time, sum, Nx, Ny, world_size, "serial");
    cout << "elapsed time [s]: " << elapsed_time << endl;
}
