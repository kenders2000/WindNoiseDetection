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

#include "DecisionTreeClass.hpp"

void DTree::readTextFilesTrees (char * fileprefixIn) {
    fileprefix=fileprefixIn;
//    filenameLen=filenameLenIn;
    int i, j, bagi;
    int noBranchesTmp = 0;
    int maxBrancheLenTmp = 0;
    char str [50];

    int maxnoBranchesTmp = 0;
    int maxmaxBrancheLenTmp = 0;

    

    //First determin the number of bags
    FILE * pFile;
    // pFile = fopen("dectrees/snrClass_bag_1.txt", "r");
      sprintf(str, "%s_bag_1.txt", fileprefix);
      pFile = fopen(str,"r");

    if (pFile == NULL) {
        puts("Cannot open source file");
    }
    char mystring [10000];

    fgets(mystring, sizeof (mystring), pFile); // puts (mystring);
    fgets(mystring, sizeof (mystring), pFile); //puts (mystring);
    sscanf(mystring, "%i", &noClasses); //printf(" %i \n",noBranches);

    fgets(mystring, sizeof (mystring), pFile); //  puts (mystring);
    fgets(mystring, sizeof (mystring), pFile); // puts (mystring);
    sscanf(mystring, "%i", &noBags); //printf(" %i \n",noBags);
    fclose(pFile);

    ////////////////////////////////////
    ///////////////////////  Load all sizes of arrays
    for (bagi = 0; bagi < noBags; bagi++) {

      sprintf(str, "%s_bag_%i.txt", fileprefix, bagi + 1);

      


        pFile = fopen(str, "r");

        fgets(mystring, sizeof (mystring), pFile); // puts (mystring);
        fgets(mystring, sizeof (mystring), pFile); //puts (mystring);
        // sscanf( mystring, "%i", &noClasses);//printf(" %i \n",noBranches);

        fgets(mystring, sizeof (mystring), pFile); //puts (mystring);
        fgets(mystring, sizeof (mystring), pFile); // puts (mystring);
        sscanf(mystring, "%i", &noBags); //printf(" %i \n",noBags);

        fgets(mystring, sizeof (mystring), pFile); // puts (mystring);
        fgets(mystring, sizeof (mystring), pFile); //puts (mystring);
        sscanf(mystring, "%i", &noBranchesTmp); //printf(" %i \n",noBranches);

        fgets(mystring, sizeof (mystring), pFile); //  puts (mystring);
        fgets(mystring, sizeof (mystring), pFile); //  puts (mystring);
        sscanf(mystring, "%i", &maxBrancheLenTmp); //printf(" %i \n",maxBrancheLen);

        if (noBranchesTmp > maxnoBranchesTmp)
            maxnoBranchesTmp = noBranchesTmp;
        if (maxBrancheLenTmp > maxmaxBrancheLenTmp)
            maxmaxBrancheLenTmp = maxBrancheLenTmp;

        fclose(pFile);
    }


    ///  NOW allocate memory for decision trees
    
    noBranches=new int[noBags];
    maxBrancheLen = new int[noBags];
    
    branchLengths=new int*[noBags];
    for (i=0;i<noBags;i++){branchLengths[i]=new int[maxnoBranchesTmp];}
            
    
    
    branchLogic=new int** [noBags];
     for (i=0;i<noBags;i++){
         branchLogic[i]=new int*[maxnoBranchesTmp];
         for (j=0;j<maxnoBranchesTmp;j++)
         branchLogic[i][j]=new int[maxmaxBrancheLenTmp];
     }

    
        branchValues=new float** [noBags];
     for (i=0;i<noBags;i++){
         branchValues[i]=new float*[maxnoBranchesTmp];
         for (j=0;j<maxnoBranchesTmp;j++)
         branchValues[i][j]=new float[maxmaxBrancheLenTmp];
     }
        
            branchVectI=new int** [noBags];
     for (i=0;i<noBags;i++){
         branchVectI[i]=new int*[maxnoBranchesTmp];
         for (j=0;j<maxnoBranchesTmp;j++)
         branchVectI[i][j]=new int[maxmaxBrancheLenTmp];
     }
            
            
               classLabels=new int*[noBags];
    for (i=0;i<noBags;i++){classLabels[i]=new int[maxnoBranchesTmp];}


    
 

    // now load each bag
    for (bagi = 0; bagi < noBags; bagi++) {




      sprintf(str, "%s_bag_%i.txt", fileprefix, bagi + 1);
     

        pFile = fopen(str, "r");

        if (pFile == NULL) {
            puts("Cannot open source file");
        }
        fgets(mystring, sizeof (mystring), pFile); // puts (mystring);
        fgets(mystring, sizeof (mystring), pFile); //puts (mystring);
        // sscanf( mystring, "%i", &noClasses);//printf(" %i \n",noBranches);


        fgets(mystring, sizeof (mystring), pFile);// puts (mystring);
        fgets(mystring, sizeof (mystring), pFile); // puts (mystring);
        //sscanf( mystring, "%i", &noBags);//printf(" %i \n",noBags);

        fgets(mystring, sizeof (mystring), pFile);  //puts (mystring);
        fgets(mystring, sizeof (mystring), pFile);// puts (mystring);
        sscanf(mystring, "%i", &noBranches[bagi]); //printf(" %i \n",noBranches);

        fgets(mystring, sizeof (mystring), pFile);  // puts (mystring);
        fgets(mystring, sizeof (mystring), pFile); // puts (mystring);
        sscanf(mystring, "%i", &maxBrancheLen[bagi]); //printf(" %i \n",maxBrancheLen);

        //Read branch lengths
        fgets(mystring, sizeof (mystring), pFile); //  puts (mystring);
        //fgets (mystring , sizeof(mystring) , pFile);  puts (mystring);

        for (i = 0; i < noBranches[bagi]; i++) {
            fscanf(pFile, "%d", &branchLengths[bagi][i]);
            // printf("%i\n",branchLengths[i]);
        }
        fgets(mystring, sizeof (mystring), pFile); //  skip to end of line


        //Read branch Logic

        fgets(mystring, sizeof (mystring), pFile); //  puts (mystring);
        for (i = 0; i < noBranches[bagi]; i++) {
            for (j = 0; j < branchLengths[bagi][i]; j++) {
                int test;
                fscanf(pFile, "%d", &test); //&branchLogic[i][j]
                //printf("%i\n",test);
                branchLogic[bagi][i][j] = test;
            }
            fgets(mystring, sizeof (mystring), pFile); //puts (mystring); //  skip to end of line

        }


        //Read branch Values
        fgets(mystring, sizeof (mystring), pFile); //  puts (mystring);
        for (i = 0; i < noBranches[bagi]; i++) {
            for (j = 0; j < branchLengths[bagi][i]; j++) {
                float test;
                fscanf(pFile, "%f", &test); //&branchLogic[i][j]
                //printf("%f\n",test);
                branchValues[bagi][i][j] = test;
            }
            fgets(mystring, sizeof (mystring), pFile); //puts (mystring); //  skip to end of line

        }



        //Read branch Vector indicies
        fgets(mystring, sizeof (mystring), pFile); //  puts (mystring);
        for (i = 0; i < noBranches[bagi]; i++) {
            for (j = 0; j < branchLengths[bagi][i]; j++) {
                int test;
                fscanf(pFile, "%d", &test); //&branchLogic[i][j]
                //printf("%d\n",test);
                branchVectI[bagi][i][j] = test;
            }
            fgets(mystring, sizeof (mystring), pFile); //puts (mystring); //  skip to end of line
        }





        //Read classLabels
        fgets(mystring, sizeof (mystring), pFile); //  puts (mystring);
        //fgets (mystring , sizeof(mystring) , pFile);  puts (mystring);
        for (i = 0; i < noBranches[bagi]; i++) {
            fscanf(pFile, "%d", &classLabels[bagi][i]);
            //printf("%i\n",classLabelsSNR[bagi][i]);
        }
        fgets(mystring, sizeof (mystring), pFile); //  skip to end of line



        fclose(pFile);
    }
               
    result=new int*[noBags];
    for (i=0;i<noBags;i++){result[i]=new int[maxnoBranchesTmp];}
                         
     test=new int** [noBags];
     for (i=0;i<noBags;i++){
         test[i]=new int*[maxnoBranchesTmp];
         for (j=0;j<maxnoBranchesTmp;j++)
         test[i][j]=new int[maxmaxBrancheLenTmp];
     }      
                         
                         
               return;
}


