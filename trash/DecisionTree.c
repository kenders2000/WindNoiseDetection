//
//  DecisionTree.c
//  The Good Audio Recorder
//
//  Created by kenders on 10/09/2013.
//  Copyright (c) 2013 The University of Salford. All rights reserved.
//

#include <stdio.h>
#include <stdbool.h>


//#include "baggedtree_level_1_tree12_20000_all_return_levelbal_pruned.h"
//#include "baggedtree_snr_1_tree12_20000_all_return_levelbal_pruned.h"
//#include "baggedtree_level_10_tree12_20000_all_return_levelbal.h"
//#include "baggedtree_snr_10_tree12_20000_all_return_levelbal.h"

 
static int  noBags;
static int  *noBranches;
static int  *maxBrancheLen;
static int  **branchLengths;
static int***branchLogic;
static float  ***branchValues;
static int  ***branchVectI;
static int  **classLabels;
static int  noClasses;



void loadDecTree(fileprefix,int filenameLen)
{
    readTextFilesTrees(filenameLen);

    
}

void readTextFilesTrees(char * fileprefix,int filenameLen) {
    int i, j, bagi;
    int noBranchesTmp = 0;
    int maxBrancheLenTmp = 0;
    char str [50];

    int maxnoBranchesTmp = 0;
    int maxmaxBrancheLenTmp = 0;

    
    noClasses=malloc(sizeof(int)*NTrees);
    noBags=malloc(sizeof(int)*NTrees);

    //First determin the number of bags
    FILE * pFile;
    // pFile = fopen("dectrees/snrClass_bag_1.txt", "r");
      sprintf(str, "%s_bag_1.txt", fileprefix);
      pFile = fopen(str);

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
    for (bagi = 0; bagi < noBagsSNR; bagi++) {

      sprintf(str, "%s_bag_%i.txt", fileprefix, bagi + 1);

      


        pFile = fopen(str, "r");

        fgets(mystring, sizeof (mystring), pFile); // puts (mystring);
        fgets(mystring, sizeof (mystring), pFile); //puts (mystring);
        // sscanf( mystring, "%i", &noClasses);//printf(" %i \n",noBranches);

        fgets(mystring, sizeof (mystring), pFile); //puts (mystring);
        fgets(mystring, sizeof (mystring), pFile); // puts (mystring);
        sscanf(mystring, "%i", &noBagsSNR); //printf(" %i \n",noBags);

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
    noBranchesSNR = malloc(sizeof (int)*noBagsSNR);
    maxBrancheLenSNR = malloc(sizeof (int)*noBagsSNR);

    branchLengthsSNR = (int**) malloc(noBagsSNR * sizeof (int*));
    for (i = 0; i < noBagsSNR; i++)
        branchLengthsSNR[i] = malloc(sizeof (int)*maxnoBranchesTmp);

    branchLogicSNR = (int***) malloc(noBagsSNR * sizeof (int**));
    for (i = 0; i < noBagsSNR; i++) {
        branchLogicSNR[i] = (int**) malloc(maxnoBranchesTmp * sizeof (int*));
        for (j = 0; j < maxnoBranchesTmp; j++)
            branchLogicSNR[i][j] = malloc(maxmaxBrancheLenTmp * sizeof (int));
    }

    branchValuesSNR = (float***) malloc(maxnoBranchesTmp * sizeof (float**));
    for (i = 0; i < noBagsSNR; i++) {
        branchValuesSNR[i] = (float**) malloc(maxnoBranchesTmp * sizeof (float*));
        for (j = 0; j < maxnoBranchesTmp; j++) {
            branchValuesSNR[i][j] = malloc(maxmaxBrancheLenTmp * sizeof (float));
        }
    }


    branchVectISNR = (int***) malloc(maxnoBranchesTmp * sizeof (int**));
    for (i = 0; i < noBagsSNR; i++) {
        branchVectISNR[i] = (int**) malloc(maxnoBranchesTmp * sizeof (int*));
        for (j = 0; j < maxnoBranchesTmp; j++) {
            branchVectISNR[i][j] = malloc(maxmaxBrancheLenTmp * sizeof (int));
        }
    }

    classLabelsSNR = (int**) malloc(noBagsSNR * sizeof (int*));
    for (i = 0; i < noBagsSNR; i++)
        classLabelsSNR[i] = malloc(sizeof (int)*maxnoBranchesTmp);



    // now load each bag
    for (bagi = 0; bagi < noBagsSNR; bagi++) {




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
        sscanf(mystring, "%i", &noBranchesSNR[bagi]); //printf(" %i \n",noBranches);

        fgets(mystring, sizeof (mystring), pFile);  // puts (mystring);
        fgets(mystring, sizeof (mystring), pFile); // puts (mystring);
        sscanf(mystring, "%i", &maxBrancheLenSNR[bagi]); //printf(" %i \n",maxBrancheLen);

        //Read branch lengths
        fgets(mystring, sizeof (mystring), pFile); //  puts (mystring);
        //fgets (mystring , sizeof(mystring) , pFile);  puts (mystring);

        for (i = 0; i < noBranchesSNR[bagi]; i++) {
            fscanf(pFile, "%d", &branchLengthsSNR[bagi][i]);
            // printf("%i\n",branchLengths[i]);
        }
        fgets(mystring, sizeof (mystring), pFile); //  skip to end of line


        //Read branch Logic

        fgets(mystring, sizeof (mystring), pFile); //  puts (mystring);
        for (i = 0; i < noBranchesSNR[bagi]; i++) {
            for (j = 0; j < branchLengthsSNR[bagi][i]; j++) {
                int test;
                fscanf(pFile, "%d", &test); //&branchLogic[i][j]
                //printf("%i\n",test);
                branchLogicSNR[bagi][i][j] = test;
            }
            fgets(mystring, sizeof (mystring), pFile); //puts (mystring); //  skip to end of line

        }


        //Read branch Values
        fgets(mystring, sizeof (mystring), pFile); //  puts (mystring);
        for (i = 0; i < noBranchesSNR[bagi]; i++) {
            for (j = 0; j < branchLengthsSNR[bagi][i]; j++) {
                float test;
                fscanf(pFile, "%f", &test); //&branchLogic[i][j]
                //printf("%f\n",test);
                branchValuesSNR[bagi][i][j] = test;
            }
            fgets(mystring, sizeof (mystring), pFile); //puts (mystring); //  skip to end of line

        }



        //Read branch Vector indicies
        fgets(mystring, sizeof (mystring), pFile); //  puts (mystring);
        for (i = 0; i < noBranchesSNR[bagi]; i++) {
            for (j = 0; j < branchLengthsSNR[bagi][i]; j++) {
                int test;
                fscanf(pFile, "%d", &test); //&branchLogic[i][j]
                //printf("%d\n",test);
                branchVectISNR[bagi][i][j] = test;
            }
            fgets(mystring, sizeof (mystring), pFile); //puts (mystring); //  skip to end of line
        }





        //Read classLabels
        fgets(mystring, sizeof (mystring), pFile); //  puts (mystring);
        //fgets (mystring , sizeof(mystring) , pFile);  puts (mystring);
        for (i = 0; i < noBranchesSNR[bagi]; i++) {
            fscanf(pFile, "%d", &classLabelsSNR[bagi][i]);
            //printf("%i\n",classLabelsSNR[bagi][i]);
        }
        fgets(mystring, sizeof (mystring), pFile); //  skip to end of line



        fclose(pFile);
    }

}

///////////////////////////////////
///////////////////////////////////
///////////////////////////////////


int decisionTreeFun(float *x) {
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
    int*out;
    out = malloc(sizeof (int)*noBagsSNR);
    for (int bagi = 0; bagi < noBagsSNR; bagi++)
        out[bagi] = 0;


    for (int bagi = 0; bagi < noBagsSNR; bagi++) {
        out[bagi] = 0;
        int* result;
        int ** test;
        result = malloc(sizeof (int)*noBranchesSNR[bagi]);
        test = (int**) malloc(noBranchesSNR[bagi] * sizeof (int*));
        for (int i = 0; i < noBranchesSNR[bagi]; i++)
            test[i] = malloc(maxBrancheLenSNR[bagi] * sizeof (int));

        int bi, ni;

        for (bi = 0; bi < noBranchesSNR[bagi]; bi++) {
            result[bi] = 1;

            for (ni = 0; ni < branchLengthsSNR[bagi][bi]; ni++) {
                //printf("%f ",branchValues[bi][ni]);

                test[bi][ni] = 0;
                if (branchLogicSNR[bagi][bi][ni] == 1) {
                    if (x[branchVectISNR[bagi][bi][ni] - 1] < branchValuesSNR[bagi][bi][ni]) {
                        test[bi][ni] = 1;
                    }
                }

                if (branchLogicSNR[bagi][bi][ni] == 0) {
                    if (x[branchVectISNR[bagi][bi][ni] - 1] >= branchValuesSNR[bagi][bi][ni]) {
                        test[bi][ni] = 1;
                    }
                }
                result[bi] = result[bi] * test[bi][ni];
                // printf("%i ",test[bi][ni]);

            }
            //  printf("\n %i ",bi+1);

        }


        for (int i = 0; i < noBranchesSNR[bagi]; i++) {
            //  printf("%i \n",result[i]);
            if (result[i] == 1) {
                out[bagi] = classLabelsSNR[bagi][i];
            }
        }
        //class=Class(find(result==1));
         //printf("%i \n",out[bagi]);
    }

    // now provide the class estimate that is most popular
    int * count;
    count = (int*) malloc(sizeof (int)*noClassesSNR);
    int * ClassTest;
    ClassTest = (int*) malloc(sizeof (int)*noClassesSNR);
    for (int j = 0; j < noClassesSNR; j++)
        ClassTest[j] = j;
    for (int i = 0; i < noClassesSNR; i++)
        count[i] = 0;


    for (int i = 0; i < noBagsSNR; i++) {
        // printf("%i \n",out[i]);
        for (int j = 0; j < noClassesSNR; j++) {
            if (out[i] == ClassTest[j])
                count[j] = count[j] + 1;

        }

    }
    int output = 0;
    int max = 0;
    for (int j = 0; j < noClassesSNR; j++) {
        if (count[j] >= max) {
            output = j;
            max = count[j];
        }
     //   printf("%i \n",count[j]);
    }
  //  printf("\n");
   // printf("%i \n",output);
    return output;


}