#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#define N 50000

int nn;
int *X[N+1],*apX, *Y;
long long *sumaX, *sumaX2, sumaY, *sumaXY; 
double *A, *B;

int main(int np, char*p[])
{
    int i,j;
    double sA,sB;
    clock_t ta,t;

    assert(np==2);

    nn = atoi(p[1]);
    assert(nn<=N);
    srand(1);

    printf("Dimensio dades =~ %g Mbytes\n",((double)(nn*(nn+11))*4)/(1024*1024)); 

    //creacio matrius i vectors
    apX = calloc(nn*nn,sizeof(int)); assert (apX);
    Y = calloc(nn,sizeof(int)); assert (Y);
    sumaX = calloc(nn,sizeof(long long)); assert (sumaX);
    sumaX2 = calloc(nn,sizeof(long long)); assert (sumaX2);
    sumaXY = calloc(nn,sizeof(long long)); assert (sumaXY);
    A = calloc(nn,sizeof(double)); assert (A);
    B = calloc(nn,sizeof(double)); assert (B);

    // Inicialitzacio
    X[0] = apX;
    for (i=0;i<nn;i++) {
        for (j=0;j<nn;j+=8)            
            X[i][j]=rand()%100+1;
        Y[i]=rand()%100 - 49;
	X[i+1] = X[i] + nn;
    }
    
    // calcul de sumatoris
    sumaY = 0;
    for (i=0;i<nn;i++) {
	sumaX[i] = sumaX2[i] = sumaXY[i] = 0;
        for (j=0;j<nn;j++) {
		sumaX[i] += X[i][j];
		sumaX2[i] += X[i][j] * X[i][j];
		sumaXY[i] += X[i][j] * Y[j];
	}
	sumaY += Y[i];
    }

    // calcul linealitat
    for (i=0;i<nn;i++) {
	B[i] = sumaXY[i] - (sumaX[i] * sumaY)/nn;
	B[i] = B[i] / (sumaX2[i] - (sumaX[i] * sumaX[i])/nn);
	A[i] = (sumaY -B[i]*sumaX[i])/nn;
    }

    // comprovacio
    sA = sB = 0;
    for (i=0;i<nn;i++) {
            //printf("%lg, %lg\n",A[i],B[i]);
	    sA += A[i];
	    sB += B[i];
    }

    printf("Suma elements de A: %lg B:%lg\n",sA,sB);

exit(0);
}
