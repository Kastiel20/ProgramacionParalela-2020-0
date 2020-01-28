//============================================================================
// Name        : trabajo01.cpp
// Author      : ManuelColan
// Version     : 
// Copyright   : your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "mpi.h"
#include <vector>
#include <cstdlib>
#include <iostream>
#include <time.h>
using namespace std;
int LXOR(vector<int> vec, int n);
int main(int argc, char *argv[]) {
    int tama, rank, size;
    int local_lxor,total_lxor;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    srand(time(NULL));
    if (argc < 2) {
        if (rank == 0) {
            cout << "No se ha especificado numero de elementos, multiplo "
            		"de la cantidad de entrada, por defecto sera " << size * 100;
            cout << "\nUso: <ejecutable> <cantidad>" << endl;
        }
        tama = size * 100;
    } else {
        tama = atoi(argv[1]);
        if (tama < size)
        	tama = size;
        else {
            int i = 1, num = size;
            while (tama > num) {
                ++i;
                num = size*i;
            }
            if (tama != num) {
                if (rank == 0)
                    cout << "Cantidad cambiada a " << num << endl;
                tama = num;
            }
        }
    }
    vector<int> VectorA, VectorALocal;
	VectorA.resize(tama, 0);
	VectorALocal.resize(tama/size, 0);
	if (rank == 0) {
		for (long i = 0; i < tama; ++i) {
			VectorA[i] = rand()%2; // Vector A recibe valores 1, 2, 3, ..., tama
		}
	}
	MPI_Scatter(&VectorA[0], // Valores a compartir
	            tama/size, // Cantidad que se envia a cada proceso
	            MPI_INT, // Tipo del dato que se enviara
	            &VectorALocal[0], // Variable donde recibir los datos
	            tama/size, // Cantidad que recibe cada proceso
	            MPI_INT, // Tipo del dato que se recibira
	            0,  // proceso principal que reparte los datos
	            MPI_COMM_WORLD); // Comunicador (En este caso, el global)
	local_lxor=LXOR(VectorALocal,tama/size);
	MPI_Reduce(&local_lxor, // Elemento a enviar
	            &total_lxor, // Variable donde se almacena la reunion de los datos
				1, // Cantidad de datos a reunir
	            MPI_INT, // Tipo del dato que se reunira
	            MPI_LXOR, // Operacion aritmetica a aplicar
	            0, // Proceso que recibira los datos
	            MPI_COMM_WORLD); // Comunicador

	if(rank==0){
			 cout<<"Vector:"<<endl;
			 for(int i=0;i<tama;i++){
				 cout<<VectorA[i]<<" ";
			 }
			 cout<<"Aplicar LXOR a grupos de "<<tama/size<<" elementos de A resulta: "<<total_lxor;
		 }
	 MPI_Finalize();
	return 0;
}

int LXOR(vector<int> vec, int n){
	int lxor=vec[0];
	for (int i=1; i<n;i++){
		lxor= lxor != vec[i];
	}
	return lxor;
}
