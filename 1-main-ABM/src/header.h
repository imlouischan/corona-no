#include <stdio.h>
#include <sqlite3.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_vector.h>

#define float double

typedef struct{

  unsigned int *idx_h;   /*idx of the household*/
  unsigned int *idx_p;   /*idx of the place*/
  unsigned short int *employment;   /*employment type*/
  int *school_grade;
  int *hosp_entrance;
  int *hosp_leaving;
  int *icu_entrance;
  int *icu_leaving;
  int *vaccinated;
  int *risk_group;
  int *time_vaccination;
  int *risk_category; /* 0: without risk factors; 1: with risk factors; 2: healthcare workers */
  double *vaccine_efficacy_1st_Ia;
  double *vaccine_efficacy_1st_I;
  double *vaccine_efficacy_1st_sd;
  double *vaccine_efficacy_1st_d;
  double *vaccine_efficacy_1st_V;
  double *vaccine_efficacy_2nd_Ia;
  double *vaccine_efficacy_2nd_I;
  double *vaccine_efficacy_2nd_sd;
  double *vaccine_efficacy_2nd_d;
  double *vaccine_efficacy_2nd_V;
  int *vaccine_priority_1;
  int *vaccine_priority_2;
  int *vaccine_priority_3;
  int *death;
  int *county_priority;
  int *time_of_infection;
  int *no_of_infected;
  unsigned short int *hospitalized; /*hospitalization status*/
  unsigned int n_i; /*number of individuals*/
  unsigned short int *status; /*epidemic status*/
  short int *age; /*age type of individual*/
  unsigned int *cell;
  unsigned int *mun;
  unsigned int *county;
  double *x;
  double *y;
  int *R_community;
  int *imported_carrier;
  int *home_office;
  int *next_transition;
  int *stay_at_home;
  int *quarantine;
  double *age_sus;
  unsigned int n_home_office; // No. of people doing home office
  unsigned int n_working_who_can_do_ho; // No. of people who is currently working and could do home office
  double *prop_doing_home_office;
  double *age_prob_asymp;
  int *id_cluster; /* ID of the cluster in each place ( school/workplace) */

} Individuals;

typedef struct{

  int *** vaccination_list; // index of individuals according to the priority
  int **  element_vax_list; // index on the vector, vaccination_list
  unsigned int end_vaccination_list[11];
  int region_priority_N;                  // the number of targeted   individuals in regional prioritization
  int region_priority_counter;            // the number of vaccinated individuals in regional prioritization
  double region_priority_frac;            // the fraction of population in targeted counties
  int vaccination_region_N[3][356];       // targeted vaccination number in prioritized regions
  int vaccination_total_N[3];             // targeted vaccination number in Norway
  double vaccination_region_frac[3][356]; // the fraction of vaccination number in each county
  int half_vaccination[356]; // targeted   population size to stop regional prioritization
  int half_vaccinated[356];  // vaccinated population size to stop regional prioritization

} Vaccination;

typedef struct{

  unsigned int idx_i_ini; /*first idx of the individuals*/
  unsigned int idx_i_fin; /*last idx of the individuals*/
  unsigned int fertile; /*0: cannot have childrens*/
  unsigned int idx_c; /*individuals in the family*/
  unsigned int n_i; /*individuals in the family*/
  unsigned int *idx_i; /*idx of the individuals*/

} Household;

typedef struct{

  Household *household; /*the households*/
  unsigned int n_h; /*number of households*/

} Households;

typedef struct{

  unsigned int *idx_i;  /*idx of the individuals*/
  unsigned int *idx_hw; /*idx of hws*/
  unsigned int *idx_patient; /*idx of patients*/
  unsigned int *isolated_patients;
  unsigned int n_i; /*number of hws*/
  unsigned int patients;
  unsigned int n_PHW1; /*number of nurses	*/
  unsigned int n_PHW2; /*number of doctors*/
  unsigned int cat_n_PHW1; /*number of nurses, before assignment	*/
  unsigned int cat_n_PHW2; /*number of doctors, before assignment*/
  unsigned int hosp_id;
  unsigned int beds;
  unsigned int n_icu;
  unsigned int idx_c; /*idx of the cell*/
  int *ward_size;
  int *ward_type;
  int **wards_idx_patient;
  int **wards_idx_hw;
  int n_wards;
  int *wards_n_i;
  int *wards_patients;
  double *ward_beds_avail_dist;
  double x;
  double y;

} Hospital;

typedef struct{

  Hospital *hospital;
  unsigned int n_hosp; /*number of hospitals*/
  unsigned int n_tot_PHW1; /*number of nurses working in hospitals*/
  unsigned int n_tot_PHW2; /*number of doctors working in hospitals*/
  double **daily_hospitalization_by_age;
  double **mean_los_by_age;

} Hospitals;

typedef struct{

  unsigned int *idx_i; /* idx of the individuals */
  unsigned int n_i; /* number of individuals */
  unsigned int cat_n_i; /*number of individuals, before assignment*/
  unsigned short int type; /*type of employment (P0i or P1)*/
  unsigned int idx_c; /*idx of the cell*/
  short int cl_status; /*closure status: 0=open place (i.e. transmission can occur); 1+ =closed place (acts as a counter)*/
  double x; /* x location of the place */
  double y; /* y location of the place */
  unsigned int *school_str3; /*schools to be closed in case strategy3 is applied to the current place*/
  unsigned int n_school_str3; /*number of schools to be closed in case strategy3 is applied to the current place*/
  int n_clusters; // no. of clusters, or class rooms/departments, in each place
  int **cluster; // vector with indeces of people in each cluster;
  int *cluster_size; // no. of people in the cluster

} Place;

