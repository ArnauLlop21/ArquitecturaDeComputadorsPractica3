#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <pthread.h>


#define N 50000

int nn;
unsigned short NThreads = 1;
int *X[N+1],*apX, *Y;
long long *sumaX, *sumaX2, sumaY, *sumaXY; 
double *A, *B;
pthread_mutex_t lock;
char maquina = 0;
char *stringMaquina;

/*
	Authors: Arnau Llop Iglesias
*/
void *paralelSum(void * k){
		int condFinal = ((*(int *)k)+1)*(nn/NThreads);
		long long sumaThread = 0;
		if((*(int *)k)==(NThreads-1)) condFinal = nn;

        for (int i = (*(int *)k)*(nn/NThreads) ; i < condFinal ; i += 1) {
			sumaX[i] = sumaX2[i] = sumaXY[i] = 0;
			for (int j = 0;j < nn ; j++) {
				sumaX[i] += X[i][j];
				sumaX2[i] += X[i][j] * X[i][j];
				sumaXY[i] += X[i][j] * Y[j];
			}
			sumaThread += Y[i];
    	}
		pthread_mutex_lock(&lock);
		sumaY += sumaThread;
		pthread_mutex_unlock(&lock);
    return NULL;
}
	/* 
		Authors: Arnau Llop Iglesias
	*/
void *paralelLine(void * k){
		int condFinal = ((*(int *)k)+1)*(nn/NThreads);
		if((*(int *)k)==(NThreads-1)) condFinal = nn;

        for (int i = (*(int *)k)*(nn/NThreads) ; i < condFinal ; i += 1) {
            B[i] = sumaXY[i] - (sumaX[i] * sumaY)/nn;
            B[i] = B[i] / (sumaX2[i] - (sumaX[i] * sumaX[i])/nn);
            A[i] = (sumaY -B[i]*sumaX[i])/nn;
    	}

    return NULL;
}


	/* 
		Author: Departament d'Enginyeria Informatica i Matematiques. URV.
		Modifications from original: Arnau Llop Iglesias
	*/
int main(int np, char*p[])
{
    int i,j;
    clock_t ta,t;
//    clock_t t0,t1,t2,t3,t4,t5;
    double sA,sB;

	assert(np == 3 || np == 4);
	
	nn = atoi(p[1]);
	assert(nn <= N);

	if(np == 4){
		NThreads = atoi(p[3]);
		assert(NThreads > 0);
	}
	maquina = atoi(p[2]);
	if (maquina == 1){
		stringMaquina = "teen";
	}else{
		stringMaquina = "orca";
	}
    
	pthread_t list[NThreads];
    int threadArgs[NThreads];
	int codiResultat = 0;
    srand(1);
	printf("\n\nCodi executat en la maquina %s %d %d\n", stringMaquina, NThreads, nn);
    printf("Dimensio dades =~ %g Mbytes\n",((double)(nn*(nn+11))*4)/(1024*1024)); 

 //   t0 = clock();
    //creacio matrius i vectors
    apX = calloc(nn*nn,sizeof(int)); assert (apX);
    Y = calloc(nn,sizeof(int)); assert (Y);
    sumaX = calloc(nn,sizeof(long long)); assert (sumaX);
    sumaX2 = calloc(nn,sizeof(long long)); assert (sumaX2);
    sumaXY = calloc(nn,sizeof(long long)); assert (sumaXY);
    A = calloc(nn,sizeof(double)); assert (A);
    B = calloc(nn,sizeof(double)); assert (B);


//    t1 = clock();
    // Inicialitzacio
    X[0] = apX;
    for (i=0;i<nn;i++) {
        for (j=0;j<nn;j+=8)            
            X[i][j]=rand()%100+1;
        Y[i]=rand()%100 - 49;
	X[i+1] = X[i] + nn;
    }
    

//    t2 = clock();
    // calcul de sumatoris
    sumaY = 0;
    for(int k = 0 ; k < NThreads ; k += 1){
        threadArgs[k] = k;
		codiResultat = pthread_create(&list[k], NULL, paralelSum, &threadArgs[k]);
		assert(!codiResultat);
	}
	for(int k = 0 ; k < NThreads ; k += 1){
		codiResultat = pthread_join(list[k], NULL);
		assert(!codiResultat);
	}
 //   t3 = clock();
    // calcul linealitat
      sA = sB = 0;
    for(int k = 0 ; k < NThreads ; k += 1){
        threadArgs[k] = k;
		codiResultat = pthread_create(&list[k], NULL, paralelLine, &threadArgs[k]);
		assert(!codiResultat);
	}
	for(int k = 0 ; k < NThreads ; k += 1){
		codiResultat = pthread_join(list[k], NULL);
		assert(!codiResultat);
	}
 //   t4 = clock();
    // comprovacio
    sA = sB = 0;
    for (i=0;i<nn;i++) {
            //printf("%lg, %lg\n",A[i],B[i]);
	    sA += A[i];
	    sB += B[i];
    }
//    t5 = clock();

    printf("%d %d Suma elements de A: %lg B:%lg\n",NThreads, nn, sA,sB);
	//Uncomment for debugging and to know the precise times of the different loops
  /*  printf("%d %d Temps callocs: %.5f\n",NThreads, nn, (double)(t1-t0)/CLOCKS_PER_SEC);
    printf("%d %d Temps Inicialització: %.5f\n",NThreads, nn, (double)(t2-t1)/CLOCKS_PER_SEC);
    printf("%d %d Temps Sumatoris: %.5f\n", NThreads, nn, (double)(t3-t2)/CLOCKS_PER_SEC);
    printf("%d %d Temps Linealitat: %.5f\n", NThreads, nn, (double)(t4-t3)/CLOCKS_PER_SEC);
    printf("%d %d Temps Comprovacio: %.5f\n", NThreads, nn, (double)(t5-t4)/CLOCKS_PER_SEC);*/
exit(0);
}
