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
static int noBags;
static int *noBranches;
static int *maxBrancheLen;
static int **branchLengths;
static int ***branchLogic;
static float ***branchValues;
static int ***branchVectI;
static int **classLabels;
static int noClasses;

static int noBagsSNR;
static int *noBranchesSNR;
static int *maxBrancheLenSNR;
static int **branchLengthsSNR;
static int ***branchLogicSNR;
static float ***branchValuesSNR;
static int ***branchVectISNR;
static int **classLabelsSNR;
static int noClassesSNR;

static int noBagsLEVEL;
static int *noBranchesLEVEL;
static int *maxBrancheLenLEVEL;
static int **branchLengthsLEVEL;
static int ***branchLogicLEVEL;
static float ***branchValuesLEVEL;
static int ***branchVectILEVEL;
static int **classLabelsLEVEL;
static int noClassesLEVEL;
//int  class_vec[1];


void loadDecTree()
{
    readTextFilesLEVELTrees();
  readTextFilesSNRTrees();
    
}

void readTextFilesSNRTrees() {
    int i, j, bagi;
    int noBranchesTmp = 0;
    int maxBrancheLenTmp = 0;

    int maxnoBranchesTmp = 0;
    int maxmaxBrancheLenTmp = 0;

    //First determin the number of bags
    FILE * pFile;

        pFile = fopen("dectrees/snrClass_bag_1.txt", "r");
  

    if (pFile == NULL) {
        puts("Cannot open source file");
    }
    char mystring [10000];

    fgets(mystring, sizeof (mystring), pFile); // puts (mystring);
    fgets(mystring, sizeof (mystring), pFile); //puts (mystring);
    sscanf(mystring, "%i", &noClassesSNR); //printf(" %i \n",noBranches);

    fgets(mystring, sizeof (mystring), pFile); //  puts (mystring);
    fgets(mystring, sizeof (mystring), pFile); // puts (mystring);
    sscanf(mystring, "%i", &noBagsSNR); //printf(" %i \n",noBags);
    fclose(pFile);

    ////////////////////////////////////
    ///////////////////////  Load all sizes of arrays
    char str [50];
    for (bagi = 0; bagi < noBagsSNR; bagi++) {


            sprintf(str, "dectrees/snrClass_bag_%i.txt", bagi + 1);
      


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




            sprintf(str, "dectrees/snrClass_bag_%i.txt", bagi + 1);
     

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

void readTextFilesLEVELTrees() {
    int i, j, bagi;
    int noBranchesTmp = 0;
    int maxBrancheLenTmp = 0;

    int maxnoBranchesTmp = 0;
    int maxmaxBrancheLenTmp = 0;

    //First determin the number of bags
    FILE * pFile;

        pFile = fopen("dectrees/levelClass_bag_1.txt", "r");
  

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
    char str [50];
    for (bagi = 0; bagi < noBags; bagi++) {


            sprintf(str, "dectrees/levelClass_bag_%i.txt", bagi + 1);
      


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
    noBranches = malloc(sizeof (int)*noBags);
    maxBrancheLen = malloc(sizeof (int)*noBags);

    branchLengths = (int**) malloc(noBags * sizeof (int*));
    for (i = 0; i < noBags; i++)
        branchLengths[i] = malloc(sizeof (int)*maxnoBranchesTmp);

    branchLogic = (int***) malloc(noBags * sizeof (int**));
    for (i = 0; i < noBags; i++) {
        branchLogic[i] = (int**) malloc(maxnoBranchesTmp * sizeof (int*));
        for (j = 0; j < maxnoBranchesTmp; j++)
            branchLogic[i][j] = malloc(maxmaxBrancheLenTmp * sizeof (int));
    }

    branchValues = (float***) malloc(maxnoBranchesTmp * sizeof (float**));
    for (i = 0; i < noBags; i++) {
        branchValues[i] = (float**) malloc(maxnoBranchesTmp * sizeof (float*));
        for (j = 0; j < maxnoBranchesTmp; j++) {
            branchValues[i][j] = malloc(maxmaxBrancheLenTmp * sizeof (float));
        }
    }


    branchVectI = (int***) malloc(maxnoBranchesTmp * sizeof (int**));
    for (i = 0; i < noBags; i++) {
        branchVectI[i] = (int**) malloc(maxnoBranchesTmp * sizeof (int*));
        for (j = 0; j < maxnoBranchesTmp; j++) {
            branchVectI[i][j] = malloc(maxmaxBrancheLenTmp * sizeof (int));
        }
    }

    classLabels = (int**) malloc(noBags * sizeof (int*));
    for (i = 0; i < noBags; i++)
        classLabels[i] = malloc(sizeof (int)*maxnoBranchesTmp);



    // now load each bag
    for (bagi = 0; bagi < noBags; bagi++) {




            sprintf(str, "dectrees/levelClass_bag_%i.txt", bagi + 1);
     

        pFile = fopen(str, "r");

        if (pFile == NULL) {
            puts("Cannot open source file");
        }
        fgets(mystring, sizeof (mystring), pFile); // puts (mystring);
        fgets(mystring, sizeof (mystring), pFile); //puts (mystring);
        // sscanf( mystring, "%i", &noClasses);//printf(" %i \n",noBranches);


        fgets(mystring, sizeof (mystring), pFile); //puts (mystring);
        fgets(mystring, sizeof (mystring), pFile); // puts (mystring);
        //sscanf( mystring, "%i", &noBags);//printf(" %i \n",noBags);

        fgets(mystring, sizeof (mystring), pFile); // puts (mystring);
        fgets(mystring, sizeof (mystring), pFile); //puts (mystring);
        sscanf(mystring, "%i", &noBranches[bagi]); //printf(" %i \n",noBranches);

        fgets(mystring, sizeof (mystring), pFile); //  puts (mystring);
        fgets(mystring, sizeof (mystring), pFile); //  puts (mystring);
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
            //printf("%i\n",classLabels[bagi][i]);
        }
        fgets(mystring, sizeof (mystring), pFile); //  skip to end of line



        fclose(pFile);
    }

}




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
    out = malloc(sizeof (int)*noBags);
    for (int bagi = 0; bagi < noBags; bagi++)
        out[bagi] = 0;


    for (int bagi = 0; bagi < noBags; bagi++) {
        out[bagi] = 0;
        int* result;
        int ** test;
        result = malloc(sizeof (int)*noBranches[bagi]);
        test = (int**) malloc(noBranches[bagi] * sizeof (int*));
        for (int i = 0; i < noBranches[bagi]; i++)
            test[i] = malloc(maxBrancheLen[bagi] * sizeof (int));

        int bi, ni;

        for (bi = 0; bi < noBranches[bagi]; bi++) {
            result[bi] = 1;

            for (ni = 0; ni < branchLengths[bagi][bi]; ni++) {
                //printf("%f ",branchValues[bi][ni]);

                test[bi][ni] = 0;
                if (branchLogic[bagi][bi][ni] == 1) {
                    if (x[branchVectI[bagi][bi][ni] - 1] < branchValues[bagi][bi][ni]) {
                        test[bi][ni] = 1;
                    }
                }

                if (branchLogic[bagi][bi][ni] == 0) {
                    if (x[branchVectI[bagi][bi][ni] - 1] >= branchValues[bagi][bi][ni]) {
                        test[bi][ni] = 1;
                    }
                }
                result[bi] = result[bi] * test[bi][ni];
                // printf("%i ",test[bi][ni]);

            }
            //  printf("\n %i ",bi+1);

        }


        for (int i = 0; i < noBranches[bagi]; i++) {
            //  printf("%i \n",result[i]);
            if (result[i] == 1) {
                out[bagi] = classLabels[bagi][i];
            }
        }
        //class=Class(find(result==1));
        // printf("%i \n",out[bagi]);
    }

    // now provide the class estimate that is most popular
    int * count;
    count = (int*) malloc(sizeof (int)*noClasses);
    int * ClassTest;
    ClassTest = (int*) malloc(sizeof (int)*noClasses);
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
    int max = 0;
    for (int j = 0; j < noClasses; j++) {
        if (count[j] >= max) {
            output = j;
            max = count[j];
        }
        // printf("%i \n",count[j]);
    }
    //printf("\n");
    //printf("%i \n",output);
    return output;


}


int decisionTreeFunSnr(float *x) {
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