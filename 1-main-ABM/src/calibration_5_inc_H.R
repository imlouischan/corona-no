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
args = (commandArgs(T))
if (length(args) == 0){ args = c(10) }
bestN   = as.numeric(args[1]) # number of selected beta
print(paste0("bestN   = ", bestN, "     (number of selected beta)"))

# directory
path <- paste0("/cluster/projects/nn9755k/yathc/")
# calibration
scenario_name <- "0_calibration"
# folder name
folder_scenario <- paste0("sim_",    scenario_name)
folder_in_out   <- paste0("in_out_", folder_scenario)

## data (for plotting) #########################################################

# load data
df_rds = readRDS(paste0("../../../ibm-vaccination/",
                        "/0-pre-processing/case_data/input/",
                        "data_regional_prio.RDS"))
# order of levels
df_rds$incidence$type = factor(df_rds$incidence$type,
                               levels = c("cases", "hosp", "ICU", "death"),
                               labels = c("Infections", "Hospitalizations", "Ventilators", "Deaths"))
df_rds$age$type       = factor(df_rds$age$type,
                               levels = c("cases", "hosp", "ICU", "death"),
                               labels = c("Infections", "Hospitalizations", "Ventilators", "Deaths"))
df_rds$prior$type     = factor(df_rds$prior$type,
                               levels = c("cases", "hosp", "ICU", "death"),
                               labels = c("Infections", "Hospitalizations", "Ventilators", "Deaths"))
# until July 2021
date_last = as.Date("2021-07-31")
# keep incidence
df_rds$incidence =
  df_rds$incidence %>%
  filter(date <= date_last) %>%
  complete(date, type, fill = list(value = 0)) %>%
  group_by(type) %>%
  arrange(date)
# from incidence to cumulative
df_rds$cumulative =
  df_rds$incidence %>%
  mutate(value = cumsum(value))
# priority groups
df_rds$prior$prior_0 = factor(df_rds$prior$prior_0,
                              levels = c(1:-1),
                              labels = c("Plus", "Neutral", "Minus"))
# complete zeros
df_rds$age =
  df_rds$age %>%
  tidyr::complete(age, type, fill = list(value = 0))
# as.factor
df_rds$age$age = factor(df_rds$age$age)
levels(df_rds$age$age)[levels(df_rds$age$age) == "80-89"] = "80+"
levels(df_rds$age$age)[levels(df_rds$age$age) == "90+"]   = "80+"
df_rds$age =
  df_rds$age %>%
  group_by(age, type) %>%
  mutate(value = sum(value))
df_rds$age = as.data.frame(df_rds$age)

# rename for matching
colnames(df_rds$incidence)  = c("time",  "Target_name", "Target_value")
colnames(df_rds$cumulative) = c("time",  "Target_name", "Target_value")
colnames(df_rds$prior)     = c("Group", "Target_value", "Target_name")
colnames(df_rds$age)       = c("Group", "Target_name", "Target_value")
# add columns for matching
df_rds$prior$time = date_last
df_rds$age$time   = date_last
df_rds$prior$Group_name = "Priority"
df_rds$age$Group_name   = "Age"

# data of inc_H
df_inc_H =
  df_rds$incidence %>%
  filter(Target_name == "Hospitalizations")

# data of age_H
df_age_H =
  df_rds$age %>%
  filter(Target_name == "Hospitalizations")
df_age_H = df_age_H[1:9, ]

# data of age distribution of 4 outcomes
df_age_4 =
  df_rds$age
df_age_4 = df_age_4[!duplicated(df_age_4), ]

# for melting
colnames(df_inc_H)[colnames(df_inc_H) == "Target_value"] = "Data"
colnames(df_age_H)[colnames(df_age_H) == "Target_value"] = "Data"
colnames(df_age_4)[colnames(df_age_4) == "Target_value"] = "Data"

## model output ################################################################

# load data (national_cases)
# path_file <- paste("/Users/louis/Downloads", "lhs_beta_R0_sort.txt", sep = "/")
( path_file <- paste0(path, folder_scenario, "/", "lhs_beta_R0_sort.txt") )
df_beta_R0_sort <- read.table(path_file,
                              header = T,
                              sep = "\t")