typedef struct{

  Place *place; /*the places*/
  unsigned int n_p; /*number of places*/
  int* P00_i_r;  // DS: think each of length number regions, and gives first and last indices of places types within that region
  int* P00_f_r;
  int* P01_i_r;
  int* P01_f_r;
  int* P02_i_r;
  int* P02_f_r;
  int* P03_i_r;
  int* P03_f_r;
  int* P04_i_r;
  int* P04_f_r;
  int* P1_i_r;
  int* P1_f_r;

} Places;

typedef struct{

  unsigned int c;   /*ID for cell name (numeric)*/
  unsigned int r;   /*ID for region name (numeric)*/
  unsigned int n_i; /*number of individuals within the cell*/
  unsigned int n_i_orig ; /*original number of individual within the cell*/
  unsigned int mun;    /*Governmental code for municipalities*/
  unsigned int mun_ID; /* sequential ID (1:N_MUN) for municipalities*/
  unsigned int county; /*ID for municipalities*/
  unsigned int county_ID; /*sequential ID (1:N_COUNTIES)  municipalities*/
  unsigned int idx_i_ini; /*first idx of the individuals*/
  unsigned int idx_i_fin; /*last idx of the individuals*/
  unsigned int idx_h_ini; /*first idx of the individuals*/
  unsigned int idx_h_fin; /*last idx of the individuals*/
  double x; /*x location of the cell*/
  double y; /*y location of the cell*/
  unsigned short int *dist; /*distances from other cells within the same region*/
  double nk;        /* for each cell c nk[c]=(\sum_d n_d k(c,d)) / (BETA_R*DELTAT) where d runs over the cells of th region cell c belongs to and k(c,d)=1/(1+(dist(c,d)/a)^b)*/
  unsigned int *bc; /* border cells for current cell*/
  unsigned short int *bc_dist; /*kernel of border cells*/
  unsigned int n_bc; /*number of border cells*/
  unsigned int *schools; /*schools belonging to the cell*/
  unsigned int n_schools; /*number of schools*/
  unsigned int *neighbors; /*cells that are <= param.RADIUS_STR3 km apart from the current cell*/
  unsigned int n_neighbors; /*number of neighbors*/
  int Hospital;

} Cell;

typedef struct{

  unsigned int idx_c_ini; /* first idx of the cells*/
  unsigned int idx_c_fin; /* last idx of the cells*/
  unsigned int r; /* region name (numeric)*/
  unsigned int n_i; /* number of individuals*/
  double x; /* longitude of the capital*/
  double y; /* latitude of the capital*/
  double gdpppp; /*GDP*/
  unsigned int exteut; /* passengers from exteu*/
  double hh2prop; /*probability for an dult of being alone with 1 children or with another adult */
  double hh3prop; /*probability for an dult of being alone with 2 childrens or with another adult and 1 children*/
  double hh4prop; /*probability for an dult of being alone with 3 childrens or with another adult and 2 childrens*/

} Region;

typedef struct{

  int *distance; // max distance travelled (Km), range 0-max_distance
  int max_distance; // maximum distance value in the dataset
  double **proportion; // Proportion of people who travelled a certain distance in a specific date

} Mobility;

typedef struct{

  int *municipality; /* municipality where the cases are reported (sequential ID, see mun_ID in Cell) 	*/
  int *county;       /* county where the cases are reported (sequential ID, see county_ID in Cell) 	*/
  int *time;         /* time */
  int *n;            /* number of cases */
  int n_rows_file;   /* no. of rows in the seeding file, defined in read_seeding_data() */
  int max_seed_time; /* last day of seeding, defined in read_seeding_data() */

} Seeding;

typedef struct{
  double *prop_risk_groups; /* proporion of people in risk groups 9 age groups: [0,9], [10,19], ..., [80+]*/
  double *hosp; /* probability of hospitalization by age group */
  double *icu;              /* probability of icu admission by age group for hospitalized patients */
  int n_age_groups;      /* no. of age groups for the probability vector, defined in read_seeding_data() */
  double **contact_matrix;

} Probability;

typedef struct{

  unsigned int n_i;         /* number of individuals */
  unsigned int n_cells;     /* number of cells */
  unsigned int *individuals; /* Individuals belonging the the municipality */
  unsigned int *cells;       /* Cells belonging the the municipality */

} Municipality;

typedef struct{

  unsigned int n_i;         /* number of individuals */
  unsigned int n_cells;     /* number of cells */
  unsigned int *individuals; /* Individuals belonging the the municipality */
  unsigned int *cells;       /* Cells belonging the the municipality */
  unsigned int prioritized;

} County;

typedef struct{

  int n_mun;           /* number of municipalities in the area */
  int *municipalities; /* municipalities belonging to the area */

} Area;

