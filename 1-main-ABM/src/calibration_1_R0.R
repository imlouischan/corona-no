## new script ##################################################################
cat("\014")    # clear the console
rm(list=ls())  # remove all variables
graphics.off() # close all plots
set.seed(2020) # fix randomness
suppressMessages(library(ggplot2))
suppressMessages(library(dplyr))
suppressMessages(library(tidyverse))
## input #######################################################################

# input parameter (simulation index)
args = (commandArgs(T))
if (length(args) == 0){ args = c(1, "/cluster/projects/nn9755k/yathc/sim_0_calibration/in_out_sim_0_calibration/o1") }
if (length(args) == 0){ args = c(1, "../In_out/output_files") }
n             = as.numeric(  args[1]) # simulation index
directory_sim = as.character(args[2]) # directory of each simulation
print(paste0("n =             ", n,             "     (simulation index)"))
print(paste0("directory_sim = ", directory_sim, "     (directory of each simulation)"))

## reproduction number #########################################################

# load data (Rt)
# path_file <- paste("/Users/louis/Downloads", "Rt.txt", sep = "/")
path_file <- paste(directory_sim, "Rt.txt", sep = "/")
txt_Rt <- read.table(path_file,
                     header = T,
                     sep = "\t")

# case reproduction number
txt_Rt %>%
  group_by(time) %>%
  summarise(N_infector = n(),
            N_infected = sum(N_inf),
            Rt = N_infected / N_infector) -> df_Rt

print( filename <- paste0(directory_sim, "/Rt_aggregated_", n, ".txt") )
write.table(df_Rt,
            file = filename,
            quote = F,
            sep = "\t",
            row.names = F, col.names = T)

## incidence ###################################################################

# load data (national_cases)
# path_file <- paste("/Users/louis/Downloads", "national_cases.txt", sep = "/")
path_file <- paste(directory_sim, "national_cases.txt", sep = "/")
txt_nc <- read.table(path_file,
                     header = T,
                     sep = "\t")

# total incidence
txt_nc$inc = txt_nc$inc_I # rowSums(txt_nc[, c("inc_I")]) #, "inc_Ia", "inc_E2")])
df_inc = data.frame(x = txt_nc$time,     # time
                    y = log(txt_nc$inc)) # total incidence in log

# choose the time interval
minT = 7                          # one week
maxT = which.max(txt_nc$inc)      # peak
windowT = maxT - minT             # time interval

if( windowT < 7 ){ # too short
  
  df_inc_RES = NULL
  linearT    = NULL
  r          = NA
  R_merler   = NA
  R_gt       = NA
  R_mean     = NA
  # R0         = mean( df_Rt$Rt[(1:7)+14] ) # 2 weeks ago
  R0         = mean( df_Rt$Rt[(1:7)] )    # today
  
} else {           # long enough
  
  df_lin = df_inc[minT:maxT, ]
  
  # linear part of a curve
  f <- function (d) {
    m <- lm(y~x, as.data.frame(d))
    return(coef(m)[2])
  }
  co <- zoo::rollapply(df_lin, 3, f, by.column = F)
  co.cl <- kmeans(co, 2)
  b.points <- which(co.cl$cluster == match(max(co.cl$centers), co.cl$centers))+1
  df_inc_RES <- df_lin[b.points, ]
  
  # update the time interval
  linearT = min(df_inc_RES$x):max(df_inc_RES$x)
  
  # growth rate
  fitting = lm(y ~ x, df_inc_RES) # fitting linear models
  r = fitting$coefficients["x"] # slope r
  
  # reproduction number (see ref:merler2010role) ############################### method 1
  omega = 1/3 # 1 / latent period
  gamma = 1/5 # 1 / infectious period
  R_merler = (1 + r/gamma) * (1 + r / omega)
  
  # R using generation time #################################################### method 2
  gt = 7.544286 # generation time
  R_gt = exp(r * gt)
  
  # R within the linear interval ############################################### method 3
  R_mean = mean( df_Rt$Rt[linearT] )
  
  # R during the 1st month ##################################################### method 4
  # R0 = mean( df_Rt$Rt[(1:7)+14] ) # 2 weeks ago
  R0 = mean( df_Rt$Rt[(1:7)] )    # today
  
}

## time period #################################################################

df_time <- read.table(paste0("../../../ibm-vaccination/1-main-ABM/In_out/input_files/scenarios_txt/time.txt"),
                      row.names = 1, stringsAsFactors = F)
