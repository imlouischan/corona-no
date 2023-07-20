## new script ##################################################################
cat("\014")    # clear the console
rm(list=ls())  # remove all variables
graphics.off() # close all plots
set.seed(2020) # fix randomness
suppressMessages(library(ggplot2))
suppressMessages(library(dplyr))
## input #######################################################################

scenarios_list = read.table("../In_out/input_files/scenarios_txt/scenarios_N.txt", 
                            header = T, sep = "\t", comment.char = "", 
                            stringsAsFactors = F)

print(nrow( scenarios_list))

system( paste0("rm -r /cluster/projects/nn9755k/yathc/out_file_scenario;        mkdir /cluster/projects/nn9755k/yathc/out_file_scenario") )

## bash.sh script for array job submission #####################################

K = "#!/bin/bash\n\n"

for( sc in 1:nrow( scenarios_list) ){
  
  K = paste0(K, 
             "sbatch ", 
             "/cluster/projects/nn9755k/yathc/sim_", scenarios_list$Name[sc], "/in_out_sim_", scenarios_list$Name[sc], "/sim_array.sh ",
             "../src ",
             "/cluster/projects/nn9755k/yathc/sim_", scenarios_list$Name[sc], "/in_out_sim_", scenarios_list$Name[sc],"\n\n")
}

# save as sh file
write(K, file = paste0( "../src/run_sim2_scenarios.sh" ))
