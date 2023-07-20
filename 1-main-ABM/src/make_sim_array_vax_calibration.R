## new script ##################################################################
cat("\014")    # clear the console
rm(list=ls())  # remove all variables
graphics.off() # close all plots
set.seed(2020) # fix randomness
suppressMessages(library(ggplot2))
suppressMessages(library(dplyr))
## input #######################################################################

# input parameter
args = (commandArgs(T))
if (length(args) == 0){ args = c(10000, 1000, 2) }
N_rep   = as.numeric(args[1]) # number of total parameter sets
N_job   = as.numeric(args[2]) # number of array jobs each submission
N_sub   = as.numeric(args[3]) # number of submissions
print(paste0("N_rep   = ", N_rep, "     (number of total parameter sets)"))
print(paste0("N_job   = ", N_job, "     (number of array jobs each submission)"))
print(paste0("N_sub   = ", N_sub, "     (number of submissions)"))

N_par = N_rep / N_sub # number of parameter sets each submission
print(paste0("N_par   = ", N_par, "     (number of parameter sets each submission)"))
N_sim = N_rep / N_job / N_sub # number of simulations each array job
print(paste0("N_sim   = ", N_sim, "     (number of simulations each array job)"))

# directory
path <- paste0("/cluster/projects/nn9755k/yathc/")

## calibration #################################################################

# calibration
print( scenario_name <- "0_calibration" )

# folder name
folder_scenario <- paste0("sim_",    scenario_name)
folder_in_out   <- paste0("in_out_", folder_scenario)

# create folders
system( paste0( "rm -r ", path, folder_scenario) )
system( paste0( "mkdir ", path, folder_scenario) )
system( paste0( "mkdir ", path, folder_scenario, "/", folder_in_out ) )
system( paste0( "cp -r ../In_out/input_files ", path, folder_scenario, "/." ) )

system( paste0("rm -r /cluster/projects/nn9755k/yathc/out_file_calibration; mkdir /cluster/projects/nn9755k/yathc/out_file_calibration") )

## print details of the scenario (baseline) ####################################

# scenarios
scenarios_list = read.table("../In_out/input_files/scenarios_txt/scenarios_N.txt",
                            header = T, sep = "\t", comment.char = "",
                            stringsAsFactors = F)

# create a file with names of the scenario and input files
name_files <- rbind( cbind ( "NAME_VAX_PRIORITY_SCEN_1", paste0( scenarios_list[ 1, "Name_priority_1"],  ".txt" ) ) ,
                     cbind ( "NAME_VAX_PRIORITY_SCEN_2", paste0( scenarios_list[ 1, "Name_priority_2"],  ".txt" ) ) ,
                     cbind ( "NAME_VAX_PRIORITY_SCEN_3", paste0( scenarios_list[ 1, "Name_priority_3"],  ".txt" ) ) ,
                     cbind ( "NAME_VAX_PROFILE_SCEN"   , paste0( scenarios_list[ 1, "Name_profile"],     ".txt" ) ) ,
                     cbind ( "NAME_ADHERENCE_SCEN"     , paste0( scenarios_list[ 1, "Name_adherence"],   ".txt" ) ) ,
                     cbind ( "NAME_DOSES_SCEN"         , paste0( scenarios_list[ 1, "Name_doses"],       ".txt" ) ) ,
                     cbind ( "NAME_IMPORT_SCEN"        , paste0( scenarios_list[ 1, "Name_import"],          ".txt" ) ) ,
                     cbind ( "NAME_CONTROL_SCEN"       , paste0( scenarios_list[ 1, "Name_control"],         ".txt" ) ) ,
                     cbind ( "NAME_REOPEN_SCEN"        , paste0( scenarios_list[ 1, "Name_relative_reopen"], ".txt" ) ) ,
                     cbind ( "NAME_GEO"                , paste0( scenarios_list[ 1, "Name_geo"],             ".txt" ) ) ,
                     cbind(  "NAME_SCENARIO" ,   scenarios_list[ 1, "Name"] ),
                     cbind ( "PRIORITIZATION_1", scenarios_list[ 1, "Prioritization_1"] ),
                     cbind ( "PRIORITIZATION_2", scenarios_list[ 1, "Prioritization_2"] ),
                     cbind ( "PRIORITIZATION_3", scenarios_list[ 1, "Prioritization_3"] ),
                     cbind ( "PROFILE",          scenarios_list[ 1, "Profile"] ),
                     cbind ( "P_TRANS",          scenarios_list[ 1, "P_trans"] ),
                     cbind ( "ADHERENCE",        scenarios_list[ 1, "Adherence"] ),
                     # cbind ( "VAX_RATE",       scenarios_list[ 1, "Rate"] ),
                     cbind ( "SEASONALITY",    scenarios_list[ 1, "Seasonality"] ),
                     cbind ( "DELAY_SCEN",             scenarios_list[ 1, "Delay"] ),
                     cbind ( "DOSES_SCEN",             scenarios_list[ 1, "Doses"] ),
                     cbind ( "REGIONAL",               scenarios_list[ 1, "Regional"] ),
                     cbind ( "REGIONAL_PRIORITY",      scenarios_list[ 1, "Regional_priority"] ),
                     cbind ( "GEO",                    scenarios_list[ 1, "Geo"] ),
                     # cbind ( "VIKEN",                  scenarios_list[ 1, "Viken"] ),
                     cbind ( "REGIONAL_START",           scenarios_list[ 1, "Regional_start"] ),
                     # cbind ( "REOPEN_PERCENT",         scenarios_list[ 1, "Reopen_percent"] ),
                     # cbind ( "CONTROL_H_REGION",       scenarios_list[ 1, "Control_prev_H"] ),
                     cbind ( "KEEP_R_CONSTANT",        scenarios_list[ 1, "R_const"] ),
                     cbind ( "IMPORT",                 scenarios_list[ 1, "Import"] ),
                     cbind ( "REPROD_NUM",             scenarios_list[ 1, "R_eff"] ) )
