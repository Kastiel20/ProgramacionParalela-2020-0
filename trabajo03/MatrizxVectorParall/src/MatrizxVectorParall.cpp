#include <iostream>
#include <cstdlib>
#include <ctime>
#include "mpi.h"

using namespace std;

int main(int argc, char * argv[]) {

    int numeroProcesadores,
            idProceso,fila,columna;
    long **A, // Matriz a multiplicar
            *x, // Vector que vamos a multiplicar
            *y, // Vector donde almacenamos el resultado
            *miFila, // La fila que almacena localmente un proceso
            *comprueba; // Guarda el resultado final (calculado secuencialmente), su valor
                        // debe ser igual al de 'y'

    double tInicio, // Tiempo en el que comienza la ejecucion
            tFin; // Tiempo en el que acaba la ejecucion

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numeroProcesadores);
    MPI_Comm_rank(MPI_COMM_WORLD, &idProceso);
if(idProceso == 0){
    	do{ cout<<"Tenga en cuenta que el numero de filas que asigne, no puede eceder el numero de procesadores"<<endl;
    		cout<<"Numero de filas (1-"<<numeroProcesadores<<"): ";
    		cin>>fila;
    		if(fila>numeroProcesadores) cout<<"Numero fuera de rango\n";
    	}while(fila>numeroProcesadores);
    	cout<<"Numero de columnas: ";
    	cin>>columna;
    	MPI_Bcast(&fila,1,MPI_INT,0,MPI_COMM_WORLD);
    	MPI_Bcast(&columna,1,MPI_INT,0,MPI_COMM_WORLD);
    }
    else{
    	MPI_Bcast(&fila,1,MPI_INT,0,MPI_COMM_WORLD);
    	MPI_Bcast(&columna,1,MPI_INT,0,MPI_COMM_WORLD);
    }


    A = new long *[fila]; // Reservamos tantas filas como procesos haya
    x = new long [columna]; // El vector sera del mismo tamanio que el numero
    // de procesadores

    // Solo el proceso 0 ejecuta el siguiente bloque
    if (idProceso == 0) {
        A[0] = new long [fila * columna];
        for (unsigned int i = 1; i < fila; i++) {
            A[i] = A[i - 1] + columna;
        }
        // Reservamos especio para el resultado
        y = new long [fila];

        // Rellenamos 'A' y 'x' con valores aleatorios
        srand(time(0));
        cout << "La matriz y el vector generados son " << endl;
        for (unsigned int i = 0; i < fila; i++) {
            for (unsigned int j = 0; j < columna; j++) {
                if (j == 0) cout << "[";
                A[i][j] = rand() % 10;
                cout << A[i][j];
                if (j == columna - 1){
                	cout << "]";
                	if(i<columna){
                		x[i] = rand() % 10;
                		cout << "\t  [" << x[i] << "]";
                	}
                	cout<< endl;
                }
                else cout << "  ";
            }
        }
        for (unsigned int k = fila; k < columna; k++) {
        	if(k<columna){
        	   x[k] = rand() % 10;
        	   cout << "\t\t  [" << x[k] << "]";
        	   cout<<endl;
        	}
        }
        cout << "\n";

        // Reservamos espacio para la comprobacion
        comprueba = new long [fila];
        // Lo calculamos de forma secuencial
        for (unsigned int i = 0; i < fila; i++) {
            comprueba[i] = 0;
            for (unsigned int j = 0; j < columna; j++) {
                comprueba[i] += A[i][j] * x[j];
            }
        }
    } // Termina el trozo de codigo que ejecuta solo 0

    // Reservamos espacio para la fila local de cada proceso
    miFila = new long [columna];

    // Repartimos una fila por cada proceso, es posible hacer la reparticion de esta
    // manera ya que la matriz esta creada como un unico vector.
    MPI_Scatter(A[0], // Matriz que vamos a compartir
            columna, // Numero de columnas a compartir
            MPI_LONG, // Tipo de dato a enviar
            miFila, // Vector en el que almacenar los datos
            columna, // Numero de columnas a compartir
            MPI_LONG, // Tipo de dato a recibir
            0, // Proceso raiz que envia los datos
            MPI_COMM_WORLD); // Comunicador utilizado (En este caso, el global)

    // Compartimos el vector entre todas los procesos
    MPI_Bcast(x, // Dato a compartir
            columna, // Numero de elementos que se van a enviar y recibir
            MPI_LONG, // Tipo de dato que se compartira
            0, // Proceso raiz que envia los datos
            MPI_COMM_WORLD); // Comunicador utilizado (En este caso, el global)


    // Hacemos una barrera para asegurar que todas los procesos comiencen la ejecucion
    // a la vez, para tener mejor control del tiempo empleado
    MPI_Barrier(MPI_COMM_WORLD);
    // Inicio de medicion de tiempo
    tInicio = MPI_Wtime();

    long subFinal = 0;
    for (unsigned int i = 0; i < columna; i++) {
        subFinal += miFila[i] * x[i];
    }

    // Otra barrera para asegurar que todas ejecuten el siguiente trozo de codigo lo
    // mas proximamente posible
    MPI_Barrier(MPI_COMM_WORLD);
    // fin de medicion de tiempo
    tFin = MPI_Wtime();

    // Recogemos los datos de la multiplicacion, por cada proceso sera un escalar
    // y se recoge en un vector, Gather se asegura de que la recoleccion se haga
    // en el mismo orden en el que se hace el Scatter, con lo que cada escalar
    // acaba en su posicion correspondiente del vector.
    MPI_Gather(&subFinal, // Dato que envia cada proceso
            1, // Numero de elementos que se envian
            MPI_LONG, // Tipo del dato que se envia
            y, // Vector en el que se recolectan los datos
            1, // Numero de datos que se esperan recibir por cada proceso
            MPI_LONG, // Tipo del dato que se recibira
            0, // proceso que va a recibir los datos
            MPI_COMM_WORLD); // Canal de comunicacion (Comunicador Global)

    // Terminamos la ejecucion de los procesos, despues de esto solo existira
    // el proceso 0
    // Ojo! Esto no significa que los demas procesos no ejecuten el resto
    // de codigo despues de "Finalize", es conveniente asegurarnos con una
    // condicion si vamos a ejecutar mas codigo (Por ejemplo, con "if(rank==0)".
    MPI_Finalize();

    if (idProceso == 0) {

        unsigned int errores = 0;

        cout << "El resultado obtenido y el esperado son:" << endl;
        for (unsigned int i = 0; i < fila; i++) {
            cout << "\t" << y[i] << "\t|\t" << comprueba[i] << endl;
            if (comprueba[i] != y[i])
                errores++;
        }

        delete [] y;
        delete [] comprueba;
        delete [] A[0];

        if (errores) {
            cout << "Hubo " << errores << " errores." << endl;
        } else {
            cout << "No hubo errores" << endl;
            cout << "El tiempo tardado ha sido " << tFin - tInicio << " segundos." << endl;
        }

    }

    delete [] x;
    delete [] A;
    delete [] miFila;

}
