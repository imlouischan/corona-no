## read excel file #############################################################

# input file
para = gdata::read.xls("parameters_vaccination.xlsx")

# two columns
para = subset(para, IBM != "-")
text = para[, c(4, 2)]

# adjustment of probability of ICU and death ###################################

# factor of probability of ICU
adj_u = rep(1, 9)
# for 1.0* IHR
adj_u = c(1.75,
          0.731707317073171,
          2.17391304347826,
          0.872817955112219,
          1.00149476831091,
          0.984102952308857,
          1.02770330652368,
          1.05329949238579,
          1.01910828025478)
# for 0.5* IHR
adj_u = c(1.84210526315789,
          0.769230769230769,
          2.7027027027027,
          0.823529411764706,
          0.99554234769688,
          0.984102952308857,
          0.99052540913006,
          1.06274007682458,
          1.31147540983607)

# factor of probability of death
adj_d = rep(1, 9)
# for 1.0* IHR
adj_d = c(1,
          1,
          2.5,
          1.66666666666667,
          1.79487179487179,
          0.788177339901478,
          1.25             /38*43,   # manual adjustment
          1.32404181184669 /73*76,   # manual adjustment
          1.16234390009606 /132*121) # manual adjustment
# for 0.5* IHR
adj_d = c(1,
          1,
          3.33333333333333,
          0.8,
          0.864197530864198,
          0.373831775700935,
          0.629575402635432,
          0.695333943275389,
          0.712603062426384)

# name of variables
vari_u = c("PROB_ICU_0_9",
           "PROB_ICU_10_19",
           "PROB_ICU_20_29",
           "PROB_ICU_30_39",
           "PROB_ICU_40_49",
           "PROB_ICU_50_59",
           "PROB_ICU_60_69",
           "PROB_ICU_70_79",
           "PROB_ICU_80p")
vari_d = c("PROB_DEATH_0_9",
           "PROB_DEATH_10_19",
           "PROB_DEATH_20_29",
           "PROB_DEATH_30_39",
           "PROB_DEATH_40_49",
           "PROB_DEATH_50_59",
           "PROB_DEATH_60_69",
           "PROB_DEATH_70_79",
           "PROB_DEATH_80p")
vari_d_risk = c("PROB_DEATH_RG_0_9",
                "PROB_DEATH_RG_10_19",
                "PROB_DEATH_RG_20_29",
                "PROB_DEATH_RG_30_39",
                "PROB_DEATH_RG_40_49",
                "PROB_DEATH_RG_50_59",
                "PROB_DEATH_RG_60_69",
                "PROB_DEATH_RG_70_79",
                "PROB_DEATH_RG_80p")

# apply the factors
for(n in 1:length(adj_u)) text$Value[text$IBM == vari_u[n]] = text$Value[text$IBM == vari_u[n]] * adj_u[n]
for(n in 1:length(adj_d)) text$Value[text$IBM == vari_d[n]] = text$Value[text$IBM == vari_d[n]] * adj_d[n]
for(n in 1:length(adj_d)) text$Value[text$IBM == vari_d_risk[n]] = text$Value[text$IBM == vari_d_risk[n]] * adj_d[n]

## print text file #############################################################

# output file
write.table(text,
            file = paste0("../../../ibm-vaccination/1-main-ABM/In_out/input_files/",
                          "parameters_vaccination", ".txt"),
            quote = F,
            sep = "\t",
            row.names = F, col.names = F)