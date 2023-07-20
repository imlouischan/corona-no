## new script ##################################################################
cat("\014")    # clear the console
rm(list=ls())  # remove all variables
graphics.off() # close all plots
set.seed(2020) # fix randomness
suppressMessages(library(ggplot2))
suppressMessages(library(dplyr))
suppressMessages(library(tidyverse))
## input #######################################################################

# input parameter
# message("No input parameter")

# directory
path <- paste0("/cluster/projects/nn9755k/yathc/")
# calibration
scenario_name <- "0_calibration"
# folder name
folder_scenario <- paste0("sim_",    scenario_name)
folder_in_out   <- paste0("in_out_", folder_scenario)

## calibration #################################################################

# load data: beta and parameters
# ( filename <- paste0("/Users/louis/Downloads", "/", "lhs_beta.txt") )
( filename <- paste0(path, folder_scenario, "/", "lhs_beta.txt") )
txt_beta = read.table(filename,
                      header = F, sep = "\t", comment.char = "",
                      stringsAsFactors = F)

# beta & R0
df_beta_R0 = txt_beta
colnames(df_beta_R0) = c("beta", "beta_cpt1", "beta_cpt2", paste0("sus", 1:9))
df_beta_R0$R_merler = NA
df_beta_R0$R_gt     = NA
df_beta_R0$R_mean   = NA
df_beta_R0$R0       = NA
df_beta_R0$lse      = NA

# load data: R0 and parameters each simulation
N_sim = nrow(df_beta_R0)
count_fail = 0
for(n_sim in 1:N_sim){ # each simulation
  
  ( filename <- paste0(path, folder_scenario, "/", folder_in_out, "/o", n_sim, "/output_files/",
                       "calibration_R0_", n_sim, ".txt") )
  
  if( file.exists(filename) ){ # yes file
    
    txt_par = read.table(filename,
                         header = T, sep = "\t", comment.char = "",
                         stringsAsFactors = F)
    
    # select a method to calibrate
    df_beta_R0$R_merler[n_sim] = txt_par$R_merler # method 1
    df_beta_R0$R_gt[n_sim]     = txt_par$R_gt     # method 2
    df_beta_R0$R_mean[n_sim]   = txt_par$R_mean   # method 3
    df_beta_R0$R0[n_sim]       = txt_par$R0       # method 4
    df_beta_R0$lse[n_sim]      = txt_par$lse      # least squares
    
    message("n_sim = ", n_sim,
            "; R0 = ", format(df_beta_R0$R0[n_sim], digits = 3, nsmall = 2),
            "; lse = ", format(df_beta_R0$lse[n_sim]), ";")
    
  } else { # no file
    
    # counter
    count_fail = count_fail + 1
    message("n_sim = ", n_sim, "; Error reading the file, NA assigned!", " count_fail = ", count_fail)
    
    # remove folder (for resubmission)
    system( paste0( "rm -r ", path, folder_scenario, "/", folder_in_out, "/o", n_sim) )
    
  } # file.exists
} # n_sim

# save file
( filename <- paste0(path, folder_scenario, "/", "lhs_beta_R0.txt") )
write.table(df_beta_R0,
            file = filename,
            quote = F, sep = "\t",
            row.names = T, col.names = T)