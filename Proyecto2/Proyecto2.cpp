/**
 *Diego Alonzo 20172
 */ 
// Importacion de librerias
#include <unistd.h>
#include <iostream>
#include <pthread.h>
using namespace std;
pthread_mutex_t lock;
struct Matrix
{
    int matriz[3][3];
};
// SECCION DE CALCULO DE DETERMINANTE
void* determinantFirst(void* arg){
    sleep(1);
    Matrix localM = *(Matrix*) arg;
    int first = localM.matriz[0][0]*(localM.matriz[1][1]*localM.matriz[2][2]-localM.matriz[2][1]*localM.matriz[1][2]);
    *(int*) arg = first;
    return (void*) arg;
}
void* determinantSecond(void* arg){
    sleep(1);
    Matrix localM = *(Matrix*) arg;
    int second = (-1)*localM.matriz[0][1]*(localM.matriz[1][0]*localM.matriz[2][2]-localM.matriz[2][0]*localM.matriz[1][2]);
    *(int*) arg = second; 
    return (void*) arg; 
}
void* determinantThird(void* arg){
    sleep(1);
    Matrix localM = *(Matrix*) arg;
    int third = localM.matriz[0][2]*(localM.matriz[1][0]*localM.matriz[2][1]-localM.matriz[2][0]*localM.matriz[1][1]);
    *(int*) arg = third; 
    return (void*) arg; 
}
void* calculateDeterminant(void* arg){
    Matrix localM = *(Matrix*) arg;
    pthread_t first;
    pthread_t second;
    pthread_t third;
    int* value;
    int determinant = 0;
    pthread_create(&first, NULL, &determinantFirst, (void*)&localM);
    pthread_create(&second, NULL, &determinantSecond, (void*)&localM);
    pthread_create(&third, NULL, &determinantThird, (void*)&localM);
    pthread_join( first, (void **) &value );
    determinant += *value;
    pthread_join( second, (void **) &value );
    determinant += *value;
    pthread_join( third, (void **) &value );
    determinant += *value;
    *(int*) arg = determinant;   
    return (void*) arg;
}
// FIN SECCION DE CALCULO DE DETERMINANTE
// SECCION DE TRANSPUESTA 
void* calculateTranspuesta(void* arg){
    Matrix localM = *(Matrix*) arg;
    // Se reserva memoria dinamica para la struct a devolver
    Matrix *newM = (Matrix*) malloc(sizeof(Matrix));
    for (int k = 0 ; k < 3 ; k++ )
    {
        for (int i = 0; i < 3; i++)
        {
            newM->matriz[i][k]=localM.matriz[k][i];
        }
        
    }
    return (void*) newM;
}
// FIN DE SECCION TRANSPUESTA
int main()
{
    pthread_t tDeterminant;
    pthread_t tTranspuest;
    Matrix matrixInput; 
    int *determinant;
    Matrix *trans;
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
    if (pthread_join( tDeterminant, (void **) &determinant ) != 0 ){ //In case the join fails
        perror("Failed to join the thread\n");
        return 2;
    }
    if (pthread_join( tTranspuest, (void **) &trans ) != 0 ){ //In case the join fails
        perror("Failed to join the thread\n");
        return 2;
    }
    cout << "Determinante: " << *determinant << endl;
    // IMPRESION DE MATRIZ NORMAL
    // for (int i = 0; i < 3; i++)
    // {
    //     for (int k = 0; k < 3; k++)
    //     {
    //         cout << matrixInput.matriz[i][k];
    //     }
    //     cout<<endl;
    // } 
    // cout<<endl;
    // IMPRESION DE MATRIZ TRANSPUESTA
    // for (int i = 0; i < 3; i++)
    // {
    //     for (int k = 0; k < 3; k++)
    //     {
    //         cout << trans->matriz[i][k];
    //     }
    //     cout<<endl;
    // } 
    return 0;
}
