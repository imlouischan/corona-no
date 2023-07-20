## new script ##################################################################
cat("\014")    # clear the console
rm(list=ls())  # remove all variables
graphics.off() # close all plots
set.seed(2020) # fix randomness
suppressMessages(library(ggplot2))
suppressMessages(library(dplyr))
suppressMessages(library(tidyverse))
## read safezone data ##########################################################

# load data
df_rds = readRDS(paste0("../../../ibm-vaccination/",
                        "/0-pre-processing/case_data/input/",
                        "data_regional_prio.RDS"))
# order of levels
df_rds$incidence$type = factor(df_rds$incidence$type,
                               levels = c("cases", "hosp", "ICU", "death"),
                               labels = c("Cases", "Hospitalizations", "ICUs", "Deaths"))
df_rds$age$type       = factor(df_rds$age$type,
                               levels = c("cases", "hosp", "ICU", "death"),
                               labels = c("Cases", "Hospitalizations", "ICUs", "Deaths"))
df_rds$prior$type     = factor(df_rds$prior$type,
                               levels = c("cases", "hosp", "ICU", "death"),
                               labels = c("Cases", "Hospitalizations", "ICUs", "Deaths"))
# keep incidence
df_rds$incidence =
  df_rds$incidence %>%
  filter(date <= "2021-07-31") %>%
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
  complete(age, type, fill = list(value = 0))
# as.factor
df_rds$age$age = factor(df_rds$age$age)

## rolling mean ################################################################
# https://www.storybench.org/how-to-calculate-a-rolling-average-in-r/

# (2-week for rolling mean)
time_window      = 29                 # days of rolling average
time_window_half = (time_window-1)/2  # one-side

# add rolling mean
df_rds$incidence =
  df_rds$incidence %>%
  group_by(type) %>%
  mutate(rollmean = (zoo::rollmean(value, k = time_window, fill = NA)))

## ggplot ######################################################################
fig = list()

fig[["incidence"]] =
  ggplot(df_rds$incidence, aes(x = date,
                               y = value)) +
  geom_point(shape = 1) +
  geom_line(aes(y = rollmean),
            lwd = 1) +
  facet_grid(rows = vars(type),
             scales = "free_y", space = "fixed") +
  scale_x_date(breaks = c(seq(from = as.Date("2021-01-01"),
                              to   = as.Date("2022-12-01"),
                              by   = "month")),
               labels = c(t(outer(2021:2022, c("Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"), paste, sep = "-"))),
               expand = c(0, 0) ) +
  scale_y_continuous(limits = c(0, NA)) +
  labs(#title = target,
    x = "Time",
    y = "Counts") +
  theme_bw(base_size = 15) +
  theme(legend.position = "bottom",
        axis.text.x = element_text(angle = 45, hjust = 1))

fig[["cumulative"]] =
  ggplot(df_rds$cumulative, aes(x = date,
                                y = value)) +
  geom_line(lwd = 1) +
  geom_label(data = . %>%
               filter(date == max(date)),
             aes(x = as.Date("2021-01-01"),
                 y = Inf,
                 label = value),
             hjust = "inward", vjust = "inward", size = 5) +
  facet_grid(rows = vars(type),
             scales = "free_y", space = "fixed") +
  scale_x_date(breaks = c(seq(from = as.Date("2021-01-01"),
                              to   = as.Date("2022-12-01"),
                              by   = "month")),
               labels = c(t(outer(2021:2022, c("Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"), paste, sep = "-"))),
               expand = c(0, 0) ) +
  scale_y_continuous(limits = c(0, NA),
                     expand = c(0, 0)) +
  labs(#title = target,
    x = "Time",
    y = "Counts") +
  theme_bw(base_size = 15) +
  theme(legend.position = "bottom",
        axis.text.x = element_text(angle = 45, hjust = 1))

fig[["prior"]] =
  ggplot(df_rds$prior, aes(x = prior_0,
                           y = value)) +
  geom_col(col = "black", alpha = 0) +
  geom_text(aes(label = value), vjust = -0.5, size = 3) +
  facet_grid(rows = vars(type),
             scales = "free_y", space = "fixed") +
  scale_y_continuous(limits = c(0, NA),
                     expand = expansion(mult = c(0, 0.2))) +
  labs(#title = target,
    x = "Priority",
    y = "Counts") +
  theme_bw(base_size = 15) +
  theme(legend.position = "bottom",
        axis.text.x = element_text(angle = 45, hjust = 1))

fig[["age"]] =
  ggplot(df_rds$age, aes(x = age,
                         y = value)) +
  geom_col(col = "black", alpha = 0) +
  geom_text(aes(label = value), vjust = -0.5, size = 3) +
  facet_grid(rows = vars(type),
             scales = "free_y", space = "fixed") +
  scale_y_continuous(limits = c(0, NA),
                     expand = expansion(mult = c(0, 0.2))) +
  labs(#title = target,
    x = "Age",
    y = "Counts") +
  theme_bw(base_size = 15) +
  theme(legend.position = "bottom",
        axis.text.x = element_text(angle = 45, hjust = 1))

# plot together
p <-
  cowplot::plot_grid(plotlist = fig,
                     nrow = 1,
                     labels = "AUTO",
                     hjust = 0,
                     label_size = 20, label_fontface = "plain",
                     align = "hv", axis = "b")

# save figure
print( name_plot <- paste0("output/", "data", ".png") )
ggplot2::ggsave(file = name_plot, plot = p, width = 11*1.5, height = 8.5*1, type = "cairo")