## new script ##################################################################
cat("\014")    # clear the console
rm(list=ls())  # remove all variables
graphics.off() # close all plots
set.seed(2020) # fix randomness
suppressMessages(library(ggplot2))
suppressMessages(library(dplyr))
## start date ##################################################################
Day1 = as.Date("2021-01-01")
DayT = as.Date("2021-08-31")
time = as.numeric(DayT - Day1 + 1)
## new folder ##################################################################

# folder
system( paste0("rm -r scenarios_csv; mkdir scenarios_csv") )
# save as csv
write.table(c(Year  = as.numeric(format(Day1,"%Y")),
              Month = as.numeric(format(Day1,"%m")),
              Date  = as.numeric(format(Day1,"%d")),
              date_start = as.character(Day1),
              data_end   = as.character(DayT),
              maxT       = time),
            file = paste0("scenarios_csv/", "time", ".csv"),
            quote = T,
            sep = ";",
            row.names = T, col.names = F)

# folder
system( paste0("rm -r ",
               "../../../ibm-vaccination/1-main-ABM/In_out/input_files/", "scenarios_txt; ",
               "mkdir ",
               "../../../ibm-vaccination/1-main-ABM/In_out/input_files/", "scenarios_txt") )
# save as txt
write.table(c(Year  = as.numeric(format(Day1,"%Y")),
              Month = as.numeric(format(Day1,"%m")),
              Date  = as.numeric(format(Day1,"%d")),
              date_start = as.character(Day1),
              data_end   = as.character(DayT),
              maxT       = time),
            file = paste0("../../../ibm-vaccination/1-main-ABM/In_out/input_files/",
                          "scenarios_txt/", "time", ".txt"),
            quote = F,
            sep = "\t",
            row.names = T, col.names = F)

## combinations ################################################################ oppdrag-473

# vaccination order
Prioritization_1 = c("18p") # PM
Prioritization_2 = c("65p") # AZ (not used anymore)
Prioritization_3 = c("18p") # JJ (not used anymore)

# permutations of order below aged 45
priority_perm = LETTERS[1:5]
matrix_perm = gtools::permutations(length(priority_perm), length(priority_perm), priority_perm)
Priority_order = plyr::mdply(matrix_perm, "paste0")$V1
Priority_order = Priority_order[1]

# vaccine efficacy profile
Profile = "H"

# transmissibility of vaccinated individuals
P_trans = 35

# adherence percentage
Adherence = c("H")

# time delay between 1st and 2nd dose
Delay = 12 # week

# which vaccines to include
Doses = "PM"

# regional prioritization
Regional = c(0.6,              # baseline
             seq(0, 3.0, 0.1)) # alternative
# avoid digits accuracy issue
# https://cran.r-project.org/doc/FAQ/R-FAQ.html#Why-doesn_0027t-R-think-these-numbers-are-equal_003f
Regional = round(Regional*10)/10

# targeted population in regional prioritization (8 = 45+; 10 = 18+; 9999 = non-stop)
# to stop regional prioritization
Regional_priority = c(10)

# regional prioritization
Geo = c(0,               # baseline
        seq(-30, 40, 5)) # alternative

# start time of regional prioritization (Jan: Day 1)
day_mth = c(0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31) # days each month
Regional_start = c(
  cumsum(day_mth)[1] + 1, # Jan: Day 1
  cumsum(day_mth)[2] + 1, # Feb: Day 32
  cumsum(day_mth)[3] + 1, # Mar: Day 60
  cumsum(day_mth)[4] + 1, # Apr: Day 91
  cumsum(day_mth)[5] + 1, # May: Day 121
  cumsum(day_mth)[6] + 1, # Jun: Day 152
  cumsum(day_mth)[7] + 1) # Jul: Day 182
# cumsum(day_mth)[8] + 1) # Aug: Day 213
# cumsum(day_mth)[9] + 1, # Sep: Day 244
# cumsum(day_mth)[10] + 1, # Oct: Day 274
# cumsum(day_mth)[11] + 1, # Nov: Day 305
# 9999) # no reopening

# control scenario
R_const = c(0,  #  0  = baseline using SYSVAK
            -1, # -1  = no vaccination
            1)  #  1  = vaccination in IBM

# Transmission reduction due to seasonality, off=0, on=1.
# Invokes use of file seasonality_off.txt / seasonality_on.txt in the model, respectively
# Seasonality = c(0, 20, 50)
# Seasonality = c(20, 50)
Seasonality = c(50)

# importation (monthly rate)
Import = c("M")

# reproduction number at the beginning
R_eff = c(0.66)

## scenarios ###################################################################

# one scenario
scenariosA = expand.grid(Prioritization_1  = Prioritization_1,
                         Prioritization_2  = Prioritization_2,
                         Prioritization_3  = Prioritization_3,
                         Priority_order    = Priority_order,
                         Profile           = Profile,
                         P_trans           = P_trans,
                         Adherence         = Adherence,
                         Delay             = Delay,
                         Doses             = Doses)
