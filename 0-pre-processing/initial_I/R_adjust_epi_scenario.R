## new script ##################################################################
cat("\014")    # clear the console
rm(list=ls())  # remove all variables
graphics.off() # close all plots
set.seed(2020) # fix randomness
suppressMessages(library(ggplot2))
suppressMessages(library(dplyr))
suppressMessages(library(tidyr))
options(dplyr.summarise.inform = FALSE) # Suppress summarise info
## read data ###################################################################

# load data: 13521 cells
df_pop <- read.table(paste0("../../../ibm-vaccination/1-main-ABM/In_out/input_files/",
                            "population_grunnkretser_2021-20210427_grunnkrets_column_and_new_abm_muni_codes.txt"),
                     sep = "\t")
colnames(df_pop) = c("x", "y", "c", "r", "count", "mun", "mun_ID", "county", "county_ID", "grunnkrets")
# mun_code
df_pop$mun_code = factor(paste0("municip", sprintf("%04d", df_pop$mun)),
                         levels = fhidata::norway_locations_b2020$municip_code)
# mun_name
df_pop$mun_name = factor(paste0("municip", sprintf("%04d", df_pop$mun)),
                         levels = fhidata::norway_locations_b2020$municip_code,
                         labels = fhidata::norway_locations_b2020$municip_name)
# grunnkrets_code
df_pop$grunnkrets_code = factor(paste0("grunnkrets", sprintf("%08d", df_pop$grunnkrets)))
# load ward_code
df_ward <- read.table("input/oslo_grunnkrets_ward_mapping.csv",
                      sep = ",",
                      header = T)
# ward_code
df_pop$ward_code = factor(df_pop$grunnkrets_code,
                          levels = df_ward$grunnkrets,
                          labels = df_ward$bydel)
# add fhidata::norway_locations_b2020
df_pop = cbind(ibmdata = df_pop,
               fhidata = fhidata::norway_locations_b2020[as.numeric(df_pop$mun_code), ])

## read data ###################################################################

# read initial values
df_epi <- read.table("input/epi_scenario_municip_ibm.txt",
                     header = T,
                     stringsAsFactors = F)

# mun_ID in IBM
df_epi$mun_ID = factor(df_epi$county,
                       levels = c(as.character(df_pop$fhidata.municip_code), # 350 municip
                                  as.character(df_ward$bydel)), # 15 ward (fhidata::norway_locations_ward_b2020$ward_code)
                       # c("municip3029"), # 1 missing municip
                       # c("municip5042", "municip5043", "municip5429", "municip5441", "municip5442")), # 5 missing municip
                       labels = c(as.character(df_pop$ibmdata.mun_ID), # 350 municip
                                  as.character(rep(1, length(as.character(df_ward$bydel)))))) # west/east
# c(117),
# c(320, 320, 302, 301, 318))) # some of the 350 municip
df_epi$mun_ID = as.numeric(as.character(df_epi$mun_ID)) - 1
# name for IBM
df_epi$compartment_ibm <- paste(df_epi$compartment, df_epi$mun_ID, sep = "_")

## merge (mun & ward) ##########################################################

# mean(relative_reproduction_number)
df_epi_R = subset(df_epi, compartment == "relative_reproduction_number")
df_epi_R = aggregate(value ~ compartment_ibm, df_epi_R, mean)

# sum(others)
df_epi_I = subset(df_epi, compartment != "relative_reproduction_number")
df_epi_I = aggregate(value ~ compartment_ibm, df_epi_I, sum)
# round
df_epi_I$value = mc2d::rbern(nrow(df_epi_I), df_epi_I$value - floor(df_epi_I$value)) + floor(df_epi_I$value)

# both
df_epi_ibm = rbind(df_epi_R, df_epi_I)

# check
sum( df_epi_ibm$value[grepl("E1a_" , df_epi_ibm$compartment_ibm, fixed = T)] )
sum( df_epi_ibm$value[grepl("E1_"  , df_epi_ibm$compartment_ibm, fixed = T)] )
sum( df_epi_ibm$value[grepl("E2_"  , df_epi_ibm$compartment_ibm, fixed = T)] )
sum( df_epi_ibm$value[grepl("Ia_"  , df_epi_ibm$compartment_ibm, fixed = T)] )
sum( df_epi_ibm$value[grepl("I_"   , df_epi_ibm$compartment_ibm, fixed = T)] )
sum( df_epi_ibm$value[grepl("R_"   , df_epi_ibm$compartment_ibm, fixed = T)] )
# check
sum( df_epi_ibm$value[grepl("prev_H_"  , df_epi_ibm$compartment_ibm, fixed = T)] )
sum( df_epi_ibm$value[grepl("prev_ICU_", df_epi_ibm$compartment_ibm, fixed = T)] )

