cmake_minimum_required(VERSION 3.20)
project(robo_aspirador C)

set(CMAKE_C_STANDARD 11)

add_executable(robo_aspirador
        main.c
        vacuum.c
        plan.c
        tarefa-03/common.c
        tarefa-03/common.h
        tarefa-03/objectiveFunction.c
        tarefa-03/objectiveFunction.h
        tarefa-03/anneling.c
        tarefa-03/anneling.h
        tarefa-03/beam.c
        tarefa-03/beam.h
)

target_link_libraries(robo_aspirador m)
