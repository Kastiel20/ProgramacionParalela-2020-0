/*
 ============================================================================
 Name        : Factorial.c
 Author      : Manuel Colan Torres
 Version     :
 Copyright   : Your copyright notice
 Description : Compute Pi in MPI C++
 ============================================================================
 */
#include <math.h> 
#include "mpi.h" 
#include <iostream>
using namespace std;
 
int main(int argc, char *argv[]) {
	int n, rank, size, i,residuo;
    long long int prod,parcial_prod;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0) {
	        cout << "Ingrese un numero (n!): ";
	        cin>>n;
	    }
	    MPI_Bcast(&n,1,MPI_INT,0,MPI_COMM_WORLD);
	    //recorrido agil.
	    //inclusive si el numero no es multiplo al numero de procesadores
	    //este algoritmo lo resuelve tratando de que sea optimo.
	    //Sea n=7, con p=3
	    //n! = 1x2x3x4x5x6x7
	    //proceso 0: 1x4x7
	    //proceso 1: 2x5
	    //proceso 2: 3x6
	    parcial_prod = 1;
	    residuo = n%size;
	    for (int i = 0; i < n / size; i++) {
	    	parcial_prod *= size*i+rank+1;
	    	cout<<parcial_prod<<"  proceso: "<<rank<<endl;
	    }
	    if ( rank < residuo) {
	    	 for (int i = (n/size); i < (n / size)+1; i++) {
	    	 	    parcial_prod *= size*i+rank+1;
	    	 	    cout<<parcial_prod<<"  proceso: "<<rank<<endl;
	    	 }
		}

	    // Reunimos los datos en un solo proceso usando MPI_Reduce
	    // mediante la operacion MPI_PROD
	    MPI_Reduce(&parcial_prod, // Elemento a enviar
	            &prod, // Variable donde se almacena la reunion de los datos
	            1, // Cantidad de datos a reunir
	            MPI_LONG_LONG_INT, // Tipo del dato que se reunira
	            MPI_PROD, // Operacion aritmetica a aplicar
	            0, // Proceso que recibira los datos
	            MPI_COMM_WORLD); // Comunicador

	    if (rank == 0)
	        cout <<n<< "! = " << prod << endl;


	    MPI_Finalize();
	    return 0;
	}

