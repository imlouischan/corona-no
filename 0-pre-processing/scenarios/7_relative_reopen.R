# relative reproduction number at reopen
scenarios_priority = unique(scenarios$R_const)

## vector ##############################################################
suppressMessages(library(ggplot2))
df_melt = NULL
for( i in 1:length(scenarios_priority) ){
  
  ## read data ###################################################################
  
  # read initial values
  df_reopen <- read.csv("ref/scaling_reopn.csv",
                       header = T,
                       stringsAsFactors = F)
  
  # Oslo: from ward to municip0301
  df_reopen$location_code = factor(df_reopen$location_code,
                            levels = c(fhidata::norway_locations_b2020$municip_code,       # 356 municip
                                       fhidata::norway_locations_ward_b2020$ward_code),    # 15 ward 
                            labels = c(fhidata::norway_locations_b2020$municip_code,       # 356 municip
                                       fhidata::norway_locations_ward_b2020$municip_code)) # 15 ward 
  # Oslo: mean of 15 ward
  df_reopen = aggregate(df_reopen$scaling_sep, by = list(location_code = df_reopen$location_code), mean)
  
  ## save ##############################################################
  
  # file name
  print( filename <- paste0("relative_reopen_", scenarios_priority[i]) )
  # save as csv
  write.table(df_reopen,
              file = paste0("scenarios_csv/", filename, ".csv"),
              quote = T,
              sep = ";",
              row.names = T, col.names = NA)
  # save as txt
  write.table(df_reopen$x,
              file = paste0("../../../ibm-vaccination/1-main-ABM/In_out/input_files/",
                            "scenarios_txt/", filename, ".txt"),
              quote = F,
              sep = "\t",
              row.names = F, col.names = F)
  
}
