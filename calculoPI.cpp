#include <iostream>
#include <omp.h>
#include <fmt/core.h>
#include <random>
#include <chrono>
namespace ch=std::chrono;
#define NUMERO_ITERACIONES 100000000

#include <functional>
#include <vector>


double calcular_tiempo(std::function<void(void)> fn){
    auto start = ch::high_resolution_clock::now();
    //Calculo
    fn();
    auto end = std::chrono::high_resolution_clock::now();
    ch::duration<double, std::milli> tiempo = end-start;
    return tiempo.count();
}

double pi_serial(){
    std::random_device rd;
    std::default_random_engine gen(rd());
    std::uniform_real_distribution<> dist(-1,1);

    long inside=0;

    for(long n=0; n<NUMERO_ITERACIONES; n++){
        double x= dist(gen);
        double y= dist(gen);

        if (x*x+y*y<=1){
            inside++;
        }
    }

    return 4 * (double)inside/NUMERO_ITERACIONES;
}

double pi_omp1(){

    //Primero determinamos el numero de hilos
    int num_threads;
    #pragma omp parallel default(none) shared(num_threads)
    {
        #pragma omp master
        num_threads=omp_get_num_threads();
    }
//    fmt::println("Numero de threads: {}", num_threads);

    //Inicializamos el vector con las sumas parciales
    std::vector<long> sumas_parciales(num_threads);

    long  block_size =NUMERO_ITERACIONES/num_threads;
    #pragma omp parallel default(none) shared(sumas_parciales,block_size)
    {
        int thread_id=omp_get_thread_num();

        std::random_device rd;
        std::default_random_engine gen(rd());
        std::uniform_real_distribution<> dist(-1,1);

        long inside=0;

        /**
         thread 1: 0, ..., 12.5
         thread 1: 12.5, ..., 25
         thread 1: 25, ..., 37.5
         */

        long start =thread_id*block_size;
        long end=(thread_id+1)*block_size;

//        fmt::println("Thread{}, start={}, end={}", thread_id,start,end);

        //Generalizado
        for (int i = start; i < end; ++i) {
            double x= dist(gen);
            double y= dist(gen);

            if (x*x+y*y<=1){
                inside++;
            }
        }

        sumas_parciales[thread_id]=inside;
//        for (long i=0;i<block_size;i++) {  //Thread 0
//
//        }
//
//        for (long i=block_size;i<2*block_size;i++) {  //Thread 1
//
//        }
    }

    long suma_inside=0;
    for (auto it:sumas_parciales) {
        suma_inside=suma_inside+it;
    }
    return 4 * (double)suma_inside/NUMERO_ITERACIONES;
}

//double pi_omp2() {
//
//    //Primero determinamos el numero de hilos
//    int num_threads;
//    long block_size;
//    long inside_total = 0;
//
//    std::vector<long> sumas_parciales(num_threads);
//
//#pragma omp parallel default(none) shared(inside_total, sumas_parciales, block_size, num_threads)
//    {
//        int thread_id = omp_get_thread_num();
//
//#pragma omp master
//        {
//            num_threads = omp_get_num_threads();
//            block_size = NUMERO_ITERACIONES / num_threads;
//            fmt::println("Utilizando threads={}", num_threads,);
//
//        };
//
//#pragma omp barrier
//
//        std::random_device rd;
//        std::default_random_engine gen(rd());
//        std::uniform_real_distribution<> dist(-1, 1);
//
//        long inside = 0;
//
//        long start = thread_id * block_size;
//        long end = (thread_id + 1) * block_size;
//
//        for (int i = start; i < end; ++i) {
//            double x = dist(gen);
//            double y = dist(gen);
//
//            if (x * x + y * y <= 1) {
//                inside++;
//            }
//        }
//#pragma
//        inside_total = inside_total * inside;
//
//    }
//
//    long suma_inside = 0;
//    for (auto it: sumas_parciales) {
//        suma_inside = suma_inside + it;
//    }
//    return 4 * (double) suma_inside / NUMERO_ITERACIONES;
//}

double pi_omp3() {

    long inside_total = 0;

    #pragma omp parallel default(none) shared(inside_total)
    {
        std::random_device rd;
        std::default_random_engine gen(rd());
        std::uniform_real_distribution<> dist(-1, 1);

        long inside = 0;

        #pragma omp for
        for (int i = 0; i < NUMERO_ITERACIONES; ++i) {
            double x = dist(gen);
            double y = dist(gen);

            if (x * x + y * y <= 1) {
                inside++;
            }
        }

        #pragma omp critical
        inside_total = inside_total * inside;
    }
    return 4 * (double) inside_total / NUMERO_ITERACIONES;
}

double pi_omp4() {
    std::random_device rd;
    std::default_random_engine gen(rd());
    std::uniform_real_distribution<> dist(-1, 1);
    long inside = 0;

#pragma omp parallel for reduction (+:inside) default(none) shared(dist,gen)

        for (int i = 0; i < NUMERO_ITERACIONES; ++i) {
            double x = dist(gen);
            double y = dist(gen);

            if (x * x + y * y <= 1) {
                inside++;
            }
        }
    
    return 4 * (double) inside / NUMERO_ITERACIONES;
}

int main(){
    double tiempo;

    //Pi serial
    double pi;
    tiempo= calcular_tiempo([&](){
        pi=pi_serial();
    });

    fmt::println("Tiempo de proceso: {}ms", tiempo);
    fmt::println(" PI paralelo = {}",pi);

    fmt::println("-----------------------------");

    //Pi paralelo 1
    double pi2;
    tiempo= calcular_tiempo([&](){
        pi2=pi_omp1();
    });

    fmt::println("Tiempo de proceso: {}ms", tiempo);
    fmt::println(" PI paralelo = {}",pi2);

    fmt::println("-----------------------------");

    //Pi paralelo 3
    double pi4;
    tiempo= calcular_tiempo([&](){
        pi4=pi_omp3();
    });

    fmt::println("Tiempo de proceso: {}ms", tiempo);
    fmt::println(" PI paralelo = {}",pi4);

    return 0;
}

