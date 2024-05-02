cmake_minimum_required(VERSION 3.24)
project(Parallel_Prog C)

set(CMAKE_C_STANDARD 11)

add_executable(Parallel_Prog
        1_communications/mpi_comms.c
        2_def_integral/def_integral_seq.c
        3_indef_integral/indef_integral_seq.c
        4_integral2D/integral2D_par.c)
