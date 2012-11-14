#include <iostream>
#include <math.h>
int main() {
    #pragma omp parallel for
    for (int i=0; i<=10; i++) { 
        int j = 0;
        for (int k=0; k<=100000000 + pow(float(i) / 10.0 + 1.0, 8); k++) {
            j += 1;
        }
        std::cout << "I have i=" << i << " and j=" << j << std::endl;
    }
    return 0;
}
