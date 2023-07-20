# # vaccine profile
scenarios_profile = unique(scenarios$Profile)
scenarios_trans   = unique(scenarios$P_trans)
scenarios_delay   = unique(scenarios$Delay)

## vector ##############################################################

fig = list()
for( i in 1:length(scenarios_profile) ){ # different VE_S
  for( j in 1:length(scenarios_trans) ){ # different VE_V
    for( k in 1:length(scenarios_delay) ){ # different time_wait_1st_2nd
      
      time_linear_1st   = c( 4,  4,  4)*7 # time of linear increase after 1st dosel
      time_wait_1st_2nd = c( 6,  6,  6)*7 # time interval between 1st and 2nd dose
      for(m in 1:3) time_wait_1st_2nd[m] = scenarios_delay[k]*7 - time_linear_1st[m] # for mRNA vaccine
      time_linear_2nd   = c( 2,  2,  2)*7 # time of linear increase after 2nd dose
      
      if (scenarios_profile[i] == "H") { # high VE #############################
        # VE against asymptomatic
        VE_Ia = data.frame(dose1 = c(55, 55, 55),
                           dose2 = c(77, 77, 77)) / 100
        # VE against  symptomatic
        VE_I  = data.frame(dose1 = c(71, 71, 71),
                           dose2 = c(91, 91, 91)) / 100
        # VE against hospitalization
        VE_S  = data.frame(dose1 = c(78, 78, 78),
                           dose2 = c(94, 94, 94)) / 100
        # VE against death
        VE_D  = data.frame(dose1 = c(84, 84, 84),
                           dose2 = c(94, 94, 94)) / 100
      } else if (scenarios_profile[i] == "L") { # low VE #######################
        # VE against asymptomatic
        VE_Ia = data.frame(dose1 = c(35, 35, 35),
                           dose2 = c(59, 59, 59)) / 100
        # VE against  symptomatic
        VE_I  = data.frame(dose1 = c(47, 47, 47),
                           dose2 = c(85, 85, 85)) / 100
        # VE against hospitalization
        VE_S  = data.frame(dose1 = c(60, 60, 60),
                           dose2 = c(94, 94, 94)) / 100
        # VE against death
        VE_D  = data.frame(dose1 = c(60, 60, 60),
                           dose2 = c(94, 94, 94)) / 100
      } # scenarios_profile[i]
      
      # VE against transmissibility
      VE_V  = data.frame(dose1 = c(scenarios_trans[j], scenarios_trans[j], scenarios_trans[j]),
                         dose2 = c(scenarios_trans[j], scenarios_trans[j], scenarios_trans[j])) / 100
      
      ## input file ##############################################################
      
      profile_vector = c(
        
        # time of linear increase after 1st dose
        RAMP_UP_1ST_1 = time_linear_1st[1],
        RAMP_UP_1ST_2 = time_linear_1st[2],
        RAMP_UP_1ST_3 = time_linear_1st[3],
        # time interval between 1st and 2nd dose
        DELAY_EFFECT_1 = time_wait_1st_2nd[1],
        DELAY_EFFECT_2 = time_wait_1st_2nd[2],
        DELAY_EFFECT_3 = time_wait_1st_2nd[3],
        # time of linear increase after 2nd dose
        RAMP_UP_2ND_1 = time_linear_2nd[1],
        RAMP_UP_2ND_2 = time_linear_2nd[2],
        RAMP_UP_2ND_3 = time_linear_2nd[3],
        # 1st does of vaccine 1
        VACCINE_EFF_1ST_1_Ia = VE_Ia$dose1[1],
        VACCINE_EFF_1ST_1_I  = VE_I$dose1[1],
        VACCINE_EFF_1ST_1_S  = VE_S$dose1[1],
        VACCINE_EFF_1ST_1_D  = VE_D$dose1[1],
        VACCINE_EFF_1ST_1_V  = VE_V$dose1[1],
        # 2nd does of vaccine 1
        VACCINE_EFF_2ND_1_Ia = VE_Ia$dose2[1],
        VACCINE_EFF_2ND_1_I  = VE_I$dose2[1],
        VACCINE_EFF_2ND_1_S  = VE_S$dose2[1],
        VACCINE_EFF_2ND_1_D  = VE_D$dose2[1],
        VACCINE_EFF_2ND_1_V  = VE_V$dose2[1],
        # 1st does of vaccine 2
        VACCINE_EFF_1ST_2_Ia = VE_Ia$dose1[2],
        VACCINE_EFF_1ST_2_I  = VE_I$dose1[2],
        VACCINE_EFF_1ST_2_S  = VE_S$dose1[2],
        VACCINE_EFF_1ST_2_D  = VE_D$dose1[2],
        VACCINE_EFF_1ST_2_V  = VE_V$dose1[2],
        # 2nd does of vaccine 2
        VACCINE_EFF_2ND_2_Ia = VE_Ia$dose2[2],
        VACCINE_EFF_2ND_2_I  = VE_I$dose2[2],
        VACCINE_EFF_2ND_2_S  = VE_S$dose2[2],
        VACCINE_EFF_2ND_2_D  = VE_D$dose2[2],
        VACCINE_EFF_2ND_2_V  = VE_V$dose2[2],
        # 1st does of vaccine 3
        VACCINE_EFF_1ST_3_Ia = VE_Ia$dose1[3],
        VACCINE_EFF_1ST_3_I  = VE_I$dose1[3],
        VACCINE_EFF_1ST_3_S  = VE_S$dose1[3],
        VACCINE_EFF_1ST_3_D  = VE_D$dose1[3],
        VACCINE_EFF_1ST_3_V  = VE_V$dose1[3],
        # 2nd does of vaccine 3
        VACCINE_EFF_2ND_3_Ia = VE_Ia$dose2[3],
        VACCINE_EFF_2ND_3_I  = VE_I$dose2[3],
        VACCINE_EFF_2ND_3_S  = VE_S$dose2[3],
        VACCINE_EFF_2ND_3_D  = VE_D$dose2[3],
        VACCINE_EFF_2ND_3_V  = VE_V$dose2[3])
      
      ## save ##############################################################
      
      # file name
      print( filename <- paste0("vaccine_profile_",
                                scenarios_profile[i], "_",
                                scenarios_trans[j], "_",
                                scenarios_delay[k]) )
      # save as csv
      write.table(profile_vector,
                  file = paste0("scenarios_csv/", filename, ".csv"),
                  quote = T,
                  sep = ";",
                  row.names = T, col.names = NA)
      # save as txt
      write.table(profile_vector,
                  file = paste0("../../../ibm-vaccination/1-main-ABM/In_out/input_files/",
                                "scenarios_txt/", filename, ".txt"),
                  quote = F,
                  sep = "\t",
                  row.names = T, col.names = F)
      
      ## ggplot #############################################################
      
      # vaccine type (mRNA)
      v = 1
      # 5 VE in list()
      VE = list(VE_Ia, VE_I, VE_S, VE_D, 1 - VE_V)
      names(VE) = c("Asymptomatic", "Symptomatic", "Hospitalization", "Death", "Transmission")
      # 5 VE in data.frame()
      df_VE = NULL
      time_max = 210
      for( i in 1:length(VE) ){
        df_VE = rbind(df_VE,
                      data.frame(day = seq(0, time_max),
                                 VE = c(VE[[i]]$dose1[v] * seq(0, time_linear_1st[v]) / time_linear_1st[v],                                         # linear_1st
                                        rep(VE[[i]]$dose1[v], time_wait_1st_2nd[v]),                                                                # constant_1st
                                        VE[[i]]$dose1[v] + (VE[[i]]$dose2[v] - VE[[i]]$dose1[v]) * seq(1, time_linear_2nd[v]) / time_linear_2nd[v], # linear_2nd
                                        rep(VE[[i]]$dose2[v], time_max - (time_linear_1st[v] + time_wait_1st_2nd[v] + time_linear_2nd[v]))),        # constant_2nd
                                 name = names(VE)[i])
        )
      }
      df_VE$name = factor(df_VE$name, levels = names(VE))
      
      # ggplot
      p = ggplot(df_VE) +
        geom_line(aes(x = day,
                      y = VE,
                      col = name,
                      group = name),
                  lwd = 1) +
        fhiplot::theme_fhi_lines() +
        scale_x_continuous(breaks = c(seq(from = 0,
                                          to   = time_max,
                                          by   = 7*2)),
                           labels = c(seq(from = 0,
                                          to   = time_max,
                                          by   = 7*2)/7),
                           expand = c(0, 0) ) +
        scale_y_continuous(limits = c(0, 1),
                           breaks = seq(0, 1, 0.1),
                           expand = c(0, 0)) +
        labs(#title = "Seasonality",
          x = "Weeks",
          y = "Vaccine effectiveness",
          col = "Protection") +
        theme_bw(base_size = 24) +
        theme(legend.position = "right")
      
      # save figure
      print( name_plot <- paste0("ref/", filename, ".png") )
      ggplot2::ggsave(file = name_plot, plot = p, width = 11, height = 8.5, type = "cairo")
      
    }
  }
}

# plot
# cowplot::plot_grid(plotlist = fig,
#                    ncol = 4,
#                    labels = "AUTO",
#                    hjust = 0,
#                    label_size = 24, label_fontface = "plain",
#                    align = "hv")