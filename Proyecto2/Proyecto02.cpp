/**
 * UNIVERSIDAD DEL VALLE DE GUATEMALA
 * PROGRAMACION DE MICROPROCESADORES
 * PROYECTO 2: PROGRAMA QUE DEVUELVE EL DETERMINANTE, LA MATRIZ TRANSPUESTA, INVERSA Y ADJUNTA
 * Roberto Rios 20979
 * Diego Alonzo 20172
 * Daniel Cabrera 20289
 */ 

// Importacion de librerias
#include <unistd.h>
#include <iostream>
#include <pthread.h>
#include <semaphore.h>

#define NUM_THREADS 10

using namespace std;
sem_t determinanteSem;

struct Matrix
{
    int matriz[3][3];
    int determinant = 0;
    int transpuest[3][3];
    int adj[3][3];
    double inv[3][3]; // pasar a double: 1/det * matrix[i][j]
};

// SECCION DE CALCULO DE DETERMINANTE
void* determinantFirst(void* arg){
    Matrix localM = *(Matrix*) arg;
    int first = localM.matriz[0][0]*(localM.matriz[1][1]*localM.matriz[2][2]-localM.matriz[2][1]*localM.matriz[1][2]);
    *(int*) arg = first;
    return (void*) arg;
}
void* determinantSecond(void* arg){
    Matrix localM = *(Matrix*) arg;
    sem_wait(&determinanteSem);
    sleep(1);
    int second = (-1)*localM.matriz[0][1]*(localM.matriz[1][0]*localM.matriz[2][2]-localM.matriz[2][0]*localM.matriz[1][2]);
    *(int*) arg = second;
    sem_wait(&determinanteSem);
    return (void*) arg; 
}
void* determinantThird(void* arg){
    Matrix localM = *(Matrix*) arg;
    sem_wait(&determinanteSem);
    sleep(1);
    int third = localM.matriz[0][2]*(localM.matriz[1][0]*localM.matriz[2][1]-localM.matriz[2][0]*localM.matriz[1][1]);
    *(int*) arg = third;
    sem_wait(&determinanteSem);
    return (void*) arg; 
}
void* calculateDeterminant(void* arg){
    Matrix localM = *(Matrix*) arg;
    pthread_t first;
    pthread_t second;
    pthread_t third;
    int* value;
    localM.determinant=0;
    pthread_create(&first, NULL, &determinantFirst, (void*)&localM);
    pthread_create(&second, NULL, &determinantSecond, (void*)&localM);
    pthread_create(&third, NULL, &determinantThird, (void*)&localM);
    sem_post(&determinanteSem);    
    pthread_join( first, (void **) &value );
    sem_post(&determinanteSem);
    localM.determinant += *value;
    sem_post(&determinanteSem);
    pthread_join( second, (void **) &value );
    localM.determinant += *value;
    sem_post(&determinanteSem);
    pthread_join( third, (void **) &value );
    localM.determinant += *value;
    *(int*) arg = localM.determinant;
    return arg;
}
// FIN SECCION DE CALCULO DE DETERMINANTE

// SECCION DE TRANSPUESTA 
void* calculateTranspuesta(void* arg){
    Matrix *localM = (Matrix*) arg;
    for (int k = 0 ; k < 3 ; k++ )
    {
        for (int i = 0; i < 3; i++)
        {
            localM->transpuest[i][k]=localM->matriz[k][i];
        }
        
    }
    return NULL;
}
// FIN DE SECCION TRANSPUESTA

// SECCION DE ADJUNTA 
void* calculateAdjunta(void* arg)
{
    Matrix *localM = (Matrix*) arg;
    localM->adj[0][0] = ((localM->matriz[1][1] * localM->matriz[2][2]) - (localM->matriz[2][1] * localM->matriz[1][2]));
    localM->adj[0][1] = -((localM->matriz[1][0] * localM->matriz[2][2]) - (localM->matriz[2][0] * localM->matriz[1][2]));
    localM->adj[0][2] = ((localM->matriz[1][0] * localM->matriz[2][1]) - (localM->matriz[2][0] * localM->matriz[1][1]));

    localM->adj[1][0] = -((localM->matriz[0][1] * localM->matriz[2][2]) - (localM->matriz[2][1] * localM->matriz[0][2]));
    localM->adj[1][1] = ((localM->matriz[0][0] * localM->matriz[2][2]) - (localM->matriz[2][0] * localM->matriz[0][2]));
    localM->adj[1][2] = -((localM->matriz[0][0] * localM->matriz[2][1]) - (localM->matriz[2][0] * localM->matriz[0][1]));

    localM->adj[2][0] = ((localM->matriz[0][1] * localM->matriz[1][2]) - (localM->matriz[1][1] * localM->matriz[0][2]));
    localM->adj[2][1] = -((localM->matriz[0][0] * localM->matriz[1][2]) - (localM->matriz[1][0] * localM->matriz[0][2]));
    localM->adj[2][2] = ((localM->matriz[0][0] * localM->matriz[1][1]) - (localM->matriz[1][0] * localM->matriz[0][1]));
	return NULL; 
} 
// FIN DE SECCION ADJUNTA

