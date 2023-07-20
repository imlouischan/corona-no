#!/bin/bash

# Job name:
#SBATCH --job-name=post_calibration

# Project:
#SBATCH --account=nn9755k

# Wall time limit:
#SBATCH --time=24:00:00

# Other parameters:
#SBATCH --mem-per-cpu=4G

####SBATCH --output=/cluster/projects/nn9755k/yathc/out_file_calibration/slurm-%j.txt
#SBATCH --output=/cluster/projects/nn9755k/yathc/out_file_calibration/slurm-%A_%a.txt --array=1

## Set up job environment:
set -o errexit  # Exit the script on any error
set -o nounset  # Treat any unset variables as an error

module --quiet purge  # Reset the modules to the system default
module load R/4.2.1-foss-2022a
####module list

## Do some work:
$1
$2
$3
$4
