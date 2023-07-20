## selecting change points #####################################

# (4-week for selecting change points)
# i.e. at least 2 weeks between 2 change points
time_window      = 29                 # days of rolling average
time_window_half = (time_window-1)/2  # one-side

# each type of data
for(j in levels(df_rds$incidence$type)){
  
  # 1 type
  df_H =
    df_rds$incidence %>%
    filter(type == j)
  
  # add columns
  df_H$nbr = 0 # number of neighbor
  df_H$max = 0
  df_H$min = 0
  df_H$cpt = NA # change point or not
  for(t in 1:length(df_H$date)){ # full period
    for(t1 in -time_window_half:time_window_half){ # window
      
      # counting
      if(t+t1 >= 1 & t+t1 <= length(df_H$date)){ # within the period
        if(!is.na(df_H$rollmean[t]) & !is.na(df_H$rollmean[t+t1])){ # those with rolling mean
          
          df_H$nbr[t] = df_H$nbr[t] + 1                                               # count 1 neighbor
          if((df_H$rollmean[t] >= df_H$rollmean[t+t1])) df_H$max[t] = df_H$max[t] + 1 # count 1 for max
          if((df_H$rollmean[t] <= df_H$rollmean[t+t1])) df_H$min[t] = df_H$min[t] + 1 # count 1 for min
          
        }
      }
    }
    
    # selecting
    if(df_H$nbr[t] > time_window_half + 1) # not the first or last point of rolling mean
      if(df_H$nbr[t] == df_H$max[t] | df_H$nbr[t] == df_H$min[t]) # local max/min
        df_H$cpt[t] = T # select as a change point
  }
  
  # only 1 change point in a window
  for(t in which(df_H$cpt)) # each change point
    for(t1 in 1:time_window_half) # window
      if(t+t1 < length(df_H$date))
        df_H$cpt[t+t1] = NA # select the first one by removing others
  # check
  subset(df_H, cpt == T)
  # check (shifting 12 days earlier, see below)
  if(j == "Hospitalizations") print(subset(df_H, cpt == T)$date - 12)
  
  # split into sections
  df_H$section = as.factor(cumsum(ifelse(is.na(df_H$cpt), 0, df_H$cpt)) + 1)
  
  ## ggplot ######################################################################
  p =
    ggplot(df_H) +
    geom_line(aes(x = date,
                  y = rollmean),
              lwd = 1) +
    geom_vline(xintercept = df_H$date[df_H$cpt == T] - 0.5,
               lwd = 1) +
    geom_point(aes(x = date,
                   y = value, col = section)) +
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
  
  # save figure
  print( name_plot <- paste0("output/", "data_", j, "_cpt", ".png") )
  ggplot2::ggsave(file = name_plot, plot = p, width = 11*1, height = 8.5*1, type = "cairo")
}

## shifting from infection to hospitalizations #################################

# time from symptom onset to hospitalization (distribution from excel)
para = gdata::read.xls("../../../0-pre-processing/parameters/parameters_vaccination.xlsx")
# weight from data
weighted.mean(para[which(para$variable_name == "onset_to_hosp") + (0:9)*3, c(2)],
              subset(df_rds$age, type == "Hospitalizations")$value)
# 7.779086
# time from infection to pre-symptomatic (E1)
3
# time from pre-symptomatic to symptomatic (E2)
2