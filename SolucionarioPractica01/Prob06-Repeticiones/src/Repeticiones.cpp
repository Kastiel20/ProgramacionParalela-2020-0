/*
 ============================================================================
 Name        : Repeticiones.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Compute Pi in MPI C++
 ============================================================================
 */
#include "mpi.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <time.h>

using namespace std;

int main(int argc, char *argv[]) {
    int  cont, parcial_cont, rank, size,n, x;


    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    srand(time(NULL));
     int  *V, *VectorLocal;

    if (rank == 0) {
        cout << "Ingrese el numero de elementos del vector: "<< endl;
        cin>>n;

		cout << "Ingrese el valor buscado: "<<endl;
		cin>>x;

    }


    V= new int[n];
    VectorLocal= new int [n/size];

    MPI_Bcast(&n,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&x,1,MPI_INT,0,MPI_COMM_WORLD);

    if(rank==0){
    	cout<< "Vector generado es : [ ";
    					for(int i=0;i<n;i++){
    			        	V[i]=rand()%10;
    			        	cout<<V[i]<<" ";
    					}
    					cout <<"]"<<endl;

    }


    // Distribuimos los valores del vector V
    MPI_Scatter(&V[0], // Valores a compartir
            n / size, // Cantidad que se envia a cada proceso
            MPI_INT, // Tipo del dato que se enviara
            &VectorLocal[0], // Variable donde recibir los datos
            n / size, // Cantidad que recibe cada proceso
            MPI_INT, // Tipo del dato que se recibira
            0,  // proceso principal que reparte los datos
            MPI_COMM_WORLD); // Comunicador (En este caso, el global)

    // Conteo de ocurrencias de x en el subvector VectorALocal
    parcial_cont = 0;
    for (int i = 0; i < n / size; ++i) {
    	if (VectorLocal[i]==x)
        	parcial_cont ++;
    }

    // Reunimos los datos en un solo proceso, aplicando una operacion
    // aritmetica, en este caso, la suma.
    MPI_Reduce(&parcial_cont, // Elemento a enviar
            &cont, // Variable donde se almacena la reunion de los datos
            1, // Cantidad de datos a reunir
            MPI_INT, // Tipo del dato que se reunira
            MPI_SUM, // Operacion aritmetica a aplicar
            0, // Proceso que recibira los datos
            MPI_COMM_WORLD); // Comunicador

    if (rank == 0)
        cout << "El numero de ocurrencias del valor "<<x<<" en el vector es : " << cont << endl;

    MPI_Finalize();
    delete [] V;
    delete [] VectorLocal;
    return 0;
}
