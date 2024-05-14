#!/bin/bash
#SBATCH -J mpi_comms
#SBATCH --output=com_%j.out
#SBATCH --error=com_%j.err
#SBATCH -p ippd-cpu
#SBATCH --time=00:02:00
#SBATCH --nodes=1
#SBATCH --ntasks=4
#SBATCH --cpus-per-task=1

module load GCC/10.2.0
module load OpenMPI/4.1.2-GCC-10.2.0-with-slurm

export OMP_NUM_THREADS=1

make par || exit 1 # Exit if make fails

mpirun -n 1 integral2D_par surface_10_10.dat 10 10
mpirun -n 2 integral2D_par surface_10_10.dat 10 10
mpirun -n 4 integral2D_par surface_10_10.dat 10 10

mpirun -n 1 integral2D_par surface_50_50.dat 50 50
mpirun -n 2 integral2D_par surface_50_50.dat 50 50
mpirun -n 4 integral2D_par surface_50_50.dat 50 50

mpirun -n 1 integral2D_par surface_100_100.dat 100 100
mpirun -n 2 integral2D_par surface_100_100.dat 100 100
mpirun -n 4 integral2D_par surface_100_100.dat 100 100

mpirun -n 1 integral2D_par surface_1000_1000.dat 1000 1000
mpirun -n 2 integral2D_par surface_1000_1000.dat 1000 1000
mpirun -n 4 integral2D_par surface_1000_1000.dat 1000 1000

mpirun -n 1 integral2D_par surface_10000_10000.dat 10000 10000
mpirun -n 2 integral2D_par surface_10000_10000.dat 10000 10000
mpirun -n 4 integral2D_par surface_10000_10000.dat 10000 10000

mpirun -n 1 integral2D_par surface_20000_20000.dat 20000 20000
mpirun -n 2 integral2D_par surface_20000_20000.dat 20000 20000
mpirun -n 4 integral2D_par surface_20000_20000.dat 20000 20000