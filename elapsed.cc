#include "elapsed.h"

double elapsed(timeval a, timeval b){
    return b.tv_sec - a.tv_sec + (b.tv_usec - a.tv_usec) / 1000000.0;
}