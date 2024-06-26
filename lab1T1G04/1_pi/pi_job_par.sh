#!/bin/bash

#SBATCH --job-name=pi
#SBATCH -p ippd-cpu
#SBATCH --output=pi_%j.out
#SBATCH --error=pi_%j.err
#SBATCH --cpus-per-task=8
#SBATCH --ntasks=1
#SBATCH --nodes=1
#SBATCH --time=00:00:30

#Compile the binary using Makefile
make par || exit 1 #exit if it doesn't work
make seq || exit 1 #exit if it doesn't work

./pi_seq 100000000
#Execute the binary that we compiled earlier
./pi_par 100000000
