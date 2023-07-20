# # number of doses per month
scenarios_doses = unique(as.character(scenarios$Doses))
scenarios_delay = unique(as.character(scenarios$Delay))

## vector ##############################################################

doses_melt_plot = NULL
for( i in 1:length(scenarios_doses) ){
  for( k in 1:length(scenarios_delay) ){ # different time_wait_1st_2nd
    
    # number of doses per week
    if( scenarios_doses[i] == "PM"  & scenarios_delay[k] == 12 ){ # scenario ####
      
      # load vaccine data (normalized)
      df_vpk = read.csv2("../initial_V/output/VaksinertPerKommune.csv",
                         row.names = 1)
      
      # subset: 1st dose only
      df_vpk_subset = subset(df_vpk, dose == 1)
      
      # total 1st doses in Norway
      df = aggregate(count ~ time, data = df_vpk_subset, sum)
      
      # first date
      df_time <- read.table(paste0("../../../ibm-vaccination/1-main-ABM/In_out/input_files/scenarios_txt/time.txt"),
                            row.names = 1, stringsAsFactors = F)
      date1 = as.Date(df_time["date_start", ])
      # time
      df$time = df$time + date1 - 1
      
      # make sure no missing date
      df = merge(df,
                 data.frame(time = as.Date(min(df$time):max(df$time), "1970-01-01")),
                 all = T)
      
      # NA -> 0
      df$count[is.na(df$count)] = 0
      
      # Cap to start at Day 1
      df = df[df$time >= Day1, ]
      # Cap to end at Day T
      df = df[df$time <= DayT, ]
      
      # output
      doses = data.frame(date            = df$time,
                         mRNA            = df$count,
                         Virus_vector    = 0,
                         Protein_subunit = 0)
      
      # For adding time col to csv output
      date_vec = doses$date
      
    } # end if scenarios_doses[i] == "PM"  & scenarios_delay[k] == 12
    
    
    if( grepl("Base", scenarios_doses[i], fixed = T) ){ # baseline ####
      N_days = as.integer(difftime(DayT, Day1, units="days")) + 1
      date_vec = Day1 + 0:(N_days-1) # For adding time col to csv output
      doses = data.frame(
        mRNA            = rep(0, N_days),
        Virus_vector    = rep(0, N_days),
        Protein_subunit = rep(0, N_days))
    }
    
    # number of doses per day (as three vaccines)
    doses = data.frame(vac1 = doses$mRNA,
                       vac2 = doses$Virus_vector,
                       vac3 = doses$Protein_subunit)
    # integer
    doses = round(doses)
    
    # Check that total dose count is high enough to vaccinate everybody
    print(paste0("Dose scenario ", scenarios_doses[i], ", Sum(doses) = ", sum(doses)))
    # stopifnot(sum(doses) >= 3.5e6)
    
    ## save ##############################################################
    
    # file name
    print( filename <- paste0("vaccine_doses_",
                              scenarios_doses[i], "_",
                              scenarios_delay[k]) )
    # save as csv
    write.table(data.frame(date = date_vec,
                           vac1 = doses$vac1,
                           vac2 = doses$vac2,
                           vac3 = doses$vac3
    ),
    file = paste0("scenarios_csv/", filename, ".csv"),
    quote = T,
    sep = ";",
    row.names = T, col.names = NA)
    # save as txt
    write.table(doses,
                file = paste0("../../../ibm-vaccination/1-main-ABM/In_out/input_files/",
                              "scenarios_txt/", filename, ".txt"),
                quote = F,
                sep = "\t",
                row.names = F, col.names = F)
    
    ## ggplot  #####################################################################
    p =
      ggplot(df %>%
               filter(time < as.Date("2021-08-01")),
             aes(x = time,
                 y = cumsum(count))) +
      geom_line(lwd = 1) +
      fhiplot::theme_fhi_lines() +
      scale_x_date(breaks = c(seq(from = as.Date("2021-01-01"),
                                  to   = as.Date("2022-12-01"),
                                  by   = "month")),
                   labels = c(t(outer(2021:2022, c("Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"), paste, sep = "-"))),
                   expand = c(0, 0) ) +
      scale_y_continuous(limits = c(0, 4e6),
                         expand = c(0, 0)) +
      labs(#title = paste0("Total = ", sum(df$count)),
        # x = "Time",
        y = "Number of 1st doses distributed") +
      theme_bw(base_size = 24) +
      theme(legend.position = "bottom",
            axis.title.x = element_blank(),
            axis.text.x = element_text(angle = 45, hjust = 1))
    # save figure
    print( name_plot <- paste0("ref/", "dose_1st", ".png") )
    ggplot2::ggsave(file = name_plot, plot = p, width = 11, height = 8.5, type = "cairo")
    
  }
}