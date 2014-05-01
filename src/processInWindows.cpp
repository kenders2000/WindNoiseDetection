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


#include "processInWindows.hpp"

struct wavfile {
    char id[4]; // should always contain "RIFF"
    int32_t totallength; // total file length minus 8
    char type[4]; // should always contain "WAVE "
    char typeId[4]; // should always contain "fmt "
    int32_t fmtchunksize; // format chunk size
    int16_t compression; // 1 pcm 3 ieee
    int16_t nochan; //
    int32_t fs; // fs
    int32_t bytes_per_sec; // byes / sec
    int16_t blockalign; //
    int16_t bits_per_sample; //
} __attribute__((__packed__));

static struct wavfile header;

void loadWav(char * filename, char * outFilename, char *treeDir, float gain, int frameAve,float thresh) {
    char str1[100], str2[100];
    FILE * pFile;


    sprintf(str1, "trees/%s/levelClass", treeDir);
    sprintf(str2, "trees/%s/snrClass", treeDir);
    DTree LevelTree, SNRTree;
    printf("\nLoading Decision Trees");
    LevelTree.readTextFilesTrees(str1);
    SNRTree.readTextFilesTrees(str2);
    printf("\nDone");

    //http://www.sonicspot.com/guide/wavefiles.html
    //http://yannesposito.com/Scratch/en/blog/2010-10-14-Fun-with-wav/char 
    //char  filename[]="test16S.wav";
    //char  filename[]="iphone1.wav";

    FILE * wav;

    wav = fopen(filename, "r");
    //check openable
    int test = sizeof (header);
    if (fread(&header, sizeof (header), 1, wav) < 1) {
        fprintf(stderr, "Can't read input file header %s\n", filename);
        exit(1);
    }
    printf("\nWavefile Header");
    printf("chunk ID %.*s\n", 4, header.id);
    printf("Total length %i\n", header.totallength);
    printf("should say WAVE %.*s\n", 4, header.type);
    printf("should say fmt : %.*s\n", 4, header.typeId);
    printf("format chunk size : %i\n", header.fmtchunksize);
    printf("Compression code %i\n", header.compression);
    printf("No. Channels %i\n", header.nochan);
    printf("Fs  %i \n", header.fs);
    printf("bytes_per_sec  %i \n", header.bytes_per_sec);
    printf("blockalign  %i\n", header.blockalign);
    printf("bits_per_sample  %i\n", header.bits_per_sample);

    if (header.fs != 44100.0) {
        fprintf(stderr, "Input must be sampled at 44100 Hz \n");
        exit(1);
    }
    // Skip though rest of header to find data 
    char datahead[4];
    int32_t datasize;
    int skip = 0;
    while (skip == 0) {
        fread(&datahead, sizeof (datahead), 1, wav);
        //  Skips through unitll it finds 'data'
        skip = memcmp(datahead, "data", 4) == 0;
        if (skip == 0) {
            fseek(wav, -3, SEEK_CUR);
            //printf("should say data %.*s\n", 4, datahead);
        }
    }

    // read data header 
    // printf("should say data %.*s\n", 4, datahead);
    fread(&datasize, sizeof (int32_t), 1, wav);

    printf("datasize %i\n", datasize);
    int32_t NoBlocks = datasize / (header.bits_per_sample / 8 * header.nochan);

    // allocate memory for time domain items
    float *window;
    float *windowOver;
    float *windowPrev;
    window = (float*) malloc(sizeof (float)*WIN_N);
    windowOver = (float*) malloc(sizeof (float)*WIN_N);
    windowPrev = (float*) malloc(sizeof (float)*WIN_N);

    float * mfcc = (float*) malloc((16) * sizeof (float));
    float * mfccO = (float*) malloc((16) * sizeof (float));
    float * mfccP = (float*) malloc((16) * sizeof (float));

    float *feats;
    feats = (float*) malloc(sizeof (float)*12 * 3);

    int wN = 0;
    int wNo = WIN_N / 2;

    sprintf(str2, "%s.b", outFilename);

    remove(str2);

    pFile = fopen(str2, "w");



    //if the wav is 16 bit PCM
    int counter;
    int classLevel;
    int classSNR;
    int i;
    int frame = 0;
    int sumclassSNR = 0;
    int sumclassLevel = 0;
    float avermsAW = 0;
    initVarsFeats(WIN_N, header.fs);
    for (i = 0; i < (WIN_N); i++)
        window[i] = 0;

    if (1 == 1)//(header.bits_per_sample==16 && header.compression==1)
    {



        int i, c, n;
        for (i = 0; i < (NoBlocks); i++) {
            //channelsum[i]=0;
            for (c = 0; c < (header.nochan); c++) {
                float tmp2;
                float normV;
                if (header.bits_per_sample == 16 && header.compression == 1) {
                    int16_t tmp;
                    fread(&tmp, sizeof (int16_t), 1, wav);
                    tmp2 = (float) tmp;
                    normV = (65535.0 / 2.0);
                } else if (header.bits_per_sample == 32 && header.compression == 1) {
                    int32_t tmp = 0;
                    fread(&tmp, sizeof (int32_t), 1, wav);
                    tmp2 = (float) tmp;
                    normV = (2147483648.0);
                } else if (header.bits_per_sample == 32 && header.compression == 3) {
                    float tmp = 0;
                    fread(&tmp, sizeof (float), 1, wav);
                    tmp2 = tmp;
                    normV = 1; //(16777216.0/2.0);
                } else {
                    printf("Supported formats are 16 and 32 bit signed integer, and IEEE float little-endian - any number of channels");
                    break;
                }


                float tmp3 = (tmp2 / (float) header.nochan) / normV * 158489.0 * gain; //*  158489=  10^(104/20)
                window[wN] = window[wN] + tmp3;
                // printf("%f \n",window[wN]);
                windowOver[wNo] = window[wN];

                //fprintf(pFile,"%f \n",channelsum[i]);
            } //65,535
            //close(pFile);

            //every WIN_N samples do this
            wN++;
            if (wN == WIN_N) {
                for (n = 0; n < WIN_N / 2; n++)
                    windowOver[n] = windowPrev[n];
                for (n = (WIN_N) / 2; n < WIN_N; n++)
                    windowOver[n] = window[n];
                computeMFCCs(window, WIN_N, header.fs, mfcc);
                // for ( i=0;i<12;i++)         printf("%f \n",mfcc[i]);
                computeMFCCs(windowOver, WIN_N, header.fs, mfccO);
                computeMFCCs(windowPrev, WIN_N, header.fs, mfccP);


                for (n = 0; n < 36; n++) {
                    if (n < 12) {
                        feats[n] = mfccP[n];
                    } else if (n < 24) {
                        feats[n] = mfccO[n - 12];
                    } else {
                        feats[n] = mfcc[n - 24];
                    }
                    //  printf("%f \n",feats[n]);
                }

                if (frame > 3) {

                    classLevel = LevelTree.decisionTreeFun(feats);
                    classSNR = SNRTree.decisionTreeFun(feats);
                    sumclassLevel = sumclassLevel + classLevel;
                    sumclassSNR = sumclassSNR + classSNR;
                    avermsAW = avermsAW + pow(10, mfcc[0] / 10.0);
                    counter++;
                }
                int n;
                if ((frame % frameAve) == 0 && frame > 1) {
                    float avedBA = 10.0 * log10(avermsAW / (float) counter);
                    float aveLevel = sumclassLevel / (float) counter;
                    float aveSNR = sumclassSNR / (float) counter;
                    float time = (float) WIN_N / header.fs * (float) frame - (float) WIN_N / header.fs / 2.0;
                    float tmp = 1;
                    float Combined = 0;
                    if (aveLevel > 1) {
                        tmp = 1.0 / 5.0;
                        Combined = (5 - aveSNR) * tmp;
                    }
                    printf(" %0.2f %0.0f %0.2f %0.2f %0.2f\n", time, avedBA, aveLevel, aveSNR, Combined);
                    fprintf(pFile, " %0.2f %0.0f %0.2f %0.2f %0.2f\n", time, avedBA, aveLevel, aveSNR, Combined);

                    sumclassLevel = 0;
                    sumclassSNR = 0;
                    counter = 0;
                    avermsAW = 0;
                    // printf("%f %i %i %f\n",mfcc[0],classLevel,classSNR,(float)(5-classSNR)/5.0*3.0*(float)classLevel);
                }

                //reset counters /vectors
                for (n = 0; n < 12; n++)
                    mfccP[n] = mfcc[n];
                for (n = 0; n < WIN_N; n++)
                    windowPrev[n] = window[n];
                wN = 0;
                frame++;
                for (n = 0; n < (WIN_N); n++)
                    window[n] = 0;
            }



            // printf("%f \n",channelsum[i]);
            //fprintf(pFile,"%f \n",channelsum[i]);

        }
    }
    fclose(pFile);


    // Global stats
    float aveAveLevel = 0;
    float maxAveLevel = 0;
    float minAveSNR = 5;
    float aveAveSNR = 0;
    //float thresh = 0.2;
    float avecomb = 0;
    float maxcomb = 0;
    float count_wF5 = 0;
    float count_wF4 = 0;
    float count_wF3 = 0;
    float count_wF2 = 0;
    float count_wF1 = 0;
    float count_wF0 = 0;
    counter = 0;
    //  pFile2 = fopen ( 'tmp.txt' , "w");
    pFile = fopen(str2, "r");
    char mystring[1000];
    while (fgets(mystring, sizeof (mystring), pFile) != NULL) {

        float dBA, t, level, snr, comb;
        sscanf(mystring, "%f %f %f %f %f", &t, &dBA,
                &level, &snr, &comb);

        if (level > maxAveLevel)
            maxAveLevel = level;

        if (snr < minAveSNR)
            minAveSNR = snr;

        if (comb > maxcomb)
            maxcomb = comb;

        avecomb = avecomb + comb;
        aveAveSNR = aveAveSNR + snr;
        aveAveLevel = aveAveLevel + level;

        if (level < 1)
            count_wF0++;


        if (snr >= 0 && snr < 1 && level >= 1)
            count_wF5++;
        if (snr >= 1 && snr < 2 && level >= 1)
            count_wF4++;
        if (snr >= 2 && snr < 3 && level >= 1)
            count_wF3++;
        if (snr >= 3 && snr < 4 && level >= 1)
            count_wF2++;
        if (snr >= 4 && snr < 5 && level >= 1)
            count_wF1++;



        //printf("%f %f %f %f %f \n", t, dBA, 
        //       level,snr, comb);
        counter++;
    }
    fclose(pFile);
    count_wF0 = count_wF0 / counter * 100.0;
    count_wF1 = count_wF1 / counter * 100.0;
    count_wF2 = count_wF2 / counter * 100.0;
    count_wF3 = count_wF3 / counter * 100.0;
    count_wF4 = count_wF4 / counter * 100.0;
    count_wF5 = count_wF5 / counter * 100.0;

    avecomb = avecomb / counter;
    aveAveSNR = aveAveSNR / counter;
    aveAveLevel = aveAveLevel / counter;
    
     // remove(outFilename);

    FILE * pFile2;
    pFile2 = fopen(outFilename, "w");
    pFile = fopen(str2, "r");
    fprintf(pFile2, "Microphone Wind Noise Detection - University of Salford - the Good Recording Project http://www.goodrecording.net \n\n", filename);
    fprintf(pFile2, "Microphone Wind noise Analysis for input file %s\n\n", filename);
    fprintf(pFile2, "Wind Noise Statistics, %% number of frames with wind noise detected at the following Signal to noise Ratios (high values = good quality low values = bad quality). also presented is the range in Quality Degradation that each SNR range represents.\n\n");
    fprintf(pFile2, "SNR range;\t>20dB,\t20 to 10dB,\t10 to 0dB,\t0 to -10dB,\t-10 to -20dB,\t<-20dB\n", count_wF0, count_wF1, count_wF2, count_wF3, count_wF4, count_wF5);
    fprintf(pFile2, "Qual. Degradation;\t<24%%,\t24 to 40%%,\t40 to 54%%,\t54 to 69%%,\t69 to 95%%,\t>95%%\n");
    fprintf(pFile2, "%% of time in each SNR range;\t%0.1f,\t%0.1f,\t%0.1f,\t%0.1f,\t%0.1f,\t%0.1f\n", count_wF0, count_wF1, count_wF2, count_wF3, count_wF4, count_wF5);
    fprintf(pFile2, "\n");
    fprintf(pFile2, "Wind noise time history\n\n");
    fprintf(pFile2, "T(s)\t\tQuality Degradation(%%)\t dBA \n");

    while (fgets(mystring, sizeof (mystring), pFile) != NULL) {

        float dBA, t, level, snr, comb;
        sscanf(mystring, "%f %f %f %f %f;", &t, &dBA,
                &level, &snr, &comb);
        float x=snr;
        float qual= 0.0093616465*pow(x,9) + -0.2408574076*pow(x,8) + 2.6409491876*pow(x,7) + -16.0598328947*pow(x,6) + 59.1080787296*pow(x,5) + -135.0673590478*pow(x,4) + 189.6816801547*pow(x,3) + -156.8624014598*pow(x,2) + 83.7429623557*pow(x,1) + 4.5226369050;
        if (comb==0)
            qual=100;

        fprintf(pFile2, "%4.1f\t\t%0.0f\t\t\t\t\t\t%4.0f\n", t, 100-qual, dBA);
        counter++;
    }

    ///////////////////////////// Find contiguous regions without wind noise
    fclose(pFile);
    pFile = fopen(str2, "r");
    int clean = 1; // 
    int initialised = 0;
    int first = 0;
    float win = 0;
    float start = 0;
    fprintf(pFile2, "\nWind free regions from - to (s) using a Threshold of %2.0f\n\n",thresh);

    while (fgets(mystring, sizeof (mystring), pFile) != NULL) {

        float dBA, t, level, snr, comb;
        
        sscanf(mystring, "%f %f %f %f %f", &t, &dBA,
                &level, &snr, &comb);
               float x=snr;
        float qual= 0.0093616465*pow(x,9) + -0.2408574076*pow(x,8) + 2.6409491876*pow(x,7) + -16.0598328947*pow(x,6) + 59.1080787296*pow(x,5) + -135.0673590478*pow(x,4) + 189.6816801547*pow(x,3) + -156.8624014598*pow(x,2) + 83.7429623557*pow(x,1) + 4.5226369050;
        if (comb==0)
            qual=100;
    comb=100-qual;
        
        if (comb >= thresh && clean == 1 && initialised == 1 && first == 1) // previous frame was clean now its windy
        {
            fprintf(pFile2, "%0.2f\t%0.2f\n", start, t - win * 2);
            clean = 0;

        } else if (comb < thresh && clean == 0 && initialised == 1) // previous frame was windy now its clean
        {
            start = t - win * 2;
            clean = 1;
            first = 1;
        }

        if (initialised == 0) {
            win = t / 2;

            if (comb >= thresh) {
                clean = 0;

            } else {
                clean = 1;
                first = 1;
                start = t - win * 2;
            }
            initialised = 1;
        }
        counter++;
    }
    ///////////////////////////

    fclose(pFile);
    fclose(pFile2);
    //remove(str2);

}

