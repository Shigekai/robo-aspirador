cmake_minimum_required(VERSION 3.20)
project(robo_aspirador C)

set(CMAKE_C_STANDARD 11)

add_executable(robo_aspirador
        main.c
        vacuum.c
        plan.c
)

target_link_libraries(robo_aspirador m)
