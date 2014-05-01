/*Copyright (c) <2014> <Paul Kendrick>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/


#include "featureExtraction.h"
#include "kiss_fft130/kiss_fft.h"

    static float * f;
    static float * W;
    float *Xre, *Xim; // DFT of x (real and imaginary parts)
    static float * window; 
    //static double  complex * windowc, *Wc; 
    static float* Mel ;
    static float * Melfull ;
    static float * MelSpec;
    static float * triang;
    static float * PSD ;
    static float ** triangFull;
    #define PI2 6.2832
    static kiss_fft_cfg  cfg;
    static kiss_fft_cpx *cin;
    static kiss_fft_cpx *cout;

void initVarsFeats(int N,int Fs)
{
    int meln=16;
    float MAXF = 8000.0;
    float MINF = 20.0;
    int fftn = N;
    int i,i2;
       cin = (kiss_fft_cpx*)malloc(sizeof(kiss_fft_cpx)*N);
       cout = (kiss_fft_cpx*)malloc(sizeof(kiss_fft_cpx)*N);

    cfg = kiss_fft_alloc(N,0,0,0);

    window = malloc(sizeof(float)*N); 
    f = malloc(sizeof(float)*N); 
    W = malloc(sizeof(float)*N); 
    //float complex z1
//    Wc = malloc(sizeof(complex float )*N); 
 //   windowc = malloc(sizeof(complex float )*N); 

    
    Xre = malloc(sizeof(float)*N); 
    Xim = malloc(sizeof(float)*N);     
    
    for (i=0;i<=N/2;i++)
    {
        f[i]=((float)i) / ((float)N) * ((float)Fs);
       // printf("%f \n,",f[i]);

    }
    aWeightingPreComp(f,N);
    
    Mel = malloc((meln+3) * sizeof(float));
    Melfull = malloc((N/2+1) * sizeof(float));
    
    MelSpec = malloc(meln * sizeof(float));
    triang = malloc((N/2+1)  * sizeof(float));
    PSD = malloc((N/2+1)  * sizeof(float));
     //triangFull=malloc((N/2+1)*meln  * sizeof(float));
     
    triangFull=(float **) malloc(meln*sizeof(float *));
    for(i=0;i<meln;i++)
        triangFull[i]=(float *) malloc((N/2+1)*sizeof(float));
    //triangFull[meln][N/2] ;;

     float k=2595.0;
    for (i=0;i<fftn/2+1;++i)
    {
       // PSD[i]=2*pow(spec[i]/sqrt(fftn),2);
        Melfull[i] = log10(1+f[i]/700.0)*k;
        // printf("%f\n",spec[i]);
    }
     PSD[0]=PSD[0]/2;
    //meldatatype meldata;
    //Take the spectrum
    float MelSpacing = (log10(1.0+MAXF/(float)700.0)*k-log10(1+MINF/(float)700.0)*k)/(float)(meln+1.0);
    for ( i=0;i<meln+3;++i) //iterate over mel freq banks
    {
        // Mel(i)=2595 * log10(1+minf/700)+MelSpacing*(i-1)+MelSpacing;
        //  f_frommel(i) = 700*(10.^(Mel(i)/2595)-1);
        
        Mel[i]=(log10(1.0+MINF/700)*k)+MelSpacing*(i)+MelSpacing;//  Mel centres
    }
    
    //  FILE * pFile;
    // pFile = fopen ("dataoutMELSPEC.txt","w");
    
    for ( i=0;i<meln;++i) //iterate over mel freq banks
    {
        float  Mel_low_up, Mel_hig_up;
        
        if (i==0)
        {
            Mel_low_up = 0;
        }
        else{
            Mel_low_up= Mel[i-1];
        }
        Mel_hig_up= Mel[i+1];
        
        
        for( i2=0;i2<fftn/2;i2++)
        {
            float m, c;
            if (Melfull[i2]<Mel[i]){
                m=(1.0-0.0)/(Mel[i]-Mel_low_up);
                c=m*Mel_low_up;
                triangFull[i][i2]=m*Melfull[i2]-c;
            }else if (Melfull[i2]>Mel[i]){
                m=(0.0-1.0)/( Mel_hig_up-Mel[i]);
                c=-m*Mel[i]+1.0;
                triangFull[i][i2]=m*Melfull[i2]+c;
            }
            
            
            if(triangFull[i][i2]<0.0) {triangFull[i][i2]=0.0;};
        }
        
        
    }
    
}

/*void DFT ( float *x,float *z,int nfft)
{
    
int jj;//z=zeros(1,nfft);
float complex   Sum=0;
int n,k;
for (n=0 ; n<nfft ; ++n) 
     
    // Calculate DFT of x using brute force
    for (k=0 ; k<nfft ; ++k)
    {
        // Real part of X[k]
        Xre[k] = 0;
        for (n=0 ; n<nfft ; ++n) 
            Xre[k] += x[n] * cos(n * k * PI2 / nfft);
         
        // Imaginary part of X[k]
        Xim[k] = 0;
        for (n=0 ; n<nfft ; ++n) Xim[k] -= x[n] * sin(n * k * PI2 / nfft);
         
        // Power at kth frequency bin
        z[k] = sqrt(Xre[k]*Xre[k] + Xim[k]*Xim[k]);
    }
     
//return;
}*/

