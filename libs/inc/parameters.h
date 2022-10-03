#ifndef PARAMETERS_H
#define PARAMETERS_H

#define NCH 8
#define NSAMPLES 8
#define PATH_MAX 2000

#define FL 60
#define SPC 256
#define FS (SPC * FL)
#define TS 1.0 / FS

#define BATCHSIZE 3
#define BUFFERSIZE (int)(3000.0 / (float)BATCHSIZE)

#define DEBUG 0

#define MAXLOST 5

#endif
