//
//  aweighting.c
//  The Good Audio Recorder
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
*///

#include <stdio.h>
#include <math.h>
//#import "Helper.h"
static float *AWfun;

void aWeighting(float * f, float * a ,int Len, float * rms,int fftn )
{
    // Compute AW input is the magnitude (a)
//%
//W_dB = Aw(f)
//%
//% Evaluates A weighting in dB for each frequency in f
//% according to equation in ISO EN 61672-1:2003

//% constants (section 5.4.11)
//double f1 = 20.60; //% Hz
//double f2 = 107.7; //% Hz
//double f3 = 737.9;// % Hz
//double f4 = 12194;// % Hz
//double f1000 = 1000;
//    double freq;
    
//double W1000 = (pow(f4,2)) * (pow(f1000,4)) / ((pow(f1000,2) + pow(f1,2)) * sqrt(pow(f1000,2) + pow(f2,2)) *
  //                                                                            sqrt(pow(f1000,2) + pow(f3,2)) * (pow(f1000,2) + pow(f4,2)));
    float P=0;
    float PLin=0;

   // int Len=sizeof(&f)/sizeof(double);
    int i;
    for ( i=1;i<fftn/2+1;i++)
    {
//        freq=f[i];
         //AWfun[i] = (pow(f4,2)) * (pow(freq,4)) / ((pow(freq,2) + pow(f1,2)) * sqrt(pow(freq,2) + pow(f2,2)) *
         //                                         sqrt(pow(freq,2) + pow(f3,2)) * (pow(freq,2) + pow(f4,2)))/W1000   ;
        P=P+pow(AWfun[i]*a[i],2);
        PLin=PLin+pow(a[i],2);

        //printf("%f\n,",AWfun[i]);
    }
    rms[0]=sqrt(P/((float)fftn));
    rms[1]=sqrt(PLin/((float)fftn));


}

void aWeightingPreComp(float * f,int fftn)
{
    //%
    //W_dB = Aw(f)
    //%
    //% Evaluates A weighting in dB for each frequency in f
    //% according to equation in ISO EN 61672-1:2003
    
    //% constants (section 5.4.11)
    float f1 = 20.60; //% Hz
    float f2 = 107.7; //% Hz
    float f3 = 737.9;// % Hz
    float f4 = 12194;// % Hz
    float f1000 = 1000;
    float freq;
    AWfun = (float*)malloc((int)(fftn ) * sizeof(float));

    // double AWfun[fftn/2];
    float W1000 = (pow(f4,2)) * (pow(f1000,4)) / ((pow(f1000,2) + pow(f1,2)) * sqrt(pow(f1000,2) + pow(f2,2)) *
                                                   sqrt(pow(f1000,2) + pow(f3,2)) * (pow(f1000,2) + pow(f4,2)));
    // int Len=sizeof(&f)/sizeof(double);
    int i;
    for ( i=1;i<fftn;i++)
    {
        
        freq=f[i];
        AWfun[i] = (pow(f4,2)) * (pow(freq,4)) / ((pow(freq,2) + pow(f1,2)) * sqrt(pow(freq,2) + pow(f2,2)) *
                                                  sqrt(pow(freq,2) + pow(f3,2)) * (pow(freq,2) + pow(f4,2)))/W1000   ;
        //P=P+pow(W*ai],2);
    // printf("%f \n",AWfun[i]);

    }
   // return AWfun;

}



//Function to find highest (maximum) value in array
double maximumValue(float* array,int length )
{
    float max = array[0];       // start with max = first element
    int i;
    for( i = 1; i<length; i++)
    {
        if(array[i] > max)
            max = array[i];
    }
    return max;                // return highest value in array
}