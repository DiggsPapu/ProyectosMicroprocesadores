%%cu
/**
 * --------------------------------------------------------
 * Universidad del Valle de Guatemala
 * CC3056 - Programación de Microprocesadores
 * --------------------------------------------------------
 * Proyecto Final
 * 
 * --------------------------------------------------------
 * Cálculo de medios
 * Emplea los datos obtenidos por el sensor y obtiene los calculos
 * Integrantes:
 *
 * Diego Alonzo, 20172
 * Roberto Rios, 20979
 * Daniel Cabrera, 20289
 *
 * --------------------------------------------------------
 */
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <cuda_runtime.h>
#include <cmath>
#include <bits/stdc++.h>
#include <cstdlib>
#include <iomanip>


#define SIZE 93
#define SHMEM_SIZE 250
// Kernel operation active^2+reactive^2
 _global_ void operation(float *A, float *B, float *C, int numElements){
    // Create the id's
   	int o = threadIdx.x + blockDim.x * blockIdx.x;
    C[0] += A[o];
}
_global_ void operation1(float *A, float *C, int numElements){
    // Create the id's
   	int o = threadIdx.x + blockDim.x * blockIdx.x;
    for (int k = 0 ; k < numElements ; k++){
        if (o<numElements){  
          *C += A[o];
        }
    }
}
//Kernel para contabilizar la cantidad de tiempo que estaba en luz y en sombra
_global_ void lineCount(bool *A,int *B, int *C){
    //Memoria compartida
    _shared_ int partial_sum[SHMEM_SIZE];
    _shared_ int partial_sum2[SHMEM_SIZE];
    //Id
    int tid = blockIdx.x*blockDim.x+threadIdx.x;
    partial_sum[threadIdx.x] = A[tid];
    partial_sum2[threadIdx.x] = A[tid];
    //Sync hilos
    __syncthreads();
    for (int l = 1; l<blockDim.x; l++)
    {
        if (A[tid]){
          if (threadIdx.x %(2*l)==0)
          {
              partial_sum[threadIdx.x] += 1;
          }
        }
        else{
            if (threadIdx.x %(2*l)==0)
            {
                partial_sum2[threadIdx.x] += 1;
            }
        }
        
    }
    __syncthreads();
    if (threadIdx.x == 0)
    {
        *C+=partial_sum[0];
        *B+=partial_sum2[0];
    }
    
    
}
//Kernel de suma con reduccion de vectores
_global_ void sum(float *A, float *C){
    //Memoria compartida
    _shared_ int partial_sum[SHMEM_SIZE];
    //Obtencion del id
    int tid = blockIdx.x*blockDim.x+threadIdx.x;
    //Definicion de la suma parcial del bloque
    partial_sum[threadIdx.x] = A[tid];
    //Sincronizacion del hilo
    __syncthreads();
    //Suma del bloque 
    for (int l = 1; l<blockDim.x; l++)
    {
        // En caso de que cumpla
        if (threadIdx.x %(2*l)==0)
        {
            //Se hace la suma parcial del bloque
            partial_sum[threadIdx.x] += partial_sum[threadIdx.x+l];
        }
    }
    //Sinc de hilos
    __syncthreads();
    if (threadIdx.x == 0)
    {
        //Se suma al puntero el valor obtenido por la suma en el bloque
        *C+=partial_sum[0];
    }
}