txt_nc$time     = txt_nc$time  + as.Date(df_time["date_start", ]) - 1
df_inc$time     = df_inc$x     + as.Date(df_time["date_start", ]) - 1
df_inc_RES$time = df_inc_RES$x + as.Date(df_time["date_start", ]) - 1
df_Rt$time      = df_Rt$time   + as.Date(df_time["date_start", ]) - 1

## input parameters ############################################################

# load data
# path_file <- paste("/Users/louis/Downloads", "lhs_beta.txt", sep = "/")
path_file <- paste(directory_sim, "../../../lhs_beta.txt", sep = "/")
txt_lhs <- read.table(path_file,
                      header = F,
                      sep = "\t")
colnames(txt_lhs) = c("beta", "beta_cpt1", "beta_cpt2", paste0("sus", 1:9))

# beta in time
df_beta = data.frame(time = df_inc$time,
                     beta = txt_lhs[n, "beta"])
# change points
df_beta$beta[df_beta$time >= as.Date("2021-01-28")] = as.numeric(txt_lhs[n, "beta_cpt1"])
df_beta$beta[df_beta$time >= as.Date("2021-03-11")] = as.numeric(txt_lhs[n, "beta_cpt2"])

# susceptibility
df_sus = reshape2::melt(txt_lhs[n, paste0("sus", 1:9)])
df_sus$variable = c("0-9", "10-19", "20-29", "30-39", "40-49", "50-59", "60-69", "70-79", "80+")

## plot reproduction number ####################################################
ggfig = list()

# plot incidence in log-scale
ggfig[["inc"]] =
  ggplot(df_inc, aes(time, y)) +
  geom_line() +
  scale_x_date(breaks = c(seq(from = as.Date("2021-01-01"),
                              to   = as.Date("2022-12-01"),
                              by   = "month")),
               labels = c(t(outer(2021:2022, c("Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"), paste, sep = "-"))),
               expand = c(0, 0) ) +
  ggtitle(paste0("R0 = ",    round(R0*100)/100, "; ",
                 "slope = ", round(r *100)/100)) +
  xlab("time (until max incidence)") +
  ylab("log(incidence)") +
  theme_bw(base_size = 16) +
  theme(axis.text.x = element_text(angle = 45, hjust = 1))
# add the linear part of a curve
if( !is.null(dim(df_inc_RES)) ){
  ggfig[["inc"]] = ggfig[["inc"]] +
    geom_point(data = df_inc_RES, aes(time, y), col = "red") +
    geom_smooth(data = df_inc_RES[, c("time", "y")], method = lm)
}

# plot reproduction number in time
ggfig[["R"]] =
  ggplot(df_Rt, aes(time, Rt)) +
  geom_line() +
  geom_point(data = df_Rt[linearT, ], aes(time, Rt), col = "red") +
  geom_hline(yintercept = mean(R0), col = "red") +
  scale_x_date(breaks = c(seq(from = as.Date("2021-01-01"),
                              to   = as.Date("2022-12-01"),
                              by   = "month")),
               labels = c(t(outer(2021:2022, c("Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"), paste, sep = "-"))),
               expand = c(0, 0) ) +
  ggtitle(paste0("R_merler = ", round(R_merler*100)/100, "; ",
                 "R_gt = ",     round(R_gt    *100)/100, "; ",
                 "R_mean = ",   round(R_mean  *100)/100, "; ",
                 "R0 = ",       round(R0      *100)/100)) +
  xlab("time (entire period)") +
  ylab("Rt") +
  theme_bw(base_size = 16) +
  theme(axis.text.x = element_text(angle = 45, hjust = 1))

# plot beta in time
ggfig[["beta"]] =
  ggplot(df_beta) +
  geom_line(aes(x = time,
                y = beta),
            lwd = 1) +
  scale_x_date(breaks = c(seq(from = as.Date("2021-01-01"),
                              to   = as.Date("2022-12-01"),
                              by   = "month")),
               labels = c(t(outer(2021:2022, c("Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"), paste, sep = "-"))),
               expand = c(0, 0) ) +
  xlab("time (entire period)") +
  ylab("beta") +
  theme_bw(base_size = 16) +
  theme(axis.text.x = element_text(angle = 45, hjust = 1))

