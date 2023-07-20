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

## linear regression ###########################################################

# load data
# ( filename <- paste0("/Users/louis/Downloads", "/", "lhs_beta_R0.txt") )
( filename <- paste0(path, folder_scenario, "/", "lhs_beta_R0.txt") )
df_beta_R0 = read.table(file = filename,
                        header = T, sep = "\t", comment.char = "",
                        stringsAsFactors = F)

# different method to define R0
# df_beta_R0$R0 = df_beta_R0$R_merler # method 1
# df_beta_R0$R0 = df_beta_R0$R_gt     # method 2
# df_beta_R0$R0 = df_beta_R0$R_mean   # method 3
# df_beta_R0$R0 = df_beta_R0$R0       # method 4

# linear region
R0_L = 0.5
R0_H = 1.5
df_beta_R0_subset = subset(df_beta_R0, R0 > R0_L & R0 < R0_H)

# linear regression
( fit = lm(beta ~ R0, data = df_beta_R0_subset) )
# prediction interval of linear regression
fit_predict = predict(fit, newdata = NULL, interval = "confidence")
fit_predict = cbind(df_beta_R0_subset, fit_predict)
# select beta within prediction interval
fit_predict = subset(fit_predict, beta > lwr & beta < upr)
df_beta_R0_subset = fit_predict[, c("beta", "R0")]

## transform beta ##############################################################

# select some beta
indices = sample((1:nrow(df_beta_R0_subset))[!is.na(df_beta_R0_subset$R0)], bestN, replace = F)
df_beta_R0_select = df_beta_R0_subset[indices, ]

# transform beta given a specific R0
df_beta_trans = NULL
for( R0 in seq(R0_L, R0_H, 0.01) ){ # different R0
  
  df_beta_trans = rbind(df_beta_trans,
                        data.frame(beta = df_beta_R0_select$beta + ( R0 - df_beta_R0_select$R0 ) * fit$coefficients[2],
                                   R0 = R0))
} # different R0

# save selected beta
( filename <- paste0("../In_out/input_files/", "calibration_beta_R0", ".txt") )
write.table(df_beta_trans[order(df_beta_trans$R0, df_beta_trans$beta), ],
            file = filename,
            quote = F, sep = "\t",
            row.names = F, col.names = T)

## plot ########################################################################
fig = list()

# ggplot
fig[["full"]] =
  ggplot() +
  geom_point(data = df_beta_R0, aes(x = R0,
                                    y = beta),
             col = "black", alpha = 0.3) +
  geom_smooth(data = df_beta_R0_subset, aes(x = R0,
                                            y = beta),
              method = lm, formula = y ~ x, se = T,
              col = "blue") +
  geom_point(data = df_beta_trans, aes(x = R0,
                                       y = beta),
             col = "red", alpha = 0.1, shape = 4, size = 5, stroke = 1) +
  theme_bw(base_size = 24)

# fig[["full"]] = fig[["full"]] +
#   xlim(0, 5) +
#   ylim(0, 3)

# zoom in
fig[["zoom"]] = fig[["full"]] +
  xlim(R0_L, R0_H) +
  ylim(0, 0.8)

# plot together
p <- cowplot::plot_grid(plotlist = fig,
                        nrow = 1,
                        labels = "AUTO",
                        hjust = 0,
                        label_size = 24, label_fontface = "plain",
                        align = "hv")

# save figure
print( name_plot <- paste0(path, folder_scenario, "/", "calibration_beta_R0", ".png") )
ggplot2::ggsave(file = name_plot, plot = p, width = 11, height = 8.5, type = "cairo")