# selected simulations
for(n in as.numeric(rownames(df_beta_R0_sort)[1:bestN])){
  
  # selected simulations
  print(paste0("n = ", n))
  
  # directory of each simulation
  directory_sim <- paste0("/cluster/projects/nn9755k/yathc/",
                          "sim_0_calibration/in_out_sim_0_calibration/o", n,
                          "/output_files")
  # directory_sim <- paste0("/Users/louis/Downloads")
  
  # load data (national_cases)
  path_file <- paste(directory_sim, "national_cases.txt", sep = "/")
  txt_nc <- read.table(path_file,
                       header = T,
                       sep = "\t")
  
  # time period
  df_time <- read.table(paste0("../../../ibm-vaccination/1-main-ABM/In_out/input_files/scenarios_txt/time.txt"),
                        row.names = 1, stringsAsFactors = F)
  txt_nc$time     = txt_nc$time  + as.Date(df_time["date_start", ]) - 1
  
  # model output: until July 2021
  txt_nc =
    txt_nc %>%
    filter(time <= date_last)
  
  # model output: incidence of hospitalizations
  txt_nc$inc_H = txt_nc$cum_H - lag(txt_nc$cum_H, default = 0)
  df_inc_H = cbind(df_inc_H, Model = txt_nc$inc_H)
  colnames(df_inc_H)[length(colnames(df_inc_H))] = n
  
  # model output: age distribution of hospitalizations
  df_age_H_sim = txt_nc %>%
    filter(time == max(time)) %>%
    select(paste0("cum_H_", 0:100)) %>%
    rowwise() %>%
    summarise(cum_H_0_9   = sum(c_across(paste0("cum_H_", 0:9))),
              cum_H_10_19 = sum(c_across(paste0("cum_H_", 10:19))),
              cum_H_20_29 = sum(c_across(paste0("cum_H_", 20:29))),
              cum_H_30_39 = sum(c_across(paste0("cum_H_", 30:39))),
              cum_H_40_49 = sum(c_across(paste0("cum_H_", 40:49))),
              cum_H_50_59 = sum(c_across(paste0("cum_H_", 50:59))),
              cum_H_60_69 = sum(c_across(paste0("cum_H_", 60:69))),
              cum_H_70_79 = sum(c_across(paste0("cum_H_", 70:79))),
              cum_H_80p   = sum(c_across(paste0("cum_H_", 80:100))))
  df_age_H = cbind(df_age_H, t(df_age_H_sim))
  colnames(df_age_H)[length(colnames(df_age_H))] = n
  
  # model output: age distribution of 4 outcomes
  df_age_4_sim = txt_nc %>%
    filter(time == max(time)) %>%
    select(c(paste0("cum_I_", 0:100),
             paste0("cum_H_", 0:100),
             paste0("cum_ICU_", 0:100),
             paste0("cum_D_", 0:100))) %>%
    rowwise() %>%
    summarise(cum_I_0_9   = sum(c_across(paste0("cum_I_", 0:9))),
              cum_H_0_9   = sum(c_across(paste0("cum_H_", 0:9))),
              cum_ICU_0_9   = sum(c_across(paste0("cum_ICU_", 0:9))),
              cum_D_0_9   = sum(c_across(paste0("cum_D_", 0:9))),
              
              cum_I_10_19 = sum(c_across(paste0("cum_I_", 10:19))),
              cum_H_10_19 = sum(c_across(paste0("cum_H_", 10:19))),
              cum_ICU_10_19 = sum(c_across(paste0("cum_ICU_", 10:19))),
              cum_D_10_19 = sum(c_across(paste0("cum_D_", 10:19))),
              
              cum_I_20_29 = sum(c_across(paste0("cum_I_", 20:29))),
              cum_H_20_29 = sum(c_across(paste0("cum_H_", 20:29))),
              cum_ICU_20_29 = sum(c_across(paste0("cum_ICU_", 20:29))),
              cum_D_20_29 = sum(c_across(paste0("cum_D_", 20:29))),
              
              cum_I_30_39 = sum(c_across(paste0("cum_I_", 30:39))),
              cum_H_30_39 = sum(c_across(paste0("cum_H_", 30:39))),
              cum_ICU_30_39 = sum(c_across(paste0("cum_ICU_", 30:39))),
              cum_D_30_39 = sum(c_across(paste0("cum_D_", 30:39))),
              
              cum_I_40_49 = sum(c_across(paste0("cum_I_", 40:49))),
              cum_H_40_49 = sum(c_across(paste0("cum_H_", 40:49))),
              cum_ICU_40_49 = sum(c_across(paste0("cum_ICU_", 40:49))),
              cum_D_40_49 = sum(c_across(paste0("cum_D_", 40:49))),
              
              cum_I_50_59 = sum(c_across(paste0("cum_I_", 50:59))),
              cum_H_50_59 = sum(c_across(paste0("cum_H_", 50:59))),
              cum_ICU_50_59 = sum(c_across(paste0("cum_ICU_", 50:59))),
              cum_D_50_59 = sum(c_across(paste0("cum_D_", 50:59))),
              
              cum_I_60_69 = sum(c_across(paste0("cum_I_", 60:69))),
              cum_H_60_69 = sum(c_across(paste0("cum_H_", 60:69))),
              cum_ICU_60_69 = sum(c_across(paste0("cum_ICU_", 60:69))),
              cum_D_60_69 = sum(c_across(paste0("cum_D_", 60:69))),
              
              cum_I_70_79 = sum(c_across(paste0("cum_I_", 70:79))),
              cum_H_70_79 = sum(c_across(paste0("cum_H_", 70:79))),
              cum_ICU_70_79 = sum(c_across(paste0("cum_ICU_", 70:79))),
              cum_D_70_79 = sum(c_across(paste0("cum_D_", 70:79))),
              
              cum_I_80p   = sum(c_across(paste0("cum_I_", 80:100))),
              cum_H_80p   = sum(c_across(paste0("cum_H_", 80:100))),
              cum_ICU_80p   = sum(c_across(paste0("cum_ICU_", 80:100))),
              cum_D_80p   = sum(c_across(paste0("cum_D_", 80:100))))
  df_age_4 = cbind(df_age_4, t(df_age_4_sim))
  colnames(df_age_4)[length(colnames(df_age_4))] = n
  
}

