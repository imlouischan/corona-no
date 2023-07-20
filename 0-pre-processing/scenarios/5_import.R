# # number of importation per month
scenarios_import = unique(as.character(scenarios$Import))

## data from fhi01 #############################################################

# load data
df_import = read.csv2("ref/ImporterteCovidAgeMSIS_Provedato.csv",
                      check.names = F)
colnames(df_import) = c("Time", "Age", "Count")

# Jan-Aug 2021
df_import = subset(df_import, Time >= "2021-01-01" & Time < "2021-09-01" )
# monthly
df_import$Time = format(as.Date(df_import$Time), "%Y-%m")
df_import = aggregate(Count ~ Time, df_import, sum)

## vector ##############################################################
for( i in 1:length(scenarios_import) ){
  
  # number of import per month
  import_monthly = rep(0, 3+12)
  
  if( grepl("M", scenarios_import[i], fixed = T) ){ # M               ####
    import_monthly = c(df_import$Count,
                       rep(0, 7))
  }
  
  # days of months
  day = c(c(31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31), c(31, 28, 31))
  # import per day
  import = rep(import_monthly / day, day)
  # date
  names(import) = 1:length(import) + as.Date("2021-01-01") - 1
  # from Day 1
  import = import[names(import) >= Day1]
  # to Day T
  import = import[1:time]
  # integer
  # import = round(import)
  
  ## save ##############################################################
  
  # file name
  print( filename <- paste0("import_", scenarios_import[i]) )
  # save as csv
  write.table(import,
              file = paste0("scenarios_csv/", filename, ".csv"),
              quote = T,
              sep = ";",
              row.names = T, col.names = NA)
  # save as txt
  write.table(import,
              file = paste0("../../../ibm-vaccination/1-main-ABM/In_out/input_files/",
                            "scenarios_txt/", filename, ".txt"),
              quote = F,
              sep = "\t",
              row.names = F, col.names = F)
  
  ## ggplot  #####################################################################
  
  # data.frame
  df_import = data.frame(time = as.Date(names(import)),
                         import)
  # plot
  p =
    ggplot(df_import %>%
             filter(time < as.Date("2021-08-01")),
           aes(x = time,
               y = import)) +
    geom_line(lwd = 1) +
    fhiplot::theme_fhi_lines() +
    scale_x_date(breaks = c(seq(from = as.Date("2021-01-01"),
                                to   = as.Date("2022-12-01"),
                                by   = "month")),
                 labels = c(t(outer(2021:2022, c("Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"), paste, sep = "-"))),
                 expand = c(0, 0) ) +
    labs(#title = paste0("Total = ", sum(df_import$import)),
      # x = "Time",
      y = "Daily number of imported cases") +
    theme_bw(base_size = 24) +
    theme(legend.position = "bottom",
          axis.title.x = element_blank(),
          axis.text.x = element_text(angle = 45, hjust = 1))
  # save figure
  print( name_plot <- paste0("ref/", filename, ".png") )
  ggplot2::ggsave(file = name_plot, plot = p, width = 11, height = 8.5, type = "cairo")
  
}