# plot susceptibility
ggfig[["sus"]] =
  ggplot(df_sus) +
  geom_col(aes(x = variable,
               y = value),
           col = "black", alpha = 0) +
  geom_text(aes(x = variable,
                y = value,
                label = round(value*100)/100), vjust = -0.5, size = 3) +
  scale_y_continuous(limits = c(0, NA),
                     expand = expansion(mult = c(0, 0.2))) +
  labs(#title = target,
    x = "Age",
    y = "Susceptibility") +
  theme_bw(base_size = 15) +
  theme(legend.position = "bottom",
        axis.text.x = element_text(angle = 45, hjust = 1))

# plot together
p <- cowplot::plot_grid(plotlist = ggfig,
                        nrow = 2,
                        labels = "AUTO",
                        hjust = 0,
                        label_size = 24, label_fontface = "plain",
                        align = "hv")
# save figure
print( name_plot <- paste0(directory_sim, "/Rt_", n, ".png") )
ggplot2::ggsave(file = name_plot, plot = p, width = 11, height = 8.5, type = "cairo")

## place #######################################################################

# load data (place_of_infection)
path_file <- paste(directory_sim, "place_of_infection.txt", sep = "/")
txt_place <- read.table(path_file,
                        header = T,
                        sep = "\t")

# merge workplace and university
df_place = data.frame(community = txt_place$community,
                      work.uni  = txt_place$work + txt_place$uni,
                      school    = txt_place$school,
                      house     = txt_place$house)
# fraction
df_place = df_place[nrow(df_place), ] / sum(df_place[nrow(df_place), ])

## data (for least square) #####################################################

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

## least squares ################################################################

# model output: until July 2021
txt_nc =
  txt_nc %>%
  filter(time <= date_last)

# model output: incidence of hospitalizations
txt_nc$inc_H = txt_nc$cum_H - lag(txt_nc$cum_H, default = 0)
df_inc_H$Target_value_sim = txt_nc$inc_H

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
df_age_H$Target_value_sim = t(df_age_H_sim)

# weight of age distribution
lse_age_w = df_age_H$Target_value / sum(df_age_H$Target_value) * length(df_age_H$Target_value)
lse_age_w = 1
# least squares
lse =
  sum((df_inc_H$Target_value - df_inc_H$Target_value_sim)^2) +
  sum(( (df_age_H$Target_value - df_age_H$Target_value_sim) / lse_age_w )^2)

## plot hospitalizations #######################################################
ggfig = list()

# plot incidence of hospitalizations
ggfig[["inc_H"]] =
  ggplot(df_inc_H) +
  geom_line(aes(x = time,
                y = Target_value_sim),
            lwd = 1) +
  geom_point(aes(x = time,
                 y = Target_value)) +
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
  theme(legend.position = "bottom",
        axis.text.x = element_text(angle = 45, hjust = 1))

# age distribution of hospitalizations
ggfig[["age_H"]] =
  ggplot(df_age_H) +
  geom_col(aes(x = Group,
               y = Target_value_sim),
           col = "black", alpha = 0) +
  geom_point(aes(x = Group,
                 y = Target_value)) +
  geom_text(aes(x = Group,
                y = Target_value_sim,
                label = Target_value_sim), vjust = -0.5, size = 3) +
  geom_text(aes(x = Group,
                y = Target_value,
                label = Target_value), vjust = -0.5, size = 3) +
  scale_y_continuous(limits = c(0, NA),
                     expand = expansion(mult = c(0, 0.2))) +
  labs(#title = target,
    x = "Age",
    y = "total_H") +
  theme_bw(base_size = 15) +
  theme(legend.position = "bottom",
        axis.text.x = element_text(angle = 45, hjust = 1))

# plot together
p <- cowplot::plot_grid(plotlist = ggfig,
                        nrow = 2,
                        labels = "AUTO",
                        hjust = 0,
                        label_size = 24, label_fontface = "plain",
                        align = "hv")
# save figure
print( name_plot <- paste0(directory_sim, "/inc_H_", n, ".png") )
ggplot2::ggsave(file = name_plot, plot = p, width = 8.5, height = 11, type = "cairo")

## model output ################################################################

# model output for calibration
calibration = cbind(df_place,
                    R_merler = R_merler,
                    R_gt     = R_gt,
                    R_mean   = R_mean,
                    R0       = R0,
                    lse      = lse)
rownames(calibration) = n

# save least squares
print( filename <- paste0(directory_sim, "/calibration_R0_", n, ".txt") )
write.table(calibration,
            file = filename,
            quote = F,
            sep = "\t",
            row.names = T, col.names = T)