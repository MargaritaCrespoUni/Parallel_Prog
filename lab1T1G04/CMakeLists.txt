cmake_minimum_required(VERSION 3.24)
project(lab1T1G04 C)

set(CMAKE_C_STANDARD 11)

include_directories(2_sort)

add_executable(lab1T1G04
        1_pi/pi_seq.c
        2_sort/main.c
        2_sort/sort.c
        2_sort/sort.h
        3_primes/primes.c 1_pi/pi_par.c 1_pi/pi_task.c)