# add mean values
df_inc_H = cbind(df_inc_H,
                 Mean = rowMeans(df_inc_H[, colnames(df_inc_H) %in% as.numeric(rownames(df_beta_R0_sort)[1:bestN])]))
df_age_H = cbind(df_age_H,
                 Mean = rowMeans(df_age_H[, colnames(df_age_H) %in% as.numeric(rownames(df_beta_R0_sort)[1:bestN])]))
df_age_4 = cbind(df_age_4,
                 Mean = rowMeans(df_age_4[, colnames(df_age_4) %in% as.numeric(rownames(df_beta_R0_sort)[1:bestN])]))

# print ratio of data and model output
df_ratio = reshape2::dcast(cbind(Ratio = df_age_4$Data / df_age_4$Mean,
                                 df_age_4)[, 1:3],
                           Group ~ Target_name, value.var = "Ratio")
print(df_ratio)
# save as csv
print( filename <- paste0(path, folder_scenario, "/calibration_5_ratio-bestN_", format(bestN, scientific = F), ".csv") )
write.table(df_ratio,
            file = filename,
            sep = ";", dec = ".",
            row.names = F, col.names = T)

# melt for ggplot
df_inc_H = reshape2::melt(df_inc_H,
                          id.vars = c("time", "Target_name"))
df_age_H = reshape2::melt(df_age_H,
                          id.vars = c("Group", "Target_name", "time", "Group_name"))
df_age_4 = reshape2::melt(df_age_4,
                          id.vars = c("Group", "Target_name", "time", "Group_name"))

## plot hospitalizations #######################################################
ggfig = list()