int DTree::decisionTreeFun(float *x) {
    // loadDecTree();
    //    % x is the vector to be classified
    //% branches is the total number of branches in the tree
    //% storeBranchLength is the length of each branch
    //% x(storeBranchVecI) tells us which item of x is to evaluted at each branch
    //% storeBranchVal stores the vale which x(storeBranchVecI) is to be tested
    //% against
    //% storeBranchLogic tells us wheather the logic test result is 0 or 1,1 is x(storeBranchVecI) < storeBranchVal
    //% 0 is x(storeBranchVecI) >= storeBranchVal
    //#include "baggedtree_snr_10_tree12_20000_all_return_levelbal.h"
    //  static int noBags;
    //  static int noBranches;
    //  static int maxBrancheLen; 
    //  static int *branchLengths;
    //  static int **branchLogic;
    //  static float ** branchValues;
    //  static int **branchVectI;
    //  static int **classLabels;
    //for bi=1:branches
    //for(int i = 0; i < 36; i++)
    // printf("%f \n",x[i]);
   // int*out;
    //int* result;
    //int ** test;
    
    
     int out[noBags];
       for (int bagi = 0; bagi < noBags; bagi++)
        out[bagi] = 0;
   
    //out = new int[noBags];//out = malloc(sizeof (int)*noBags);
    //for (int bagi = 0; bagi < noBags; bagi++)
    //    out[bagi] = 0;

   int j, bagi, bi, ni;

    for ( bagi = 0; bagi < noBags; bagi++) {
        out[bagi] = 0;
        //int result[noBranches[bagi]];
         //   result =(int*) malloc(sizeof (int)*noBranches[bagi]);

         
        //int test[noBranches[bagi]][maxBrancheLen[bagi]];
        // test=   new int*[noBranches[bagi]];
       // for ( i=0; i<noBranches[bagi];i++)
         //    test[i]=new int[maxBrancheLen[bagi]];
                

        for (bi = 0; bi < noBranches[bagi]; bi++) {
            result[bagi][bi] = 1;

            for (ni = 0; ni < branchLengths[bagi][bi]; ni++) {
                //printf("%f ",branchValues[bi][ni]);

                test[bagi][bi][ni] = 0;
                if (branchLogic[bagi][bi][ni] == 1) {
                    if (x[branchVectI[bagi][bi][ni] - 1] < branchValues[bagi][bi][ni]) {
                        test[bagi][bi][ni] = 1;
                    }
                }

                if (branchLogic[bagi][bi][ni] == 0) {
                    if (x[branchVectI[bagi][bi][ni] - 1] >= branchValues[bagi][bi][ni]) {
                        test[bagi][bi][ni] = 1;
                    }
                }
                result[bagi][bi] = result[bagi][bi] * test[bagi][bi][ni];
                // printf("%i ",test[bi][ni]);

            }
            //  printf("\n %i ",bi+1);

        }

        for ( j = 0; j < noBranches[bagi]; j++) 
        {
            //  printf("%i \n",result[i]);
            if (result[bagi][j] == 1) {
                out[bagi] = classLabels[bagi][j];
            }
        }
        //class=Class(find(result==1));
         //printf("%i \n",out[bagi]);
//        delete out;
    }
   
    // now provide the class estimate that is most popular
    int  count[noClasses];
    int ClassTest[noClasses];
    
    for (int j = 0; j < noClasses; j++)
        ClassTest[j] = j;
    for (int i = 0; i < noClasses; i++)
        count[i] = 0;


    for (int i = 0; i < noBags; i++) {
        // printf("%i \n",out[i]);
        for (int j = 0; j < noClasses; j++) {
            if (out[i] == ClassTest[j])
                count[j] = count[j] + 1;

        }

    }
    int output = 0;
    int max = count[0];
    int storeMaxclasses[noClasses];
    for (int j = 1; j < noClasses; j++) {
        if (count[j] > max)   
        {
            output = j;
            max = count[j];
        }
       
     //   printf("%i \n",count[j]);
    }
    
    int countave=0;
        for (int j = 1; j < noClasses; j++) 
        {
            
        if (count[j] == max)   
        {
            storeMaxclasses[countave]=j;
            countave++;
        }
        }
    if (countave == 1)
    {
    }  
    else if (countave ==0)// 0 do nowt
    {
    }
    else if (countave % 2 ==1)// if odd number of same value, select central one
    {
        int in=(countave-1)/2;
        output=storeMaxclasses[in+1];
    }
    else if (countave % 2 ==0)   //even then 
    {
        int in=(countave/2);
        in=in - rand() % 2;
        output=storeMaxclasses[in];
    }
    
    
    //delete out,,test;
    return output;


}