## save ########################################################################

# only non-zero rows
df_epi_ibm = subset(df_epi_ibm, value > 0)

# folder
system( paste0("rm -r output; mkdir output") )
# file name
print( filename <- paste0("epi_scenario_regions") )
# save as csv
write.table(df_epi_ibm,
            file = paste0("output/", filename, ".csv"),
            quote = T,
            sep = ";",
            row.names = T, col.names = NA)
# save as txt
write.table(df_epi_ibm,
            file = paste0("../../../ibm-vaccination/1-main-ABM/In_out/input_files/", filename, ".txt"),
            quote = F,
            sep = "\t",
            row.names = F, col.names = F)

## relative_reproduction_number only ###########################################

# mean(relative_reproduction_number)
df_epi_R = subset(df_epi, compartment == "relative_reproduction_number")
df_epi_R = aggregate(value ~ mun_ID, df_epi_R, mean)
df_epi_R = cbind(fhidata::norway_locations_b2020[, c("municip_code", "municip_name")],
                 df_epi_R)

# save as csv
write.table(df_epi_R,
            file = paste0("output/", "relative_reproduction_number_356", ".csv"),
            quote = T,
            sep = ";",
            row.names = T, col.names = NA)

## ggplot (map) ################################################################

# take fhidataold::norway_map_municips_b2020
df_map = as.data.frame(fhidataold::norway_map_municips_b2020)
df_map$location_code = factor(df_map$location_code,
                              levels = fhidata::norway_locations_b2020$municip_code)
# add fhidata::norway_locations_b2020
df_map = cbind(df_map,
               fhidata::norway_locations_b2020[as.numeric(df_map$location_code), ])

# adding relative_reproduction_number
df_map$R = factor(df_map$location_code,
                  levels = df_epi_R$municip_code,
                  labels = df_epi_R$value)
df_map$R = as.numeric(as.character(df_map$R))
# normalized
df_map$R = df_map$R/max(df_map$R)

# population size of each municip group (from fhidata::norway_population_b2020)
df_pop_no = subset(fhidata::norway_population_b2020,
                   year == "2020" &
                     level == "municip" &
                     age >= 0)
df_mun = aggregate(pop ~ location_code, df_pop_no, sum)
colnames(df_mun) = c("fhidata.municip_code", "ibmdata.count")
# adding population size
df_map$pop = factor(df_map$location_code,
                    levels = df_mun$fhidata.municip_code,
                    labels = log10(df_mun$ibmdata.count))
df_map$pop = as.numeric(as.character(df_map$pop))

# population density
# https://en.wikipedia.org/wiki/List_of_municipalities_of_Norway (30 Mar 2022)
df_area = read.table(paste0("../../../ibm-vaccination/0-pre-processing/initial_I/input/",
                            "area_municip.txt"))
colnames(df_area) = "area"
# adding population density
df_map$den = factor(df_map$location_code,
                    levels = df_mun$fhidata.municip_code,
                    labels = log10(df_mun$ibmdata.count / df_area$area))
df_map$den = as.numeric(as.character(df_map$den))

# load vaccination coverage
csv_vpk = read.csv2(paste0("../../../ibm-vaccination/0-pre-processing/initial_V/output/",
                           "VaksinertPerKommune.csv"),
                    sep = ";",
                    row.names = 1)
# format
df_time <- read.table(paste0("../../../ibm-vaccination/1-main-ABM/In_out/input_files/scenarios_txt/time.txt"),
                      row.names = 1, stringsAsFactors = F)
csv_vpk$time = csv_vpk$time + as.Date(df_time["date_start", ]) - 1
# # population size of each municip group (from fhidata::norway_population_b2020)
# df_pop_no_18 = subset(df_pop_no,
#                       age >= 18)
# df_mun_18 = aggregate(pop ~ location_code, df_pop_no_18, sum)
# population size of each municip group (from fhi01 and Anja)
df_mun_18 = readRDS("../../../ibm-vaccination/0-pre-processing/initial_V/input/Popdata_freg_fnr_bosatt_municip_by_age_riskgroup.rds") %>%
  group_by(municip_code) %>%
  filter(municip_code != "missingmunicip9999") %>%
  summarise(pop = sum(pop_fnr_bosatt)) %>%
  ungroup()