# save
write.table(name_files,
            file = paste0(path, folder_scenario, "/input_files/names_scenario.txt"),
            quote = F, sep = "\t",
            row.names = F, col.names = F)

## beta ########################################################################

# range of 5 betas at change points
beta_range = as.matrix(data.frame(
    #betaR = c(min = 0, max = 4),  # community
    betaH = c(min = 0.1, max = 0.3),  # household
    # betaP = c(min = 0, max = 4),  # workplace
    # betaS = c(min = 0, max = 4),  # school
    beta_cpt1 = c(min = 0.4, max = 0.7), # at change point 1
    beta_cpt2 = c(min = 0.25, max = 0.4), # at change point 2
    sus1 = c(min = 0.7, max = 1.0), # susceptibility by age groups
    sus2 = c(min = 0.7, max = 1.0),
    sus3 = c(min = 0.7, max = 1.0),
    sus4 = c(min = 0.4, max = 1.0),
    sus5 = c(min = 0.4, max = 0.8),
    sus6 = c(min = 0.7, max = 1.0),
    sus7 = c(min = 0.2, max = 0.6),
    sus8 = c(min = 0.2, max = 0.5),
    sus9 = c(min = 0.1, max = 0.5)))

# number of parameters/beta
N_beta <- ncol(beta_range)
# Latin hypercube sampling (from 0 to 1)
beta_01 <- lhs::randomLHS(N_rep, N_beta)
# min of beta
# matrix(rep(beta_range["min", ], each = N_rep), nrow = N_rep)
beta_min = t(replicate(N_rep, beta_range["min", ]))
# diff of beta
# matrix(rep(beta_range["max", ] - beta_range["min", ], each = N_rep), nrow = N_rep)
beta_diff = t(replicate(N_rep, beta_range["max", ] - beta_range["min", ]))
# beta with selected range
beta_lhs = beta_min + beta_01 * beta_diff
# names of beta
colnames(beta_lhs) = colnames(beta_range)

# (!) rounding the input values for more submission in bash
beta_lhs = round(beta_lhs*10000)/10000
# sorting according to betaH
index_sort = order(beta_lhs[, 1], decreasing = T)
beta_lhs = beta_lhs[index_sort, ]

# save
write.table(beta_lhs,
            file = paste0(path, folder_scenario, "/lhs_beta.txt"),
            quote = F, sep = "\t",
            row.names = F, col.names = F)

## bash.sh script for array job submission #####################################

