#include "elapsed.h"

double elapsed(clock_t a, clock_t b){
    return (double)(b-a)/CLOCKS_PER_SEC;
}

