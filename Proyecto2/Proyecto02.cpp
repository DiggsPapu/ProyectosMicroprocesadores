/**
 * UNIVERSIDAD DEL VALLE DE GUATEMALA
 * PROGRAMACION DE MICROPROCESADORES
 * PROYECTO 2: PROGRAMA QUE CALCULA EL DETERMINANTE, LA MATRIZ TRANSPUESTA, INVERSA Y ADJUNTA DE UNA MATRIZ 3X3
 * 
 * AUTORES:
 * Roberto Rios 20979
 * Diego Alonzo 20172
 * Daniel Cabrera 20289
 * 
 */ 



// Importacion de librerias
#include <unistd.h>
#include <iostream>
#include <pthread.h>
#include <iomanip>

// definir constantes
#define MAIN_THREADS 4

// alias
using namespace std;


// VARIABLES GLOBALES
pthread_mutex_t mutex;


// STRUCT PARA EL PASO DE ARGS
struct Matrix
{
    int matriz[3][3];
    int det = 0;
    int transpuest[3][3];
    int adj[3][3];
    double inv[3][3];
};



void* calculateDeterminant(void* arg)
{
    Matrix *localM = (Matrix*) arg;

    pthread_mutex_lock(&mutex);
    localM->det = 0;
    localM->det += localM->matriz[0][0]*(localM->matriz[1][1]*localM->matriz[2][2]-localM->matriz[2][1]*localM->matriz[1][2]);
    localM->det += (-1)*localM->matriz[0][1]*(localM->matriz[1][0]*localM->matriz[2][2]-localM->matriz[2][0]*localM->matriz[1][2]);
    localM->det += localM->matriz[0][2]*(localM->matriz[1][0]*localM->matriz[2][1]-localM->matriz[2][0]*localM->matriz[1][1]);
    pthread_mutex_unlock(&mutex);

    return NULL;
}
// FIN SECCION DE CALCULO DE DETERMINANTE



// SECCION DE TRANSPUESTA 
void* calculateTranspuesta(void* arg)
{
    Matrix *localM = (Matrix*) arg;

    pthread_mutex_lock(&mutex);
    for (int k = 0 ; k < 3 ; k++ )
    {
        for (int i = 0; i < 3; i++)
        {
            localM->transpuest[i][k]=localM->matriz[k][i];
        }
    }
    pthread_mutex_unlock(&mutex);

    return NULL;
}
// FIN DE SECCION TRANSPUESTA



// SECCION DE ADJUNTA 
void* calculateAdjunta(void* arg)
{
    Matrix *localM = (Matrix*) arg;

    pthread_mutex_lock(&mutex);

    localM->adj[0][0] = ((localM->matriz[1][1] * localM->matriz[2][2]) - (localM->matriz[2][1] * localM->matriz[1][2]));
    localM->adj[1][0] = -((localM->matriz[1][0] * localM->matriz[2][2]) - (localM->matriz[2][0] * localM->matriz[1][2]));
    localM->adj[2][0] = ((localM->matriz[1][0] * localM->matriz[2][1]) - (localM->matriz[2][0] * localM->matriz[1][1]));

    localM->adj[0][1] = -((localM->matriz[0][1] * localM->matriz[2][2]) - (localM->matriz[2][1] * localM->matriz[0][2]));
    localM->adj[1][1] = ((localM->matriz[0][0] * localM->matriz[2][2]) - (localM->matriz[2][0] * localM->matriz[0][2]));
    localM->adj[2][1] = -((localM->matriz[0][0] * localM->matriz[2][1]) - (localM->matriz[2][0] * localM->matriz[0][1]));

    localM->adj[0][2] = ((localM->matriz[0][1] * localM->matriz[1][2]) - (localM->matriz[1][1] * localM->matriz[0][2]));
    localM->adj[1][2] = -((localM->matriz[0][0] * localM->matriz[1][2]) - (localM->matriz[1][0] * localM->matriz[0][2]));
    localM->adj[2][2] = ((localM->matriz[0][0] * localM->matriz[1][1]) - (localM->matriz[1][0] * localM->matriz[0][1]));

    pthread_mutex_unlock(&mutex);

	return NULL; 
    
} 
// FIN DE SECCION ADJUNTA


// CALCULO DE MATRIZ INVERSA
void* calculateInverse(void* args)
{
    Matrix *matrix = (Matrix*) args; 
    
    pthread_mutex_lock(&mutex);

    if (matrix->det != 0)
    {
        for (int i = 0 ; i < 3; i++ )
        {
            for (int j = 0; j < 3; j++)
            {
                matrix->inv[i][j] = (1.0 / matrix->det) * matrix->adj[i][j];
            }
        }
    }
    pthread_mutex_unlock(&mutex);

    return NULL;

} 
// FIN DE SECCION INVERSA