for( n_sub in 1:N_sub ){ # number of submissions

    # SBATCH options
    K <- paste0("#!/bin/bash

# Job name:
#SBATCH --job-name=calibration

# Project:
#SBATCH --account=nn9755k

# Wall time limit:
#SBATCH --time=48:00:00

# Other parameters:
#SBATCH --mem-per-cpu=8G

####SBATCH --output=/cluster/projects/nn9755k/yathc/out_file_calibration/slurm-%j_", scenario_name, "_", "i", ".txt
#SBATCH --output=/cluster/projects/nn9755k/yathc/out_file_calibration/slurm-%A_", scenario_name, "_", n_sub, "_%a.txt --array=1-", N_job, "

## Set up job environment:
set -o errexit  # Exit the script on any error
set -o nounset  # Treat any unset variables as an error

module --quiet purge  # Reset the modules to the system default
module load R/4.2.1-foss-2022a
####module load GSL/2.5-GCC-7.3.0-2.30
####module list

## Do some work:
")

    # index of simulations
    index_sim = (1:N_par) + N_par*(n_sub - 1)
    # parameter sets
    K <- paste0(K,     "betaH=(", paste(beta_lhs[index_sim, "betaH"    ], collapse = " "), ")\n\n")
    K <- paste0(K, "beta_cpt1=(", paste(beta_lhs[index_sim, "beta_cpt1"], collapse = " "), ")\n\n")
    K <- paste0(K, "beta_cpt2=(", paste(beta_lhs[index_sim, "beta_cpt2"], collapse = " "), ")\n\n")
    K <- paste0(K,      "sus1=(", paste(beta_lhs[index_sim, "sus1"     ], collapse = " "), ")\n\n")
    K <- paste0(K,      "sus2=(", paste(beta_lhs[index_sim, "sus2"     ], collapse = " "), ")\n\n")
    K <- paste0(K,      "sus3=(", paste(beta_lhs[index_sim, "sus3"     ], collapse = " "), ")\n\n")
    K <- paste0(K,      "sus4=(", paste(beta_lhs[index_sim, "sus4"     ], collapse = " "), ")\n\n")
    K <- paste0(K,      "sus5=(", paste(beta_lhs[index_sim, "sus5"     ], collapse = " "), ")\n\n")
    K <- paste0(K,      "sus6=(", paste(beta_lhs[index_sim, "sus6"     ], collapse = " "), ")\n\n")
    K <- paste0(K,      "sus7=(", paste(beta_lhs[index_sim, "sus7"     ], collapse = " "), ")\n\n")
    K <- paste0(K,      "sus8=(", paste(beta_lhs[index_sim, "sus8"     ], collapse = " "), ")\n\n")
    K <- paste0(K,      "sus9=(", paste(beta_lhs[index_sim, "sus9"     ], collapse = " "), ")\n\n")

    for( n_sim in 1:N_sim ){ # number of simulations each array job

        # index of each simulation
        index_sim = paste0("$((", n_sim, " + ", N_sim, " * ($SLURM_ARRAY_TASK_ID - 1) + ", format(N_par*(n_sub - 1), scientific = F), "))")
        # directory of each simulation
        directory_sim = paste0(path, folder_scenario, "/", folder_in_out, "/o", index_sim)
        # if()
        K <- paste0(K, 'if [ ! -d "', directory_sim, '" ]; then', "\n")
        # create folders
        K <- paste0(K, "mkdir ",     directory_sim, "\n")
        K <- paste0(K, "chmod 755 ", directory_sim, "\n" )
        K <- paste0(K, "ln -s ", path, folder_scenario, "/", "input_files ", directory_sim, "/\n")

        # index of each array
        index_array = paste0("$((", n_sim - 1, " + ", N_sim, " * ($SLURM_ARRAY_TASK_ID - 1) ))")
        # run the main code
        K <- paste0(K, "./ABM",
                    " -e ", directory_sim,
                    " -i 0",
                    " -bR ", "${betaH", "[", index_array, "]}",
                    " -bH ", "${betaH", "[", index_array, "]}",
                    " -bP ", "${betaH", "[", index_array, "]}",
                    " -bS ", "${betaH", "[", index_array, "]}",
                    " -beta_cpt1 ", "${beta_cpt1", "[", index_array, "]}",
                    " -beta_cpt2 ", "${beta_cpt2", "[", index_array, "]}",
                    " -sus1 ", "${sus1", "[", index_array, "]}",
                    " -sus2 ", "${sus2", "[", index_array, "]}",
                    " -sus3 ", "${sus3", "[", index_array, "]}",
                    " -sus4 ", "${sus4", "[", index_array, "]}",
                    " -sus5 ", "${sus5", "[", index_array, "]}",
                    " -sus6 ", "${sus6", "[", index_array, "]}",
                    " -sus7 ", "${sus7", "[", index_array, "]}",
                    " -sus8 ", "${sus8", "[", index_array, "]}",
                    " -sus9 ", "${sus9", "[", index_array, "]}",
                    " -dur 5 -pres_dur 3 -lat 2 -s ", "1",
                    " -soc_dist 0 -soc_dist_2 0 -soc_dist_w 1 -amp_fac 1",
                    " -v -1\n")
        # permission
        K <- paste0(K, "chmod -R 755 ", directory_sim, "/output_files\n")

        # calculate R0 using LSE
        K <- paste0(K, "Rscript calibration_1_R0.R ", index_sim, " ", directory_sim, "/output_files\n")
        # remove Rt.txt file to reduce storage
        K <- paste0(K, paste0("rm ", directory_sim, "/output_files/", "Rt.txt\n"))
        # if()
        K <- paste0(K, paste0("fi", "\n\n"))

    } # n_sim

    # save as sh file
    write(K, file = paste0(     path, folder_scenario, "/", folder_in_out, "/sim_", "array_", n_sub, ".sh"))
    # permission
    system(paste0("chmod 755 ", path, folder_scenario, "/", folder_in_out, "/sim_", "array_", n_sub, ".sh"))

} # n_sub
