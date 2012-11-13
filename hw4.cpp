// #include <stdio>
#include <math.h>
#include <cmath>
#include <iostream>
#include <fstream>

#define SIZE 32

using namespace std;
// #include <iostream>
// #include <math.h>
// int main() {
//     double checkthis[4][4];  // This makes a 4x4, zero-based array.
//     for(int i=0; i<4; ++i) {
//         for(int j=0; j<4; ++j) {
//             checkthis[i][j] = pow(sin(i), 2) + pow(cos(j), 2);
//             cout << i << "," << j << " is " << checkthis[i][j] << '\n';
//         }
//     }
//     return 0;
// }

 
// #include <iostream>
// #include <fstream>
// 
// using namespace std;
// 
// int main(void)
// {
//     char a[2][10] = { "abcd", "efgh" };
//     ofstream ofp;
//     
//     ofp.open("file.txt", ios::out);
//     ofp.write((char *) a, sizeof a);
// 
//     ofp.close();
//     return 0;
// }


int main() {
    
    int Nx = SIZE;
    int Ny = Nx;
    float pi = 3.14159;
    float t0 = 0;
    float kappa = 0.4;
    float tmax = 0.5 * pow(pi, 2) / kappa;
    cout << "tmax is " << tmax << endl;
    float x0 = 0;
    float y0 = x0;
    float xmax = pi;
    float ymax = xmax;
    float dx = (xmax - x0) / Nx;
    float dy = (ymax - y0) / Ny;
    cout << "dx is " << dx << endl;
    float dt = dx * dy / 4 / kappa;
    cout << "dt is " << dt << endl;
    int Nsteps = std::ceil((tmax - t0) / dt);
    cout << "Nsteps is " << Nsteps << endl;
    float T[Nx][Ny][2];

    //ICs:
    for (int i=0; i<Nx; i++) {
        for (int j=0; j<Ny; j++) {
            for (int k=0; k<2; k++) {
                T[i][j][k] = 0.0;
            }
        }
    }
    for (int i=0; i<Nx; i++) {
        for (int k=0; k<2; k++) {
            T[i][ 0][k] = pow(sin(i * dx), 2);
            T[i][-1][k] = pow(cos(i * dx), 2);
        }
    }

    for (int k=0; k<Nsteps-1; k++) {
        int next_k = k%2;
        int this_k = (k+1)%2;
        for (int i=0; i<Nx-1; i++) {
            for (int j=0; j<Ny-1; j++) {
                T[i][ 0][next_k] = pow(sin(i * dx), 2);
                T[i][-1][next_k] = pow(cos(i * dx), 2);
                T[i][ j][next_k] = T[i][j][this_k] + dt * kappa / dx / dy * (
                                        + T[i - 1][j][this_k] + T[i + 1][j][this_k]
                                        + T[i][j - 1][this_k] + T[i][j + 1][this_k]
                                        - 4 * T[i][j][this_k]
                                    );
            }
        }
        for (int j=0; j<Ny-1; j++) {
            T[-1][j][next_k] = T[0][j][next_k];  // Periodic left/right boundaries
        }
        cout << "\rStep " << k << " of " << Nsteps << " (" << float(k)/Nsteps*100 << "\% done).";
    }
cout << " " << endl;
ofstream outputFile;
outputFile.open("output.csv", ios::out);
// outputFile.write((char *) T, sizeof T);
for (int i=0; i<Nx; i++) {
    for (int j=0; j<Ny-1; j++) {
        outputFile << T[i][j][1] << ", ";
    }
    outputFile << T[i][Ny-1][1];
    outputFile << "\n";
}
outputFile.close();

//     fig1 = plt.figure(1, figsize=(6, 4), dpi=100)
//     ax1 = fig1.add_subplot(1, 1, 1)
//     x, y = np.mgrid[0:Nx, 0:Ny]
//     ax1.imshow(T[:, :, 1].T, cmap='hot')
//     CS = ax1.contour(y, x, T[:, :, 1].T)
//     plt.clabel(CS, inline=1, fontsize=10)
//     filename = 'heatmaps/heatmap_t%s.png' % str(k).zfill(4)
//     plt.savefig(filename)
//     #ax1.cla()
//     plt.show()

}