colnames(df_mun_18) = c("fhidata.municip_code", "ibmdata.count")
# subset
csv_vpk_sub =
  csv_vpk %>%
  filter(time >= "2021-01-01" & time < "2021-08-01") %>%
  complete(mun, fill = list(count = 0)) %>%
  group_by(mun) %>%
  summarise(count = sum(count)) %>%
  filter(mun != 9999) %>%
  mutate(df_mun_18) %>%
  mutate(coverage = count / ibmdata.count)
# adding vaccination coverage
df_map$vac = factor(df_map$location_code,
                    levels = csv_vpk_sub$fhidata.municip_code,
                    labels = csv_vpk_sub$coverage)
df_map$vac = as.numeric(as.character(df_map$vac))

# https://www.r-spatial.org/r/2018/10/25/ggplot2-sf.html
# the whole world
world <- rnaturalearth::ne_countries(scale = "medium", returnclass = "sf")
# without Norway
world_no = subset(world, name != "Norway")

# plot
fig_map =
  ggplot() +
  geom_sf(data = world_no,
          fill = "lightgray",
          alpha = 1) +
  geom_polygon(data = fhidataold::norway_map_municips_b2020,
               aes(x = long,
                   y = lat,
                   group = group),
               fill = "darkgray",
               col = "black",
               lwd = 0.1) +
  # scale_fill_viridis_c() +
  guides(fill = guide_colourbar(barheight = 20)) +
  # viridis::scale_fill_viridis(discrete = T) +
  # scale_fill_brewer(palette = "Paired") +
  # scale_fill_manual(values = (colorRampPalette(RColorBrewer::brewer.pal(8, "Paired"))(25))) +
  # scale_fill_manual(values = rep(RColorBrewer::brewer.pal(8, "Paired"), 3)) +
  coord_sf(xlim = c(4.5, 31.5), ylim = c(57.5, 71.5), expand = F) + # Norway
  # coord_sf(xlim = c(10.4, 11.1), ylim = c(59.7, 60.3), expand = F) + # Oslo
  # coord_sf(xlim = c(9.8, 11.8), ylim = c(58.8, 60.8), expand = F) + # Viken
  theme_bw(base_size = 20) +
  # guides(fill = guide_legend(title.position = "top",
  #                            ncol = 1)) +
  theme(legend.position = "right",
        panel.background = element_rect(fill = "azure"),
        axis.text.x = element_text(angle = 0, hjust = 0.5),
        legend.title = element_text(vjust = 1))

# two plots
fig = list()
# population size
fig[["pop"]] = fig_map +
  geom_polygon(data = df_map,
               aes(x = long,
                   y = lat,
                   fill = pop,
                   group = group),
               col = "black",
               lwd = 0.1) +
  scale_fill_viridis_c(breaks = 0:10, labels = scales::math_format(10^.x), limits = c(2, 6)) +
  labs(title = "Population",
       x = "longitude",
       y = "latitude",
       fill = "")
# density
fig[["den"]] = fig_map +
  geom_polygon(data = df_map,
               aes(x = long,
                   y = lat,
                   fill = den,
                   group = group),
               col = "black",
               lwd = 0.1) +
  scale_fill_viridis_c(breaks = 0:10, labels = scales::math_format(10^.x), limits = c(-1, 4)) +
  labs(title = "Population density",
       x = "longitude",
       y = "latitude",
       fill = "")
# relative_reproduction_number
fig[["R"]] = fig_map +
  geom_polygon(data = df_map,
               aes(x = long,
                   y = lat,
                   fill = R,
                   group = group),
               col = "black",
               lwd = 0.1) +
  scale_fill_viridis_c() +
  labs(title = "Relative reproduction number",
       x = "longitude",
       y = "latitude",
       fill = "")
# vaccination coverage
fig[["vac"]] = fig_map +
  geom_polygon(data = df_map,
               aes(x = long,
                   y = lat,
                   fill = vac,
                   group = group),
               col = "black",
               lwd = 0.1) +
  scale_fill_viridis_c() +
  labs(title = "Vaccination coverage",
       x = "longitude",
       y = "latitude",
       fill = "")

# add zoom in
fig[["pop"]] =
  fig[["pop"]] +
  annotation_custom(
    ggplotGrob(
      fig[["pop"]] +
        coord_sf(xlim = c(10.4-2, 11.1+2), ylim = c(59.7-1, 60.3+1), expand = F) +
        theme(axis.title.x = element_blank(),
              axis.title.y = element_blank(),
              axis.text.x  = element_blank(),
              axis.text.y  = element_blank(),
              axis.ticks.y = element_blank(),
              axis.ticks.x = element_blank(),
              title           = element_blank(),
              legend.position = "none",
              plot.margin = unit(c(0,0,0,0), "cm"),
              plot.background  = element_rect(fill = "transparent", color = NA))),
    xmin = 15.0, xmax = 31.5, ymin = 57.5, ymax = 65.0)
