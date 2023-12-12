//
// Created by fing.labcom on 12/12/2023.
//
#include <iostream>
#include <omp.h>
#include <fmt/core.h>
#include <functional>
#include <vector>
#define MAX_NUMEROS 1024

typedef std::function<float(float,float) > binary_op;

float sum_reduction(const float *input, const int n) {
    float suma = 0.0f;
    for (int i = 0; i < n; ++i)
        suma = suma + input[i];
    return suma;
}

float sum_paralela(const float *input, const int n, binary_op op) {
    std::vector<long> sumas_parciales(24);
    long  block_size =n/24;


    #pragma omp parallel default(none) shared(sumas_parciales,block_size,input)
    {
        int thread_id=omp_get_thread_num();

        long start =thread_id*block_size;
        long end=(thread_id+1)*block_size;
        float suma = 0.0f;

        if(thread_id==23){
            end=MAX_NUMEROS;
        }

        for (int i = start; i < end; ++i) {
            suma=suma+input[i];
        }

        sumas_parciales[thread_id]=suma;

    }
    float suma_total=0;
    for (auto it:sumas_parciales) {
        suma_total=suma_total+it;
    }

    return suma_total;
}

float sum_paralela2(const float *input, const int n, binary_op op){
    int num_threads;
    long block_size;
    float suma_total;
    #pragma omp parallel default(none) shared(num_threads,block_size,input,op,suma_total)
    {
        #pragma omp master
        {
            num_threads=omp_get_num_threads();
            block_size=MAX_NUMEROS/num_threads;
//            fmt::println("Numero de hilos: {}, block:size: {}", num_threads,block_size);

        }

        #pragma omp barrier
        int thread_id=omp_get_thread_num();
        long start=thread_id*block_size;
        long end=(thread_id+1)*block_size;

        if(thread_id==num_threads-1){
            end=MAX_NUMEROS;
        }

//        fmt::println("thread_{}, start: {}, end: {}", thread_id,start,end);

        float suma=0;
        for (int i= start; i<end; i++) {
            suma=op(suma,input[i]);
        }

        //En esta seccion el critical es para que todo se sincronice
        #pragma omp critical
        suma_total=suma_total+suma;

        #pragma omp barrier
//        fmt::println("thread_{}, suma={}", thread_id,suma);

    }

    return suma_total;
}

float sum_paralela3(const float *input, const int n, binary_op op){

    int num_threads;
    float suma_total=0;

    #pragma omp parallel default(none) shared(num_threads,input,op,suma_total)
    {
        #pragma omp master
        {
            num_threads = omp_get_num_threads();
        }

        #pragma omp barrier
        int thread_id=omp_get_thread_num();
        float suma=0;
        for (int i = thread_id; i < MAX_NUMEROS; i+=num_threads) {
            suma=op(suma,input[i]);
        }

        #pragma  omp critical
        suma_total=op(suma_total,suma);

    }

    return suma_total;
}

int main(){
    auto op_add = [](auto v1, auto v2) {
        return v1 + v2;
    };

    std::vector<float> datos (MAX_NUMEROS);
    for (int i = 0; i < MAX_NUMEROS; ++i) {
        datos[i]=i+1;
    }

    //Serial
    float a= sum_reduction(datos.data(),datos.size());
    fmt::println("Suma Serial = {}",a);

    //Paralela
    float b= sum_paralela(datos.data(),datos.size(),op_add);
    fmt::println("Suma ParalelaYO = {}",b);

    //Paralela2
    float c= sum_paralela2(datos.data(),datos.size(), op_add);
    fmt::println("Suma Paralela 2 = {}",c);

    //Paralela3
    float d= sum_paralela3(datos.data(),datos.size(), op_add);
    fmt::println("Suma Paralela Salto = {}",d);

    return 0;
}