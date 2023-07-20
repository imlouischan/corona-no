# # adherence percentage
scenarios_adherence = unique(as.character(scenarios$Adherence))

## vector ##############################################################

fig = list()
for( i in 1:length(scenarios_adherence) ){
  
  if(scenarios_adherence[i] == "H")
    adherence = c(0,
                  0,
                  0,
                  7.7e-01,
                  8.1e-01,
                  8.7e-01,
                  8.8e-01,
                  9.6e-01,
                  9.7e-01,
                  9.7e-01,
                  9.3e-01)
  if(scenarios_adherence[i] == "L")
    adherence = c(0, 0.7, 0.8, 0.6, 0.6, 0.7, 0.7, 0.9, 0.9, 0.9, 0.9)
  if(scenarios_adherence[i] == "100")
    adherence = rep(1, 11)
  
  ## input file ##############################################################
  
  adherence_vector = c(
    
    DAY_START_VACCINATION	= 1,
    WASTE               	= 0,
    
    ADHERENCE_1_0_11   = adherence[1],
    ADHERENCE_1_12_15  = adherence[2],
    ADHERENCE_1_16_17  = adherence[3],
    ADHERENCE_1_18_24  = adherence[4],
    ADHERENCE_1_25_39  = adherence[5],
    ADHERENCE_1_40_44  = adherence[6],
    ADHERENCE_1_45_54  = adherence[7],
    ADHERENCE_1_55_64  = adherence[8],
    ADHERENCE_1_65_74  = adherence[9],
    ADHERENCE_1_75_84  = adherence[10],
    ADHERENCE_1_85p    = adherence[11],
    
    ADHERENCE_2_0_11   = adherence[1],
    ADHERENCE_2_12_15  = adherence[2],
    ADHERENCE_2_16_17  = adherence[3],
    ADHERENCE_2_18_24  = adherence[4],
    ADHERENCE_2_25_39  = adherence[5],
    ADHERENCE_2_40_44  = adherence[6],
    ADHERENCE_2_45_54  = adherence[7],
    ADHERENCE_2_55_64  = adherence[8],
    ADHERENCE_2_65_74  = adherence[9],
    ADHERENCE_2_75_84  = adherence[10],
    ADHERENCE_2_85p    = adherence[11],
    
    ADHERENCE_3_0_11   = adherence[1],
    ADHERENCE_3_12_15  = adherence[2],
    ADHERENCE_3_16_17  = adherence[3],
    ADHERENCE_3_18_24  = adherence[4],
    ADHERENCE_3_25_39  = adherence[5],
    ADHERENCE_3_40_44  = adherence[6],
    ADHERENCE_3_45_54  = adherence[7],
    ADHERENCE_3_55_64  = adherence[8],
    ADHERENCE_3_65_74  = adherence[9],
    ADHERENCE_3_75_84  = adherence[10],
    ADHERENCE_3_85p    = adherence[11])
  
  ## check ##############################################################
  
  # print to check
  # print( scenarios_adherence[i] )
  # print( adherence_vector )
  
  # plot
  fig[[i]] <- lattice::levelplot(t(as.matrix(adherence_vector)),
                                 main = scenarios_adherence[i],
                                 xlab = "",
                                 ylab = "",
                                 scales = list(x = list(rot = 90)),
                                 # at = c(0:9),
                                 # colorkey = list(labels = list(at = c(0:9))),
                                 col.regions = gplots::redgreen(100))
  
  ## save ##############################################################
  
  # file name
  print( filename <- paste0("vaccine_adherence_", scenarios_adherence[i]) )
  # save as csv
  write.table(adherence_vector,
              file = paste0("scenarios_csv/", filename, ".csv"),
              quote = T,
              sep = ";",
              row.names = T, col.names = NA)
  # save as txt
  write.table(adherence_vector,
              file = paste0("../../../ibm-vaccination/1-main-ABM/In_out/input_files/",
                            "scenarios_txt/", filename, ".txt"),
              quote = F,
              sep = "\t",
              row.names = T, col.names = F)
  
}

# plot
cowplot::plot_grid(plotlist = fig,
                   ncol = 3,
                   labels = "AUTO",
                   hjust = 0,
                   label_size = 24, label_fontface = "plain",
                   align = "hv")