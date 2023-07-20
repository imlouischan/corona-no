# dynamical control
scenarios_control = unique(scenarios$R_const)

## vector ##############################################################

fig = list()
for( i in 1:length(scenarios_control) ){
  
  # control or not
  if(scenarios_control[i] == 200){
    H_min = c( 50) # threshold min
    H_mid = c(125) # threshold middle
    H_max = c(200) # threshold max
  }else if(scenarios_control[i] == 400){
    H_min = c( 50) # threshold min
    H_mid = c(125) # threshold middle
    H_max = c(200) # threshold max
  }else{
    H_min =  50 # threshold min
    H_mid = 125 # threshold middle
    H_max = 200 # threshold max
  }
  # four corresponding Rt
  Rt = c(1.2, 1.05, 1, 0.8)
  
  ## input file ##############################################################
  
  control_vector = c(CONTROL_H_Rt_0 = Rt[1], 
                     CONTROL_H_1 = H_min, 
                     CONTROL_H_Rt_1 = Rt[2], 
                     CONTROL_H_2 = H_mid, 
                     CONTROL_H_Rt_2 = Rt[3], 
                     CONTROL_H_3 = H_max, 
                     CONTROL_H_Rt_3 = Rt[4])
  
  ## save ##############################################################
  
  # file name
  print( filename <- paste0("control_", scenarios_control[i]) )
  # save as csv
  write.table(control_vector,
              file = paste0("scenarios_csv/", filename, ".csv"),
              quote = T,
              sep = ";",
              row.names = T, col.names = NA)
  # save as txt
  write.table(control_vector,
              file = paste0("../../../ibm-vaccination/1-main-ABM/In_out/input_files/", 
                            "scenarios_txt/", filename, ".txt"),
              quote = F,
              sep = "\t",
              row.names = T, col.names = F)
  
}