typedef struct{

  unsigned int n_c; /*number of cells*/
  unsigned int n_mun; /*number of municipalities*/
  unsigned int n_county; /*number of counties*/
  Cell *cell; /*the cells*/
  unsigned int n_r; /*number of regions*/
  Municipality *municipality; /*municipalities */
  County *county; /*counties*/
  Region *region; /*the regions*/
  double **dist; /*distances among capitals, to the power of RHO (gravity)*/
  unsigned int n_area; /*number of areas*/
  Area *area; /*areas to group municipalities*/

} Grid;

typedef struct{

  /* H household, P0 school, P1 workplace, A asymptomatic, S symptomatic */
  double **H_P;    /* 1.-exp( -param.RELATIVE_INF_PRES*param.BETA_H*param.DELTAT/i ) where i is the number of individuals*/
  double **H_A;    /* 1.-exp( -param.RELATIVE_INF_AS*BETA_H*DELTAT/i ) where i is the number of individuals*/
  double **H_S;    /* 1.-exp( -BETA_H*DELTAT/i ) where i is the number of individuals*/
  double **P0_P;   /* 1.-exp( -param.RELATIVE_INF_PRES*param.BETA_P0*param.DELTAT/i ) where i is the number of individuals*/
  double **P0_A;   /* 1.-exp( -param.RELATIVE_INF_AS*BETA_P0*DELTAT/i ) where i is the number of individuals*/
  double **P0_S;   /* 1.-exp( -BETA_P0*DELTAT/i ) where i is the number of individuals*/
  double **P1_P;   /* 1.-exp( -param.RELATIVE_INF_PRES*param.BETA_P1*param.DELTAT/i ) where i is the number of individuals*/
  double **P1_A;   /* 1.-exp( -param.RELATIVE_INF_AS*BETA_P1*DELTAT/i ) where i is the number of individuals*/
  double **P1_S;   /* 1.-exp( -BETA_P1*DELTAT/i ) where i is the number of individuals*/

} Prob_trans;

typedef struct{

  double **time_to_hosp;
  double **time_to_ICU;
  double **LOS_hosp;
  double **LOS_hosp_ICU;
  double *ICU_admission; /*Age-dependent Probs for ICU admission change in 3 different windows 1) Start - 1 April, 2) 1 April - 30 April, 3) 30 April - End  */
  double **hosp_admission;
  double **LOS_ICU;

} Hosp_param;

typedef struct{

  unsigned int *I_c;          /* prevalence symptomatic in the cell */
  unsigned int *I_r;          /* prevalence symptomatic in the region */
  unsigned int *Ia_c;         /* prevalence asymptomatic in the cell */
  unsigned int *Ia_r;         /* prevalence asymptomatic in the region */
  unsigned int *E2_c;         /* prevalence pre-symptomatic in the cell */
  unsigned int *E2_r; 		    /* prevalence pre-symptomatic in the region */
  unsigned int *inc_I_r;      /* incidence symptomatic in the region */
  unsigned int *inc_Ia_r;     /* incidence asymptomatic in the region */
  unsigned int *inc_E2_r; 	  /* incidence pre-symptomatic in the region */
  unsigned int *cum_inc_I_r;  /* cumulative incidence symptomatic in the region */
  unsigned int *cum_inc_Ia_r; /* cumulative incidence asymptomatic in the region */
  unsigned int *cum_inc_E2_r; /* cumulative incidence pre-symptomatic in the region */
  unsigned int *cum_rec_r;
  unsigned int *n_hosp;     /* no. of people hospitalized */
  unsigned int *n_icu;      /* no. of people in ICU */
  unsigned int *hosp_inc;   /* hospital incidence by 10 yr age groups */
  unsigned int *inc_hosp;   /* incidence of people admitted to hospitals */
  unsigned int *inc_icu;    /* incidence of people admitted to ICUs */
  unsigned int *inf_mun;
  unsigned int **inf_mun_age;
  unsigned int *sus_r;
  unsigned int *sus_c;
  unsigned int *rec_r;
  unsigned int *rec_c;
  unsigned int vaccinated_1;
  unsigned int vaccinated_2;
  unsigned int vaccinated_3;
  unsigned int quarantined;
  unsigned int contact_tracing;
  unsigned int household_screening;
  unsigned int test_house;
  unsigned int test_place;
  unsigned int *quarantined_c;

  unsigned int inf_community;
  unsigned int inf_household;
  unsigned int inf_school;
  unsigned int inf_work;
  unsigned int inf_uni;

  double susceptibles;
  double susceptibles_community;
  double susceptibles_households;
  double susceptibles_schools;
  double susceptibles_workplaces;

  // cumulative incidence by Age
  unsigned int cum_I[101];
  unsigned int cum_H[101];
  unsigned int cum_ICU[101];
  unsigned int cum_D[101];
  unsigned int cum_V1[101];
  unsigned int cum_V2[101];
  unsigned int cum_V3[101];
  // prevalence by Age
  unsigned int prev_H[101];
  unsigned int prev_ICU[101];

  // cumulative incidence by county
  int cum_I_county[11];
  int cum_H_county[11];
  int cum_ICU_county[11];
  int cum_D_county[11];
  int cum_V1_county[11];
  int cum_V2_county[11];
  int cum_V3_county[11];
  int prev_H_county[11];
  // cumulative incidence by municipality
  int cum_I_mun[356];
  int cum_H_mun[356];
  int cum_ICU_mun[356];
  int cum_D_mun[356];
  int cum_V1_mun[356];
  int cum_V2_mun[356];
  int cum_V3_mun[356];
  int prev_H_mun[356];
  // cumulative incidence by cell
  int cum_I_cell[13521];
  int cum_H_cell[13521];
  int cum_ICU_cell[13521];
  int cum_D_cell[13521];
  int cum_V1_cell[13521];
  int cum_V2_cell[13521];
  int cum_V3_cell[13521];
  int prev_H_cell[13521];
  //
  unsigned int ppl_to_vaccinate[11];
  double prop_reg_vax[11];
  double prop_nat_vax;
  double pp_to_vax_nat;
  double pp_to_vax_reg;

} Counter;

