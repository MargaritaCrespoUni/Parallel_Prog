#!/bin/bash
#SBATCH -J def_integral_par
#SBATCH --output=int_%j.out
#SBATCH --error=int_%j.err
#SBATCH -p ippd-cpu
#SBATCH --time=00:02:00
#SBATCH --nodes=1
#SBATCH --ntasks=4
#SBATCH --cpus-per-task=6

module load GCC/10.2.0
module load OpenMPI/4.1.2-GCC-10.2.0-with-slurm

make seq || exit 1 # Exit if make fails
make par || exit 1 # Exit if make fails

./def_integral_seq 1000000000 1000
./def_integral_par 1000000000 1000
