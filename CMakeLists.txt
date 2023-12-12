cmake_minimum_required(VERSION 3.26)
project(openmp01)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 10.1.1
)

FetchContent_MakeAvailable(fmt)

#Habilita el soporte para OpenMP
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

add_executable(openmp01 openmp01.cpp)
add_executable(openmp02 openmp02.cpp)
add_executable(calculoPI  calculoPI.cpp)
add_executable(ejercicio  ejercicio.cpp)

target_link_libraries(openmp01 fmt::fmt)
target_link_libraries(openmp02 fmt::fmt)
target_link_libraries(calculoPI fmt::fmt)
target_link_libraries(ejercicio fmt::fmt)

