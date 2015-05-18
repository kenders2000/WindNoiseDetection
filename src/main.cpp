/* 
 * File:   main.cpp
 * Author: ags056
 *
Copyright (c) <2014> <Paul Kendrick>

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
//http://www.thegeekstuff.com/2013/01/c-argc-argv/

//#include <cstdlib>
#include "processInWindows.hpp"
#include <string>
//#include <stdio.h>
extern "C" {
#include <unistd.h>
#include <stdio.h>

}

using namespace std;
using std::string;
void replaceExt(string& s, const string& newExt) {
   string::size_type i = s.rfind('.', s.length());
   if (i != string::npos) {
      s.replace(i+1, newExt.length(), newExt);
   }
}

//void loadWav();

/*
 * 
 */
int main(int argc, char* argv[]) {
    int testForIO = 0;
        int status;
    // char * inputFile = "iphone1.wav";
    // char * outputFile = "iphone1.wav";
    int opt = 0;
    char *in_fname = (char *)"iphone1.wav";
    char *out_fname = (char *)"iphone1.txt";
    const char *json_fname = (char *)"iphone1.json";
    char *trees[3];
    float thresh=25;
    trees[0] =(char *)"dectrees_10_5000";
    trees[1] = (char *)"dectrees_10_20000";
    trees[2] = (char *)"dectrees_10_100000";
    int tree = 1;
    char *treeDir;
    treeDir = trees[1];
    int treeFail = 0;
    float gain = 1;
    int frameAve = 43;
    int verbose=0;
    char *treeFileLoc = (char *)"trees";
    while ((opt = getopt(argc, argv, "i:o:t:g:f:h:w:j:v:l:")) != -1) {
        switch (opt) {
                        case 'l':{
                treeFileLoc= optarg;
            break;}
             case 'w':{
                thresh = atof(optarg);
                //printf("\nThreshold=%0.0f", thresh);
                //thresh=thresh;
                break;}
            case 'v':{
                verbose = atof(optarg);

                break;}
            case 'g':{
                gain = atof(optarg);
                //printf("\nGain=%f", gain);
                break;}
            case 'f':{
                frameAve = atoi(optarg);
                //printf("\nAnalysis period=%f ms", (float) (frameAve * WIN_N) / 44100.0 * 1000);
                break;}
            case 'h':{
                printf("\nwindnoisedetection.exe -i wav_filename -o output_filename [-t 1] [-g 1] [-f 43]");
                printf("\n-i and -o are required parameters, they provide the input .wav filename and the output filename respectively. ");
                printf("\n-t n, sets the tree size to n, where n is an integer (1,2 or 3). The default is 1.");
                printf("\n-g 1, sets the gain to apply. The default is 1.");
                printf("\n-f n, set the size of the analysis window, windows are 1024 samples.  The default is 43.");
                break;}
            case 'i':{
                in_fname = optarg;
                //printf("\nInput file =%s", in_fname);
                break;}
            case 'o':{
                out_fname = optarg;
                //printf("\nOutput file=%s", out_fname);
//                std::string Str = std::string(optarg);
//                replaceExt(Str, "json");
//                json_fname= Str.c_str();
//                printf("\nOutput file=%s", json_fname);

                break;}
            case 'j':{
                json_fname = optarg;
                //printf("\nOutput file=%s", json_fname);

                break;}
            case 't':{
                tree = atoi(optarg) - 1;
                if (tree < 3) {
                    treeDir = trees[tree];
                    //printf("\nUsing %s", trees[tree]);
                } else {
                    treeFail = 1;
                    //printf("\nMust be a number between 1 and 3 inclusive");

                }

                break;}



            case '?':{
                /* Case when user enters the command as
                 * $ ./cmd_exe -i
                 */
                if (optopt == 'i') {
                    //printf("\nMissing  input file");
                    /* Case when user enters the command as
                     * # ./cmd_exe -o
                     */
                } else if (optopt == 'o') {
                    //printf("\nMissing  output file");
                } else {
                    //printf("\nInvalid option received");
                }
                //printf("\n");
                break;}
        }
        testForIO = 1;
    }




    if (testForIO == 0 || treeFail == 1) {
        if (verbose ==1)
        {
        printf("\nIncorrect or missing input parameters");
        printf("\nwindnoisedetection.exe -i wav_filename -o output_filename [-t 1] [-g 1] [-f 43]");
        printf("\n-i and -o are required parameters, they provide the input .wav filename and the output filename respectively. ");
        printf("\n-t n, sets the tree size to n, where n is an integer (1,2 or 3). The default is 1.");
        printf("\n-g 1, sets the gain to apply. The default is 1.");
        printf("\n-f n, set the size of the analysis window, windows are 1024 samples.  The default is 43.");printf("\n");
        }
        status=0;
    }else
    {    
        //
       status= loadWav(in_fname, out_fname,json_fname, treeDir, gain, frameAve,thresh,verbose,treeFileLoc);
    }
   if (status==1)
           exit (EXIT_FAILURE);
       //printf("\nError!");
     if (status==0)
         exit(status);
      //printf("\nSuccess!");

    return status;
}

