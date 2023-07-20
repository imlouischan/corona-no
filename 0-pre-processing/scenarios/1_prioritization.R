## vaccine prioritization in age and risk group ################################

# age groups
age_groups = paste0(0:109, "_year_old")
# risk groups
risk_groups = c("General population (without risk factors)",
                "People with risk factors",
                "Healthcare workers")

## matrix ######################################################################
fig = list()
scenarios_order = unique(Priority_order)
for( k in 1:length(scenarios_order) ){ # priority order
  
  for( j in 1:3 ){ # vaccine prioritization for vaccine 1, 2, 3
    
    scenarios_prioritization = unique(as.character(scenarios[[paste0("Prioritization_", j)]]))
    for( i in 1:length(scenarios_prioritization) ){ # prioritization scenarios
      
      # age and risk groups
      priority_matrix = matrix(9999,
                               length(age_groups),
                               length(risk_groups),
                               dimnames = list(age_groups, risk_groups))
      
      # vaccine prioritization for non-AZ
      if( grepl("18p", scenarios_prioritization[i], fixed = T) ){
        
        # elderly
        priority_matrix[(85:109)+1,  ] = 0
        priority_matrix[(75:84 )+1,  ] = 1
        priority_matrix[(65:74 )+1,  ] = 2
        
        # Healthcare workers
        priority_matrix[(18:64) +1, 3] = 3
        
        # People with risk factors
        priority_matrix[(55:64) +1, 2] = 4
        priority_matrix[(45:54) +1, 2] = 5
        priority_matrix[(18:44) +1, 2] = 6
        
        # General population (without risk factors)
        priority_matrix[(55:64) +1, 1] = 7
        priority_matrix[(45:54) +1, 1] = 8
        priority_matrix[c(18:24, 40:44) +1, 1] = 9
        priority_matrix[(25:39) +1, 1] = 10
        
      }
      
      # vaccine prioritization for non-AZ
      if( grepl("16p", scenarios_prioritization[i], fixed = T) ){
        
        # elderly
        priority_matrix[(85:109)+1,  ] = 0
        priority_matrix[(75:84 )+1,  ] = 1
        priority_matrix[(65:74 )+1,  ] = 2
        
        # Healthcare workers
        priority_matrix[(16:64) +1, 3] = 3
        
        # People with risk factors
        priority_matrix[(55:64) +1, 2] = 4
        priority_matrix[(45:54) +1, 2] = 5
        priority_matrix[(16:44) +1, 2] = 6
        
        # General population (without risk factors)
        priority_matrix[(55:64) +1, 1] = 7
        priority_matrix[(45:54) +1, 1] = 8
        priority_matrix[c(18:24, 40:44) +1, 1] = 9
        priority_matrix[(25:39) +1, 1] = 10
        
        # younger
        priority_matrix[(16:17) +1, 1] = 11
        
      }
      
      # vaccine prioritization for non-AZ
      if( grepl("12p", scenarios_prioritization[i], fixed = T) ){
        
        # elderly
        priority_matrix[(85:109)+1,  ] = 0
        priority_matrix[(75:84 )+1,  ] = 1
        priority_matrix[(65:74 )+1,  ] = 2
        
        # Healthcare workers
        priority_matrix[(12:64) +1, 3] = 3
        
        # People with risk factors
        priority_matrix[(55:64) +1, 2] = 4
        priority_matrix[(45:54) +1, 2] = 5
        priority_matrix[(12:44) +1, 2] = 6
        
        # General population (without risk factors)
        priority_matrix[(55:64) +1, 1] = 7
        priority_matrix[(45:54) +1, 1] = 8
        priority_matrix[c(18:24, 40:44) +1, 1] = 9
        priority_matrix[(25:39) +1, 1] = 10
        
        # younger
        priority_matrix[(16:17) +1, 1] = 11
        priority_matrix[(12:15) +1, 1] = 12
        
      }
      
      # vaccine prioritization for AZ (65+)
      if( grepl("65p", scenarios_prioritization[i], fixed = T) ){
        
        # elderly
        priority_matrix[(85:109)+1,  ] = 0
        priority_matrix[(75:84 )+1,  ] = 1
        priority_matrix[(65:74 )+1,  ] = 2
        
      }
      
      ## check ##############################################################
      
      # plot
      fig[[j]] <- lattice::levelplot(t(priority_matrix),
                                     main = scenarios_prioritization[i],
                                     xlab = "",
                                     ylab = "",
                                     scales = list(x = list(rot = 90)),
                                     at = c(0:14)-0.5,
                                     # colorkey = list(labels = list(at = c(0:9)-0.5)),
                                     col.regions = gplots::redgreen(100))
      
      ## save ##############################################################
      
      # file name
      
      print( filename <- paste0("prioritization_", j, "_", scenarios_prioritization[i], "_", scenarios_order[k]) )
      # save as csv
      write.table(t(priority_matrix),
                  file = paste0("scenarios_csv/", filename, ".csv"),
                  quote = T,
                  sep = ";",
                  row.names = T, col.names = NA)
      # save as txt
      write.table(t(priority_matrix),
                  file = paste0("../../../ibm-vaccination/1-main-ABM/In_out/input_files/",
                                "scenarios_txt/", filename, ".txt"),
                  quote = F,
                  sep = "\t",
                  row.names = F, col.names = F)
      
    }
    # plot
    cowplot::plot_grid(plotlist = fig,
                       nrow = 1,
                       labels = "AUTO",
                       hjust = 0,
                       label_size = 24, label_fontface = "plain",
                       align = "hv")
    # save figure
    # print( name_plot <- paste0("1_prioritization", ".png") )
    # ggplot2::ggsave(file = name_plot, width = 11*2, height = 8.5*2, type = "cairo")
  }
} # priority order