void computeMFCCs(float *windowIn, int N, int Fs,float *mfcc)
{   
    int i;
    float rms[2];
    // apply hanning window
    float sum=0;
        float sum2=0;

    //Wc[0]=0;
    for ( i = 0; i < N; i++) 
    {
    float multiplier = 0.5 * (1.0 - cosf(2.0*PI2/2.0*(float)i/((float)N-1)));
    window[i] = multiplier * windowIn[i];
    sum=sum+pow(multiplier,2)/N;
                //Wc[i]=(double)window[i] + (double)0.0* I;;
                        cin[i].r = (double)window[i];
                        cin[i].i = 0;
                    sum2=sum2+pow(window[i],2)/N;

    }
    
    sum=sqrtf(sum);// Normliseing facotr for window.
    sum2=sqrtf(sum2);// Normliseing facotr for window.


    //DFT(window, W,N);
    kiss_fft(cfg,cin,cout);   //fft code
       for ( i = 0; i < N; i++)    //Magnitude spectrum
          W[i]=sqrt(pow((float)cout[i].r,2) + pow((float)cout[i].i,2))/ sqrt((float)N/2);
    
    aWeighting(f, W ,N, rms,N );   // Compute a-weighted rms
     melSpec(Fs, f, W,N,mfcc);  
            
    
    float SPLA= 20 * log10(rms[0]/sum);
    float SPL= 20 * log10(rms[1]/sum);
     mfcc[0]=SPLA;
     
   
   /*FILE * pFile;
   pFile = fopen ("win1T.txt","w");
       for (i=0;i<=N;i++)
       {
        fprintf(pFile,"%f \n",window[i]);
       }
   fclose(pFile);
   
   pFile = fopen ("win1F.txt","w");
       for (i=0;i<=N;i++)
       {
        fprintf(pFile,"%f \n",W[i]);
       }
       fclose(pFile);*/

}
    

