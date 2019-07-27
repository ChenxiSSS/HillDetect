/*
    dczheng
    created 2019-07-28
*/

#include "allvars.h"

#define ADD_PARAR( a ) {\
    strcpy( tag[nt], &((#a)[4]) );\
    addr[nt] = &a;\
    id[nt++] = REAL;\
}

#define ADD_PARAI( a ) {\
    strcpy( tag[nt], &((#a)[4]) );\
    addr[nt] = &a;\
    id[nt++] = INT;\
}

#define ADD_PARAS( a ) {\
    strcpy( tag[nt], &((#a)[4]) );\
    addr[nt] = a;\
    id[nt++] = STRING;\
}

#define MAXTAGS 300
#define REAL 1
#define STRING 2
#define INT 3

void read_parameters( char *fn ) {

    FILE *fd;
    void *addr[MAXTAGS];
    char tag[MAXTAGS][50], buf[200],
        buf1[200], buf2[200], buf3[200];
    int id[MAXTAGS], nt, i, j, errflag=0;;

    if ( ThisTask == 0 ) {
        fd = fopen( fn, "r" );
        printf( "read parameters ...\n" );
        if ( NULL == fd ){
            printf( "Faile to Open Parameter file %s\n", fn );
        }
    
        nt = 0;
    
        ADD_PARAS( All.InputDir );
        ADD_PARAS( All.OutputDir );
        ADD_PARAS( All.FileNameList );
        ADD_PARAS( All.PhiDir );
    
        ADD_PARAR( All.Mu );
        ADD_PARAR( All.Nu );
        ADD_PARAR( All.Lambda1 );
        ADD_PARAR( All.Lambda2 );
        ADD_PARAR( All.TimeStep );
        ADD_PARAR( All.Tol );
        ADD_PARAR( All.RSigma );
    
        ADD_PARAI( All.MaxIters );
        ADD_PARAI( All.IsSavePhi );
        ADD_PARAI( All.LogNorm );
        ADD_PARAI( All.SigmaClipping );
    
        while( !feof( fd ) ) {
            *buf = 0;
            fgets( buf, 200, fd );
            if ( sscanf( buf, "%s%s%s", buf1, buf2, buf3 ) < 2 )
                continue;
            if ( buf1[0] == '%' )
                continue;
            for ( i=0, j=-1; i<nt; i++ )
                if ( strcmp( buf1, tag[i] ) == 0 ) {
                    j = i;
                    tag[i][0] = 0;
                    break;
                }
            if ( j>=0 ) {
                switch ( id[j] ) {
                    case REAL:
                        *( (double*)addr[j] ) = atof( buf2 );
                        printf(  "%-15s %g\n", buf1, *((double*)addr[j]) );
                        break;
                    case INT:
                        *( (int*)addr[j] ) = atoi( buf2 );
                        printf( "%-15s %d\n", buf1, *((int*)addr[j]) );
                        break;
                    case STRING:
                        strcpy( (char*)addr[j], buf2 );
                        printf( "%-15s %s\n", buf1, buf2 );
                        break;
                }
            }
            else {
                printf( "Error in file %s:  Tag: '%s', not allowed or multiple define.\n", fn, buf1 );
                errflag = 1;
            }
        }
        for ( i=0; i<nt; i++ ) {
            if ( *tag[i] ) {
                printf( "Error. I miss a value for tag '%s' in parameter file '%s'.\n", tag[i], fn );
                errflag = 1;
            }
        }
        if ( errflag )
            exit(20190723);
        fclose( fd );
     }
     MPI_Bcast( &All, sizeof( struct global_parameters_struct  ), MPI_BYTE, 0, MPI_COMM_WORLD );

     put_sep;
    
}
