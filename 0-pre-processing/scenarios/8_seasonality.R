# seasonality
scenarios_seasonality = unique(scenarios$Seasonality)

## save ##############################################################

# TODO loop over scenarios

seasonality_plot = NULL
for (i in 1:length(scenarios_seasonality)) {
  
  # seasonality file
  seasonality_1 = read.csv(paste0("ref/seasonality_", scenarios_seasonality[i], ".csv"))
  
  # Repeat seasonality for one more year
  seasonality_1_year2 = seasonality_1
  seasonality_1_year2$day = seasonality_1_year2$day + 365
  seasonality_1 = rbind(seasonality_1, seasonality_1_year2)
  
  # date
  seasonality_1$date = seasonality_1$day + as.Date("2021-01-01") - 1
  # Cap at same date as Import, which is 2022-03-31
  seasonality_1 = seasonality_1[seasonality_1$date < "2022-04-01", ]
  
  # from Day 1
  seasonality_1 = seasonality_1[seasonality_1$date >= Day1, ]
  # from Day T
  seasonality_1 = seasonality_1[1:time, ]
  
  # Normalise beta to first day
  seasonality_1$beta = seasonality_1$beta / seasonality_1$beta[1]
  
  
  
  # # Make the same df for seasonality_0, where beta=1 always
  # seasonality_0 = seasonality_1
  # seasonality_0$beta = rep(1, length(seasonality_1$beta))
  
  # Select case
  # if (scenarios_seasonality[i] == 0) {
  #   seasonality = seasonality_0
  # } else if (scenarios_seasonality[i] == 1) {
  seasonality = seasonality_1
  # }
  
  seasonality_plot = rbind(seasonality_plot,
                           cbind(seasonality, percent = scenarios_seasonality[i]))
  
  # file name
  print( filename <- paste0("seasonality_", scenarios_seasonality[i]))
  # save as csv
  write.table(seasonality[, c("date", "beta")],
              file = paste0("scenarios_csv/", filename, ".csv"),
              quote = T,
              sep = ";",
              row.names = F, col.names = T)
  # save as txt
  write.table(seasonality[, c("beta")],
              file = paste0("../../../ibm-vaccination/1-main-ABM/In_out/input_files/",
                            "scenarios_txt/", filename, ".txt"),
              quote = F,
              sep = "\t",
              row.names = F, col.names = F)
}

# # file name
# print( filename <- "seasonality_1")
# # save as txt
# write.table(seasonality_1[, c("beta")],
#             file = paste0("scenarios_txt/", filename, ".txt"),
#             quote = F,
#             sep = "\t",
#             row.names = F, col.names = F)
# # save as csv
# write.table(seasonality_1[, c("beta")],
#             file = paste0("scenarios_csv/", filename, ".csv"),
#             quote = T,
#             sep = ";",
#             row.names = T, col.names = NA)

## ggplot ######################################################################
p =
  ggplot() +
  geom_line(data = seasonality_plot %>%
              filter(date < as.Date("2021-08-01")),
            aes(x = date,
                y = beta,
                # col = as.factor(percent),
                group = percent),
            lwd = 1) +
  fhiplot::theme_fhi_lines() +
  scale_x_date(breaks = c(seq(from = as.Date("2021-01-01"),
                              to   = as.Date("2022-12-01"),
                              by   = "month")),
               labels = c(t(outer(2021:2022, c("Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"), paste, sep = "-"))),
               expand = c(0, 0) ) +
  # scale_y_continuous(limits = c(0, 2),
  #                    breaks = seq(0, 2, 0.2),
  #                    expand = c(0, 0)) +
  labs(#title = "Seasonality",
    # x = "Time",
    y = "Relative seasonality",
    col = "Seasonality") +
  theme_bw(base_size = 24) +
  theme(legend.position = "bottom",
        axis.title.x = element_blank(),
        axis.text.x = element_text(angle = 45, hjust = 1))

# save figure
print( name_plot <- paste0("ref/", "seasonality", ".png") )
ggplot2::ggsave(file = name_plot, plot = p, width = 11, height = 8.5, type = "cairo")