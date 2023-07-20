## new script ##################################################################
cat("\014")    # clear the console
rm(list=ls())  # remove all variables
graphics.off() # close all plots
set.seed(2020) # fix randomness
suppressMessages(library(ggplot2))
suppressMessages(library(dplyr))
suppressMessages(library(lhs))
## input #######################################################################

# input parameter
args   <- commandArgs(trailingOnly = T)
if (length(args) == 0){ args = c(1, 10, 10, 5) }
sc      = as.numeric(args[1]) # index of scenarios
N_beta  = as.numeric(args[2]) # number of beta
N_seeds = as.numeric(args[3]) # number of seeds
N_sim   = as.numeric(args[4]) # number of simulations each array job
print(paste0("sc      = ", sc,      "     (index of scenarios)"))
print(paste0("N_beta  = ", N_beta,  "     (number of beta)"))
print(paste0("N_seeds = ", N_seeds, "     (number of seeds)"))
print(paste0("N_sim   = ", N_sim,   "     (number of simulations each array job)"))

# directory
path <- paste0("/cluster/projects/nn9755k/yathc/")

## scenarios ###################################################################

# scenarios
scenarios_list = read.table("../In_out/input_files/scenarios_txt/scenarios_N.txt",
                            header = T, sep = "\t", comment.char = "",
                            stringsAsFactors = F)

# one scenario
print( scenario_name <- scenarios_list[sc, "Name"] )
print( R0            <- scenarios_list[sc, "R_eff"] )

# folder name
folder_scenario <- paste0( "sim_", scenario_name )
folder_lhs      <- paste0( "sim_", "0_calibration" )

folder_in_out          <- paste0("in_out_", folder_scenario )
folder_in_out_lhs      <- paste0("in_out_", folder_lhs)

# create folders
system( paste0( "rm -r ", path, folder_scenario) )
system( paste0( "mkdir ", path, folder_scenario) )
system( paste0( "mkdir ", path, folder_scenario, "/", folder_in_out ) )
system( paste0( "cp -r ../In_out/input_files ", path, folder_scenario, "/." ) )

## print details of the scenario ###############################################

# create a file with names of the scenario and input files
name_files <- rbind( cbind ( "NAME_VAX_PRIORITY_SCEN_1", paste0( scenarios_list[ sc, "Name_priority_1"],  ".txt" ) ) ,
                     cbind ( "NAME_VAX_PRIORITY_SCEN_2", paste0( scenarios_list[ sc, "Name_priority_2"],  ".txt" ) ) ,
                     cbind ( "NAME_VAX_PRIORITY_SCEN_3", paste0( scenarios_list[ sc, "Name_priority_3"],  ".txt" ) ) ,
                     cbind ( "NAME_VAX_PROFILE_SCEN"   , paste0( scenarios_list[ sc, "Name_profile"],     ".txt" ) ) ,
                     cbind ( "NAME_ADHERENCE_SCEN"     , paste0( scenarios_list[ sc, "Name_adherence"],   ".txt" ) ) ,
                     cbind ( "NAME_DOSES_SCEN"         , paste0( scenarios_list[ sc, "Name_doses"],       ".txt" ) ) ,
                     cbind ( "NAME_IMPORT_SCEN"        , paste0( scenarios_list[ sc, "Name_import"],          ".txt" ) ) ,
                     cbind ( "NAME_CONTROL_SCEN"       , paste0( scenarios_list[ sc, "Name_control"],         ".txt" ) ) ,
                     cbind ( "NAME_REOPEN_SCEN"        , paste0( scenarios_list[ sc, "Name_relative_reopen"], ".txt" ) ) ,
                     cbind ( "NAME_GEO"                , paste0( scenarios_list[ sc, "Name_geo"],             ".txt" ) ) ,
                     cbind(  "NAME_SCENARIO" ,   scenarios_list[ sc, "Name"] ),
                     cbind ( "PRIORITIZATION_1", scenarios_list[ sc, "Prioritization_1"] ),
                     cbind ( "PRIORITIZATION_2", scenarios_list[ sc, "Prioritization_2"] ),
                     cbind ( "PRIORITIZATION_3", scenarios_list[ sc, "Prioritization_3"] ),
                     cbind ( "PROFILE",          scenarios_list[ sc, "Profile"] ),
                     cbind ( "P_TRANS",          scenarios_list[ sc, "P_trans"] ),
                     cbind ( "ADHERENCE",        scenarios_list[ sc, "Adherence"] ),
                     # cbind ( "VAX_RATE",       scenarios_list[ sc, "Rate"] ),
                     # cbind ( "GEOGRAPHIC",     scenarios_list[ sc, "Geographic"] ),
                     cbind ( "SEASONALITY",    scenarios_list[ sc, "Seasonality"] ),
                     cbind ( "DELAY_SCEN",             scenarios_list[ sc, "Delay"] ),
                     cbind ( "DOSES_SCEN",             scenarios_list[ sc, "Doses"] ),
                     cbind ( "REGIONAL",               scenarios_list[ sc, "Regional"] ),
                     cbind ( "REGIONAL_PRIORITY",      scenarios_list[ sc, "Regional_priority"] ),
                     cbind ( "GEO",                    scenarios_list[ sc, "Geo"] ),
                     # cbind ( "VIKEN",                  scenarios_list[ sc, "Viken"] ),
                     cbind ( "REGIONAL_START",           scenarios_list[ sc, "Regional_start"] ),
                     # cbind ( "REOPEN_PERCENT",         scenarios_list[ sc, "Reopen_percent"] ),
                     # cbind ( "CONTROL_H_REGION",       scenarios_list[ sc, "Control_prev_H"] ),
                     cbind ( "KEEP_R_CONSTANT",        scenarios_list[ sc, "R_const"] ),
                     cbind ( "IMPORT",                 scenarios_list[ sc, "Import"] ),
                     cbind ( "REPROD_NUM",             scenarios_list[ sc, "R_eff"] ) )

