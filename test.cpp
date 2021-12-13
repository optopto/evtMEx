#include "mex.h"
#include <iostream>
#include <stdio.h>
#include <string.h>

using namespace std;


void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[]) {
    
   
    unsigned int FRAMESCAP = (unsigned int)mxGetScalar(prhs[0]);
    
    FRAMESCAP = FRAMESCAP + 10; 
    plhs[1]= mxCreateDoubleScalar(FRAMESCAP);
    printf("hola");
    cout << "==== compilado con exito ====" << endl;
    
}