int main() {
    //Creacion del numero de elementos
    int numElements = 500;
    //Calculo de los bytes para float, bool e int en numElements
    size_t size = numElements * sizeof(float);
    size_t boolSize = numElements * sizeof(bool);
    size_t integerSize = numElements * sizeof(int);
    std::string linea,current_t,temp,pressure,line,altitude;
    // host para temperatura, presion, linea, altitud.
    float *h_temp = (float *)malloc(size); 
    float *h_pressure = (float *)malloc(size);
    bool *h_line = (bool *)malloc(boolSize);
    float *h_altitude = (float *)malloc(size);
    // Los host para recibir los promedios
    float *h_promedio_temp = (float *)malloc(sizeof(float));
    float *h_promedio_altitude = (float *)malloc(sizeof(float));    
    float *h_promedio_pressure = (float *)malloc(sizeof(float));
    // host para saber cuando estuvo en la linea y cuando no
    int *h_inLine = (int *)malloc(sizeof(int));
    int *h_outLine = (int *)malloc(sizeof(int));
    
    int i=0;                                                                                   
    std::ifstream archivo ("datos.csv");
    if (archivo.fail()) {
        std::cerr << "Unable to open file" << "\n";         
        return 1;
    }
    std::getline(archivo,linea);
    while (getline(archivo,linea))
    {                
      std::stringstream stream(linea);
      std::getline(stream, current_t, ',');
      std::getline(stream , temp, ',');
      std::getline(stream , pressure, ',');
      std::getline(stream, altitude, ',');
      std::getline(stream, line, ',');
      if(strtof(temp.c_str(), nullptr) != 0)
      {
          h_temp[i] = strtof(temp.c_str(), nullptr);
      }
      if(strtof(pressure.c_str(), nullptr) != 0)
      {
          h_pressure[i] = strtof(pressure.c_str(), nullptr);
      }
     if(strtof(altitude.c_str(), nullptr) != 0){
          h_altitude[i] = strtof(altitude.c_str(), nullptr);
      }
     std::cout<<"Hour: "<<current_t<<" temperature: " << h_temp[i]  << " °C    altitude: "  << h_altitude[i] << " mts    pressure: "  << h_pressure[i] << " Pa \n";
     i++;
     if (line=="En Linea"){
         h_line[i] = true;
     }
     else{
         h_line[i] = false;
     }
     
    }
    archivo.close();
    // Creacion device de temperatura, presion, altitud y linea
      float *d_temp = NULL;
      cudaMalloc((void **) &d_temp, size);
      float *d_pressure = NULL;
      cudaMalloc((void **) &d_pressure, size);
      float *d_altitude = NULL;
      cudaMalloc((void **) &d_altitude, size);
      bool *d_line = NULL;
      cudaMalloc((void **) &d_line, boolSize);
      //Creacion del device para los promedios
      float *d_promedio_temp = NULL;
      cudaMalloc((void **) &d_promedio_temp, sizeof(float));
      float *d_promedio_altitude = NULL;
      cudaMalloc((void **) &d_promedio_altitude, sizeof(float));
      float *d_promedio_pressure = NULL;
      cudaMalloc((void **) &d_promedio_pressure, sizeof(float));
      //Creacion del device para las sombra y sol
      int *d_inline = NULL;
      cudaMalloc((void **) &d_inline, sizeof(int));      
      int *d_outline = NULL;
      cudaMalloc((void **) &d_outline, sizeof(int));
      //Copy memory host to device
      cudaMemcpy(d_temp, h_temp, size, cudaMemcpyHostToDevice);
      cudaMemcpy(d_pressure, h_pressure, size, cudaMemcpyHostToDevice);
      cudaMemcpy(d_altitude, h_altitude, size, cudaMemcpyHostToDevice);
      cudaMemcpy(d_line,h_line,boolSize,cudaMemcpyHostToDevice);
      int blocksPerGrid = (int)ceil(numElements/SIZE);      
    //Ejecucion en kernel para obtener el promedio de la altitud
    sum<<<blocksPerGrid, SIZE>>>(d_altitude, d_promedio_altitude);
    cudaMemcpy(h_promedio_altitude, d_promedio_altitude, sizeof(float), cudaMemcpyDeviceToHost);
    *h_promedio_altitude = *h_promedio_altitude/(numElements*1.018);    
    //Ejecucion en kernel para obtener el promedio de la presion      
    sum<<<blocksPerGrid, SIZE>>>(d_pressure, d_promedio_pressure);
    cudaMemcpy(h_promedio_pressure, d_promedio_pressure, sizeof(float), cudaMemcpyDeviceToHost);
    *h_promedio_pressure = *h_promedio_pressure/(numElements*1.018);  
    //Ejecucion en kernel para obtener el promedio de la temperatura
    sum<<<blocksPerGrid, SIZE>>>(d_temp, d_promedio_temp);
    cudaMemcpy(h_promedio_temp, d_promedio_temp, sizeof(float), cudaMemcpyDeviceToHost);
    *h_promedio_temp = *h_promedio_temp/(numElements);
    //Ejecucion de kernel para contar tiempo en sombra y en luz
    lineCount<<<blocksPerGrid, SIZE>>>(d_line,d_inline,d_outline);
    cudaMemcpy(h_inLine,d_inline,sizeof(int),cudaMemcpyDeviceToHost);
    cudaMemcpy(h_outLine,d_outline,sizeof(int),cudaMemcpyDeviceToHost);
    std::cout<<"Promedio de temperatura(°C): "<<*h_promedio_temp<<" Promedio de presion (Pa): "<<*h_promedio_pressure<<" Promedio de altitud(mts): "<<*h_promedio_altitude<<" Tiempo de luz(s): "<<*h_inLine<<" Tiempo de sombra(s): "<<*h_outLine;
    //Guardarlo en un nuevo csv
    std::ofstream myfile;
    myfile.open("calc.csv");
    myfile<<"Promedio de temperatura,Promedio de altitud,Promedio de presion,Tiempo en el sol, Tiempo de sombra\n"<<*h_promedio_temp<<","<<*h_promedio_altitude<<","<<*h_promedio_pressure<<","<<*h_inLine<<","<<*h_outLine<<"\n";//Aqui se ingresa el promedio de cada cosa
    myfile.close();
    //Liberacion de memoria reservada en el device
    cudaFree(d_temp);
    cudaFree(d_pressure);
    cudaFree(d_promedio_temp);
    //Liberacion de memoria reservada en el host
    free(h_temp);
    free(h_pressure);
    free(h_altitude);
    free(h_promedio_temp);
    return 0; 
 }