# plot incidence of hospitalizations
ggfig[["inc_H"]] =
  ggplot(df_inc_H) +
  geom_line(data = . %>%
              filter(!(variable %in% c("Data", "Mean"))),
            aes(x = time,
                y = value,
                col = variable,
                group = variable),
            lwd = 1, alpha = 0.5) +
  geom_line(data = . %>%
              filter(variable %in% c("Mean")),
            aes(x = time,
                y = value),
            lwd = 0.5, alpha = 1.0) +
  geom_point(data = . %>%
               filter(variable %in% c("Data")),
             aes(x = time,
                 y = value)) +
  scale_x_date(breaks = c(seq(from = as.Date("2021-01-01"),
                              to   = as.Date("2022-12-01"),
                              by   = "month")),
               labels = c(t(outer(2021:2022, c("Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"), paste, sep = "-"))),
               expand = c(0, 0) ) +
  scale_y_continuous(limits = c(0, NA)) +
  labs(#title = target,
    x = "Time",
    y = "inc_H") +
  theme_bw(base_size = 15) +
  theme(legend.position = "none",
        axis.text.x = element_text(angle = 45, hjust = 1))

# age distribution of hospitalizations
ggfig[["age_H"]] =
  ggplot(df_age_H) +
  geom_col(data = . %>%
             filter(!(variable %in% c("Data", "Mean"))),
           aes(x = Group,
               y = value,
               fill = variable,
               group = variable),
           position = "dodge",
           lwd = 0.2,
           alpha = 0.5) +
  geom_col(data = . %>%
             filter(variable %in% c("Mean")),
           aes(x = Group,
               y = value),
           position = "dodge",
           col = "black", fill = NA, lwd = 0.2,
           alpha = 0.5) +
  geom_point(data = . %>%
               filter(variable %in% c("Data")),
             aes(x = Group,
                 y = value)) +
  # geom_text(data = . %>%
  #             filter(variable != "Data"),
  #           aes(x = Group,
  #               y = value,
  #               label = value), vjust = -0.5, size = 3) +
  geom_text(data = . %>%
              filter(variable == "Data"),
            aes(x = Group,
                y = value,
                label = value), vjust = -0.5, size = 5) +
  scale_y_continuous(limits = c(0, NA),
                     expand = expansion(mult = c(0, 0.2))) +
  labs(#title = target,
    x = "Age",
    y = "total_H") +
  theme_bw(base_size = 15) +
  theme(legend.position = "none",
        axis.text.x = element_text(angle = 45, hjust = 1))

# plot together
p <- cowplot::plot_grid(plotlist = ggfig,
                        nrow = 2,
                        labels = "AUTO",
                        hjust = 0,
                        label_size = 24, label_fontface = "plain",
                        align = "hv")
# save figure
print( name_plot <- paste0(path, folder_scenario, "/calibration_5_inc_H-bestN_", format(bestN, scientific = F), ".png") )
ggplot2::ggsave(file = name_plot, plot = p, width = 8.5, height = 11, type = "cairo")

## plot 4 outcomes #############################################################

p =
  ggplot(df_age_4) +
  geom_col(data = . %>%
             filter(!(variable %in% c("Data", "Mean"))),
           aes(x = Group,
               y = value,
               fill = variable,
               group = variable),
           position = "dodge",
           lwd = 0.2,
           alpha = 0.5) +
  geom_col(data = . %>%
             filter(variable %in% c("Mean")),
           aes(x = Group,
               y = value),
           position = "dodge",
           col = "black", fill = NA, lwd = 0.2,
           alpha = 0.5) +
  geom_point(data = . %>%
               filter(variable %in% c("Data")),
             aes(x = Group,
                 y = value)) +
  # geom_text(data = . %>%
  #             filter(variable != "Data"),
  #           aes(x = Group,
  #               y = value,
  #               label = value), vjust = -0.5, size = 3) +
  geom_text(data = . %>%
              filter(variable == "Data"),
            aes(x = Group,
                y = value,
                label = value), vjust = -0.5, size = 5) +
  facet_wrap(vars(Target_name),
             scales = "free_y") +
  scale_y_continuous(limits = c(0, NA),
                     expand = expansion(mult = c(0, 0.2))) +
  labs(#title = target,
    x = "Age",
    y = "Count") +
  theme_bw(base_size = 15) +
  theme(legend.position = "none",
        axis.text.x = element_text(angle = 45, hjust = 1))
# save figure
print( name_plot <- paste0(path, folder_scenario, "/calibration_5_age_4-bestN_", format(bestN, scientific = F), ".png") )
ggplot2::ggsave(file = name_plot, plot = p, width = 11, height = 8.5, type = "cairo")