typedef struct {

  // ------------------- input from scenarios_N.txt
  char NAME_VAX_PRIORITY_SCEN_1[100];
  char NAME_VAX_PRIORITY_SCEN_2[100];
  char NAME_VAX_PRIORITY_SCEN_3[100];
  char NAME_VAX_PROFILE_SCEN[100];
  char NAME_ADHERENCE_SCEN[100];
  char NAME_DOSES_SCEN[100];
  char NAME_GEO[100];
  char NAME_IMPORT_SCEN[100];
  char NAME_CONTROL_SCEN[100];
  char NAME_REOPEN_SCEN[100];
  char NAME_SCENARIO[100];
  char PRIORITIZATION_1[100];
  char PRIORITIZATION_2[100];
  char PRIORITIZATION_3[100];
  char PROFILE[100];
  char P_TRANS[100]; // transmissibility of vaccinated individuals
  char ADHERENCE[100];
  char DELAY_SCEN[100];
  char DOSES_SCEN[100]; // for the number of doses each month
  double REGIONAL;
  int REGIONAL_PRIORITY;
  int GEO;
  int REGIONAL_START;      // start day of regional prioritization
  int VIKEN;            // regional prioritization (0 = Oslo only; 1 = Oslo & Viken)
  int REOPEN_DAY;      // Day of reopening
  double REOPEN_PERCENT;
  int KEEP_R_CONSTANT;
  char IMPORT[100];         // level of imported cases
  double REPROD_NUM;  // R_eff at time 0

  int **DOSES_DAY; // the number of doses each day, to be read directly from file
  int import_daily[400]; // daily imported cases
  double SEASON_DAILY[365];  // amplitude of seasonality

  char VAX_RATE[100]; // constant or dynamic (increase in the monthly doses)
  char GEOGRAPHIC[100];
  int SEASONALITY; // amplitude of cosine function

  int CONTROL_H_REGION;       // 0 = national; 1 = regional;
  int CONTROL_H[3];           // threshold of prev_H
  double CONTROL_H_Rt[3+1];   // Rt between threshold of prev_H
  int CONTROL_SWITCH;    // 1 = keep close; 0 = reopen
  int CONTROL_FULL_OPEN;    // 0 = not yet fully reopen; 1 = fully reopened
  int CONTROL_FULL_OPEN_t;    // time to fully reopen
  int *PREV_H_AREA_MIN;
  int *PREV_H_AREA_MID;
  int *PREV_H_AREA_MAX;
  int PREV_H_AREA_MIN_TOTAL;
  int PREV_H_AREA_MID_TOTAL;
  int PREV_H_AREA_MAX_TOTAL;

  double CONTROL_Rt_MIN; // threshold of Rt
  double CONTROL_Rt_MAX; // threshold of Rt
  int CONTROL_Rt_SWITCH; // 0 = open; 1 = close;
  double Rt_lag;

  double SUS_0_9;
  double SUS_10_19;
  double SUS_20_29;
  double SUS_30_39;
  double SUS_40_49;
  double SUS_50_59;
  double SUS_60_69;
  double SUS_70_79;
  double SUS_80p;
  double PROB_ASYMP;

  double RELATIVE_INF_PRES;
  double RELATIVE_INF_AS;

  double RELATIVE_COMM_CONTACTS_0_9;
  double RELATIVE_COMM_CONTACTS_10_19;
  double RELATIVE_COMM_CONTACTS_20_29;
  double RELATIVE_COMM_CONTACTS_30_39;
  double RELATIVE_COMM_CONTACTS_40_49;
  double RELATIVE_COMM_CONTACTS_50_59;
  double RELATIVE_COMM_CONTACTS_60_69;
  double RELATIVE_COMM_CONTACTS_70_79;
  double RELATIVE_COMM_CONTACTS_80p;

  double FRACTION_RG[9];

  double PROB_HOSP_0_9;
  double PROB_HOSP_10_19;
  double PROB_HOSP_20_29;
  double PROB_HOSP_30_39;
  double PROB_HOSP_40_49;
  double PROB_HOSP_50_59;
  double PROB_HOSP_60_69;
  double PROB_HOSP_70_79;
  double PROB_HOSP_80p;
  double PROB_HOSP_RG_0_9;
  double PROB_HOSP_RG_10_19;
  double PROB_HOSP_RG_20_29;
  double PROB_HOSP_RG_30_39;
  double PROB_HOSP_RG_40_49;
  double PROB_HOSP_RG_50_59;
  double PROB_HOSP_RG_60_69;
  double PROB_HOSP_RG_70_79;
  double PROB_HOSP_RG_80p;
  double PROB_DEATH_0_9;
  double PROB_DEATH_10_19;
  double PROB_DEATH_20_29;
  double PROB_DEATH_30_39;
  double PROB_DEATH_40_49;
  double PROB_DEATH_50_59;
  double PROB_DEATH_60_69;
  double PROB_DEATH_70_79;
  double PROB_DEATH_80p;
  double PROB_DEATH_RG_0_9;
  double PROB_DEATH_RG_10_19;
  double PROB_DEATH_RG_20_29;
  double PROB_DEATH_RG_30_39;
  double PROB_DEATH_RG_40_49;
  double PROB_DEATH_RG_50_59;
  double PROB_DEATH_RG_60_69;
  double PROB_DEATH_RG_70_79;
  double PROB_DEATH_RG_80p;

  double TIME_FROM_I_TO_DEATH;
  double TIME_FROM_H_TO_DEATH;
  double TIME_FROM_ICU_TO_DEATH;

  double ONSET_TO_HOSP_PROB_0_9;
  double ONSET_TO_HOSP_SIZE_0_9;
  double ONSET_TO_HOSP_PROB_10_19;
  double ONSET_TO_HOSP_SIZE_10_19;
  double ONSET_TO_HOSP_PROB_20_29;
  double ONSET_TO_HOSP_SIZE_20_29;
  double ONSET_TO_HOSP_PROB_30_39;
  double ONSET_TO_HOSP_SIZE_30_39;
  double ONSET_TO_HOSP_PROB_40_49;
  double ONSET_TO_HOSP_SIZE_40_49;
  double ONSET_TO_HOSP_PROB_50_59;
  double ONSET_TO_HOSP_SIZE_50_59;
  double ONSET_TO_HOSP_PROB_60_69;
  double ONSET_TO_HOSP_SIZE_60_69;
  double ONSET_TO_HOSP_PROB_70_79;
  double ONSET_TO_HOSP_SIZE_70_79;
  double ONSET_TO_HOSP_PROB_80_89;
  double ONSET_TO_HOSP_SIZE_80_89;
  double ONSET_TO_HOSP_PROB_90p;
  double ONSET_TO_HOSP_SIZE_90p;
  double LOS_HOSP_PROB_0_9;
  double LOS_HOSP_SIZE_0_9;
  double LOS_HOSP_PROB_10_19;
  double LOS_HOSP_SIZE_10_19;
  double LOS_HOSP_PROB_20_29;
  double LOS_HOSP_SIZE_20_29;
  double LOS_HOSP_PROB_30_39;
  double LOS_HOSP_SIZE_30_39;
  double LOS_HOSP_PROB_40_49;
  double LOS_HOSP_SIZE_40_49;
  double LOS_HOSP_PROB_50_59;
  double LOS_HOSP_SIZE_50_59;
  double LOS_HOSP_PROB_60_69;
  double LOS_HOSP_SIZE_60_69;
  double LOS_HOSP_PROB_70_79;
  double LOS_HOSP_SIZE_70_79;
  double LOS_HOSP_PROB_80_89;
  double LOS_HOSP_SIZE_80_89;
  double LOS_HOSP_PROB_90p;
  double LOS_HOSP_SIZE_90p;
  double PROB_ICU_0_9;
  double PROB_ICU_10_19;
  double PROB_ICU_20_29;
  double PROB_ICU_30_39;
  double PROB_ICU_40_49;
  double PROB_ICU_50_59;
  double PROB_ICU_60_69;
  double PROB_ICU_70_79;
  double PROB_ICU_80p;

  double TIME_TO_ICU_PROB_0_9;
  double TIME_TO_ICU_SIZE_0_9;
  double TIME_TO_ICU_PROB_10_19;
  double TIME_TO_ICU_SIZE_10_19;
  double TIME_TO_ICU_PROB_20_29;
  double TIME_TO_ICU_SIZE_20_29;
  double TIME_TO_ICU_PROB_30_39;
  double TIME_TO_ICU_SIZE_30_39;
  double TIME_TO_ICU_PROB_40_49;
  double TIME_TO_ICU_SIZE_40_49;
  double TIME_TO_ICU_PROB_50_59;
  double TIME_TO_ICU_SIZE_50_59;
  double TIME_TO_ICU_PROB_60_69;
  double TIME_TO_ICU_SIZE_60_69;
  double TIME_TO_ICU_PROB_70_79;
  double TIME_TO_ICU_SIZE_70_79;
  double TIME_TO_ICU_PROB_80p;
  double TIME_TO_ICU_SIZE_80p;

  double LOS_ICU_PROB_0_9;
  double LOS_ICU_SIZE_0_9;
  double LOS_ICU_PROB_10_19;
  double LOS_ICU_SIZE_10_19;
  double LOS_ICU_PROB_20_29;
  double LOS_ICU_SIZE_20_29;
  double LOS_ICU_PROB_30_39;
  double LOS_ICU_SIZE_30_39;
  double LOS_ICU_PROB_40_49;
  double LOS_ICU_SIZE_40_49;
  double LOS_ICU_PROB_50_59;
  double LOS_ICU_SIZE_50_59;
  double LOS_ICU_PROB_60_69;
  double LOS_ICU_SIZE_60_69;
  double LOS_ICU_PROB_70_79;
  double LOS_ICU_SIZE_70_79;
  double LOS_ICU_PROB_80p;
  double LOS_ICU_SIZE_80p;

  double LOS_HOSP_ICU_PROB_0_9;
  double LOS_HOSP_ICU_SIZE_0_9;
  double LOS_HOSP_ICU_PROB_10_19;
  double LOS_HOSP_ICU_SIZE_10_19;
  double LOS_HOSP_ICU_PROB_20_29;
  double LOS_HOSP_ICU_SIZE_20_29;
  double LOS_HOSP_ICU_PROB_30_39;
  double LOS_HOSP_ICU_SIZE_30_39;
  double LOS_HOSP_ICU_PROB_40_49;
  double LOS_HOSP_ICU_SIZE_40_49;
  double LOS_HOSP_ICU_PROB_50_59;
  double LOS_HOSP_ICU_SIZE_50_59;
  double LOS_HOSP_ICU_PROB_60_69;
  double LOS_HOSP_ICU_SIZE_60_69;
  double LOS_HOSP_ICU_PROB_70_79;
  double LOS_HOSP_ICU_SIZE_70_79;
  double LOS_HOSP_ICU_PROB_80p;
  double LOS_HOSP_ICU_SIZE_80p;

  double N_HWS[9]; // [0] = 0-9; [1] = 10-19; ...; [8] = 80+

  double RELATIVE_TRANS_MUN[356];     // 356 municipalities in Norway
  double RELATIVE_TRANS_MUN_t0[356];  // 356 municipalities in Norway at time 0
  double scale_p; // scaling factor of reopening & seasonality

  int COUNTY_PRIORITY[356]; // for each county 0 or 1
  int mun_priority[356];    // index (-1, 0, 1) of regional prioritization each municipality/cell
  double RELATIVE_TRANS_MUN_OPEN[356]; //            scaling factors of 356 municipalities at reopening with beta(R=4.5)
  double RELATIVE_TRANS_MUN_MAX[356];  // maximum of scaling factors of 356 municipalities at reopening with beta(R=1.0)

  int ELDERLY_65p; // {0,1} - 0: vaccin
  int RISK;
  int HEALTH;

  double BETA_S;   /*transmission rate for long distance foreign travels*/
  double BETA_R;   /*transmission parameter for community transmission*/
  double BETA_H;   /*transmission parameter for household transmission*/
  double BETA_P00; /*transmission parameter for primary school transmission*/
  double BETA_P0;  /*transmission parameter for secondary school transmission*/
  double BETA_P1;  /*transmission parameter for workplace transmission*/

  double BETA_S_CAL;   /*transmission rate for long distance foreign travels*/
  double BETA_H_CAL;   /*transmission parameter for household transmission*/
  double BETA_P00_CAL; /*transmission parameter for primary school transmission*/
  double BETA_P0_CAL;  /*transmission parameter for secondary school transmission*/
  double BETA_P1_CAL;  /*transmission parameter for workplace transmission*/
  double BETA_R_CAL;   /*transmission parameter for community transmission*/

  int CPT; // index of change point
  double BETA_CPT[2]; // beta at change points

  double TAU_T; /*gdp gravity model*/
  double TAU_F; /*gdp gravity model*/
  double RHO; /*gdp gravity model*/
  double G0; /*barabasi kernel*/
  double BETA; /*barabasi kernel*/
  double KAPPA; /*barabasi kernel*/
  int MAXDIST_P01; /*max distance for primary schools*/
  int MAXDIST_P03; /*max distance for secondary schools*/
  int MAXDIST_P1; /*max distance for considering all workplaces. At larger distances, workplaces are sampled*/
  double PROB_SAMPLING_WP; /*Probability of sampling workplaces at distances larger than  MAXDIST_P1*/
  double PROB_WP; /*sampled workplaces have higher probability*/
  int BC_DIST; /*max distance for short distance foreign travels*/
  int MAX_COMM_DIST; /*max distance for internal commuting*/
  double DELTAT; /*time step of the simulation*/

  int YEAR;  // 2021 = 1
  int MONTH; // JAN  = 0
  int DAY;   // 1st  = 1 (but t=1 day++)

  int MAXT ; /*max number of simulation steps (MAXT*DELTAT)=max time of the simulation*/
  int DAY_STR2_INI; /*day (since February 17) at which strategy 2 is adopted*/
  int DAY_STR2_FIN; /*day (since February 17) at which strategy 2 is stopped*/
  int CLOSESCHOOL;
  double RADIUS_STR3; /*radius in which strategy 3 is implemented*/
  double XDIM_CELLS; /*x-dimension of cells (assumed to be constant for all regions)*/
  double YDIM_CELLS; /*y-dimension of cells  (assumed to be constant for all regions)*/
  int DAY_STR2_1;
  int DAY_STR2_2;
  int DAY_START_SUMMER_HOL;
  int DAY_END_SUMMER_HOL;

  double PROP_HOME_OFFICE;
  double E1_GAMMA_ALPHA;
  double E1_GAMMA_BETA;
  double E1a_GAMMA_ALPHA;
  double E1a_GAMMA_BETA;
  double E2_GAMMA_ALPHA;
  double E2_GAMMA_BETA;
  double I_GAMMA_ALPHA;
  double I_GAMMA_BETA;
  double Ia_GAMMA_ALPHA;
  double Ia_GAMMA_BETA;
  double REL_SOC_DIST;
  double REL_SOC_DIST_2;
  double REL_SOC_DIST_W;

  double AMPL_FACTOR; // Amplification of the number of infections reported in Norway
  double REDUCE_SOC_DIST;
  double REDUCE_SOC_DIST_2;

  // vaccine parameters (vaccine_profile.txt)
  int VACCINE_DURATION;
  int VACCINE_SCENARIO;
  int DELAY_EFFECT[3];
  int DELAY_EFFECT_1_LONG;   // special: longer waiting time for 2nd dose of mRNA
  int DELAY_EFFECT_1_LONG_t; // special: time to start longer waiting time
  int RAMP_UP_TIME[2][3];
  double VACCINE_EFF_Ia[2][3];
  double VACCINE_EFF_I[2][3];
  double VACCINE_EFF_S[2][3];
  double VACCINE_EFF_D[2][3];
  double VACCINE_EFF_V[2][3];

  // vaccine_adherence.txt
  double WASTE;
  int DAY_START_VACCINATION;

  double ADHERENCE_1_0_11;
  double ADHERENCE_1_12_15;
  double ADHERENCE_1_16_17;
  double ADHERENCE_1_18_24;
  double ADHERENCE_1_25_39;
  double ADHERENCE_1_40_44;
  double ADHERENCE_1_45_54;
  double ADHERENCE_1_55_64;
  double ADHERENCE_1_65_74;
  double ADHERENCE_1_75_84;
  double ADHERENCE_1_85p;

  double ADHERENCE_2_0_11;
  double ADHERENCE_2_12_15;
  double ADHERENCE_2_16_17;
  double ADHERENCE_2_18_24;
  double ADHERENCE_2_25_39;
  double ADHERENCE_2_40_44;
  double ADHERENCE_2_45_54;
  double ADHERENCE_2_55_64;
  double ADHERENCE_2_65_74;
  double ADHERENCE_2_75_84;
  double ADHERENCE_2_85p;

  double ADHERENCE_3_0_11;
  double ADHERENCE_3_12_15;
  double ADHERENCE_3_16_17;
  double ADHERENCE_3_18_24;
  double ADHERENCE_3_25_39;
  double ADHERENCE_3_40_44;
  double ADHERENCE_3_45_54;
  double ADHERENCE_3_55_64;
  double ADHERENCE_3_65_74;
  double ADHERENCE_3_75_84;
  double ADHERENCE_3_85p;

  // vaccine_doses.txt
  int NR_VAX_DOSES_1;
  int NR_VAX_DOSES_2;
  int NR_VAX_DOSES_3;
  int DAILY_DOSES;

} Param;

