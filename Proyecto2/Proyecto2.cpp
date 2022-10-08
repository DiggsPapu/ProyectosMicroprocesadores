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
using namespace std;
sem_t determinanteSem;
struct Matrix
{
    int matriz[3][3];
    int determinant = 0;
    int transpuest[3][3];
    int adj[3][3];
    int inv[3][3];
};
// SECCION DE CALCULO DE DETERMINANTE
void* determinantFirst(void* arg){
    Matrix localM = *(Matrix*) arg;
    localM.determinant+= localM.matriz[0][0]*(localM.matriz[1][1]*localM.matriz[2][2]-localM.matriz[2][1]*localM.matriz[1][2]);
    sem_wait(&determinanteSem);
    return (void*) arg;
}
void* determinantSecond(void* arg){
    Matrix localM = *(Matrix*) arg;
    localM.determinant += (-1)*localM.matriz[0][1]*(localM.matriz[1][0]*localM.matriz[2][2]-localM.matriz[2][0]*localM.matriz[1][2]);
    sem_wait(&determinanteSem);
    return (void*) arg; 
}
void* determinantThird(void* arg){
    Matrix localM = *(Matrix*) arg;
    localM.determinant += localM.matriz[0][2]*(localM.matriz[1][0]*localM.matriz[2][1]-localM.matriz[2][0]*localM.matriz[1][1]);
    sem_post(&determinanteSem);
    return (void*) arg; 
}
void* calculateDeterminant(void* arg){
    Matrix localM = *(Matrix*) arg;
    pthread_t first;
    pthread_t second;
    pthread_t third;
    Matrix* value;
    pthread_create(&first, NULL, &determinantFirst, (void*)&localM);
    pthread_create(&second, NULL, &determinantSecond, (void*)&localM);
    pthread_create(&third, NULL, &determinantThird, (void*)&localM);
    pthread_join( first, (void **) &value );
    pthread_join( second, (void **) &value );
    pthread_join( third, (void **) &value );
    localM.determinant = value->determinant;
    return NULL;
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


int main()
{
    sem_init(&determinanteSem, 0, 1);
    pthread_t tDeterminant;
    pthread_t tTranspuest;
    pthread_t tAdjacent;
    Matrix matrixInput; 
    int *determinant;
    Matrix *trans;
    Matrix *adj;
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
    cout<<"Matriz ingresada:\n";
    // IMPRESION DE MATRIZ NORMAL
    for (int i = 0; i < 3; i++)
    {
        for (int k = 0; k < 3; k++)
        {
            cout << matrixInput.matriz[i][k]<<" ";
        }
        cout<<endl;
    } 
    cout<<endl;
    cout << "Determinante: " << matrixInput.determinant << "\nMatriz transpuesta: \n";
    // IMPRESION DE MATRIZ TRANSPUESTA
    for (int i = 0; i < 3; i++)
    {
        for (int k = 0; k < 3; k++)
        {
            cout << matrixInput.transpuest[i][k]<<" ";
        }
        cout<<endl;
    }
    cout<<"\nMatriz adjunta: \n";
    // IMPRESION DE MATRIZ ADJUNTA
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            cout << matrixInput.adj[i][j]<<" ";
        }
        cout<<endl;
    }  
    
    return 0;
}
