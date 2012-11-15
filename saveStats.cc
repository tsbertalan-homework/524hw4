#include "saveStats.h"
using namespace std;

int    saveStats(const double elapsed, const double sum, const int Nx, const int Ny, const int world_size, const char *info){
    cout << "sum is " << sum << endl;
    float mean = sum / Ny / Nx;
    cout << "mean is " << mean << endl;

    ofstream statsFile;
    char buffer [50];
    int stats_filename = sprintf(buffer, "%s-%dx%d-%dprocs.txt", info, Nx, Ny, world_size);
    statsFile.open(buffer, ios::out);
    statsFile << "sum=" << sum << endl;
    statsFile << "mean=" << mean << endl;
    statsFile << "Nx=" << Nx << endl;
    statsFile << "Ny=" << Ny << endl;
    statsFile << "elapsed=" << elapsed << endl;
    statsFile << "world_size=" << world_size << endl;
    statsFile << "info=" << info << endl;  // probably something like 'mpi' or 'omp' or 'serial'
    statsFile.close();
    
}