typedef struct {

  int n_col;
  int n_row;
  double **matrix;

} Matrix;

enum enum_status{
  S=0,    /* susceptible */
  E1=3,   /* exposed */
  E1a=6,  /* exposed and asymptomatic */
  E2=9,   /* pre-symptomatic */
  I=12,   /* infected symptomatic */
  Ia=15,  /* infeced asymptomatic */
  R=18,   /* recovered */
  D=21    /* dead */
};

enum enum_employment{
  P00=2,  /*pre-primary school; age = [3,5]  */ // DS: their contacts treated like prim school
  P01=3,  /*primary school; age = [6,15]*/ // DS: their contacts treated like prim school
  P02=4,  /* ... */
  P03=5,  /*upper secondary school; age = [16,18]*/ // DS: their contacts treated like prim school
  P04=6,  /*higher education*/ // DS: their contacts treated like professionals
  P1=1,   /*work*/ // DS: their contacts treated like professionals
  P2=0,   /*retired*/
  PHW1_H=7,  /*healthcare workers in Hosp: nurses*/
  PHW2_H=8,  /*healthcare workers in Hosp: physicians*/
  PHW1_NH=9, /*healthcare workers in NH: nurses*/
  PHW2_NH=10 /*healthcare workers in NH: physicians*/
};

