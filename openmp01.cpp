#include <iostream>
#include <fmt/core.h>
//Libreria para openMP
#include <omp.h>

int main() {
    //La seccion con amarillo indica que las variables que estan fuera
    //como se las desea que esten adentro
    fmt::println("Ejemplo OpenMP");

    int k=0;
    int thread_num;

    #pragma omp parallel master default(none) shared(thread_num)
    {
    //Para obtener el numero de hilos
        thread_num=omp_get_num_threads();
        fmt::println(" {} threads",thread_num);
        fmt::println("");
    }

    //Zona serial en la que no hacemos nada

    //Seccion paralela: #hilos=#cores(por defecto)
    #pragma omp parallel default(none) shared(k,thread_num)
    {
//        //Para obtener el numero de hilos
//        int thread_num=omp_get_num_threads();
//        fmt::println(" {} threads",thread_num);
//        fmt::println("");

        //Obtiene el id del hilo actual
        int thread_id=omp_get_thread_num();
        //No se imprimen en orden, es segun se van ejecutando
        fmt::println("[{:1}/{:1}]Secccion paralela",thread_id,thread_num);

        //Si pongo pongo none las variables de afuera no son visibles
        //en el caso del shared la variable es local a cada hilo
        //k=5;
    };

    return 0;
}