void melSpec(int Fs,float * f, float * spec,int fftn,float *mfcc)
{
    /////////////////////////////
    int meln=16;
    float MAXF = 8000.0;
    float MINF = 20.0;
    int i,i2;    
    for (i=0;i<fftn/2+1;++i)
    {
        PSD[i]=2*pow(spec[i]/sqrt(fftn),2);
              // printf("%f\n",spec[i]);
    }
       
    /////////////////////////
    //Freq 2 mel %	m = ln(1 + f/700) * 1000 / ln(1+1000/700)
    //[1] J. Makhoul and L. Cosell. "Lpcw: An lpc vocoder with
    //%         linear predictive spectral warping", In Proc IEEE Intl
    ///%         Conf Acoustics, Speech and Signal Processing, volume 1,
    //%         pages 466469, 1976. doi: 10.1109/ICASSP.1976.1170013.
       float Mel_low_up, Mel_hig_up;

    //int Len=sizeof(f)/sizeof(double);


    
    //  FILE * pFile;
    // pFile = fopen ("dataoutMELSPEC.txt","w");
       
       
    for ( i=0;i<meln;++i) //iterate over mel freq banks
    {
        MelSpec[i]=0.0;
        // printf("%f\n",Mel[i]);
        //Now compute the triangualr weigthting for each
        
        
        
        for( i2=0;i2<fftn/2;i2++)
        {
            // printf("%f\n",triang[i2]);
            
            MelSpec[i]=MelSpec[i]+triangFull[i][i2]*(PSD[i2]);
            
            //y=mx+c;  when x=Mel[i], y=1, when x = 0 (or halfway between mel points)y=0; c is the ammount of vertical movement that occours up to the point where we want the value to be 0, so it is m*MEL
            //NSLog(@"%f",triang[i]);
        }
        
        
        //freqdata->Mel[i]=(Mel[i]);
        //MelSpec[i]=(MelSpec[i]);
        //freqdata->MelSpec[i]=MelSpec[i];
        mfcc[i]=0;
        
      // fprintf(pFile, "%f \n",MelSpec[i]);
        
        //mfcc[i]=meldata.MelSpec[i];
        //   mfcc[i]=log(MelSpec[i]);
        //    meanMFCC=meanMFCC+meldata.mfcc[i] / MELN ;
    }
     //fclose(pFile);
    dctComp(MelSpec, meln,mfcc);
  
}
void dctComp(float * data, int N,float *mfcc)
{
     float norm = sqrtf(2.0/(float)N);
      float mean = 0;
      float var=0;
      int k,n;
   // double PI_N=PI/N;
    //
    for( k=0;k<N;k++)
    {
    mfcc[k]=0;
    }
    float one_over_sqrt2_times_norm=(1.0/sqrt(2))*norm;
    for( k=0;k<N;k++)
    {
    for( n=0;n<N;n++)
    {
     mfcc[k]=mfcc[k]+
             logf(data[n])*cosf(PI2/2.0/N  *(n+0.5)*k);
       // mfcc[k]=mfcc[k]+logf(data[n])*cosf( ((M_PI)*((float)n+0.5)*(float)k));    
    }
            

           if(k==0)
            {
                mfcc[k]=mfcc[k]*(1.0/sqrtf(2))*norm;
               // mfcc[k]=mfcc[k]*one_over_sqrt2_times_norm;
            }else{
                mfcc[k]=mfcc[k]*norm;
            }
        mean=mean+mfcc[k]/N;

    }
    
    //Normalise MFCC (zero mean sd ==1
    for( k=0;k<N;k++)
    {
        var=var+((mfcc[k]-mean)  *  (mfcc[k]-mean) )/(N-1);
    }
    float sd=sqrtf(var);
    
   //  FILE * pFile;
  // pFile = fopen ("mfcc.txt","w");

    for( k=0;k<N;k++)
    {
        mfcc[k]=(mfcc[k]-mean)/sd;
    // fprintf(pFile, "%f \n",mfcc[k]);
    }
  // fclose(pFile);
    // check mean and var...
    //double mean1=0;
    //double var1=0;
    //double sd,sd1;
   // for(int k=0;k<N;k++)
  //  {
//        mean1=mean1+mfcc[k];
        //var1=var1+pow(mfcc[k]-mean,2)/N;

    //}
    //mean1=mean1/N;
    //for(int k=0;k<N;k++)
    //{
    //    var1=var1+pow(mfcc[k]-mean1,2)/(N-1);
        
    //}
    //sd=sqrt(var);
    //sd1=sqrt(var1);

    
}