write.table(name_files,
            file = paste0(path, folder_scenario, "/input_files/names_scenario.txt"),
            quote = F, sep = "\t",
            row.names = F, col.names = F)

## beta ########################################################################

# load selected beta at change points
df_beta_trans = read.table(file = paste0("../In_out/input_files/", "calibration_beta_R0_sort", ".txt"),
                           header = T, sep = "\t", comment.char = "",
                           stringsAsFactors = F)

# select the top "N_beta" sets of parameters
beta = df_beta_trans[1:N_beta, c("beta", "beta_cpt1", "beta_cpt2", paste0("sus", 1:9))]

# save as txt
print(beta)
write.table(beta,
            file = paste0(path, folder_scenario, "/selected_beta.txt"),
            quote = F, sep = "\t",
            row.names = F, col.names = F)

## bash.sh script for array job submission #####################################

N_beta  = nrow(beta)               # number of parameter sets
N_job   = N_beta * N_seeds / N_sim # number of array jobs each submission
print(paste0("N_job   = ", N_job, "     (number of array jobs each submission)"))

K <- paste0("#!/bin/bash

# Job name:
#SBATCH --job-name=scenario

# Project:
#SBATCH --account=nn9755k

# Wall time limit:
#SBATCH --time=24:00:00

# Other parameters:
#SBATCH --mem-per-cpu=8G

####SBATCH --output=/cluster/projects/nn9755k/yathc/out_file_scenario/slurm-%j_", scenario_name, "_", "i", ".txt
#SBATCH --output=/cluster/projects/nn9755k/yathc/out_file_scenario/slurm-%A_", scenario_name, "_%a.txt --array=1-", N_job, "

## Set up job environment:
set -o errexit  # Exit the script on any error
set -o nounset  # Treat any unset variables as an error

module --quiet purge  # Reset the modules to the system default
module load R/4.2.1-foss-2022a
####module load GSL/2.5-GCC-7.3.0-2.30
####module list

## Do some work:
")

# parameter sets
K <- paste0(K,     "betaH=(", paste(beta[, "beta"     ], collapse = " "), ")\n\n")
K <- paste0(K, "beta_cpt1=(", paste(beta[, "beta_cpt1"], collapse = " "), ")\n\n")
K <- paste0(K, "beta_cpt2=(", paste(beta[, "beta_cpt2"], collapse = " "), ")\n\n")
K <- paste0(K,      "sus1=(", paste(beta[, "sus1"     ], collapse = " "), ")\n\n")
K <- paste0(K,      "sus2=(", paste(beta[, "sus2"     ], collapse = " "), ")\n\n")
K <- paste0(K,      "sus3=(", paste(beta[, "sus3"     ], collapse = " "), ")\n\n")
K <- paste0(K,      "sus4=(", paste(beta[, "sus4"     ], collapse = " "), ")\n\n")
K <- paste0(K,      "sus5=(", paste(beta[, "sus5"     ], collapse = " "), ")\n\n")
K <- paste0(K,      "sus6=(", paste(beta[, "sus6"     ], collapse = " "), ")\n\n")
K <- paste0(K,      "sus7=(", paste(beta[, "sus7"     ], collapse = " "), ")\n\n")
K <- paste0(K,      "sus8=(", paste(beta[, "sus8"     ], collapse = " "), ")\n\n")
K <- paste0(K,      "sus9=(", paste(beta[, "sus9"     ], collapse = " "), ")\n\n")

for( n_sim in 1:N_sim ){ # number of simulations each array job

  # index of each simulation
  index_sim = paste0("$((", n_sim, " + ", N_sim, " * ($SLURM_ARRAY_TASK_ID - 1) ))")
  # directory of each simulation
  directory_sim = paste0(path, folder_scenario, "/", folder_in_out, "/o", index_sim)
  # create folders
  K <- paste0(K, "mkdir ",     directory_sim, "\n")
  K <- paste0(K, "chmod 755 ", directory_sim, "\n" )
  K <- paste0(K, "ln -s ", path, folder_scenario, "/", "input_files ", directory_sim, "/\n")

  # index of each array for beta
  index_array_beta = paste0("$(( (", n_sim - 1, " + ", N_sim, " * ($SLURM_ARRAY_TASK_ID - 1) ) / ", N_seeds, " ))")
  # index of each array for seed
  index_array_seed = paste0("$(( (", n_sim - 1, " + ", N_sim, " * ($SLURM_ARRAY_TASK_ID - 1) ) % ", N_seeds, " + 1 ))")
  # run the main code
  K <- paste0(K, "./ABM",
              " -e ", directory_sim,
              " -i 0",
              " -bR ", "${betaH", "[", index_array_beta, "]}",
              " -bH ", "${betaH", "[", index_array_beta, "]}",
              " -bP ", "${betaH", "[", index_array_beta, "]}",
              " -bS ", "${betaH", "[", index_array_beta, "]}",
              " -beta_cpt1 ", "${beta_cpt1", "[", index_array_beta, "]}",
              " -beta_cpt2 ", "${beta_cpt2", "[", index_array_beta, "]}",
              " -sus1 ", "${sus1", "[", index_array_beta, "]}",
              " -sus2 ", "${sus2", "[", index_array_beta, "]}",
              " -sus3 ", "${sus3", "[", index_array_beta, "]}",
              " -sus4 ", "${sus4", "[", index_array_beta, "]}",
              " -sus5 ", "${sus5", "[", index_array_beta, "]}",
              " -sus6 ", "${sus6", "[", index_array_beta, "]}",
              " -sus7 ", "${sus7", "[", index_array_beta, "]}",
              " -sus8 ", "${sus8", "[", index_array_beta, "]}",
              " -sus9 ", "${sus9", "[", index_array_beta, "]}",
              " -dur 5 -pres_dur 3 -lat 2 -s ", index_array_seed,
              " -soc_dist 0 -soc_dist_2 0 -soc_dist_w 1 -amp_fac 1",
              " -v -1\n")

  # permission
  K <- paste0(K, "chmod -R 755 ", directory_sim, "/output_files\n\n")

} # n_sim

# save as sh file
write(K, file = paste0(     path, folder_scenario, "/", folder_in_out, "/sim_", "array", ".sh"))
# permission
system(paste0("chmod 755 ", path, folder_scenario, "/", folder_in_out, "/sim_", "array", ".sh"))
