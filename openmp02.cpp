#include <iostream>
#include <fmt/core.h>
//Libreria para openMP
#include <omp.h>

//Libreria para medir el tiempo
#include <chrono>
namespace ch=std::chrono;
#define MAX_ITER 100

int main() {
    //La seccion con amarillo indica que las variables que estan fuera
    //como se las desea que esten adentro
    fmt::println("Ejemplo OpenMP02");

//    int MAX_ITER=100;
    auto start = ch::high_resolution_clock::now();

    #pragma omp parallel default(none)
    {
        int thread_id= omp_get_thread_num();

        //Lo divide en bloques
        #pragma omp for
            for (int i = 0; i < MAX_ITER; ++i) {
                fmt::println("thread_id={} for index={:2}",thread_id,i);
            }


    }
    auto end = std::chrono::high_resolution_clock::now();
    ch::duration<double, std::milli> tiempo = end-start;
    fmt::println("Tiempo de proceso: {}ms", tiempo.count());


    //Si queremos trabajar con hilos es necesario poner el pragma for
    //porque caso contrario estaria haciendo el for completo para cada hilo
    #pragma omp parallel for default(none)
    for (int i = 0; i < MAX_ITER; ++i) {
        fmt::println(" index={:2}",i);
    }

    return 0;
}
