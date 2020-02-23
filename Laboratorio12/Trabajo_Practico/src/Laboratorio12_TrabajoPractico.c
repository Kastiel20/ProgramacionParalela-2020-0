/*
 ============================================================================
 Name        : Laboratorio12_TrabajoPractico.c
 Author      : JoseAlberto
 Version     :
 Copyright   : Your copyright notice
 Description : Calculo de la determinante de una matriz paralelizado con OpenMP
 ============================================================================
 */
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

double determinantOfMatrix(double **mat, int n);
void swap(double **arr, int i1, int j1, int i2, int j2);
double potencia(int a, int b);


int main(int argc, char *argv[]) {
	srand(time(NULL));
    int dimension;
    double **A;
    double det_global=0.0;
    int numThreads, tid;

    printf("\n Digite el orden de la matriz... ");
    scanf("%d",&dimension);

    // Reserva de Memoria
    A = (double **)malloc(dimension*sizeof(double*));

    for(int i=0;i<dimension;i++){
        A[i] = (double*)malloc(dimension*sizeof(double));
    }

    printf("\nLa matriz aleatoria \n") ;
    for (int i = 0; i < dimension; i++) {
        for ( int j = 0; j < dimension; j++) {
            if (j == 0) printf("[");
            A[i][j] = rand() % 5;
            printf("%f ",A[i][j]);
            if (j == dimension - 1) printf( "]");
            else printf("  ");
        }
        printf( "\n");
    }


    omp_set_num_threads(dimension);

    #pragma omp parallel private(numThreads, tid)
    {
        int tid = omp_get_thread_num();
        int numThreads = omp_get_num_threads();
        double **B;
        B = (double **)malloc((dimension-1)*sizeof(double*));

        for(int i=0;i<(dimension-1);i++){
            B[i] = (double*)malloc((dimension-1)*sizeof(double));
        }


        int x=0,y=0;
        for (int i = 0; i < dimension; i++) {
                for ( int j = 0; j < dimension; j++) {
                	if(0!=i && tid!=j){
                		B[x][y]=A[i][j];
                		y++;
                		if(y==dimension-1){
                			y=0;
                			x++;
                		}
                	}
                }
            }
        printf("\nThread %d",tid);
        double det_local;
        if((tid%2)==0){
       		det_local=1*A[0][tid]*determinantOfMatrix(B,(dimension-1));
       	}else{
       		det_local=-1*A[0][tid]*determinantOfMatrix(B,(dimension-1));
       	}
        //free(B);
		#pragma omp critical
        det_global=det_global+det_local;



    }
    printf("\n\nEl determinante de la matriz aleatoria es : %f",det_global);
    printf("\n ");
    free(A);

    return 0;
}


double determinantOfMatrix(double **mat, int n)
{
    double num1,num2;
    int index; // Initialize result
    double det = 1.0;
    long long int total = 1.0;
    // temporary array for storing row
    int temp[n + 1];

    //loop for traversing the diagonal elements
    for(int i = 0; i < n; i++)
    {
        index = i; // initialize the index

        //finding the index which has non zero value
        while(mat[index][i] == 0 && index < n) {
            index++;

        }
        if(index == n) // if there is non zero element
        {
            // the determinat of matrix as zero
            continue;

        }
        if(index != i)
        {
            //loop for swaping the diagonal element row and index row
            for(int j = 0; j < n; j++)
            {
                swap(mat, index, j, i, j);
            }
                //determinant sign changes when we shift rows
                //go through determinant properties
                det = det*potencia(-1,index-i);
       }

       //storing the values of diagonal row elements
       for(int j = 0; j < n; j++)
       {
           temp[j] = mat[i][j];

       }
       //traversing every row below the diagonal element
       for(int j = i+1; j < n; j++)
       {
           num1 = temp[i]; //value of diagonal element
           num2 = mat[j][i]; //value of next row element

           //traversing every column of row
           // and multiplying to every row
           for(int k = 0; k < n; k++)
           {
               //multiplying to make the diagonal
               // element and next row element equal
               mat[j][k] = (num1 * mat[j][k]) - (num2 * temp[k]);

           }
           total = total * num1; // Det(kA)=kDet(A);
       }
       if(i>=n-1){
           mat[i][i]=(mat[i][i])/total;
       }
    }



    //mulitplying the diagonal elements to get determinant
    for(int i = 0; i < n; i++)
    {
        det = det * mat[i][i];

    }

    return det;
    //return total; //Det(kA)/k=Det(A);
}

void swap(double **arr, int i1, int j1, int i2, int j2)
{
    int temp = arr[i1][j1];
    arr[i1][j1] = arr[i2][j2];
    arr[i2][j2] = temp;
}

double potencia(int a, int b){
    double c=1.0;
    for(int i=0;i<b;i++){
        c=c*a;
    }
    return c;
}