# many scenarios by combinations
# 0: R_const, i.e. baseline and NULL (no vaccination)
scenariosB0 = expand.grid(Regional          = 0,
                          Regional_priority = Regional_priority,
                          Geo               = 0,
                          Regional_start    = Regional_start[1],
                          Seasonality       = Seasonality,
                          R_const           = R_const)
# 1: Regional
scenariosB1 = expand.grid(Regional          = Regional,
                          Regional_priority = Regional_priority,
                          Geo               = Geo,
                          Regional_start    = Regional_start[1],
                          Seasonality       = Seasonality,
                          R_const           = R_const[length(R_const)])
# 2: Geo (with maximum threshold)
df_geo_max = read.csv2(paste0("ref/", paste0("geo_max_", max(Geo)), ".csv"))
scenariosB1 = scenariosB1[scenariosB1$Regional <
                            as.numeric(as.character(
                              factor(scenariosB1$Geo,
                                     levels = df_geo_max$Geo,
                                     labels = df_geo_max$Max)
                            )), ]
# 3: Regional_start
scenariosB3 = expand.grid(Regional          = Regional[1],
                          Regional_priority = Regional_priority,
                          Geo               = Geo[1],
                          Regional_start    = Regional_start,
                          Seasonality       = Seasonality,
                          R_const           = R_const[length(R_const)])
# combinations of duplicated
scenariosB = rbind(scenariosB0,
                   scenariosB1,
                   # scenariosB2,
                   scenariosB3)
# remove duplicated scenarios
scenariosB = scenariosB[!duplicated(scenariosB), ]
# reproduction number
scenariosC = expand.grid(Import             = Import,
                         R_eff             = R_eff)
# all scenarios
scenarios = rbind(merge(merge(scenariosA,
                              scenariosB),
                        scenariosC))
# number of scenarios
( N = nrow(scenarios) )

## names of files ##############################################################

# add names (full)
scenarios$Name = plyr::mdply(scenarios, "paste", sep = "-")$V1
# add index
scenarios$Name = paste(1:length(scenarios$Name), scenarios$Name, sep = "-")
# replace "[.]" by "p"
scenarios$Name = stringr::str_replace(scenarios$Name, "[.]", "p") # for Regional
scenarios$Name = stringr::str_replace(scenarios$Name, "[.]", "p") # for Reopen_percent
scenarios$Name = stringr::str_replace(scenarios$Name, "[.]", "p") # for R_eff
scenarios$Name = stringr::str_replace(scenarios$Name, "[.]", "p")
scenarios$Name = stringr::str_replace(scenarios$Name, "[.]", "p")
max(nchar(scenarios$Name)) # keep <= 60

# add names (age and risk groups)
scenarios$Name_priority_1  = paste("prioritization_1", scenarios$Prioritization_1, scenarios$Priority_order, sep = "_")
scenarios$Name_priority_2  = paste("prioritization_2", scenarios$Prioritization_2, scenarios$Priority_order, sep = "_")
scenarios$Name_priority_3  = paste("prioritization_3", scenarios$Prioritization_3, scenarios$Priority_order, sep = "_")
# add names (vaccine profile)
scenarios$Name_profile   = paste0("vaccine_profile_",   scenarios$Profile, "_", scenarios$P_trans, "_", scenarios$Delay)
# add names (vaccine adherence)
scenarios$Name_adherence = paste0("vaccine_adherence_", scenarios$Adherence)
# add names (vaccine doses)
scenarios$Name_doses = paste0("vaccine_doses_", scenarios$Doses, "_", scenarios$Delay)
# add names (import)
scenarios$Name_import = paste0("import_", scenarios$Import)
# add names (control)
scenarios$Name_control = paste0("control_", scenarios$R_const)
# add names (relative reopen)
scenarios$Name_relative_reopen = paste0("relative_reopen_", scenarios$R_const)
# add names (geo)
scenarios$Name_geo = paste0("geo_", scenarios$Geo)

## save ########################################################################

# save as csv
write.table(scenarios,
            file = paste0("scenarios_csv/", "scenarios_", "N", ".csv"),
            quote = T,
            sep = ";",
            row.names = T, col.names = NA)
# save as txt
write.table(scenarios,
            file = paste0("../../../ibm-vaccination/1-main-ABM/In_out/input_files/",
                          "scenarios_txt/", "scenarios_", "N", ".txt"),
            quote = T,
            sep = "\t",
            row.names = F, col.names = T)

## create other files ##########################################################

source("1_prioritization.R")
source("2_vaccine_profile.R")
source("3_vaccine_adherence.R")
source("4_vaccine_doses.R")
source("5_import.R")
source("6_control.R")
source("7_relative_reopen.R")
source("8_seasonality.R")
source("9_geo.R")