#ifndef SAVESTATS_H
#define SAVESTATS_H
#include <ctime>
#include <sys/time.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>


void saveStats(const double elapsed, const double sum, const int Nx, const int Ny, const int world_size, const char *info);
#endif

