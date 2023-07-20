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

## time period #################################################################

df_time <- read.table(paste0("../../../ibm-vaccination/1-main-ABM/In_out/input_files/scenarios_txt/time.txt"),
                      row.names = 1, stringsAsFactors = F)
date_start = as.Date(df_time["date_start", ])
# until July 2021
date_last = as.Date("2021-07-31")

## sorting #####################################################################

# load data
# ( filename <- paste0("/Users/louis/Downloads", "/", "lhs_beta_R0.txt") )
( filename <- paste0(path, folder_scenario, "/", "lhs_beta_R0.txt") )
df_beta_R0 = read.table(file = filename,
                        header = T, sep = "\t", comment.char = "",
                        stringsAsFactors = F)

# add columns
df_beta_R0$lse_log = log(df_beta_R0$lse)
df_beta_R0$lse_nlog = -log(df_beta_R0$lse)
df_beta_R0$lse_inv  = 1 / df_beta_R0$lse

# sorting
df_beta_R0_sort = df_beta_R0[order(df_beta_R0$lse), ]

# save all parameters
( filename <- paste0(path, folder_scenario, "/", "lhs_beta_R0_sort.txt") )
write.table(df_beta_R0_sort,
            file = filename,
            quote = F, sep = "\t",
            row.names = T, col.names = T)

# save selected parameters (for scenarios)
( filename <- paste0("../In_out/input_files/", "calibration_beta_R0_sort", ".txt") )
write.table(df_beta_R0_sort[1:bestN, ],
            file = filename,
            quote = F, sep = "\t",
            row.names = F, col.names = T)

## ggplot ######################################################################

