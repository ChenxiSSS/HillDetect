/*
    dczheng
    created 2019-07-28
*/

#include "allvars.h"
#include "drfftw.h"

double RSigma;
int LogNorm;
void sigma_clipping( double sigma, double mu ) {

    int i;
    double vmin;

    vmin = 1e100;
    for ( i=0; i<Npixs; i++ ) {
        if ( isnan( Data[i] || Data[i]<=0 ) )
            continue;
        vmin = ( vmin < Data[i] ) ? vmin : Data[i];
    }

    for( i=0; i<Npixs; i++ )
        if ( isnan( Data[i] || Data[i] <=0 ) )
            Data[i] = vmin;

    if ( sigma == 0 ) {
        for( i=0,mu=0; i<Npixs; i++ )
            mu += Data[i];
        mu /= Npixs;

        for( i=0, sigma=0; i<Npixs; i++ ) {
            sigma += SQR( Data[i] - mu );
        }

        sigma = sqrt( sigma / Npixs );
    }

    vmin = mu + RSigma * sigma;
    for( i=0; i<Npixs; i++ ) {
        if ( Data[i] < vmin )
            Data[i] = vmin; 
    }
    SigmaClippingVmin = vmin;

}

void data_cuting() {

    int i, j;
    int index;

    HStartCut = Height * All.CuttingYStart;
    HEndCut = Height * All.CuttingYEnd;
    WStartCut = Width * All.CuttingXStart;
    WEndCut = Width * All.CuttingXEnd;
    //printf( "%i %i %i %i\n", Height, Width, h, w  );

    if ( HEndCut<=HStartCut || WEndCut<=WStartCut ) {
        endrun( "Invalid cutting parameters." );
    }

    printf( "Height: %i, Width: %i\n", Height, Width );
    printf( "region: (%g, %g), (%g, %g)\n",
                All.CuttingYStart, All.CuttingYEnd, 
                All.CuttingXStart, All.CuttingXEnd );
    printf( "region: (%i, %i), (%i, %i)\n",
            HStartCut, HEndCut, WStartCut, WEndCut );

    //output_data( "before_cutting.dat" );

    for( i=HStartCut, index=0; i<HEndCut; i++ )
        for ( j=WStartCut; j<WEndCut; j++, index++ ) {
            Data[index] = Data[i*Width+j];
            DataRaw[index] = DataRaw[i*Width+j];
       }

    HeightGlobal = Height = HEndCut - HStartCut;
    WidthGlobal = Width = WEndCut - WStartCut;
    NpixsGlobal = Npixs = Height * Width;

    //output_data( "after_cutting.dat" );
    //endrun( "test" );
}

void normalize() {

    double vmin, vmax, dv;
    int i;

    vmax = -1e100;
    vmin = 1e100;

    for( i=0; i<Npixs; i++ ) {

        if ( isnan( Data[i] ) )
            continue;

        if ( LogNorm ) {
            if ( Data[i] > 0 )
                vmin = ( vmin < Data[i] ) ? vmin : Data[i];
        }
        else {
            vmin = ( vmin < Data[i] ) ? vmin : Data[i];
        } 
        vmax = ( vmax > Data[i] ) ? vmax : Data[i];
    }


    for( i=0; i<Npixs; i++ ) {

        if ( isnan( Data[i] ) )
            Data[i] = vmin;

        if ( LogNorm )
            if ( Data[i] < vmin )
                Data[i] = vmin;
    }

    if ( LogNorm )
        dv =log( vmax / vmin );
    else
        dv = vmax - vmin;

    for ( i=0; i<Npixs; i++ ) {
        if ( LogNorm )
            Data[i] = log( Data[i]/vmin ) / dv;
        else
            Data[i] = ( Data[i] - vmin ) / dv;
    }

}