// SECCION PARA IMPRESION DE MATRICES
void printMatrix(int matrix[3][3])
{
    for (int i = 0; i < 3; i++)
    {
        cout << " |" << setw(6) << matrix[i][0] << setw(6)
                     << setw(6) << matrix[i][1] << setw(6)
                     << setw(6) << matrix[i][2] << setw(6) << "| " << endl;
    }  
    cout<<endl;
}


// OVERRIDE PARA MATRICES CON VALORES DOUBLE
void printMatrix(double matrix[3][3], int det)
{
    if (det != 0)
    {
        cout.precision(4);
        
        for (int i = 0; i < 3; i++)
        {
            cout << " |" << setw(10) << matrix[i][0] << setw(10)
                         << setw(10) << matrix[i][1] << setw(10)
                         << setw(10) << matrix[i][2] << setw(10) << "| " << endl;
        }

    } else {
        printf("\nEl determinante de la matriz es 0, su inversa no existe!\n");
    }
    cout<<endl;
}
// FIN SECCION PARA IMPRESION DE MATRICES



// DEFINIR ARRAY DE FUNCIONES PARA CREAR THREADS CON LOOP
typedef void* (*functionType) (void* args);

// MAIN
int main()
{

    pthread_t threads[MAIN_THREADS];
    functionType functions[] = {calculateDeterminant, calculateTranspuesta, calculateAdjunta, calculateInverse};
    Matrix matrixInput; 
    int i;

    pthread_mutex_init(&mutex, NULL);


    // Ingreso de la matriz
    cout << "Ingrese la matriz de 3x3: \n"; 
    for (i = 0; i < 3; i++)
    {
        for (int k = 0; k < 3; k++)
        {
            cout << "Ingrese el valor entero de la fila " << i << ", columna " << k << ": ";  
            cin >> matrixInput.matriz[i][k];
        }
    }

    // Creacion de hilos principales
    for (i = 0; i < MAIN_THREADS; i++)
    {
        pthread_create(threads + i, NULL, functions[i], &matrixInput);
    }
    

    for (i = 0; i < MAIN_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }


    pthread_mutex_destroy(&mutex);

    
    int option;
    
    while (option != 7)
    {
        cout << "\nSeleccione entre las siguientes opciones:\n[1] Obtener la matriz ingresada\n[2] Obtener el determinante\n[3] Obtener la matriz transpuesta\n[4] Obtener la matriz adjunta\n[5] Obtener la matriz inversa\n[6] Ingresar otra matriz\n[7] Salir\n";
        cin >> option;

        if (option == 1)
        {
            // IMPRESION DE MATRIZ INGRESADA
            cout << "\nMatriz ingresada:\n";
            printMatrix(matrixInput.matriz);
            
        } 
        else if (option == 2)
        {
            // IMPRESION DEL DET
            cout << "\nDeterminante: " << matrixInput.det << endl;

        }
        else if (option == 3)
        {
            // IMPRESION DE MATRIZ TRANSPUESTA
            cout << "\nMatriz transpuesta: \n";
            printMatrix(matrixInput.transpuest);

        }
        else if (option == 4)
        {
            // IMPRESION DE MATRIZ ADJUNTA
            cout << "\nMatriz adjunta: \n";
            printMatrix(matrixInput.adj);

        }
        else if (option == 5)
        {
            // IMPRESION DE MATRIZ INVERSA
            cout << "\nMatriz inversa: \n";
            printMatrix(matrixInput.inv, matrixInput.det);

        }
        else if (option == 6)
        {

            pthread_mutex_init(&mutex, NULL);

            cout << "\nIngrese la matriz de 3x3: \n"; 
            for (i = 0; i < 3; i++)
            {
                for (int k = 0; k < 3; k++)
                {
                    cout << "Ingrese el valor entero de la fila " << i << ", columna " << k << ": ";  
                    cin >> matrixInput.matriz[i][k];
                }
            }

            // Creacion de hilos principales
            for (i = 0; i < MAIN_THREADS; i++)
            {
                pthread_create(threads + i, NULL, functions[i], &matrixInput);
            }

            for (i = 0; i < MAIN_THREADS; i++)
            {
                pthread_join(threads[i], NULL);
            }

            pthread_mutex_destroy(&mutex);

        }
        else if (option == 7)
        {
            cout << "\nGracias por utilizar nuestro programa, adios\n";  
            
        }
        else {
            // while (option != 1 || option != 2 || option != 3 || option != 4 || option != 5 || option != 6 || option != 7)
            // {
            //     cout << "\nNo ingreso una opcion valida, por favor ingrese una opcion:\n\n";
            //     cin >> option;
            // }
            // cin >> option;
            cout << "\nNo ingreso una opcion valida, por favor ingrese una opcion:\n\n";
            
        }

    }   

}