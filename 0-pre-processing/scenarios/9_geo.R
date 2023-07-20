# regional prioritization
scenarios_geo = unique(as.character(scenarios$Geo))

## load data ###################################################################

# load regional prioritization
csv_geo = read.csv2(paste0("../../0-pre-processing/scenarios/ref/cell_prior_", "356", ".csv"))

# load relative reproduction number
df_epi_R = read.csv2(paste0("../../0-pre-processing/initial_I/output/", "relative_reproduction_number_356", ".csv"),
                     row.names = 1, header = T)
# colnames
colnames(df_epi_R)[colnames(df_epi_R) == "municip_code"] = "fhidata.municip_code"
colnames(df_epi_R)[colnames(df_epi_R) == "municip_name"] = "fhidata.municip_name"
df_epi_R$value = as.numeric(df_epi_R$value) # relative reproduction number
# normalize
df_epi_R$value = df_epi_R$value / max(df_epi_R$value)

# merge 2 data frame
df_geo = merge(csv_geo, df_epi_R)

## regional prioritization #####################################################
df_map_all = data.frame(NULL)
for( i in 1:length(scenarios_geo) ){
  
  ## alternative scenario ######################################################
  df_geo = cbind(df_geo,
                 prior_alt = df_geo$prior_AB)
  
  # alternative priority
  municip_N = as.numeric(scenarios_geo[i])
  if(municip_N > 0){ # more plus
    # turn non-plus  to be plus
    sort_index1 = order(df_geo$value, decreasing = T)
    df_geo$prior_alt[sort_index1[which(df_geo$prior_AB[sort_index1] <  1)[1: municip_N]]] =  1
  }else if(municip_N < 0){ # more minus
    # turn non-minus to be minus
    sort_index0 = order(df_geo$value, decreasing = F)
    df_geo$prior_alt[sort_index0[which(df_geo$prior_AB[sort_index0] > -1)[1:-municip_N]]] = -1
  }
  # rename column
  colnames(df_geo)[colnames(df_geo) == "prior_alt"] = paste0("prior_", scenarios_geo[i])
  
  ## save as txt/csv ###########################################################
  
  # file name
  print( filename <- paste0("geo_", scenarios_geo[i]))
  # save as csv
  write.table(df_geo[, c("fhidata.municip_code", "fhidata.municip_name", "value", paste0("prior_", scenarios_geo[i]))],
              file = paste0("scenarios_csv/", filename, ".csv"),
              quote = T,
              sep = ";",
              row.names = F, col.names = T)
  # save as txt
  write.table(df_geo[, paste0("prior_", scenarios_geo[i])],
              file = paste0("../../../ibm-vaccination/1-main-ABM/In_out/input_files/",
                            "scenarios_txt/", filename, ".txt"),
              quote = F,
              sep = "\t",
              row.names = F, col.names = F)
  
  ## map #######################################################################
  
  # take fhidataold::norway_map_municips_b2020
  df_map = as.data.frame(fhidataold::norway_map_municips_b2020)
  df_map$location_code = factor(df_map$location_code,
                                levels = fhidata::norway_locations_b2020$municip_code)
  # add fhidata::norway_locations_b2020
  df_map = cbind(df_map,
                 fhidata::norway_locations_b2020[as.numeric(df_map$location_code), ])
  
  # adding groups
  df_map$R = factor(df_map$location_code,
                    levels = df_geo$fhidata.municip_code,
                    labels = df_geo[, paste0("prior_", scenarios_geo[i])])
  levels(df_map$R) = as.list(setNames(-1:1,
                                      c("Minus", "Neutral", "Plus")))
  # scenario name
  df_map$Geo = as.numeric(scenarios_geo[i])
  df_map$facet = paste0("Δn = ", ifelse(scenarios_geo[i] > 0, "+", ""), scenarios_geo[i], " (",
                        paste0(
                          paste(as.character(table(
                            factor(df_geo[, paste0("prior_", scenarios_geo[i])],
                                   levels = -1:1,
                                   labels = c("Minus", "Neutral", "Plus"))
                          )),
                          c("Minus", "Neutral", "Plus")
                          ), collapse = ", "
                        ), ")"
  )
  # all maps
  df_map_all = rbind(df_map_all, df_map)
  
  # https://www.r-spatial.org/r/2018/10/25/ggplot2-sf.html
  # the whole world
  world <- rnaturalearth::ne_countries(scale = "medium", returnclass = "sf")
  # without Norway
  world_no = subset(world, name != "Norway")
  
  # plot
  p =
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
    geom_polygon(data = df_map,
                 aes(x = long,
                     y = lat,
                     fill = R,
                     group = group),
                 col = "black",
                 lwd = 0.1) +
    scale_fill_brewer(palette = "Set2" ,
                      name   = "Prioritization of municipalities",
                      drop = F) +
    # scale_fill_viridis_c() +
    # guides(fill = guide_colourbar(barheight = 25)) +
    # viridis::scale_fill_viridis(discrete = T) +
    # scale_fill_brewer(palette = "Paired") +
    # scale_fill_manual(values = (colorRampPalette(RColorBrewer::brewer.pal(8, "Paired"))(25))) +
    # scale_fill_manual(values = rep(RColorBrewer::brewer.pal(8, "Paired"), 3)) +
    labs(#title = paste0("(", df_geo_name$Scenario, "): ", df_geo_name$Name)[i],
      x = "longitude",
      y = "latitude",
      fill = "Prioritization of municipalities") +
    coord_sf(xlim = c(4.5, 31.5), ylim = c(57.5, 71.5), expand = F) + # Norway
    # coord_sf(xlim = c(10.4, 11.1), ylim = c(59.7, 60.3), expand = F) + # Oslo
    # coord_sf(xlim = c(9.8, 11.8), ylim = c(58.8, 60.8), expand = F) + # Viken
    theme_bw(base_size = 24) +
    # guides(fill = guide_legend(title.position = "top",
    #                            ncol = 1)) +
    theme(legend.position = "bottom",
          panel.background = element_rect(fill = "azure"),
          axis.text.x = element_text(angle = 0, hjust = 0.5))
  
  # add zoom in
  p =
    p +
    facet_wrap(vars(facet)) +
    annotation_custom(
      ggplotGrob(
        p +
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
  
  # save figure
  print( name_plot <- paste0("ref/", "geo_map_", scenarios_geo[i], ".png") )
  ggplot2::ggsave(file = name_plot, plot = p, width = 11, height = 11*1, type = "cairo")
  
}

## ggplot (all maps) ###########################################################

# https://www.blopig.com/blog/2019/08/combining-inset-plots-with-facets-using-ggplot2/
# This function allows us to specify which facet to annotate
annotation_custom2 <- function (grob, xmin = -Inf, xmax = Inf, ymin = -Inf, ymax = Inf, data)
{
  layer(data = data, stat = StatIdentity, position = PositionIdentity,
        geom = ggplot2:::GeomCustomAnn,
        inherit.aes = TRUE, params = list(grob = grob,
                                          xmin = xmin, xmax = xmax,
                                          ymin = ymin, ymax = ymax))
}

# name
facet_names = unique(df_map_all$facet)
names(facet_names) = unique(df_map_all$Geo)

# plot
fun_map <- function(df_map_all){
  
  p =
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
    geom_polygon(data = df_map_all,
                 aes(x = long,
                     y = lat,
                     fill = R,
                     group = group),
                 col = "black",
                 lwd = 0.1) +
    scale_fill_brewer(palette = "Set2" ,
                      name   = "Prioritization of municipalities",
                      drop = F) +
    # scale_fill_viridis_c() +
    # guides(fill = guide_colourbar(barheight = 25)) +
    # viridis::scale_fill_viridis(discrete = T) +
    # scale_fill_brewer(palette = "Paired") +
    # scale_fill_manual(values = (colorRampPalette(RColorBrewer::brewer.pal(8, "Paired"))(25))) +
    # scale_fill_manual(values = rep(RColorBrewer::brewer.pal(8, "Paired"), 3)) +
    facet_wrap(vars(Geo),
               nrow = 3,
               labeller = labeller(Geo = facet_names)) +
    labs(#title = paste0("(", df_geo_name$Scenario, "): ", df_geo_name$Name)[i],
      x = "longitude",
      y = "latitude",
      fill = "Prioritization of municipalities") +
    coord_sf(xlim = c(4.5, 31.5), ylim = c(57.5, 71.5), expand = F) + # Norway
    # coord_sf(xlim = c(10.4, 11.1), ylim = c(59.7, 60.3), expand = F) + # Oslo
    # coord_sf(xlim = c(9.8, 11.8), ylim = c(58.8, 60.8), expand = F) + # Viken
    theme_bw(base_size = 23) +
    # guides(fill = guide_legend(title.position = "top",
    #                            ncol = 1)) +
    theme(legend.position = "bottom",
          panel.background = element_rect(fill = "azure"),
          axis.text.x = element_text(angle = 0, hjust = 0.5))
  
  return(p)
}

# plot
fun_map_zoom <- function(df_map_all){
  
  p =
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
    geom_polygon(data = df_map_all,
                 aes(x = long,
                     y = lat,
                     fill = R,
                     group = group),
                 col = "black",
                 lwd = 0.1) +
    scale_fill_brewer(palette = "Set2" ,
                      name   = "Prioritization of municipalities",
                      drop = F) +
    # scale_fill_viridis_c() +
    # guides(fill = guide_colourbar(barheight = 25)) +
    # viridis::scale_fill_viridis(discrete = T) +
    # scale_fill_brewer(palette = "Paired") +
    # scale_fill_manual(values = (colorRampPalette(RColorBrewer::brewer.pal(8, "Paired"))(25))) +
    # scale_fill_manual(values = rep(RColorBrewer::brewer.pal(8, "Paired"), 3)) +
    # facet_wrap(vars(Geo),
    #            nrow = 5) +
    labs(#title = paste0("(", df_geo_name$Scenario, "): ", df_geo_name$Name)[i],
      x = "longitude",
      y = "latitude",
      fill = "Prioritization of municipalities") +
    # coord_sf(xlim = c(4.5, 31.5), ylim = c(57.5, 71.5), expand = F) + # Norway
    # coord_sf(xlim = c(10.4, 11.1), ylim = c(59.7, 60.3), expand = F) + # Oslo
    # coord_sf(xlim = c(9.8, 11.8), ylim = c(58.8, 60.8), expand = F) + # Viken
    theme_bw(base_size = 23) +
    # guides(fill = guide_legend(title.position = "top",
    #                            ncol = 1)) +
    # theme(legend.position = "bottom",
    #       panel.background = element_rect(fill = "azure"),
    #       axis.text.x = element_text(angle = 0, hjust = 0.5)) +
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
          plot.background  = element_rect(fill = "transparent", color = NA))
  
  return(p)
  
}

p =
  fun_map(df_map_all) +
  df_map_all %>%
  split(f = .$Geo) %>%
  purrr::map(~annotation_custom2(
    grob = ggplotGrob(fun_map_zoom(.)),
    data = data.frame(Geo=unique(.$Geo)),
    xmin = 15.0, xmax = 31.5, ymin = 57.5, ymax = 65.0)
  )

# save figure
print( name_plot <- paste0("ref/", "geo_map_all_", length(scenarios_geo), ".png") )
ggplot2::ggsave(file = name_plot, plot = p, width = 11*2.5, height = 8.5*2.5, type = "cairo")

## ggplot ######################################################################

# melt for ggplot
df_geo_melt = reshape2::melt(df_geo[, -c(1:7)],
                             id.vars = "value")
colnames(df_geo_melt) = c("R", "Scenario", "Priority")
# Scenario
df_geo_melt$Scenario = factor(df_geo_melt$Scenario,
                              levels = paste0("prior_", sort(as.numeric(scenarios_geo))),
                              labels =                  sort(as.numeric(scenarios_geo)))
# Priority
df_geo_melt$Priority = factor(df_geo_melt$Priority,
                              levels = c(-1     , 0        , 1     ),
                              labels = c("Minus", "Neutral", "Plus"))
# count number of Priority
df_geo_count = as.data.frame(
  df_geo_melt %>%
    group_by(Scenario, Priority, .drop = F) %>%
    summarise(Count = n())
)
#
df_geo_count$Name = paste(df_geo_count$Count, df_geo_count$Priority)
#
df_geo_name = as.data.frame(
  df_geo_count %>%
    group_by(Scenario) %>%
    summarise(Name = paste(Name, collapse = ", "))
)
#
facet_names = paste0("Δn = ", ifelse(as.numeric(levels(df_geo_name$Scenario)) > 0, "+", ""), df_geo_name$Scenario, " (", df_geo_name$Name, ")")
names(facet_names) = df_geo_name$Scenario

# ggplot
p =
  ggplot(df_geo_melt, aes(x = R,
                          fill = Priority)) +
  geom_histogram(binwidth = 0.01,
                 size = 0.2,
                 col = "black") +
  facet_wrap(vars(Scenario),
             nrow = 3,
             labeller = labeller(Scenario = facet_names)) +
  fhiplot::scale_color_fhi(palette = "primary") +
  scale_fill_brewer(palette = "Set2" ,
                    name   = "Prioritization of municipalities") +
  labs(#title = paste0(i, " scenarios: prioritization of municipalities"),
    fill = "Prioritization of municipalities",
    x = "Relative reproduction number",
    y = "Number of municipalities") +
  theme_bw(base_size = 14) +
  theme(legend.position = "bottom")
# save figure
print( name_plot <- paste0("ref/", "geo_", i, ".png") )
ggplot2::ggsave(file = name_plot, plot = p, width = 11*1.5, height = 8.5, type = "cairo")

## maximum extra doses to Plus group (Regional) ################################

# maximum %
df_geo_max = data.frame(Geo = scenarios_geo,
                        Max = NA)
for( i in 1:length(scenarios_geo) ){
  
  # 18+ population size of each municip group (from fhidata::norway_population_b2020)
  df_pop_no = subset(fhidata::norway_population_b2020,
                     year == "2020" &
                       level == "municip" &
                       age >= 18)
  df_mun = aggregate(pop ~ location_code, df_pop_no, sum)
  colnames(df_mun) = c("fhidata.municip_code", "ibmdata.count")
  # 3 groups
  df_mun$geo =
    factor(df_mun$fhidata.municip_code,
           levels = df_geo$fhidata.municip_code,
           labels = df_geo[, paste0("prior_", scenarios_geo[i])])
  df_mun = aggregate(ibmdata.count ~ geo, df_mun, sum)
  # maximum %
  df_geo_max$Max[df_geo_max$Geo == scenarios_geo[i]] =
    df_mun$ibmdata.count[df_mun$geo == "-1"] / df_mun$ibmdata.count[df_mun$geo == "1"]
  
}
# rounding maximum %
df_geo_max$Max = floor(df_geo_max$Max * 10) / 10
# sorting
df_geo_max = df_geo_max[order(as.numeric(as.character(df_geo_max$Geo))), ]

# file name
print( filename <- paste0("geo_max_", scenarios_geo[i]))
# save as csv
write.table(df_geo_max,
            file = paste0("ref/", filename, ".csv"),
            quote = T,
            sep = ";",
            row.names = F, col.names = T)