# xmin = 4.5, xmax = 15, ymin = 66, ymax = 71.5

# add zoom in
fig[["den"]] =
  fig[["den"]] +
  annotation_custom(
    ggplotGrob(
      fig[["den"]] +
        coord_sf(xlim = c(10.4-2, 11.1+2), ylim = c(59.7-1, 60.3+1), expand = F) +
        theme(axis.title.x = element_blank(),
              axis.title.y = element_blank(),
              axis.text.x  = element_blank(),
              axis.text.y  = element_blank(),
              axis.ticks.y = element_blank(),
              axis.ticks.x = element_blank(),
              title           = element_blank(),
              legend.position = "none",
              plot.margin = unit(c(0,0,0,0), "cm"),
              plot.background  = element_rect(fill = "transparent", color = NA))),
    xmin = 15.0, xmax = 31.5, ymin = 57.5, ymax = 65.0)
# xmin = 4.5, xmax = 15, ymin = 66, ymax = 71.5

# add zoom in
fig[["R"]] =
  fig[["R"]] +
  annotation_custom(
    ggplotGrob(
      fig[["R"]] +
        coord_sf(xlim = c(10.4-2, 11.1+2), ylim = c(59.7-1, 60.3+1), expand = F) +
        theme(axis.title.x = element_blank(),
              axis.title.y = element_blank(),
              axis.text.x  = element_blank(),
              axis.text.y  = element_blank(),
              axis.ticks.y = element_blank(),
              axis.ticks.x = element_blank(),
              title           = element_blank(),
              legend.position = "none",
              plot.margin = unit(c(0,0,0,0), "cm"),
              plot.background  = element_rect(fill = "transparent", color = NA))),
    xmin = 15.0, xmax = 31.5, ymin = 57.5, ymax = 65.0)
# xmin = 4.5, xmax = 15, ymin = 66, ymax = 71.5

# add zoom in
fig[["vac"]] =
  fig[["vac"]] +
  annotation_custom(
    ggplotGrob(
      fig[["vac"]] +
        coord_sf(xlim = c(10.4-2, 11.1+2), ylim = c(59.7-1, 60.3+1), expand = F) +
        theme(axis.title.x = element_blank(),
              axis.title.y = element_blank(),
              axis.text.x  = element_blank(),
              axis.text.y  = element_blank(),
              axis.ticks.y = element_blank(),
              axis.ticks.x = element_blank(),
              title           = element_blank(),
              legend.position = "none",
              plot.margin = unit(c(0,0,0,0), "cm"),
              plot.background  = element_rect(fill = "transparent", color = NA))),
    xmin = 15.0, xmax = 31.5, ymin = 57.5, ymax = 65.0)
# xmin = 4.5, xmax = 15, ymin = 66, ymax = 71.5

# plot together
p <-
  cowplot::plot_grid(plotlist = fig,
                     nrow = 2,
                     labels = "AUTO",
                     hjust = 0,
                     label_size = 24, label_fontface = "plain",
                     align = "hv", axis = "t")
# save figure
print( name_plot <- paste0("output/", "Norway_map_", "relative_reproduction_number_356", ".png") )
ggplot2::ggsave(file = name_plot, plot = p, width = 8.5*1.5, height = 11, type = "cairo")

## ggplot (relative_reproduction_number vs population) #########################

p =
  ggplot(cbind(df_epi_R, df_mun),
         aes(x = value / max(value),
             y = log10(ibmdata.count))) +
  geom_point() +
  labs(#title = "Population",
    x = "Relative reproduction number",
    y = "log10(Population)") +
  theme_bw(base_size = 20)
# save figure
print( name_plot <- paste0("output/", "relative_reproduction_number_and_population", ".png") )
ggplot2::ggsave(file = name_plot, plot = p, width = 11, height = 8.5, type = "cairo")

## ggplot (relative_reproduction_number vs density) #########################

p =
  ggplot(cbind(df_epi_R, df_mun, df_area),
         aes(x = value / max(value),
             y = log10(ibmdata.count / area))) +
  geom_point() +
  labs(#title = "density",
    x = "Relative reproduction number",
    y = "log10(Density)") +
  theme_bw(base_size = 20)
# save figure
print( name_plot <- paste0("output/", "relative_reproduction_number_and_density", ".png") )
ggplot2::ggsave(file = name_plot, plot = p, width = 11, height = 8.5, type = "cairo")