enum hospitalized{
  NH    = 1,  /*nursing homes*/
  H     = 2,  /*hospitals*/
  H_ICU = 3,  /*hospitalized in ICU*/
  NH_H  = 4,  /*nursinghome LT patient hospitalized*/
  NO    = 0   /*not hospitalized*/
};

enum ethnic_bkg{
  Norwegian=0, /*Norwegian*/
  Immigrant=1,  /*immigrant*/
};

#define PERFORM_CONTACT_TRACING 0
#define PERFORM_HOUSE_SCREENING 0
#define N_HS 6 // note! redefined below
#define N_HT 4
#define N_AC 21 /* For defining households grouped by 5 years*/
#define N_AC_REFINED 101 /*refinement of age classes when assigning places*/
#define AC_LENGTH 5
#define ISOLATION_EFFECT 0.01
#define P_NEGATIVE_BINOMIAL_LOS 0.1248656
#define SIZE_NEGATIVE_BINOMIAL_LOS 0.6217839
#define MIN_AGE_ADULTS 4
#define MIN_DIFF_AGE_CHILDREN_ADULTS 2
#define MAX_DIFF_AGE_CHILDREN_ADULTS 13
#define MAX_DIFF_AGE_ADULTS 4
#define N_MUN 356 /*number of municipalities*/
#define N_CELL 13521 // number of cells
#define N_COUNTIES 11 /*number of counties*/
#define R 6371 /*Earth's radius (Km)*/
#define TO_RAD (3.1415926536 / 180)

