ssh -XY $USER@fram.sigma2.no

# jp & en ########################################
locale
export LANGUAGE=en_US.UTF-8
export LC_ALL=en_US.UTF-8

# load R
module load R/4.2.1-foss-2022a

# load GSL
#module load GSL/2.5-GCC-7.3.0-2.30

#module load intel/2019a
#module swap GCCcore/7.3.0 GCCcore/8.2.0

# restart everything ######################################## step 0
rm -r /cluster/projects/nn9755k/$USER/
mkdir /cluster/projects/nn9755k/$USER/
cd /cluster/projects/nn9755k/$USER/

# create folders for calibration ######################################## step 1.1 (calibration)
cd /cluster/home/$USER/GitHub/ibm-vaccination/1-main-ABM/src
Rscript make_sim_array_vax_calibration.R 100000 200 10

# run calibration ########################################
cd /cluster/home/$USER/GitHub/ibm-vaccination/1-main-ABM/src
rm ABM
gcc *.c -o ABM -O3 -Wall -I/home/gsl/include -L/home/ -lm -lgsl -lgslcblas
sbatch /cluster/projects/nn9755k/$USER/sim_0_calibration/in_out_sim_0_calibration/sim_array_1.sh
sbatch /cluster/projects/nn9755k/$USER/sim_0_calibration/in_out_sim_0_calibration/sim_array_2.sh
sbatch /cluster/projects/nn9755k/$USER/sim_0_calibration/in_out_sim_0_calibration/sim_array_3.sh
sbatch /cluster/projects/nn9755k/$USER/sim_0_calibration/in_out_sim_0_calibration/sim_array_4.sh
sbatch /cluster/projects/nn9755k/$USER/sim_0_calibration/in_out_sim_0_calibration/sim_array_5.sh
sbatch /cluster/projects/nn9755k/$USER/sim_0_calibration/in_out_sim_0_calibration/sim_array_6.sh
sbatch /cluster/projects/nn9755k/$USER/sim_0_calibration/in_out_sim_0_calibration/sim_array_7.sh
sbatch /cluster/projects/nn9755k/$USER/sim_0_calibration/in_out_sim_0_calibration/sim_array_8.sh
sbatch /cluster/projects/nn9755k/$USER/sim_0_calibration/in_out_sim_0_calibration/sim_array_9.sh
sbatch /cluster/projects/nn9755k/$USER/sim_0_calibration/in_out_sim_0_calibration/sim_array_10.sh
squeue -A nn9755k
squeue -A nn9755k | grep PD
squeue -A nn9755k | grep R  | wc -l

# select beta ######################################## step 1.2 (calibration)
cd /cluster/home/$USER/GitHub/ibm-vaccination/1-main-ABM/src
sbatch Rscript_job.sh "Rscript calibration_2_beta_R0.R" "Rscript calibration_3_beta_trans.R 10" "Rscript calibration_4_beta_lse.R 10" "Rscript calibration_5_inc_H.R 10"

# create folders for scenarios ######################################## step 2.1 (scenarios)
cd /cluster/home/$USER/GitHub/ibm-vaccination/1-main-ABM/src
Rscript make_bash_script_to_create_folders.R 10 100 200
chmod +x create_folders.sh
sbatch create_folders.sh
squeue -A nn9755k

# run scenarios ######################################## step 2.2 (scenarios)
cd /cluster/home/$USER/GitHub/ibm-vaccination/1-main-ABM/src
rm ABM
gcc *.c -o ABM -O3 -Wall -I/home/gsl/include -L/home/ -lm -lgsl -lgslcblas
Rscript make_bash_script_to_run_scenarios.R
chmod +x run_sim2_scenarios.sh
./run_sim2_scenarios.sh
squeue -A nn9755k
squeue -A nn9755k | grep PD
squeue -A nn9755k | grep R  | wc -l

# run locally ######################################## step X
cd /Users/louis/GitHub/imlouischan/ibm-vaccination/1-main-ABM/src
rm ABM
gcc *.c -o ABM -O3 -Wall -I/opt/homebrew/include -L/opt/homebrew/lib -lm -lgsl -lgslcblas
./ABM -e ../In_out -i 0 -bR 0.228 -bH 0.228 -bP 0.228 -bS 0.228 -beta_cpt1 0.5307 -beta_cpt2 0.3556 -sus1 0.9569 -sus2 0.7888 -sus3 0.7481 -sus4 0.7424 -sus5 0.5485 -sus6 0.8927 -sus7 0.5022 -sus8 0.3429 -sus9 0.3396 -dur 5 -pres_dur 3 -lat 2 -s 10 -soc_dist 0 -soc_dist_2 0 -soc_dist_w 1 -amp_fac 1 -v 6
