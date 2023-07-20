## new script ##################################################################
cat("\014")    # clear the console
rm(list=ls())  # remove all variables
graphics.off() # close all plots
set.seed(2020) # fix randomness
suppressMessages(library(ggplot2))
suppressMessages(library(dplyr))
## input #######################################################################

# input parameter
args   <- commandArgs(trailingOnly = T)
if (length(args) == 0){ args = c(10, 10, 5) }
N_beta  = as.numeric(args[1]) # number of beta
N_seeds = as.numeric(args[2]) # number of seeds
N_sim   = as.numeric(args[3]) # number of simulations each array job
print(paste0("N_beta  = ", N_beta,  "     (number of beta)"))
print(paste0("N_seeds = ", N_seeds, "     (number of seeds)"))
print(paste0("N_sim   = ", N_sim,   "     (number of simulations each array job)"))

## scenarios ###################################################################

scenarios_list = read.table("../In_out/input_files/scenarios_txt/scenarios_N.txt",
                            header = T, sep = "\t", comment.char = "",
                            stringsAsFactors = F)

print(nrow(scenarios_list))

system( paste0("rm -r /cluster/projects/nn9755k/yathc/out_file_folder;        mkdir /cluster/projects/nn9755k/yathc/out_file_folder") )

## bash.sh script for array job submission #####################################

K = paste0("#!/bin/bash

# Job name:
#SBATCH --job-name=folder

# Project:
#SBATCH --account=nn9755k

# Wall time limit:
#SBATCH --time=1:00:00

# Other parameters:
#SBATCH --mem-per-cpu=2G

####SBATCH --output=/cluster/projects/nn9755k/yathc/out_file_folder/slurm-%j.txt
#SBATCH --output=/cluster/projects/nn9755k/yathc/out_file_folder/slurm-%A_%a.txt --array=1-", nrow(scenarios_list), "

## Set up job environment:
set -o errexit  # Exit the script on any error
set -o nounset  # Treat any unset variables as an error

module --quiet purge  # Reset the modules to the system default
module load R/4.2.1-foss-2022a
####module load GSL/2.5-GCC-7.3.0-2.30
####module list

## Do some work:
Rscript ../src/make_sim_array_vax_scenarios.R $SLURM_ARRAY_TASK_ID ", N_beta, " ", N_seeds, " ", N_sim
)

# save as sh file
write(K, file = paste0("../src/create_folders.sh"))