/*
void ft_clipping( int mode ){

    int i, j, N2;
    FILE *fd;
    double fac, k;

    if ( mode == 0 )
        return;

    fd = fopen( "t.dat", "w" );
    for( i=0; i<Height; i++ ) {
        for( j=0; j<Width; j++ )
            fprintf( fd, "%g ", Data[i*Width+j] );
        fprintf( fd, "\n" );
    }
    fclose( fd );

    fftw_real *img;
    fftw_complex *fft_of_img;
    rfftwnd_plan fft_plan, fft_plan_inv;

    N2 = 2 * ( Width/2+1 );
    img = malloc( sizeof(fftw_real) * Height * N2 ) ;
    fft_of_img = ( fftw_complex* ) img;

    for( i=0; i<Height; i++ )
        for ( j=0; j<Width; j++ )
            //img[i*N2+j] = log(Data[i*Width+j]);
            img[i*N2+j] = Data[i*Width+j];

    fft_plan     = rfftw2d_create_plan( Height, Width, FFTW_REAL_TO_COMPLEX, FFTW_ESTIMATE | FFTW_IN_PLACE );
    fft_plan_inv = rfftw2d_create_plan( Height, Width, FFTW_COMPLEX_TO_REAL, FFTW_ESTIMATE | FFTW_IN_PLACE );

    rfftwnd_one_real_to_complex( fft_plan, img, NULL );

    fac = 0.01;
    fd = fopen( "fft.dat", "w" );
    for( i=0; i<Height; i++ ) {
        for( j=0; j<N2/2; j++ ) {

            if ( i > Height/2 )
                k = sqrt( SQR(i-Height) + SQR(j) );
            else
                k = sqrt( SQR(i) + SQR(j) );
            if ( k < Height/2 * fac )
                fft_of_img[i*N2/2+j].re = fft_of_img[i*N2/2+j].im = 0;

            fprintf( fd, "%g ", 
                    sqrt(
                    SQR(fft_of_img[i*N2/2+j].re)
                   +SQR(fft_of_img[i*N2/2+j].im)
                        ) / Npixs
                    );
        }
        fprintf( fd, "\n" );
    }
    fclose( fd );

    rfftwnd_one_complex_to_real( fft_plan_inv, fft_of_img, NULL );

    rfftwnd_destroy_plan( fft_plan );
    rfftwnd_destroy_plan( fft_plan_inv );

    for( i=0; i<Height; i++ )
        for ( j=0; j<Width; j++ )
            Data[i*Width+j] = img[i*N2+j] / Npixs;

    fd = fopen( "tt.dat", "w" );
    for( i=0; i<Height; i++ ) {
        for( j=0; j<Width; j++ )
            fprintf( fd, "%g ", Data[i*Width+j] );
        fprintf( fd, "\n" );
    }
    fclose( fd );

}
*/

void pre_proc( int mode ) {
    
    put_start;
    double sigma[2], mean[2], rms[2];
    int *flag, xi, yi, p;

    if ( mode==0 ) {
        if ( All.SigmaClipping ) {
            RSigma = All.RSigma;
            sigma_clipping(0, 0);
        }

        if ( All.DataCutting )
            data_cuting();

        LogNorm = All.LogNorm;
        normalize();

    }

    if ( mode==1) {
        if ( All.SigmaClipping1 ) {
            flag = malloc( sizeof(int)*Npixs );
            memset( flag, 0, sizeof(int)*Npixs );
            p = lset_Head[CurGroup];
            while( p>=0 ) {
                xi = p % WidthGlobal-XShift;
                yi = p / WidthGlobal-YShift;
                /*
                printf( "%i %i [%i] Npixs: %i, Width: %i, Height: %i\n",
                    xi, yi, p, Npixs, Width, Height );
                */
                if ( xi < 0 || xi >= Width || yi < 0 || yi >= Height  )
                    endrun( "" );

                flag[ yi*Width+xi ] = 1;
                p = lset_Next[p];
            }
            printf( "NPixs: %i, Npixs[inner]: %i, NPixs[outer]: %i\n",
                Npixs, lset_Len[CurGroup],
                Npixs-lset_Len[CurGroup] );
            get_mean_sigma_rms( Data, flag, Npixs,  mean, sigma, rms );
            free( flag );
            RSigma = All.RSigma1;
            sigma_clipping( sigma[0], mean[0]);
        }
    }
    put_end;
}
