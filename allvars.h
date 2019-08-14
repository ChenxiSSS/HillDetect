/*
    dczheng
    created 2019-07-28
*/

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"
#include "time.h"
#include "mpi.h"
#include "signal.h"
#include "limits.h"
#include "fitsio.h"
#include "macros.h"
#include "protos.h"

#define MYFILENAME_MAX 100
#define SEP_LEN  50

typedef struct GlobalParams {
    char InputDir[ MYFILENAME_MAX ],
         OutputDir[ MYFILENAME_MAX ],
         PhiDir[ MYFILENAME_MAX ],
         FileNameList[ MYFILENAME_MAX ];

    int  LogNorm, MaxIters, IsSavePhi, SigmaClipping, FTClipping,
         DataCutting;
    double  Mu, Nu, Tol, Lambda1, Lambda2, TimeStep, RSigma, DataCutRatio; 

}GlobalParams;

//extern_start
extern char sep_str[SEP_LEN];
extern double *Data, *Phi, *DataRaw;
extern int 
            Width, Height, ThisTask, NTask, FileNum,
            *edgex, *edgey, NfofEdge, NfofRegion;
extern long 
            Npixs, edgen,
            *Next, *Head, *Len, *Tail;

extern char FileName[ MYFILENAME_MAX ], *AllFileNames;
extern GlobalParams All;
//extern_end