# different number of samples
for( bestN in c(10^seq(1:floor(log10(nrow(df_beta_R0_sort)))), nrow(df_beta_R0_sort)) ){
  
  ## Fig 1: LSE ################################################################
  
  # best
  df_beta_R0_sub = df_beta_R0_sort[1:bestN, ]
  
  # plot
  p =
    GGally::ggpairs(df_beta_R0_sub[, c("beta", "beta_cpt1", "beta_cpt2", paste0("sus", 1:9), "lse_log")],
                    aes(fill = lse_log),
                    lower = list(continuous = GGally::wrap("points", shape = 21, alpha = 0.5, stroke = 0),
                                 combo = "facethist"),
                    upper = list(continuous = GGally::wrap("cor", size = 4),
                                 combo = "dot"),
                    diag  = list(continuous = GGally::wrap("densityDiag", col = "black", lwd = 1),
                                 discrete   = GGally::wrap("barDiag",     col = "black", lwd = 1)),
                    title = paste0("The best ", format(bestN, scientific = F), "/", nrow(df_beta_R0_sort), " parameter sets", ".")) +
    theme_bw(base_size = 12) +
    theme(axis.text.x = element_text(angle = 45, hjust = 1))
  
  # save figure
  print( name_plot <- paste0(path, folder_scenario, "/", "calibration_4_beta_R0", "-bestN_", format(bestN, scientific = F), ".png") )
  ggplot2::ggsave(file = name_plot, plot = p, width = 11*1, height = 8.5*1, type = "cairo")
  
  ## Fig 2: range of parameters ################################################
  
  # melt for ggplot
  df_beta_R0_sub_melt =
    reshape2::melt(df_beta_R0_sort[1:bestN, c("beta", "beta_cpt1", "beta_cpt2", paste0("sus", 1:9))])
  df_beta_R0_all_melt =
    reshape2::melt(df_beta_R0_sort[       , c("beta", "beta_cpt1", "beta_cpt2", paste0("sus", 1:9))]) %>%
    group_by(variable) %>%
    summarise(max = max(value),
              min = min(value))
  
  # ggplot
  p =
    ggplot(data = df_beta_R0_sub_melt,
           aes(x = variable,
               y = value)) +
    geom_crossbar(data = df_beta_R0_all_melt,
                  aes(x = variable, y = 0,
                      ymin = min, ymax = max),
                  fatten = 0, col = NA,
                  fill = "darkgray", alpha = 0.5) +
    geom_boxplot(alpha = 0, lwd = 1) +
    geom_jitter(shape = 1, size = 2, stroke = 1) +
    labs(#title = target,
      x = "",
      y = "") +
    theme_bw(base_size = 18) +
    theme(legend.position = "none",
          axis.text.x = element_text(angle = 45, hjust = 1))
  
  # save figure
  print( name_plot <- paste0(path, folder_scenario, "/", "calibration_4_boxplot", "-bestN_", format(bestN, scientific = F), ".png") )
  ggplot2::ggsave(file = name_plot, plot = p, width = 11*1, height = 8.5*1, type = "cairo")
  
  ## Fig 3: meaning of parameters ##############################################
  
  # beta
  df_beta = matrix(NA, nrow = length(date_start:date_last), ncol = bestN)
  rownames(df_beta) = as.character(as.Date(date_start:date_last, origin = "1970-01-01"))
  # change points
  for(t in 1:nrow(df_beta)){
    if(1)
      df_beta[t, ] = df_beta_R0_sort$beta[1:bestN]
    if(t > as.Date("2021-01-28") - date_start)
      df_beta[t, ] = df_beta_R0_sort$beta_cpt1[1:bestN]
    if(t > as.Date("2021-03-11") - date_start)
      df_beta[t, ] = df_beta_R0_sort$beta_cpt2[1:bestN]
  }
  # melt for ggplot
  df_beta = reshape2::melt(df_beta)
  colnames(df_beta) = c("Time", "Sim", "Beta")
  df_beta$Time = as.Date(df_beta$Time)
  
  # susceptibility
  df_sus =
    df_beta_R0_sub_melt %>%
    filter(grepl("sus", variable)) %>%
    mutate(variable = factor(variable,
                             levels = paste0("sus", 1:9),
                             labels = c("0-9", "10-19", "20-29", "30-39", "40-49", "50-59", "60-69", "70-79", "80+")))
  
  # ggplot
  ggfig = list()
  # beta
  ggfig[["beta"]] =
    ggplot(data = df_beta,
           aes(x = Time,
               y = Beta)) +
    tidybayes::stat_lineribbon(.width = c(.95, .5),
                               lwd = 1, alpha = 0.5) +
    scale_color_grey(start = 0.5,
                     end = 0.2) +
    scale_fill_grey(start = 0.5,
                    end = 0.2) +
    scale_x_date(breaks = c(seq(from = as.Date("2021-01-01"),
                                to   = as.Date("2022-12-01"),
                                by   = "month")),
                 labels = c(t(outer(2021:2022, c("Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"), paste, sep = "-"))),
                 expand = c(0, 0) ) +
    scale_y_continuous(limits = c(0, NA)) +
    labs(#title = target,
      x = "Time",
      y = "Beta") +
    theme_bw(base_size = 18) +
    theme(legend.position = "none",
          axis.text.x = element_text(angle = 45, hjust = 1))
  
  # susceptibility
  ggfig[["sus"]] =
    ggplot(data = df_sus,
           aes(x = variable,
               y = value)) +
    tidybayes::stat_interval(.width = c(.95, .5),
                             lwd = 5, alpha = 0.5) +
    geom_point(data = . %>%
                 group_by(variable) %>%
                 summarise(value = median(value)), 
               shape = "-", size = 15) +
    # geom_boxplot(alpha = 0, lwd = 1) +
    scale_color_grey(start = 0.5,
                     end = 0.2) +
    scale_fill_grey(start = 0.5,
                    end = 0.2) +
    scale_y_continuous(limits = c(0, 1)) +
    labs(#title = target,
      x = "Age",
      y = "Susceptibility") +
    theme_bw(base_size = 18) +
    theme(legend.position = "none",
          axis.text.x = element_text(angle = 45, hjust = 1))
  
  # plot together
  p <- cowplot::plot_grid(plotlist = ggfig,
                          nrow = 1,
                          labels = "AUTO",
                          hjust = 0,
                          label_size = 24, label_fontface = "plain",
                          align = "hv")
  # save figure
  print( name_plot <- paste0(path, folder_scenario, "/", "calibration_4_CI", "-bestN_", format(bestN, scientific = F), ".png") )
  # print( name_plot <- paste0("/Users/louis/Downloads/", "calibration_4_CI", "-bestN_", format(bestN, scientific = F), ".png") )
  ggplot2::ggsave(file = name_plot, plot = p, width = 11*1, height = 8.5*1, type = "cairo")
  
} # bestN