#!/bin/bash
#SBATCH -J indef_integral_par
#SBATCH --output=indef_%j.out
#SBATCH --error=indef_%j.err
#SBATCH -p ippd-cpu
#SBATCH --time=00:02:00
#SBATCH --nodes=1
#SBATCH --ntasks=8
#SBATCH --cpus-per-task=1

module load GCC/10.2.0
module load OpenMPI/4.1.2-GCC-10.2.0-with-slurm

make seq || exit 1 # Exit if make fails
make par || exit 1 # Exit if make fails

./indef_integral_seq 100000000 1000
mpirun -n 8 indef_integral_par 100000000 1000
