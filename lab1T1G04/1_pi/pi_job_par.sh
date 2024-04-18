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
make seq || exit 1 #exit if it doesn't work

#Execute the binary that we compiled earlier
./pi_par 10