// CALCULO DE MATRIZ INVERSA
void* calculateInverse(void* args)
{
    Matrix *localM = (Matrix*) args;

    // // determinante temporal
    localM->determinant += localM->matriz[0][0]*(localM->matriz[1][1]*localM->matriz[2][2]-localM->matriz[2][1]*localM->matriz[1][2]);
    localM->determinant += (-1)*localM->matriz[0][1]*(localM->matriz[1][0]*localM->matriz[2][2]-localM->matriz[2][0]*localM->matriz[1][2]);
    localM->determinant += localM->matriz[0][2]*(localM->matriz[1][0]*localM->matriz[2][1]-localM->matriz[2][0]*localM->matriz[1][1]);

    cout << "\nDET = " << localM->determinant << "\n";
    
    if (localM->determinant != 0)
    {
        for (int i = 0 ; i < 3; i++ )
        {
            for (int j = 0; j < 3; j++)
            {
                localM->inv[i][j] = (float) (1/localM->determinant)*(localM->adj[i][j]);
                
            }
            
        }
    }

    return NULL;

}
// FIN DE SECCION INVERSA

// SECCION PARA LA FUNCION IMPRIMIDORA DE MATRICES
void printMatrix(int matrix[3][3]){
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            cout << matrix[i][j]<<" ";
        }
        cout<<endl;
    }  
    cout<<endl;
}

// SECCION PARA LA FUNCION IMPRIMIDORA DE MATRICES double
void printMatrixDouble(double matrix[3][3]){
    cout.precision(4);
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            cout << matrix[i][j]<<" ";
        }
        cout<<endl;
    }  
    cout<<endl;
}

// MAIN
int main()
{
    sem_init(&determinanteSem, 0, 1);
    pthread_t tDeterminant;
    pthread_t tTranspuest;
    pthread_t tAdjacent;
    pthread_t tInverse;
    Matrix matrixInput;
    
    // Ingreso de la matriz
    cout << "Ingrese la matriz de 3x3: \n"; 
    for (int i = 0; i < 3; i++)
    {
        for (int k = 0; k < 3; k++)
        {
            cout << "Ingrese el valor entero de la fila " << i << ", columna " << k << ": ";  
            cin >> matrixInput.matriz[i][k];
        }
    }
    int firstPos = matrixInput.matriz[0][0];
    // Creacion de hilos principales
    if  (pthread_create(&tDeterminant, NULL, &calculateDeterminant, (void*)&matrixInput)!=0){ //In case creating threads fail.
           perror("Failed to create the thread\n");
           return 1;
    }
    if  (pthread_create(&tTranspuest, NULL, &calculateTranspuesta, (void*)&matrixInput)!=0){ //In case creating threads fail.
           perror("Failed to create the thread\n");
           return 1;
    }
    if  (pthread_create(&tAdjacent, NULL, &calculateAdjunta, (void*)&matrixInput)!=0){ //In case creating threads fail.
           perror("Failed to create the thread\n");
           return 1;
    }
    if  (pthread_create(&tInverse, NULL, &calculateInverse, (void*)&matrixInput)!=0){ //In case creating threads fail.
           perror("Failed to create the thread\n");
           return 1;
    }


    if (pthread_join( tDeterminant, NULL ) != 0 ){ //In case the join fails
        perror("Failed to join the thread\n");
        return 2;
    }
    if (pthread_join( tTranspuest, NULL ) != 0 ){ //In case the join fails
        perror("Failed to join the thread\n");
        return 2;
    }
    if (pthread_join( tAdjacent, NULL ) != 0 ){ //In case the join fails
        perror("Failed to join the thread\n");
        return 2;
    }
    if (pthread_join( tInverse, NULL ) != 0 ){ //In case the join fails
        perror("Failed to join the thread\n");
        return 2;
    }

    int option;
    
    while (true)
    {
        cout<<"Seleccione entre las siguientes opciones:\n1-Obtener la matriz ingresada\n2-Obtener el determinante\n3-Obtener la matriz transpuesta\n4-Obtener la matriz adjunta\n5-Obtener la matriz inversa\n6-Salir\n";
        cin>>option;

        switch (option)
        {
            case 1:
            // IMPRESION DE MATRIZ INGRESADA
            matrixInput.matriz[0][0] = firstPos;
            cout<<"\nMatriz ingresada:\n";
            printMatrix(matrixInput.matriz);
            break;

            case 2:
            // IMPRESION DEL DET
            cout << "Determinante: " << matrixInput.determinant <<endl;
            break;

            case 3:
            // IMPRESION DE MATRIZ TRANSPUESTA
            cout<<"Matriz transpuesta: \n";
            printMatrix(matrixInput.transpuest);
            break;

            case 4:
            // IMPRESION DE MATRIZ ADJUNTA
            cout<<"Matriz adjunta: \n";
            printMatrix(matrixInput.adj);
            break;

            case 5:
            // IMPRESION DE MATRIZ INVERSA
            cout<<"Matriz inversa: \n";
            printMatrixDouble(matrixInput.inv);
            break;

            case 6:
            cout<<"Gracias por utilizar nuestro programa\n";
            return 0;

            default:
            cout<<"No ingreso una opcion valida";
            break;
        }
    }   
}