#undef N_HS
#define N_HS 7

enum enum_ward_type{
  nonICU=0, /*normal ward*/
  ICU=1, /*ICU */
};

enum enum_ht{
  A1=0, /*Single person*/
  A1_CH=1, /*Single parent with children*/
  CPL_NOCH=2, /*Couple without children*/
  CPL_CH=3 /*Couple with children*/
};

#define N_HTE 8
#define N_HAT 14

enum enum_HAT{
  AGE_A_A1=0,
  AGE_A_A1_CH_1=1,
  AGE_A_A1_CH_2=2,
  AGE_A_A1_CH_3=3,
  AGE_C_A1_CH_1=4,
  AGE_C_A1_CH_2=5,
  AGE_C_A1_CH_3=6,
  AGE_A_CPL_NOCH=7,
  AGE_A_CPL_CH_1=8,
  AGE_A_CPL_CH_2=9,
  AGE_A_CPL_CH_3=10,
  AGE_C_CPL_CH_1=11,
  AGE_C_CPL_CH_2=12,
  AGE_C_CPL_CH_3=13
};

enum enum_HTE{
  TYPE_A1=0,
  TYPE_A1_CH_1=1,
  TYPE_A1_CH_2=2,
  TYPE_A1_CH_3=3,
  TYPE_CPL_NOCH=4,
  TYPE_CPL_CH_1=5,
  TYPE_CPL_CH_2=6,
  TYPE_CPL_CH_3=7
};

/* number of age classes (0-2,3,4,19,20-24,..., 60-64, 65+) */
#define N_AGECLASS 28

/*number of workplaces (by size) */
#define N_WP 7

/*dimensions of workplaces */
#define WP_m1 5
#define WP_m2 10
#define WP_m3 25
#define WP_m4 50
#define WP_m5 100
#define WP_m6 200
#define WP_m7 500
#define WP_s1 5
#define WP_s2 15
#define WP_s3 25
#define WP_s4 50
#define WP_s5 100
#define WP_s6 300
#define WP_s7 501

/* max size of places*/
#define N_PS 50000

/* number of employment: P0i, P1 or P2*/
#define  N_EMPLOYMENT 7

/* Norwegian population based on the input file population*/
#define NORW_POP 4393341

/*geodetics*/
#define MAJOR_AXIS 6378137.0 /*heart radius*/
#define PI 3.14159265358979323846 /*greek pi*/
#define RESOLUTION 0.1 /*cell resolution*/
#define COEFF_DISTANCE 91.17066 /*0.819*PI*MAJOR_AXIS/180.0/1000.*/;

/*epidemic parameters*/
#define MAXSHORTINT 65000.0 /*coefficient to scale distance kernel values*/

int get_line(char **line,FILE *fp);
int parser(int n,char *array[],char ***flag,char ***value,int *nflags);
char *get_value(char *flag[],char *value[],int nflags,char opt[]);
int read_home_office_data(char file[],char sep, Individuals *individuals );
int read_contact_matrix(char file[],char sep, Probability *probability );
int read_prop_of_risk_groups(char file[],char sep, Probability *probability );
int read_matrix_v2( char file[], int ncol, char sep, Matrix *matrix );
int read_matrix( char file[], int ncol, char sep, Matrix *matrix );
int read_population_data(char file[],char sep, Grid *grid);
int read_mobility_data(char file[], char sep, Mobility *mobility, Grid *grid);
int read_area_data(char file[], char sep, Grid *grid);
int read_states_data(char file[],char sep,Grid *grid);
int read_param(char file[],char sep);
int read_param_2(char file[],char sep);
int read_adherence_param( char file[], char sep );
int read_daily_doses(char file[], char sep);
int read_municip_prior(char file[], char sep);
int read_importation(char file[], char sep);
int read_seasonality(char file[], char sep);
int read_control_prev_H(char file[], char sep);
int read_relative_reopen(char file[], char sep);
int read_time(char file[], char sep);
int read_scenario_names(char file[],char sep);
void print_vaccine_parameters();
void print_parameters();
void print_scenario_names();
int read_vaccine_param(char file[],char sep);
int read_import_param(char file[], char sep);
int read_county_vax_priority(char file[],char sep);
void read_hospitalization_rates( Grid *grid, Hospitals *hospitals);
void regions( Grid *grid );
void prob_transmission( Prob_trans *pt );
void kernel( Grid *grid );
void read_mortality( Grid *grid );
void contact_matrix(Grid *grid, Households *households, Individuals *individuals,Places *places);
int SEIR_model( Grid *grid, Households *households, Individuals *individuals,Places *places, Hospitals *hospitals, Prob_trans *pt, Counter *counter , Seeding *seeding, Mobility *mobility, Probability *probability );
int read_structures_from_file( Grid *grid, Households *households, Individuals *individuals, Places *places, Hospitals *hospitals, Prob_trans *pt ,int ni, Counter *counter, Seeding *seeding, Mobility *mobility, Probability *probability );
