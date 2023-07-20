#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_permutation.h>
#include <gsl/gsl_sort.h>
#include <gsl/gsl_sort_vector.h>
#include <sqlite3.h>
#include <math.h>
#include "header.h"
#include <time.h>

extern gsl_rng *R_GLOBAL;
extern char *EXP_DIR;
extern Param param;
extern int CURRENT_SIMULATION;
extern int VERBOSE;
extern FILE *fimp;

double haversine_dist(double th1, double ph1, double th2, double ph2);
int day_of_the_year( int day, int month, int idx_1st_month, int print );
static void mobility_dist(Grid *grid, Mobility *mobility, gsl_ran_discrete_t **dist);
static void define_hospitalization_param( Hosp_param *hosp_param );
static void hospitalization( unsigned int idx_i, Grid *grid, Individuals *individuals, Counter *counter, int t, FILE* fiww, Hosp_param *hosp_param);
static void next_trans_func( unsigned int idx_i, Individuals *individuals, int t, double alpha, double beta, int flag_neg);
static void define_age_susceptibility_and_prob_of_asymptomatic( Individuals *individuals, Param *param );
static void death( Individuals * individuals, unsigned int idx_i , int t );
static void h_transmission( unsigned int idx_i, unsigned int idx_h, Households *households, Individuals *individuals,Prob_trans *pt, unsigned int r, Counter *counter, int t,FILE * fiww);
static void p_transmission( Grid *grid, unsigned int idx_i, unsigned int idx_p, Places *places, Individuals *individuals, Prob_trans *pt, unsigned int r, Counter *counter, int t, FILE * fiww);
static void r_transmission_individual_level(unsigned int r,unsigned int c, Grid *grid,Individuals *individuals, Counter *counter, Probability *probability, int t, FILE *fiww, gsl_ran_discrete_t* dist_sub);
static void define_risk_groups(Grid *grid, Individuals *individuals, Households *households, Counter *counter, Matrix *matrix, Probability *probability , Vaccination *vaccination );
void correct_efficacy_param();
void vaccination_initialized(Individuals *individuals, Vaccination *vaccination, FILE *file, Grid *grid);
void vaccination_prioritization(Grid *grid, Individuals *individuals, Households *households, Counter *counter, Matrix *matrix, Probability *probability, Vaccination *vaccination);
void vaccination_strategy(FILE *file, Individuals *individuals, Households *households, Counter *counter, int doses_day[], int t, Vaccination *vaccination, Grid *grid, Matrix *matrix, Probability *probability);
void initialization( Grid *grid, Individuals *individuals, char file[], char sep, int t, FILE* fiww, Hosp_param *hosp_param , Counter *counter);
void print_initial_conditions( Individuals * individuals );
void free_prob_transmissions( Prob_trans *pt);
void seasonality( Prob_trans *pt, int t, int doy_start );
void importation(int t, Individuals *individuals, Grid *grid, Counter *counter, FILE* fiww, Hosp_param *hosp_param);

int SEIR_model( Grid *grid, Households *households, Individuals *individuals, Places *places, Hospitals *hospitals, Prob_trans *pt, Counter *counter, Seeding *seeding, Mobility *mobility, Probability *probability ){

  char *file_name;
  register int r, c, i;
  register int t;

  int time = 1;
  int ini_inf = 0;

  // start date
  int year  = param.YEAR  - 2020;  // index of year
  int month = param.MONTH - 1;     // index of month
  int day   = param.DAY   - 1;     // index of day in a month
  int days_of_the_month[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  int doy         = day_of_the_year( day, month, 0, 1 ); // NB: January 1 = 0
  int doy_start   = day_of_the_year( day, month, 0, 1 );

  Hosp_param hosp_param;
  Matrix vaccine_priority;

  Vaccination vaccination;

  int stop_vaccination = 0;

  // FILE
  FILE *fp1, *fiww, *fvax, *finf, *fplaceinf;
  //
  file_name = (char*) calloc(1000, sizeof(char));
  sprintf(file_name, "%s/output_files/national_cases.txt", EXP_DIR);
  fp1 = fopen(file_name, "w");
  fprintf(fp1, "time\t");
  fprintf(fp1, "inc_I\tprev_I\tprev_H\tprev_ICU\t");
  fprintf(fp1, "cum_I\tcum_H\tcum_ICU\tcum_D\tcum_V_1\tcum_V_2\tcum_V_3\t");
  //
  for(int age_n = 0; age_n < 101; age_n++) fprintf(fp1, "cum_I_%d\t",   age_n);
  for(int age_n = 0; age_n < 101; age_n++) fprintf(fp1, "cum_H_%d\t",   age_n);
  for(int age_n = 0; age_n < 101; age_n++) fprintf(fp1, "cum_ICU_%d\t", age_n);
  for(int age_n = 0; age_n < 101; age_n++) fprintf(fp1, "cum_D_%d\t",   age_n);
  for(int age_n = 0; age_n < 101; age_n++) fprintf(fp1, "cum_V_1_%d\t", age_n);
  for(int age_n = 0; age_n < 101; age_n++) fprintf(fp1, "cum_V_2_%d\t", age_n);
  for(int age_n = 0; age_n < 101; age_n++) fprintf(fp1, "cum_V_3_%d\t", age_n);
  for(int age_n = 0; age_n < 101; age_n++) fprintf(fp1, "prev_H_%d\t",  age_n);
  //
  for(int county_n = 0; county_n < N_COUNTIES; county_n++) fprintf(fp1, "cum_I_county_%d\t",   county_n); // 11 counties
  for(int county_n = 0; county_n < N_COUNTIES; county_n++) fprintf(fp1, "cum_H_county_%d\t",   county_n); // 11 counties
  for(int county_n = 0; county_n < N_COUNTIES; county_n++) fprintf(fp1, "cum_ICU_county_%d\t", county_n); // 11 counties
  for(int county_n = 0; county_n < N_COUNTIES; county_n++) fprintf(fp1, "cum_D_county_%d\t",   county_n); // 11 counties
  for(int county_n = 0; county_n < N_COUNTIES; county_n++) fprintf(fp1, "cum_V_1_county_%d\t", county_n); // 11 counties
  for(int county_n = 0; county_n < N_COUNTIES; county_n++) fprintf(fp1, "cum_V_2_county_%d\t", county_n); // 11 counties
  for(int county_n = 0; county_n < N_COUNTIES; county_n++) fprintf(fp1, "cum_V_3_county_%d\t", county_n); // 11 counties
  //
  for(int county_n = 0; county_n < N_COUNTIES; county_n++) fprintf(fp1, "cum_I_county01_%d\t",   county_n); // 11 counties
  for(int county_n = 0; county_n < N_COUNTIES; county_n++) fprintf(fp1, "cum_H_county01_%d\t",   county_n); // 11 counties
  for(int county_n = 0; county_n < N_COUNTIES; county_n++) fprintf(fp1, "cum_ICU_county01_%d\t", county_n); // 11 counties
  for(int county_n = 0; county_n < N_COUNTIES; county_n++) fprintf(fp1, "cum_D_county01_%d\t",   county_n); // 11 counties
  for(int county_n = 0; county_n < N_COUNTIES; county_n++) fprintf(fp1, "cum_V_1_county01_%d\t", county_n); // 11 counties
  for(int county_n = 0; county_n < N_COUNTIES; county_n++) fprintf(fp1, "cum_V_2_county01_%d\t", county_n); // 11 counties
  for(int county_n = 0; county_n < N_COUNTIES; county_n++) fprintf(fp1, "cum_V_3_county01_%d\t", county_n); // 11 counties
  //
  for(int mun_n = 0; mun_n < N_MUN; mun_n++) fprintf(fp1, "cum_I_mun_%d\t",   mun_n); // 356 municipalities
  for(int mun_n = 0; mun_n < N_MUN; mun_n++) fprintf(fp1, "cum_H_mun_%d\t",   mun_n); // 356 municipalities
  for(int mun_n = 0; mun_n < N_MUN; mun_n++) fprintf(fp1, "cum_ICU_mun_%d\t", mun_n); // 356 municipalities
  for(int mun_n = 0; mun_n < N_MUN; mun_n++) fprintf(fp1, "cum_D_mun_%d\t",   mun_n); // 356 municipalities
  for(int mun_n = 0; mun_n < N_MUN; mun_n++) fprintf(fp1, "cum_V_1_mun_%d\t", mun_n); // 356 municipalities
  for(int mun_n = 0; mun_n < N_MUN; mun_n++) fprintf(fp1, "cum_V_2_mun_%d\t", mun_n); // 356 municipalities
  for(int mun_n = 0; mun_n < N_MUN; mun_n++) fprintf(fp1, "cum_V_3_mun_%d\t", mun_n); // 356 municipalities
  for(int mun_n = 0; mun_n < N_MUN; mun_n++) fprintf(fp1, "relative_R_mun_%d\t", mun_n); // 356 municipalities
  fprintf(fp1, "beta_H\t");
  fprintf(fp1, "Rt_lag\t");
  //
  fprintf(fp1, "scenario\tmodel");
  fprintf(fp1,"\n");
  free(file_name);

  if( VERBOSE != -1 ){
    file_name = (char*) calloc(1000, sizeof(char));
    sprintf(file_name, "%s/output_files/individ_where_when.txt", EXP_DIR);
    fiww = fopen(file_name,"w");
    fprintf(fiww, "time\tto_id\twhere\tSymp_or_not\tplace_id\tfrom_id\tfrom_id_cell\tfrom_id_muni\tto_id_cell\tfrom_id_age\tfrom_id_house\tto_id_house\ttype_place\tto_id_status\tto_id_age\tfrom_id_status\tfrom_id_region\tfrom_id_x\tfrom_id_y\tfrom_V_status\tto_V_status\n");
    free(file_name);
  }

  file_name = (char*) calloc(1000, sizeof(char));
  sprintf(file_name, "%s/output_files/imported_infections.txt", EXP_DIR);
  fimp = fopen(file_name,"w");
  fprintf(fimp, "time\tid_ind\tcounty\tage\tstatus\n");
  free(file_name);

  if( VERBOSE > 5 ){
    file_name = (char*) calloc(1000, sizeof(char));
    sprintf(file_name, "%s/output_files/vaccinations.txt", EXP_DIR);
    fvax = fopen(file_name,"w");
    fprintf(fvax, "time\tcounty\tage\tvaccine\tid\n");
    free(file_name);
  }

  file_name = (char*) calloc(1000, sizeof(char));
  sprintf(file_name, "%s/output_files/Rt.txt", EXP_DIR);
  finf = fopen(file_name,"w");
  fprintf(finf, "time\tid_ind\tN_inf\tcounty\tage\tV_status\n");
  free(file_name);

  file_name = (char*) calloc(1000, sizeof(char));
  sprintf(file_name, "%s/output_files/place_of_infection.txt", EXP_DIR);
  fplaceinf = fopen(file_name,"w");
  fprintf(fplaceinf, "time\tcommunity\twork\tschool\tuni\thouse\n"); // \tregion\n");
  free(file_name);

  individuals->n_home_office = 0;
  individuals->n_working_who_can_do_ho = 0;

  // Define some counters (each region = Norway; and each cell)
  counter->I_r           = ( unsigned int* ) calloc( grid->n_r+1, sizeof(unsigned int) );
  counter->cum_inc_I_r   = ( unsigned int* ) calloc( grid->n_r+1, sizeof(unsigned int) );
  counter->I_c           = ( unsigned int* ) calloc( grid->n_c+1, sizeof(unsigned int) );
  counter->inc_I_r       = ( unsigned int* ) calloc( grid->n_r+1, sizeof(unsigned int) );

  counter->Ia_r          = ( unsigned int* ) calloc( grid->n_r+1, sizeof(unsigned int) );
  counter->cum_inc_Ia_r  = ( unsigned int* ) calloc( grid->n_r+1, sizeof(unsigned int) );
  counter->Ia_c          = ( unsigned int* ) calloc( grid->n_c+1, sizeof(unsigned int) );
  counter->inc_Ia_r      = ( unsigned int* ) calloc( grid->n_r+1, sizeof(unsigned int) );

  counter->E2_r          = ( unsigned int* ) calloc( grid->n_r+1, sizeof(unsigned int) );
  counter->cum_inc_E2_r  = ( unsigned int* ) calloc( grid->n_r+1, sizeof(unsigned int) );
  counter->E2_c          = ( unsigned int* ) calloc( grid->n_c+1, sizeof(unsigned int) );
  counter->inc_E2_r	     = ( unsigned int* ) calloc( grid->n_r+1, sizeof(unsigned int) );

  counter->n_hosp        = ( unsigned int* ) calloc( grid->n_r+1, sizeof(unsigned int) );
  counter->n_icu         = ( unsigned int* ) calloc( grid->n_r+1, sizeof(unsigned int) );
  counter->inc_hosp      = ( unsigned int* ) calloc( grid->n_r+1, sizeof(unsigned int) );
  counter->inc_icu       = ( unsigned int* ) calloc( grid->n_r+1, sizeof(unsigned int) );

  counter->sus_r         = ( unsigned int* ) calloc( grid->n_r+1, sizeof(unsigned int) );
  counter->sus_c         = ( unsigned int* ) calloc( grid->n_c+1, sizeof(unsigned int) );

  counter->rec_r         = ( unsigned int* ) calloc( grid->n_r+1, sizeof(unsigned int) );
  counter->cum_rec_r     = ( unsigned int* ) calloc( grid->n_r+1, sizeof(unsigned int) );
  counter->rec_c         = ( unsigned int* ) calloc( grid->n_c+1, sizeof(unsigned int) );

  counter->quarantined_c = ( unsigned int* ) calloc( grid->n_c+1, sizeof(unsigned int) );

  counter->hosp_inc      = ( unsigned int* ) calloc( 11         , sizeof(unsigned int) );

  counter->susceptibles = 0;
  counter->vaccinated_1 = 0; // vaccine 1
  counter->vaccinated_2 = 0; // vaccine 2
  counter->vaccinated_3 = 0; // vaccine 3

  // Counters for 11 age groups
  for( int age_n = 0; age_n < 101; age_n++ ){
    // cumulative
    counter->cum_I[age_n]   = 0;
    counter->cum_H[age_n]   = 0;
    counter->cum_ICU[age_n] = 0;
    counter->cum_D[age_n]   = 0;
    counter->cum_V1[age_n] = 0;
    counter->cum_V2[age_n] = 0;
    counter->cum_V3[age_n] = 0;
    //prevalence
    counter->prev_H[age_n]   = 0;
    counter->prev_ICU[age_n] = 0;
  }

  // Counters for 11 counties
  for( int j = 0; j < N_COUNTIES; j++ ){
    counter->cum_I_county[j]   = 0;
    counter->cum_H_county[j]   = 0;
    counter->cum_ICU_county[j] = 0;
    counter->cum_D_county[j]   = 0;
    counter->cum_V1_county[j] = 0;
    counter->cum_V2_county[j] = 0;
    counter->cum_V3_county[j] = 0;
    counter->prev_H_county[j] = 0;
  }

  // Counters for 356 municipalities
  for( int j = 0; j < N_MUN; j++ ){
    counter->cum_I_mun[j]   = 0;
    counter->cum_H_mun[j]   = 0;
    counter->cum_ICU_mun[j] = 0;
    counter->cum_D_mun[j]   = 0;
    counter->cum_V1_mun[j] = 0;
    counter->cum_V2_mun[j] = 0;
    counter->cum_V3_mun[j] = 0;
    counter->prev_H_mun[j] = 0;
  }

  // Counters for 4978 cells
  for( int j = 0; j < grid->n_c; j++ ){
    counter->cum_I_cell[j]   = 0;
    counter->cum_H_cell[j]   = 0;
    counter->cum_ICU_cell[j] = 0;
    counter->cum_D_cell[j]   = 0;
    counter->cum_V1_cell[j] = 0;
    counter->cum_V2_cell[j] = 0;
    counter->cum_V3_cell[j] = 0;
    counter->prev_H_cell[j] = 0;
  }

  counter->inf_community = 0;
  counter->inf_household = 0;
  counter->inf_school    = 0;
  counter->inf_work      = 0;
  counter->inf_uni       = 0;

  // Define risk groups
  fprintf(stderr, "\n");
  fprintf(stderr, "Define risk groups ...");
  define_risk_groups(grid, individuals, households, counter, &vaccine_priority, probability, &vaccination);
  fprintf(stderr, "Define risk groups ... done\n");

  // Define Hospitalization parameters
  fprintf(stderr, "\n");
  fprintf(stderr,"Define Hosp. parameters ...");
  define_hospitalization_param( &hosp_param );
  fprintf(stderr,"Define Hosp. parameters ... done\n");

  // Define age susceptibility
  fprintf(stderr, "\n");
  fprintf(stderr, "Define age susceptibility ...");
  define_age_susceptibility_and_prob_of_asymptomatic(individuals, &param);
  fprintf(stderr, "Define age susceptibility ... done\n");

  // Correction for efficacy parameters (param.VACCINE_EFF_SD: conditinoal VE for severe disease / hospitalization / death)
  correct_efficacy_param();

  // Initialize the number of infections
  fprintf(stderr, "\n");
  fprintf(stderr, "Initialize the model ");
  char initial_conditions_file[1000];
  sprintf( initial_conditions_file, "%s/input_files/epi_scenario_regions.txt", EXP_DIR );
  initialization( grid, individuals, initial_conditions_file, '\t', 0, fiww, &hosp_param, counter );
  fprintf(stderr, "Initialize the model ... done\n ");
  print_initial_conditions(individuals); // Print the numbers

  // Initialize probability matrix for Random transmission in the community
  gsl_ran_discrete_t **dist;
  dist  = (gsl_ran_discrete_t **) calloc( grid->n_c + 1, sizeof( gsl_ran_discrete_t * )); // matrix
  mobility_dist(grid, mobility, dist); // define mobility probability between cells in community tranmission

  // initialization of vaccinated individuals
  if( param.KEEP_R_CONSTANT >= 0 ){
    fprintf(stderr, "\n");
    fprintf(stderr, "vaccination_initialized() ");
    vaccination_initialized(individuals, &vaccination, fvax, grid);
    fprintf(stderr, "vaccination_initialized() ... done\n");
    // assigning prioritization of each individual
    vaccination_prioritization(grid, individuals, households, counter, &vaccine_priority, probability, &vaccination);
  }

  // read baseline for alternative vaccination strategies ######################
  // Define daily doses
  if( strcmp( param.PRIORITIZATION_1, "Base" ) != 0 ){ // non-baseline
    for(int day = 0; day < param.MAXT; day++){

      // total doses for vaccine 1, 2, 3
      param.NR_VAX_DOSES_1 += param.DOSES_DAY[day][0];
      param.NR_VAX_DOSES_2 += param.DOSES_DAY[day][1];
      param.NR_VAX_DOSES_3 += param.DOSES_DAY[day][2];

    }
  } // only non-baseline

  // Loop over time ############################################################
  for( t = 1; t <= param.MAXT; t++ ){

    // Define Month and Day ####################################################

    day++;
    if( day == ( days_of_the_month[month] + 1 ) ){ // day = 1 for 1st
      day = 1;
      month++;
      if( month == 12 ){ // month = 0 for Jan
        month = 0;
        year++;
        doy = 0; // day of the year
      } // month
    } // day
    fprintf(stderr, "---------------------------------------- time: %d (year=%d month=%d day=%d doy=%d)\n", t, 2020+year, month+1, day, doy);

    // calculate param.Rt_lag ##################################################

    // JEM added: Measure Reff by looking at a certain number of days earlier
    int R_running = 0; // number of infected
    int counter_R = 0; // number of infector
    param.Rt_lag = 0; // Rt in real time
    int t_lag_Rrun = 14; // Number of days to lag the running R measure
    if (t > t_lag_Rrun) {
      for (int i=1; i <= individuals->n_i; i++) {
        if (individuals->time_of_infection[i] == t-t_lag_Rrun) {
          R_running += individuals->no_of_infected[i];
          counter_R++;
        }
      }
      if (counter_R > 0) param.Rt_lag = (double) R_running / counter_R;
    }

    // change point model ######################################################

    // define the date of change points
    if(t > 1 && 2020+year == 2021 && month+1 == 1 && day == 28) param.CPT = 1; // 28th Jan 2021
    if(t > 1 && 2020+year == 2021 && month+1 == 3 && day == 11) param.CPT = 2; // 11th Mar 2021

    // at the change point
    if(param.CPT > 0){
      param.BETA_R_CAL  = param.BETA_CPT[param.CPT-1] * 0.24;
      param.BETA_H_CAL  = param.BETA_CPT[param.CPT-1] * 1.00;
      param.BETA_P0_CAL = param.BETA_CPT[param.CPT-1] * 0.166;
      param.BETA_P1_CAL = param.BETA_CPT[param.CPT-1] * 0.188;
      fprintf(stderr, "Change point %d (t = %d): BETA_CPT = %lf,\n", param.CPT, t, param.BETA_CPT[param.CPT-1]);
      fprintf(stderr, "param.BETA_R_CAL  %lf\n",   param.BETA_R_CAL  );
      fprintf(stderr, "param.BETA_H_CAL  %lf\n",   param.BETA_H_CAL  );
      fprintf(stderr, "param.BETA_P0_CAL %lf\n",   param.BETA_P0_CAL );
      fprintf(stderr, "param.BETA_P1_CAL %lf\n\n", param.BETA_P1_CAL );
    }

    // Define seasonality ######################################################

    if( param.SEASONALITY > 0 ){
      seasonality( pt, doy, doy_start );
    }

    // Interventions ###########################################################

    // Home office:
    // Fixed propotion of people doing home office. Defined through param.PROP_HOME_OFFICE
    if( t == 1 ){
      int sel = 0;
      int working = 0;
      for( int idx_i=1 ; idx_i <= individuals->n_i ; idx_i++ ){
        if( individuals->employment[idx_i] == P1 ){
          working++;
          if( gsl_ran_bernoulli( R_GLOBAL, param.PROP_HOME_OFFICE ) == 1 ){
            individuals->home_office[idx_i] = 1;
            sel++;
          }
        }
      }
      fprintf(stderr, "No. of people doing home office: %d, %lf perc. of the total (%d),\n",
      sel,  sel * 100. / working, working );
    }

    // Vaccination:
    if( t >= param.DAY_START_VACCINATION && ( (param.NR_VAX_DOSES_1 + param.NR_VAX_DOSES_2 + param.NR_VAX_DOSES_3) != 0 ) ){

      if(year == 1) // only in 2021
      if( (counter->vaccinated_1 + counter->vaccinated_2 + counter->vaccinated_3) < (param.NR_VAX_DOSES_1 + param.NR_VAX_DOSES_2 + param.NR_VAX_DOSES_3) && stop_vaccination == 0 ){

        sscanf("national", "%s", param.GEOGRAPHIC); //param.GEOGRAPHIC = "national";
        if( strcmp( param.GEOGRAPHIC , "national") == 0 ){ // national
          if( param.KEEP_R_CONSTANT >= 1 )
          vaccination_strategy(fvax, individuals, households, counter, param.DOSES_DAY[t-1], t, &vaccination, grid, &vaccine_priority, probability);
        }
      }
    }

    // Reset hospital incidence
    for( int j = 0; j < 11; j++ ){
      counter->hosp_inc[j] = 0;
    }

    // Loop over regions = Norway only
    for( r = 1 ; r <= grid->n_r ; r++ ){

      // Reset counters for incidence and prevalence
      counter->I_r[r]      = 0;       // prevalence of infected in the country
      counter->Ia_r[r]     = 0;       // prevalence of asymp. infected in the country
      counter->E2_r[r]     = 0;       // prevalence of pre-infected in the country
      counter->inc_I_r[r]  = 0;       // incidence of infected in the country
      counter->inc_Ia_r[r] = 0;       // incidence of asymp. infected in the country
      counter->inc_E2_r[r] = 0;       // incidence of pre-infected in the country
      counter->n_hosp[r]   = 0;       // number of hospitalizations
      counter->n_icu[r]    = 0;       // number of people in ICU
      counter->inc_hosp[r] = 0;       // incidence of people admitted to hospitals
      counter->inc_icu[r]  = 0;       // incidence of people admitted to ICUs
      counter->sus_r[r]    = 0;       // prevalence of susceptible in the country
      counter->rec_r[r]    = 0;       // prevalence of recovered in the country

      for( int age_n = 0; age_n < 101; age_n++ ){
        counter->prev_H[age_n]   = 0;
        counter->prev_ICU[age_n] = 0;
      }
      for(int county_n = 0; county_n < N_COUNTIES; county_n++) counter->prev_H_county[county_n] = 0;
      for(int mun_n = 0; mun_n < N_MUN; mun_n++)               counter->prev_H_mun[mun_n]       = 0;
      for(int cell_n = 0; cell_n < grid->n_c; cell_n++)        counter->prev_H_cell[cell_n]     = 0;

      if( t == 1 ){
        counter->cum_inc_I_r[r] = ini_inf;
        counter->cum_rec_r[r] = 0;
      }

      // daily imported cases (S -> I) ########################################
      importation(t, individuals, grid, counter, fiww, &hosp_param);

      // Loop over cells
      for( c = grid->region[r].idx_c_ini ; c <= grid->region[r].idx_c_fin ; c++ ){
        counter->I_c[c]   = 0; // infected in the cell
        counter->Ia_c[c]  = 0; // asymp. infected in the cell
        counter->E2_c[c]  = 0; // pre-infected in the cell
        counter->sus_c[c] = 0; // susceptible in the cell
        counter->rec_c[c] = 0; // recovered in the cell

        // Community transmission
        r_transmission_individual_level( r, c, grid, individuals, counter, probability, t, fiww, dist[c] );

        // Loop over individuals
        for( i = grid->cell[c].idx_i_ini; i <= grid->cell[c].idx_i_fin; i++ ){

          int age_i       = individuals->age[i];                          // age       of the individual
          int age_group_i = floor(individuals->age[i] / 10);              // age group of the individual
          int county_i    = individuals->county[i];                       // county    of the individual
          int mun_i       = individuals->mun[i] - 1;                      // municip   of the individual
          int cell_i      = individuals->cell[i] - 1;                     // cell      of the individual

          // Hospital control
          if( individuals->hosp_entrance[i] == t ){
            individuals->hospitalized[i] = H;
            counter->hosp_inc[age_group_i]++;
            counter->cum_H[age_i]++;
            counter->cum_H_county[county_i]++;
            counter->cum_H_mun[mun_i]++;
            counter->cum_H_cell[cell_i]++;
          }
          //
          if( individuals->icu_entrance[i] == t ){
            individuals->hospitalized[i] = H_ICU;
            counter->cum_ICU[age_i]++;
            counter->cum_ICU_county[county_i]++;
            counter->cum_ICU_mun[mun_i]++;
            counter->cum_ICU_cell[cell_i]++;
          }
          //
          if( individuals->icu_leaving[i] == t ){
            individuals->hospitalized[i] = H;
          }
          //
          if( individuals->hosp_leaving[i] == t ){
            individuals->hospitalized[i] = NO;
            if( individuals->status[i] == I || individuals->status[i] == Ia){
              individuals->status[i] = R;
            }
          }

          // Count hospital prevalence
          if( individuals->hospitalized[i] == H || individuals->hospitalized[i] == H_ICU ){ // prev_H including both H and ICU
            counter->n_hosp[r]++;
            counter->prev_H[age_i]++;
            counter->prev_H_county[county_i]++;
            counter->prev_H_mun[mun_i]++;
            counter->prev_H_cell[cell_i]++;
          }
          if( individuals->hospitalized[i] == H_ICU ){
            counter->n_icu[r]++;
            counter->prev_ICU[age_i]++;
          }

          // Death control
          if( individuals->death[i] == 1 && individuals->status[i] != D ){
            double prob_death = 0; // to initialize
            if( individuals->hospitalized[i] == NO )    prob_death = 1. - exp( -( 1 / param.TIME_FROM_I_TO_DEATH )   * param.DELTAT);
            if( individuals->hospitalized[i] == H )     prob_death = 1. - exp( -( 1 / param.TIME_FROM_H_TO_DEATH )   * param.DELTAT);
            if( individuals->hospitalized[i] == H_ICU ) prob_death = 1. - exp( -( 1 / param.TIME_FROM_ICU_TO_DEATH ) * param.DELTAT );
            if( gsl_ran_bernoulli( R_GLOBAL, prob_death ) == 1 ){
              individuals->status[i] = D;
              counter->cum_D[age_i]++;
              counter->cum_D_county[county_i]++;
              counter->cum_D_mun[mun_i]++;
              counter->cum_D_cell[cell_i]++;
            }
          }

          // Epidemiological Transitions
          switch( individuals->status[i] ){

            case S:
            counter->sus_c[c]++;
            break;

            // E1 --> E2
            case E1:
            if( t == individuals->next_transition[i] ){
              next_trans_func( i, individuals, t, param.E2_GAMMA_ALPHA, param.E2_GAMMA_BETA, 1 );

              individuals->status[i] = E2;
              counter->inc_E2_r[r]++;

              if( VERBOSE > 5 )
              fprintf( fiww, "%d\t%d\t%s\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
              t, 9999, "-", "symptomat", 9999, i,
              individuals->cell[i],individuals->mun[i], 9999,
              individuals->age[i],individuals->idx_h[i], 9999,"-",
              individuals->status[i], 9999, 9999, 9999, 9999, 9999,
              9999, individuals->vaccinated[i]);
            }
            break;

            // E1a --> Ia
            case E1a:
            if( t == individuals->next_transition[i] ){
              next_trans_func( i, individuals, t, param.Ia_GAMMA_ALPHA, param.Ia_GAMMA_BETA, 1 );

              individuals->status[i] = Ia;

              if( VERBOSE > 5 )
              fprintf( fiww, "%d\t%d\t%s\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
              t, 9999, "-", "not_symptomat", 9999, i,
              individuals->cell[i],individuals->mun[i], 9999,
              individuals->age[i],individuals->idx_h[i], 9999,"-",
              individuals->status[i], 9999, 9999, 9999, 9999, 9999,
              9999, individuals->vaccinated[i]);

              counter->cum_inc_Ia_r[r]++;
              counter->inc_Ia_r[r]++;
            }
            break;

            case E2:
            counter->E2_c[c]++;

            if( individuals->idx_h[i] > 0 ){
              h_transmission( i, individuals->idx_h[i], households, individuals, pt, r, counter, t, fiww );
            }
            if( individuals->employment[i] != P2 ){ // if not retired
              if( individuals->idx_p[i] > 0 ){
                p_transmission( grid, i, individuals->idx_p[i], places, individuals, pt, r, counter, t, fiww );
              }
            }

            // E2 --> I
            if( t == individuals->next_transition[i] ){
              next_trans_func( i, individuals, t, param.I_GAMMA_ALPHA, param.I_GAMMA_BETA, 1 );  // next_trans

              individuals->status[i] = I;

              if( VERBOSE > 5 )
              fprintf( fiww, "%d\t%d\t%s\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
              t, 9999, "-", "symptomat", 9999, i,individuals->cell[i],
              individuals->mun[i],9999,individuals->age[i],individuals->idx_h[i],
              9999,"-",individuals->status[i],9999,9999,9999,9999,9999,
              9999, individuals->vaccinated[i]);

              counter->cum_inc_I_r[r]++;
              counter->inc_I_r[r]++;

              hospitalization( i,  grid, individuals, counter, t, fiww, &hosp_param);
              death( individuals, i, t );
            }
            break;

            case I:
            counter->I_c[c]++;

            if( individuals->idx_h[i] > 0 ){
              h_transmission( i, individuals->idx_h[i], households, individuals, pt, r, counter, t, fiww );
            }
            if( individuals->employment[i] != P2 ){ // if not retired
              if( individuals->idx_p[i] > 0 ){
                p_transmission( grid, i, individuals->idx_p[i], places, individuals, pt, r, counter, t,fiww);
              }
            }

            // Natural decolonization  I --> R
            if( t == individuals->next_transition[i] ){
              counter->rec_c[c]++;
              individuals->status[i]=R;

              if( VERBOSE > 5 )
              fprintf( fiww, "%d\t%d\t%s\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
              t, 9999, "-", "symptomat", 9999, i, individuals->cell[i],
              individuals->mun[i], 9999, individuals->age[i], individuals->idx_h[i],
              9999,"-",individuals->status[i],9999,9999,9999,9999,9999,
              9999, individuals->vaccinated[i]);

              counter->cum_rec_r[r]++;
            }
            break;

            case Ia:
            counter->Ia_c[c]++;

            if( individuals->idx_h[i] > 0 ){
              h_transmission(i, individuals->idx_h[i], households, individuals, pt, r, counter, t,fiww);
            }
            if( individuals->employment[i] != P2 ){ // if not retired
              if( individuals->idx_p[i] > 0 ){
                p_transmission( grid, i, individuals->idx_p[i], places, individuals, pt, r, counter, t,fiww);
              }
            }

            // Natural decolonization Ia --> R
            if( t == individuals->next_transition[i] ){
              counter->rec_c[c]++;
              individuals->status[i] = R;

              if( VERBOSE > 5 )
              fprintf( fiww, "%d\t%d\t%s\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
              t, 9999, "-", "not_symptomat", 9999, i, individuals->cell[i],
              individuals->mun[i],9999,individuals->age[i],individuals->idx_h[i],
              9999,"-",individuals->status[i],9999,9999,9999,9999,9999,
              9999, individuals->vaccinated[i]);

              counter->cum_rec_r[r]++;
            }
            break;
          } // end switch over status of individual i
        } // end loop over individuals of c cell

        counter->I_r[r]   += counter->I_c[c];
        counter->Ia_r[r]  += counter->Ia_c[c];
        counter->E2_r[r]  += counter->E2_c[c];
        counter->sus_r[r] += counter->sus_c[c];
        counter->rec_r[r] += counter->rec_c[c];
      } // end loop over cells

      // count number of individuals each county
      int pop_county[11] = {0};
      for(int idx_i = 1; idx_i <= individuals->n_i; idx_i++){ // each individual from 1 to 5367801
        int county_i = individuals->county[idx_i]; // county of the individual
        pop_county[county_i]++;
      }

      // for summing all age groups
      int cum_sum = 0;

      // time
      fprintf( fp1, "%.2f", t * param.DELTAT);
      // incidence
      fprintf(fp1, "\t%d", counter->inc_I_r[r] + counter->inc_Ia_r[r]);
      fprintf(fp1, "\t%d", counter->I_r[r]     + counter->Ia_r[r]);
      // prevalence
      cum_sum = 0; for(int age_n = 0; age_n < 101; age_n++) cum_sum += counter->prev_H[age_n];     fprintf(fp1, "\t%d", cum_sum);
      cum_sum = 0; for(int age_n = 0; age_n < 101; age_n++) cum_sum += counter->prev_ICU[age_n];   fprintf(fp1, "\t%d", cum_sum);
      // cumulative number
      cum_sum = 0; for(int age_n = 0; age_n < 101; age_n++) cum_sum += counter->cum_I[age_n];    fprintf(fp1, "\t%d", cum_sum);
      cum_sum = 0; for(int age_n = 0; age_n < 101; age_n++) cum_sum += counter->cum_H[age_n];    fprintf(fp1, "\t%d", cum_sum);
      cum_sum = 0; for(int age_n = 0; age_n < 101; age_n++) cum_sum += counter->cum_ICU[age_n];  fprintf(fp1, "\t%d", cum_sum);
      cum_sum = 0; for(int age_n = 0; age_n < 101; age_n++) cum_sum += counter->cum_D[age_n];    fprintf(fp1, "\t%d", cum_sum);
      cum_sum = 0; for(int age_n = 0; age_n < 101; age_n++) cum_sum += counter->cum_V1[age_n];   fprintf(fp1, "\t%d", cum_sum);
      cum_sum = 0; for(int age_n = 0; age_n < 101; age_n++) cum_sum += counter->cum_V2[age_n];   fprintf(fp1, "\t%d", cum_sum);
      cum_sum = 0; for(int age_n = 0; age_n < 101; age_n++) cum_sum += counter->cum_V3[age_n];   fprintf(fp1, "\t%d", cum_sum);
      // cumulative number each age group
      for(int age_n = 0; age_n < 101; age_n++) fprintf(fp1, "\t%d", counter->cum_I[age_n]  ); // 11 age groups
      for(int age_n = 0; age_n < 101; age_n++) fprintf(fp1, "\t%d", counter->cum_H[age_n]  ); // 11 age groups
      for(int age_n = 0; age_n < 101; age_n++) fprintf(fp1, "\t%d", counter->cum_ICU[age_n]); // 11 age groups
      for(int age_n = 0; age_n < 101; age_n++) fprintf(fp1, "\t%d", counter->cum_D[age_n]  ); // 11 age groups
      for(int age_n = 0; age_n < 101; age_n++) fprintf(fp1, "\t%d", counter->cum_V1[age_n]); // 11 age groups
      for(int age_n = 0; age_n < 101; age_n++) fprintf(fp1, "\t%d", counter->cum_V2[age_n]); // 11 age groups
      for(int age_n = 0; age_n < 101; age_n++) fprintf(fp1, "\t%d", counter->cum_V3[age_n]); // 11 age groups
      for(int age_n = 0; age_n < 101; age_n++) fprintf(fp1, "\t%d", counter->prev_H[age_n]); // 11 age groups
      // cumulative number each country
      for(int county_n = 0; county_n < N_COUNTIES; county_n++) fprintf(fp1, "\t%d", counter->cum_I_county[county_n]  ); // 11 counties
      for(int county_n = 0; county_n < N_COUNTIES; county_n++) fprintf(fp1, "\t%d", counter->cum_H_county[county_n]  ); // 11 counties
      for(int county_n = 0; county_n < N_COUNTIES; county_n++) fprintf(fp1, "\t%d", counter->cum_ICU_county[county_n]); // 11 counties
      for(int county_n = 0; county_n < N_COUNTIES; county_n++) fprintf(fp1, "\t%d", counter->cum_D_county[county_n]  ); // 11 counties
      for(int county_n = 0; county_n < N_COUNTIES; county_n++) fprintf(fp1, "\t%d", counter->cum_V1_county[county_n]); // 11 counties
      for(int county_n = 0; county_n < N_COUNTIES; county_n++) fprintf(fp1, "\t%d", counter->cum_V2_county[county_n]); // 11 counties
      for(int county_n = 0; county_n < N_COUNTIES; county_n++) fprintf(fp1, "\t%d", counter->cum_V3_county[county_n]); // 11 counties
      // normalized cumulative number each country
      for(int county_n = 0; county_n < N_COUNTIES; county_n++) fprintf(fp1, "\t%lf", (double) counter->cum_I_county[county_n]   / pop_county[county_n]); // 11 counties
      for(int county_n = 0; county_n < N_COUNTIES; county_n++) fprintf(fp1, "\t%lf", (double) counter->cum_H_county[county_n]   / pop_county[county_n]); // 11 counties
      for(int county_n = 0; county_n < N_COUNTIES; county_n++) fprintf(fp1, "\t%lf", (double) counter->cum_ICU_county[county_n] / pop_county[county_n]); // 11 counties
      for(int county_n = 0; county_n < N_COUNTIES; county_n++) fprintf(fp1, "\t%lf", (double) counter->cum_D_county[county_n]   / pop_county[county_n]); // 11 counties
      for(int county_n = 0; county_n < N_COUNTIES; county_n++) fprintf(fp1, "\t%lf", (double) counter->cum_V1_county[county_n]  / pop_county[county_n]); // 11 counties
      for(int county_n = 0; county_n < N_COUNTIES; county_n++) fprintf(fp1, "\t%lf", (double) counter->cum_V2_county[county_n]  / pop_county[county_n]); // 11 counties
      for(int county_n = 0; county_n < N_COUNTIES; county_n++) fprintf(fp1, "\t%lf", (double) counter->cum_V3_county[county_n]  / pop_county[county_n]); // 11 counties
      // cumulative number each country
      for(int mun_n = 0; mun_n < N_MUN; mun_n++) fprintf(fp1, "\t%d", counter->cum_I_mun[mun_n]  ); // 356 municipalities
      for(int mun_n = 0; mun_n < N_MUN; mun_n++) fprintf(fp1, "\t%d", counter->cum_H_mun[mun_n]  ); // 356 municipalities
      for(int mun_n = 0; mun_n < N_MUN; mun_n++) fprintf(fp1, "\t%d", counter->cum_ICU_mun[mun_n]); // 356 municipalities
      for(int mun_n = 0; mun_n < N_MUN; mun_n++) fprintf(fp1, "\t%d", counter->cum_D_mun[mun_n]  ); // 356 municipalities
      for(int mun_n = 0; mun_n < N_MUN; mun_n++) fprintf(fp1, "\t%d", counter->cum_V1_mun[mun_n]); // 356 municipalities
      for(int mun_n = 0; mun_n < N_MUN; mun_n++) fprintf(fp1, "\t%d", counter->cum_V2_mun[mun_n]); // 356 municipalities
      for(int mun_n = 0; mun_n < N_MUN; mun_n++) fprintf(fp1, "\t%d", counter->cum_V3_mun[mun_n]); // 356 municipalities
      for(int mun_n = 0; mun_n < N_MUN; mun_n++) fprintf(fp1, "\t%lf", param.RELATIVE_TRANS_MUN[mun_n]); // 356 municipalities
      fprintf(fp1, "\t%lf", param.BETA_H);
      fprintf(fp1, "\t%lf", param.Rt_lag);
      // scenario
      fprintf(fp1, "\t%s\tABM", param.NAME_SCENARIO);
      fprintf(fp1, "\n");
      fflush( fp1 );
      fflush( fp1 );

      // print on terminal
      cum_sum = 0; for(int age_n = 0; age_n < 101; age_n++) cum_sum += counter->cum_I[age_n];    fprintf(stderr, "I   = %d  \n", cum_sum);
      cum_sum = 0; for(int age_n = 0; age_n < 101; age_n++) cum_sum += counter->cum_H[age_n];    fprintf(stderr, "H   = %d  \n", cum_sum);
      cum_sum = 0; for(int age_n = 0; age_n < 101; age_n++) cum_sum += counter->cum_ICU[age_n];  fprintf(stderr, "ICU = %d  \n", cum_sum);
      cum_sum = 0; for(int age_n = 0; age_n < 101; age_n++) cum_sum += counter->cum_D[age_n];    fprintf(stderr, "D   = %d  \n", cum_sum);
      cum_sum = 0; for(int age_n = 0; age_n < 101; age_n++) cum_sum += counter->cum_V1[age_n];   fprintf(stderr, "V1  = %d  \n", cum_sum);
      cum_sum = 0; for(int age_n = 0; age_n < 101; age_n++) cum_sum += counter->cum_V2[age_n];   fprintf(stderr, "V2  = %d  \n", cum_sum);
      cum_sum = 0; for(int age_n = 0; age_n < 101; age_n++) cum_sum += counter->cum_V3[age_n];   fprintf(stderr, "V3  = %d  \n", cum_sum);

    } // end loop over regions

    // Total number of transmission in each setting (place_of_infection.txt)
    fprintf( fplaceinf, "%d\t%d\t%d\t%d\t%d\t%d\n",
    t, counter->inf_community, counter->inf_work, counter->inf_school, counter->inf_uni, counter->inf_household);

    // last day
    if( t == param.MAXT ){ // Print on file:

      // Number of secondary infections for each infected individual (Rt.txt)
      for( int id = 1; id <= individuals->n_i; id++ ){
        if( individuals->time_of_infection[id] > 0 )
        fprintf(finf, "%d\t%d\t%d\t%d\t%d\t%d\n",
        individuals->time_of_infection[id], id, individuals->no_of_infected[id], individuals->county[id], individuals->age[id], individuals->vaccinated[id]);
      }

      fclose( fp1 );
      fclose( fimp );
      fclose( finf );
      fclose( fplaceinf );

      if( VERBOSE > 5 ){
        fclose( fvax );
      }

      if( VERBOSE != -1 ){
        fclose( fiww );
      }
      return time;
    } // t == param.MAXT

    doy++;

  } // end loop over time

  // Free some variables
  for( int cf = 0; cf <= grid->region[1].idx_c_fin; cf++ ){
    gsl_ran_discrete_free( dist[cf] );
  }

  free( dist );

  return time;
} // int SEIR_model

double haversine_dist(double th1, double ph1, double th2, double ph2){

  double dx, dy, dz;
  ph1 -= ph2;
  ph1 *= TO_RAD, th1 *= TO_RAD, th2 *= TO_RAD;

  dz = sin(th1) - sin(th2);
  dx = cos(ph1) * cos(th1) - cos(th2);
  dy = sin(ph1) * cos(th1);

  return asin(sqrt(dx * dx + dy * dy + dz * dz) / 2) * 2 * R;
} // double haversine_dist

static void mobility_dist(Grid *grid, Mobility *mobility, gsl_ran_discrete_t **dist){

  double preproc_value; // temp-value of one probability
  double *pdist;        // vector of probabilities from Cell "cf"
  pdist = (double*) calloc( grid->n_c + 1, sizeof(double) );

  for(   int cf  = grid->region[1].idx_c_ini; cf  <= grid->region[1].idx_c_fin; cf++  ){ // from 4978 cells
    for( int cf2 = grid->region[1].idx_c_ini; cf2 <= grid->region[1].idx_c_fin; cf2++ ){ // to   4978 cells

      int id_vec = round( haversine_dist( grid->cell[cf].x, grid->cell[cf].y, grid->cell[cf2].x, grid->cell[cf2].y ) ); // distance between two cells
      if( id_vec > mobility->max_distance ){ // suppose no such long distance mobility
        preproc_value = 0;
      } else { // reasonable mobility
        preproc_value = mobility->proportion[grid->cell[cf].mun_ID - 1][id_vec];
      } // id_vec
      pdist[cf2] = preproc_value;
    } // cf2
    dist[cf] = gsl_ran_discrete_preproc(grid->n_c, pdist);
  } // cf
  free(pdist);

  return;
} // static void mobility_dist

static void define_hospitalization_param( Hosp_param *hosp_param ){

  // probability of hospitalization
  hosp_param->hosp_admission = (double **) calloc( 2, sizeof(double*) );

  for( int i=0; i<2; i++ ) // 0: ppl w/o riskfactors; 1; ppl w riskfactors
  hosp_param->hosp_admission[i] = ( double * ) calloc ( 11, sizeof(double) );

  hosp_param->hosp_admission[0][0]  = param.PROB_HOSP_0_9      ; // see excel
  hosp_param->hosp_admission[1][0]  = param.PROB_HOSP_RG_0_9   ; // see excel
  hosp_param->hosp_admission[0][1]  = param.PROB_HOSP_10_19    ; // see excel
  hosp_param->hosp_admission[1][1]  = param.PROB_HOSP_RG_10_19 ; // see excel
  hosp_param->hosp_admission[0][2]  = param.PROB_HOSP_20_29    ; // see excel
  hosp_param->hosp_admission[1][2]  = param.PROB_HOSP_RG_20_29 ; // see excel
  hosp_param->hosp_admission[0][3]  = param.PROB_HOSP_30_39    ; // see excel
  hosp_param->hosp_admission[1][3]  = param.PROB_HOSP_RG_30_39 ; // see excel
  hosp_param->hosp_admission[0][4]  = param.PROB_HOSP_40_49    ; // see excel
  hosp_param->hosp_admission[1][4]  = param.PROB_HOSP_RG_40_49 ; // see excel
  hosp_param->hosp_admission[0][5]  = param.PROB_HOSP_50_59    ; // see excel
  hosp_param->hosp_admission[1][5]  = param.PROB_HOSP_RG_50_59 ; // see excel
  hosp_param->hosp_admission[0][6]  = param.PROB_HOSP_60_69    ; // see excel
  hosp_param->hosp_admission[1][6]  = param.PROB_HOSP_RG_60_69 ; // see excel
  hosp_param->hosp_admission[0][7]  = param.PROB_HOSP_70_79    ; // see excel
  hosp_param->hosp_admission[1][7]  = param.PROB_HOSP_RG_70_79 ; // see excel
  hosp_param->hosp_admission[0][8]  = param.PROB_HOSP_80p      ; // see excel
  hosp_param->hosp_admission[1][8]  = param.PROB_HOSP_RG_80p   ; // see excel
  hosp_param->hosp_admission[0][9]  = param.PROB_HOSP_80p      ; // see excel
  hosp_param->hosp_admission[1][9]  = param.PROB_HOSP_RG_80p   ; // see excel
  hosp_param->hosp_admission[0][10] = param.PROB_HOSP_80p      ; // see excel
  hosp_param->hosp_admission[1][10] = param.PROB_HOSP_RG_80p   ; // see excel

  // Time to hospital: parameters of the negative binomial distribution ([.][0] = prob, [.][1] = size)
  hosp_param->time_to_hosp = (double **) calloc( 11, sizeof(double*));
  for(int i=0; i<11; i++){
    hosp_param->time_to_hosp[i]  = (double *) calloc( 2, sizeof(double));
  }

  hosp_param->time_to_hosp[0][0]  = param.ONSET_TO_HOSP_PROB_0_9; //0.1492997446;
  hosp_param->time_to_hosp[0][1]  = param.ONSET_TO_HOSP_SIZE_0_9; //1.695;

  hosp_param->time_to_hosp[1][0]  = param.ONSET_TO_HOSP_PROB_10_19; //0.1492997446;
  hosp_param->time_to_hosp[1][1]  = param.ONSET_TO_HOSP_SIZE_10_19; //1.695;

  hosp_param->time_to_hosp[2][0]  = param.ONSET_TO_HOSP_PROB_20_29; //0.1492997446;
  hosp_param->time_to_hosp[2][1]  = param.ONSET_TO_HOSP_SIZE_20_29; //1.695;

  hosp_param->time_to_hosp[3][0]  = param.ONSET_TO_HOSP_PROB_30_39; //0.2296810915;
  hosp_param->time_to_hosp[3][1]  = param.ONSET_TO_HOSP_SIZE_30_39; //2.744;

  hosp_param->time_to_hosp[4][0]  = param.ONSET_TO_HOSP_PROB_40_49; //0.3181985159;
  hosp_param->time_to_hosp[4][1]  = param.ONSET_TO_HOSP_SIZE_40_49; //4.331;

  hosp_param->time_to_hosp[5][0]  = param.ONSET_TO_HOSP_PROB_50_59; //0.2732549487;
  hosp_param->time_to_hosp[5][1]  = param.ONSET_TO_HOSP_SIZE_50_59; //3.672;

  hosp_param->time_to_hosp[6][0]  = param.ONSET_TO_HOSP_PROB_60_69; //0.3543613707;
  hosp_param->time_to_hosp[6][1]  = param.ONSET_TO_HOSP_SIZE_60_69; //5.005;

  hosp_param->time_to_hosp[7][0]  = param.ONSET_TO_HOSP_PROB_70_79; //0.3023983316;
  hosp_param->time_to_hosp[7][1]  = param.ONSET_TO_HOSP_SIZE_70_79; //4.06;

  hosp_param->time_to_hosp[8][0]  = param.ONSET_TO_HOSP_PROB_80_89; //0.2533622168;
  hosp_param->time_to_hosp[8][1]  = param.ONSET_TO_HOSP_SIZE_80_89; //2.807;

  hosp_param->time_to_hosp[9][0]  = param.ONSET_TO_HOSP_PROB_90p; //0.2533622168;
  hosp_param->time_to_hosp[9][1]  = param.ONSET_TO_HOSP_SIZE_90p; //2.807;

  hosp_param->time_to_hosp[10][0] = param.ONSET_TO_HOSP_PROB_90p; //0.2533622168;
  hosp_param->time_to_hosp[10][1] = param.ONSET_TO_HOSP_SIZE_90p; //2.807;

  // LOS in hospital: parameters of the negative binomial distribution ([.][0] = prob, [.][1] = size)
  hosp_param->LOS_hosp = (double **) calloc( 11, sizeof(double*));

  for( int i=0; i<11; i++ ){
    hosp_param->LOS_hosp[i]  = (double *) calloc( 2, sizeof(double));
  }

  hosp_param->LOS_hosp[0][0]  = param.LOS_HOSP_PROB_0_9; //0.3304904051;
  hosp_param->LOS_hosp[0][1]  = param.LOS_HOSP_SIZE_0_9; // 1.55;

  hosp_param->LOS_hosp[1][0]  = param.LOS_HOSP_PROB_10_19; //0.3304904051;
  hosp_param->LOS_hosp[1][1]  = param.LOS_HOSP_SIZE_10_19; // 1.55;

  hosp_param->LOS_hosp[2][0]  = param.LOS_HOSP_PROB_20_29; //0.3304904051;
  hosp_param->LOS_hosp[2][1]  = param.LOS_HOSP_SIZE_20_29; // 1.55;

  hosp_param->LOS_hosp[3][0]  = param.LOS_HOSP_PROB_30_39; //0.280730897;
  hosp_param->LOS_hosp[3][1]  = param.LOS_HOSP_SIZE_30_39; // 1.69;

  hosp_param->LOS_hosp[4][0]  = param.LOS_HOSP_PROB_40_49; //0.3260273973;
  hosp_param->LOS_hosp[4][1]  = param.LOS_HOSP_SIZE_40_49; // 2.38;

  hosp_param->LOS_hosp[5][0]  = param.LOS_HOSP_PROB_50_59; //0.2939666239;
  hosp_param->LOS_hosp[5][1]  = param.LOS_HOSP_SIZE_50_59; // 2.29;

  hosp_param->LOS_hosp[6][0]  = param.LOS_HOSP_PROB_60_69; //0.2838283828;
  hosp_param->LOS_hosp[6][1]  = param.LOS_HOSP_SIZE_60_69; // 2.58;

  hosp_param->LOS_hosp[7][0]  = param.LOS_HOSP_PROB_70_79; //0.2572815534;
  hosp_param->LOS_hosp[7][1]  = param.LOS_HOSP_SIZE_70_79; // 2.65;

  hosp_param->LOS_hosp[8][0]  = param.LOS_HOSP_PROB_80_89; //0.2739861523;
  hosp_param->LOS_hosp[8][1]  = param.LOS_HOSP_SIZE_80_89; // 2.77;

  hosp_param->LOS_hosp[9][0]  = param.LOS_HOSP_PROB_90p; //0.2739861523;
  hosp_param->LOS_hosp[9][1]  = param.LOS_HOSP_SIZE_90p; // 2.77;

  hosp_param->LOS_hosp[10][0] = param.LOS_HOSP_PROB_90p; //0.2739861523;
  hosp_param->LOS_hosp[10][1] = param.LOS_HOSP_SIZE_90p; // 2.77;

  // LOS in hospital for people admitted to ICUs: parameters of the negative binomial distribution  ([.][0] = prob, [.][1] = size)
  hosp_param->LOS_hosp_ICU = (double **) calloc( 11, sizeof(double*));
  for(int i=0; i<11; i++){
    hosp_param->LOS_hosp_ICU[i]  = (double *) calloc( 2, sizeof(double));
  }

  hosp_param->LOS_hosp_ICU[0][0]  = param.LOS_HOSP_ICU_PROB_0_9; //0.2579001019;
  hosp_param->LOS_hosp_ICU[0][1]  = param.LOS_HOSP_ICU_SIZE_0_9; //7.59;

  hosp_param->LOS_hosp_ICU[1][0]  = param.LOS_HOSP_ICU_PROB_10_19; //0.2579001019;
  hosp_param->LOS_hosp_ICU[1][1]  = param.LOS_HOSP_ICU_SIZE_10_19; //7.59;

  hosp_param->LOS_hosp_ICU[2][0]  = param.LOS_HOSP_ICU_PROB_20_29; //0.2579001019;
  hosp_param->LOS_hosp_ICU[2][1]  = param.LOS_HOSP_ICU_SIZE_20_29; //7.59;

  hosp_param->LOS_hosp_ICU[3][0]  = param.LOS_HOSP_ICU_PROB_30_39; //0.2579001019;
  hosp_param->LOS_hosp_ICU[3][1]  = param.LOS_HOSP_ICU_SIZE_30_39; //7.59;

  hosp_param->LOS_hosp_ICU[4][0]  = param.LOS_HOSP_ICU_PROB_40_49; //0.2579001019;
  hosp_param->LOS_hosp_ICU[4][1]  = param.LOS_HOSP_ICU_SIZE_40_49; //7.59;

  hosp_param->LOS_hosp_ICU[5][0]  = param.LOS_HOSP_ICU_PROB_50_59; //0.0925170068;
  hosp_param->LOS_hosp_ICU[5][1]  = param.LOS_HOSP_ICU_SIZE_50_59; //2.72;

  hosp_param->LOS_hosp_ICU[6][0]  = param.LOS_HOSP_ICU_PROB_60_69; //0.0605095541;
  hosp_param->LOS_hosp_ICU[6][1]  = param.LOS_HOSP_ICU_SIZE_60_69; //2.09;

  hosp_param->LOS_hosp_ICU[7][0]  = param.LOS_HOSP_ICU_PROB_70_79; //0.0827562327;
  hosp_param->LOS_hosp_ICU[7][1]  = param.LOS_HOSP_ICU_SIZE_70_79; //2.39;

  hosp_param->LOS_hosp_ICU[8][0]  = param.LOS_HOSP_ICU_PROB_80p; //0.0708838044;
  hosp_param->LOS_hosp_ICU[8][1]  = param.LOS_HOSP_ICU_SIZE_80p; //1.58;

  hosp_param->LOS_hosp_ICU[9][0]  = param.LOS_HOSP_ICU_PROB_80p; //0.0708838044;
  hosp_param->LOS_hosp_ICU[9][1]  = param.LOS_HOSP_ICU_SIZE_80p; //1.58;

  hosp_param->LOS_hosp_ICU[10][0] = param.LOS_HOSP_ICU_PROB_80p; //0.0708838044;
  hosp_param->LOS_hosp_ICU[10][1] = param.LOS_HOSP_ICU_SIZE_80p; //1.58;

  // time to ICU: parameters of the Geometric distribution (prob)
  hosp_param->time_to_ICU = (double **) calloc( 11, sizeof(double*));
  for(int i=0; i<11; i++){
    hosp_param->time_to_ICU[i]  = (double *) calloc( 2, sizeof(double));
  }

  hosp_param->time_to_ICU[0][0]  = param.TIME_TO_ICU_PROB_0_9; // 0.301;
  hosp_param->time_to_ICU[0][1]  = param.TIME_TO_ICU_SIZE_0_9; // 0.301;

  hosp_param->time_to_ICU[1][0]  = param.TIME_TO_ICU_PROB_10_19; // 0.301;
  hosp_param->time_to_ICU[1][1]  = param.TIME_TO_ICU_SIZE_10_19; // 0.301;

  hosp_param->time_to_ICU[2][0]  = param.TIME_TO_ICU_PROB_20_29; // 0.301;
  hosp_param->time_to_ICU[2][1]  = param.TIME_TO_ICU_SIZE_20_29; // 0.301;

  hosp_param->time_to_ICU[3][0]  = param.TIME_TO_ICU_PROB_30_39; // 0.301;
  hosp_param->time_to_ICU[3][1]  = param.TIME_TO_ICU_SIZE_30_39; // 0.301;

  hosp_param->time_to_ICU[4][0]  = param.TIME_TO_ICU_PROB_40_49; // 0.301;
  hosp_param->time_to_ICU[4][1]  = param.TIME_TO_ICU_SIZE_40_49; // 0.301;

  hosp_param->time_to_ICU[5][0]  = param.TIME_TO_ICU_PROB_50_59; // 0.283;
  hosp_param->time_to_ICU[5][1]  = param.TIME_TO_ICU_SIZE_50_59; // 0.283;

  hosp_param->time_to_ICU[6][0]  = param.TIME_TO_ICU_PROB_60_69; // 0.278;
  hosp_param->time_to_ICU[6][1]  = param.TIME_TO_ICU_SIZE_60_69; // 0.278;

  hosp_param->time_to_ICU[7][0]  = param.TIME_TO_ICU_PROB_70_79; // 0.213;
  hosp_param->time_to_ICU[7][1]  = param.TIME_TO_ICU_SIZE_70_79; // 0.213;

  hosp_param->time_to_ICU[8][0]  = param.TIME_TO_ICU_SIZE_80p; // 0.2;
  hosp_param->time_to_ICU[8][1]  = param.TIME_TO_ICU_PROB_80p; // 0.2;

  hosp_param->time_to_ICU[9][0]  = param.TIME_TO_ICU_SIZE_80p; // 0.2;
  hosp_param->time_to_ICU[9][1]  = param.TIME_TO_ICU_PROB_80p; // 0.2;

  hosp_param->time_to_ICU[10][0] = param.TIME_TO_ICU_SIZE_80p; // 0.2;
  hosp_param->time_to_ICU[10][1] = param.TIME_TO_ICU_PROB_80p; // 0.2;

  // Probabilities of being admitted to ICUs
  hosp_param->ICU_admission = (double *) calloc( 11, sizeof(double));

  hosp_param->ICU_admission[0]  = param.PROB_ICU_0_9; // 0.;
  hosp_param->ICU_admission[1]  = param.PROB_ICU_10_19; //0.;
  hosp_param->ICU_admission[2]  = param.PROB_ICU_20_29; //0.;
  hosp_param->ICU_admission[3]  = param.PROB_ICU_30_39; //0.;
  hosp_param->ICU_admission[4]  = param.PROB_ICU_40_49; //0.;
  hosp_param->ICU_admission[5]  = param.PROB_ICU_50_59; //0.;
  hosp_param->ICU_admission[6]  = param.PROB_ICU_60_69; //0.263;
  hosp_param->ICU_admission[7]  = param.PROB_ICU_70_79; //0.083;
  hosp_param->ICU_admission[8]  = param.PROB_ICU_80p; //0.111;
  hosp_param->ICU_admission[9]  = param.PROB_ICU_80p; //0.111;
  hosp_param->ICU_admission[10] = param.PROB_ICU_80p; //0.111;

  // LOS in ICUs ( No age information)
  hosp_param->LOS_ICU = (double **) calloc( 11, sizeof(double*));
  for(int i=0; i<11; i++){
    hosp_param->LOS_ICU[i]  = (double *) calloc( 2, sizeof(double));
  }

  hosp_param->LOS_ICU[0][0] = param.LOS_ICU_PROB_0_9; // 0.101;
  hosp_param->LOS_ICU[0][1] = param.LOS_ICU_SIZE_0_9; //1.89;

  hosp_param->LOS_ICU[1][0] = param.LOS_ICU_PROB_10_19; // 0.101;
  hosp_param->LOS_ICU[1][1] = param.LOS_ICU_SIZE_10_19; //1.89;

  hosp_param->LOS_ICU[2][0] = param.LOS_ICU_PROB_20_29; // 0.101;
  hosp_param->LOS_ICU[2][1] = param.LOS_ICU_SIZE_20_29; //1.89;

  hosp_param->LOS_ICU[3][0] = param.LOS_ICU_PROB_30_39; // 0.101;
  hosp_param->LOS_ICU[3][1] = param.LOS_ICU_SIZE_30_39; //1.89;

  hosp_param->LOS_ICU[4][0] = param.LOS_ICU_PROB_40_49; // 0.101;
  hosp_param->LOS_ICU[4][1] = param.LOS_ICU_SIZE_40_49; //1.89;

  hosp_param->LOS_ICU[5][0] = param.LOS_ICU_PROB_50_59; // 0.101;
  hosp_param->LOS_ICU[5][1] = param.LOS_ICU_SIZE_50_59; //1.89;

  hosp_param->LOS_ICU[6][0] = param.LOS_ICU_PROB_60_69; // 0.101;
  hosp_param->LOS_ICU[6][1] = param.LOS_ICU_SIZE_60_69; //1.89;

  hosp_param->LOS_ICU[7][0] = param.LOS_ICU_PROB_70_79; // 0.101;
  hosp_param->LOS_ICU[7][1] = param.LOS_ICU_SIZE_70_79; //1.89;

  hosp_param->LOS_ICU[8][0] = param.LOS_ICU_PROB_80p; // 0.101;
  hosp_param->LOS_ICU[8][1] = param.LOS_ICU_SIZE_80p; //1.89;

  hosp_param->LOS_ICU[9][0] = param.LOS_ICU_PROB_80p; // 0.101;
  hosp_param->LOS_ICU[9][1] = param.LOS_ICU_SIZE_80p; //1.89;

  hosp_param->LOS_ICU[10][0] = param.LOS_ICU_PROB_80p; // 0.101;
  hosp_param->LOS_ICU[10][1] = param.LOS_ICU_SIZE_80p; //1.89;

  return;
} // static void define_hospitalization_param

static void hospitalization( unsigned int idx_i, Grid *grid, Individuals *individuals, Counter *counter, int t, FILE* fiww, Hosp_param *hosp_param){

  int age, age_cat;
  int rg; // risk_group
  double prob_of_hospitalization;

  age = individuals->age[idx_i];
  age_cat = floor( age / 10. );
  rg = individuals->risk_category[idx_i];

  if( individuals->risk_category[idx_i] == 2 ) // heatlhcare worker
  rg = 0;

  prob_of_hospitalization = hosp_param->hosp_admission[rg][ age_cat ];
  prob_of_hospitalization *= 0.5; // manual setting (regional vaccination paper!)

  double relative_reduction = 0;
  int vac_i = individuals->vaccinated[idx_i];
  if( vac_i > 0 ){ // vaccinated
    int ramp_up_time_1st = param.RAMP_UP_TIME[0][vac_i-1];
    int delay_effect     = param.DELAY_EFFECT[vac_i-1];
    int ramp_up_time_2nd = param.RAMP_UP_TIME[1][vac_i-1];

    int delta_t = t - individuals->time_vaccination[idx_i];
    if( delta_t <= 0 ){
      relative_reduction = 0;
    }else if( delta_t > 0 && delta_t <= ramp_up_time_1st ){
      relative_reduction = individuals->vaccine_efficacy_1st_sd[idx_i] * delta_t / ramp_up_time_1st;
    }else if( delta_t > ramp_up_time_1st && delta_t <= ramp_up_time_1st + delay_effect){
      relative_reduction = individuals->vaccine_efficacy_1st_sd[idx_i];
    }
    else if( delta_t > ramp_up_time_1st + delay_effect && delta_t <= ramp_up_time_1st + delay_effect + ramp_up_time_2nd ){
      relative_reduction = individuals->vaccine_efficacy_1st_sd[idx_i] + (individuals->vaccine_efficacy_2nd_sd[idx_i] - individuals->vaccine_efficacy_1st_sd[idx_i]) * (delta_t - ramp_up_time_1st - delay_effect) / ramp_up_time_2nd;
    }
    else if( delta_t > ramp_up_time_1st + delay_effect + ramp_up_time_2nd ){
      relative_reduction = individuals->vaccine_efficacy_2nd_sd[idx_i];
    }
  } // vaccinated
  prob_of_hospitalization *= 1 - relative_reduction;

  if( gsl_ran_bernoulli( R_GLOBAL, prob_of_hospitalization ) == 1 ){ // will be hospitalized

    int time_to_hosp = 0;
    time_to_hosp = gsl_ran_negative_binomial( R_GLOBAL, hosp_param->time_to_hosp[age_cat][0],  hosp_param->time_to_hosp[age_cat][1]);
    individuals->hosp_entrance[idx_i] = t + time_to_hosp;

    int LOS_hosp =  0;
    LOS_hosp = gsl_ran_negative_binomial( R_GLOBAL, hosp_param->LOS_hosp[age_cat][0], hosp_param->LOS_hosp[age_cat][1] );
    individuals->hosp_leaving[idx_i] = individuals->hosp_entrance[idx_i] + LOS_hosp;

    // for hospitalization logging, time, id of individ, string hosp, null string,  hosp entrance time, null int, hosp discharge time, individ age
    if( VERBOSE > 5 )
    fprintf( fiww, "%d\t%d\t%s\t%s\t%d\t%d\t-\t-\t-\t-\t-\t%d\t-\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
    t, idx_i, "hosp", "-",  individuals->hosp_entrance[idx_i], 9999,
    9999,individuals->hosp_leaving[idx_i], individuals->age[idx_i],9999,9999,9999,9999,
    9999, individuals->vaccinated[idx_i]);

    if( gsl_ran_bernoulli( R_GLOBAL, hosp_param->ICU_admission[ age_cat ] ) == 1 ){ //probability->icu[ age ] ) == 1 )

      int time_to_ICU = 0; // LOS in hosp before ICU
      time_to_ICU = gsl_ran_negative_binomial(R_GLOBAL, hosp_param->time_to_ICU[age_cat][0], hosp_param->time_to_ICU[age_cat][1]);

      int LOS_ICU = 0; // LOS in ICU
      LOS_ICU = gsl_ran_negative_binomial(R_GLOBAL, hosp_param->LOS_ICU[age_cat][0],  hosp_param->LOS_ICU[age_cat][1]);

      int LOS_hosp_ICU = 0; // LOS after ICU
      LOS_hosp_ICU = gsl_ran_negative_binomial( R_GLOBAL, hosp_param->LOS_hosp_ICU[age_cat][0],  hosp_param->LOS_hosp_ICU[age_cat][1]);

      individuals->icu_entrance[idx_i] = individuals->hosp_entrance[idx_i] + time_to_ICU;
      individuals->icu_leaving[idx_i]  = individuals->icu_entrance[idx_i]  + LOS_ICU;
      individuals->hosp_leaving[idx_i] = individuals->icu_leaving[idx_i]   + LOS_hosp_ICU;

      // for hospitalization logging, time, id of individ, string hosp, null string,  hosp entrance time, icu entrance time, hosp discharge time, individ age
      if( VERBOSE > 5 )
      fprintf( fiww, "%d\t%d\t%s\t%s\t%d\t%d\t-\t-\t-\t-\t-\t%d\t-\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
      t, idx_i, "icu", "-", individuals->hosp_entrance[idx_i], individuals->icu_leaving[idx_i],
      individuals->icu_entrance[idx_i], individuals->hosp_leaving[idx_i], individuals->age[idx_i],9999,9999,9999,9999,
      9999, individuals->vaccinated[idx_i]);

    }
  }

  return;
} // static void hospitalization

static void next_trans_func( unsigned int idx_i, Individuals *individuals, int t, double alpha, double beta, int flag_neg){

  individuals->next_transition[idx_i] = t + flag_neg * (1 + (int) floor(gsl_ran_gamma(R_GLOBAL, alpha, beta )));

  return;
} // static void next_trans_func

static void define_age_susceptibility_and_prob_of_asymptomatic( Individuals *individuals, Param *param ){

  individuals->age_sus = (double*)calloc( individuals->n_i+1 , sizeof(double) );
  individuals->age_prob_asymp = (double*)calloc( individuals->n_i+1 , sizeof(double) );

  double * age_dict;
  double * age_asymp;
  int i, age_cat, age_cat2;

  age_dict = (double *) calloc( 11, sizeof(double) );
  age_dict[0]  = param->SUS_0_9;
  age_dict[1]  = param->SUS_10_19;
  age_dict[2]  = param->SUS_20_29;
  age_dict[3]  = param->SUS_30_39;
  age_dict[4]  = param->SUS_40_49;
  age_dict[5]  = param->SUS_50_59;
  age_dict[6]  = param->SUS_60_69;
  age_dict[7]  = param->SUS_70_79;
  age_dict[8]  = param->SUS_80p;
  age_dict[9]  = param->SUS_80p;
  age_dict[10] = param->SUS_80p;

  age_asymp = (double *) calloc( 5, sizeof(double) );
  age_asymp[0] = param->PROB_ASYMP;
  age_asymp[1] = param->PROB_ASYMP;
  age_asymp[2] = param->PROB_ASYMP;
  age_asymp[3] = param->PROB_ASYMP;
  age_asymp[4] = param->PROB_ASYMP;

  for( i=1; i <= individuals->n_i; i++ ){

    age_cat  = floor( individuals->age[i] / 10.0 );
    age_cat2 = floor( individuals->age[i] / 20.0 );

    individuals->age_sus[i]        = age_dict[ age_cat ];
    individuals->age_prob_asymp[i] = age_asymp[ age_cat2 ];
  }

  return;
} // static void define_age_susceptibility_and_prob_of_asymptomatic

static void death( Individuals * individuals, unsigned int idx_i , int t ){

  int age_cat;
  int rg; //risk_group
  double prob_death;

  // define age group
  age_cat = floor( individuals->age[idx_i] / 10.);
  if( age_cat > 8 ) age_cat = 8;

  // define risk group
  rg = individuals->risk_category[idx_i];
  // hws
  if( rg == 2 ) rg = 0;

  // probability of death
  prob_death = 999.;
  if( age_cat == 0 ){
    prob_death = param.PROB_DEATH_0_9 ; // see excel
    if( rg == 1 )
    prob_death = param.PROB_DEATH_RG_0_9 ; // see excel
  }
  if( age_cat == 1 ){
    prob_death = param.PROB_DEATH_10_19 ; // see excel
    if( rg == 1 )
    prob_death = param.PROB_DEATH_RG_10_19 ; // see excel
  }
  if( age_cat == 2 ){
    prob_death = param.PROB_DEATH_20_29 ; // see excel
    if( rg == 1 )
    prob_death = param.PROB_DEATH_RG_20_29 ; // see excel
  }
  if( age_cat == 3 ){
    prob_death = param.PROB_DEATH_30_39 ; // see excel
    if( rg == 1 )
    prob_death = param.PROB_DEATH_RG_30_39 ; // see excel
  }
  if( age_cat == 4 ){
    prob_death = param.PROB_DEATH_40_49 ; // see excel
    if( rg == 1 )
    prob_death = param.PROB_DEATH_RG_40_49 ; // see excel
  }
  if( age_cat == 5 ){
    prob_death = param.PROB_DEATH_50_59 ; // see excel
    if( rg == 1 )
    prob_death = param.PROB_DEATH_RG_50_59 ; // see excel
  }
  if( age_cat == 6 ){
    prob_death = param.PROB_DEATH_60_69 ; // see excel
    if( rg == 1 )
    prob_death = param.PROB_DEATH_RG_60_69 ; // see excel
  }
  if( age_cat == 7 ){
    prob_death = param.PROB_DEATH_70_79 ; // see excel
    if( rg == 1 )
    prob_death = param.PROB_DEATH_RG_70_79 ; // see excel
  }
  if( age_cat == 8 ){
    prob_death = param.PROB_DEATH_80p ; // see excel
    if( rg == 1 )
    prob_death = param.PROB_DEATH_RG_80p ; // see excel
  }

  if( prob_death == 999. ){
    fprintf(stderr, "Error in death(): prob_death undefined\n" );
    exit(1);
  }

  double relative_reduction = 0;
  int vac_i = individuals->vaccinated[idx_i];
  if( vac_i > 0 ){ // vaccinated
    int ramp_up_time_1st = param.RAMP_UP_TIME[0][vac_i-1];
    int delay_effect     = param.DELAY_EFFECT[vac_i-1];
    int ramp_up_time_2nd = param.RAMP_UP_TIME[1][vac_i-1];

    int delta_t = t - individuals->time_vaccination[idx_i];
    if( delta_t <= 0 ){
      relative_reduction = 0;
    }else if( delta_t > 0 && delta_t <= ramp_up_time_1st ){
      relative_reduction = individuals->vaccine_efficacy_1st_d[idx_i] * delta_t / ramp_up_time_1st;
    }else if( delta_t > ramp_up_time_1st && delta_t <= ramp_up_time_1st + delay_effect){
      relative_reduction = individuals->vaccine_efficacy_1st_d[idx_i];
    }
    else if( delta_t > ramp_up_time_1st + delay_effect && delta_t <= ramp_up_time_1st + delay_effect + ramp_up_time_2nd ){
      relative_reduction = individuals->vaccine_efficacy_1st_d[idx_i] + (individuals->vaccine_efficacy_2nd_d[idx_i] - individuals->vaccine_efficacy_1st_d[idx_i]) * (delta_t - ramp_up_time_1st - delay_effect) / ramp_up_time_2nd;
    }
    else if( delta_t > ramp_up_time_1st + delay_effect + ramp_up_time_2nd ){
      relative_reduction = individuals->vaccine_efficacy_2nd_d[idx_i];
    }
  } // vaccinated
  prob_death *= 1 - relative_reduction;

  if( gsl_ran_bernoulli( R_GLOBAL, prob_death ) == 1 ){ // will die
    individuals->death[idx_i] = 1;
  }

} // static void death

static void h_transmission(unsigned int idx_i, unsigned int idx_h, Households *households, Individuals *individuals, Prob_trans *pt, unsigned int r,Counter *counter , int t, FILE * fiww){

  register int i, j;
  int n;
  int contacts = 0;
  double prob_trans = 0;
  register int index;
  unsigned int *a, *b;
  int doit;
  int maxcontacts;
  int idx_hm;
  n = 0;
  doit=0;
  int county = individuals->mun[idx_i] - 1;

  for( i=1 ; i <= households->household[idx_h].n_i ; i++){ // all individuals in the house
    idx_hm = households->household[idx_h].idx_i[i];

    if( individuals->status[idx_hm] == S ){
      doit=1;
    }
    n++;
  }

  if(doit==0) // skip if no susceptible in the house
  return;

  if( n > households->household[idx_h].n_i ){
    fprintf(stderr, "Error in h_transmission: maxcontacts larger than household size\n");
    exit(1);
  }

  maxcontacts = households->household[idx_h].n_i - 1;

  // Vaccine protection against transmission
  double VE_V = 0;
  int vac_i = individuals->vaccinated[idx_i];
  if( vac_i > 0 ){ // vaccinated
    int ramp_up_time_1st = param.RAMP_UP_TIME[0][vac_i-1];
    int delay_effect     = param.DELAY_EFFECT[vac_i-1];
    int ramp_up_time_2nd = param.RAMP_UP_TIME[1][vac_i-1];

    int delta_t = t - individuals->time_vaccination[idx_i];
    if( delta_t <= 0 ){
      VE_V = 0;
    }else if( delta_t > 0 && delta_t <= ramp_up_time_1st ){
      VE_V = individuals->vaccine_efficacy_1st_V[idx_i] * delta_t / ramp_up_time_1st;
    }else if( delta_t > ramp_up_time_1st && delta_t <= ramp_up_time_1st + delay_effect){
      VE_V = individuals->vaccine_efficacy_1st_V[idx_i];
    }else if( delta_t > ramp_up_time_1st + delay_effect && delta_t <= ramp_up_time_1st + delay_effect + ramp_up_time_2nd ){
      VE_V = individuals->vaccine_efficacy_1st_V[idx_i] + (individuals->vaccine_efficacy_2nd_V[idx_i] - individuals->vaccine_efficacy_1st_V[idx_i]) * (delta_t - ramp_up_time_1st - delay_effect) / ramp_up_time_2nd;
    }else if( delta_t > ramp_up_time_1st + delay_effect + ramp_up_time_2nd ){
      VE_V = individuals->vaccine_efficacy_2nd_V[idx_i];
    }
  } // vaccinated

  switch( individuals->status[idx_i] ){
    case I:
    prob_trans = 1.-exp( -                        param.BETA_H*param.RELATIVE_TRANS_MUN[county]*param.DELTAT/n * (1.0 - VE_V));
    break;
    case Ia:
    prob_trans = 1.-exp( -param.RELATIVE_INF_AS  *param.BETA_H*param.RELATIVE_TRANS_MUN[county]*param.DELTAT/n * (1.0 - VE_V));
    break;
    case E2: // COULD ALSO APPROXIMATE HERE BY SCALING WITH param.RELATIVE_INF_PRES
    prob_trans = 1.-exp( -param.RELATIVE_INF_PRES*param.BETA_H*param.RELATIVE_TRANS_MUN[county]*param.DELTAT/n * (1.0 - VE_V));
    break;
    fprintf(stderr,"error3");
    exit(1);
    break;
  }
  contacts = gsl_ran_binomial(R_GLOBAL, prob_trans, maxcontacts);
  if(contacts==0) // skip if no successful contact
  return;

  a = (unsigned int*)calloc( maxcontacts , sizeof(unsigned int) );
  b = (unsigned int*)calloc( maxcontacts , sizeof(unsigned int) );

  index=0;

  for( i=1 ; i <= households->household[idx_h].n_i ; i++ ){ // all individuals in the house
    if(households->household[idx_h].idx_i[i] == idx_i ) // except self-contact
    continue;
    b[index++] = households->household[idx_h].idx_i[i]; // index of individuals in the house
  }

  // random draw some individuals in the house
  gsl_ran_choose(R_GLOBAL, a, contacts, b, maxcontacts, sizeof(unsigned int));

  for( j=0; j<contacts; j++ ){

    if( individuals->status[a[j]] != S )
    continue;

    // Vaccine protection against infection
    double prob_Ia = 0; // probability of protection against asymptomatic
    double prob_I  = 0; // probability of protection against  symptomatic

    int vac_i = individuals->vaccinated[a[j]];
    if( vac_i > 0 ){ // vaccinated
      int ramp_up_time_1st = param.RAMP_UP_TIME[0][vac_i-1];
      int delay_effect     = param.DELAY_EFFECT[vac_i-1];
      int ramp_up_time_2nd = param.RAMP_UP_TIME[1][vac_i-1];

      int delta_t = t - individuals->time_vaccination[a[j]];
      if( delta_t <= 0 ){
        // probability of protection
        prob_Ia = 0;
        prob_I  = 0;
      }else if( delta_t > 0 && delta_t <= ramp_up_time_1st ){
        // probability of protection
        prob_Ia = individuals->vaccine_efficacy_1st_Ia[a[j]] * delta_t / ramp_up_time_1st;
        prob_I  = individuals->vaccine_efficacy_1st_I[a[j]]  * delta_t / ramp_up_time_1st;
      }else if( delta_t > ramp_up_time_1st && delta_t <= ramp_up_time_1st + delay_effect){
        // probability of protection
        prob_Ia = individuals->vaccine_efficacy_1st_Ia[a[j]];
        prob_I  = individuals->vaccine_efficacy_1st_I[a[j]];
      }
      else if( delta_t > ramp_up_time_1st + delay_effect && delta_t <= ramp_up_time_1st + delay_effect + ramp_up_time_2nd ){
        // probability of protection
        prob_Ia = individuals->vaccine_efficacy_1st_Ia[a[j]] + (individuals->vaccine_efficacy_2nd_Ia[a[j]] - individuals->vaccine_efficacy_1st_Ia[a[j]]) * (delta_t - ramp_up_time_1st - delay_effect) / ramp_up_time_2nd;
        prob_I  = individuals->vaccine_efficacy_1st_I[a[j]]  + (individuals->vaccine_efficacy_2nd_I[a[j]]  - individuals->vaccine_efficacy_1st_I[a[j]])  * (delta_t - ramp_up_time_1st - delay_effect) / ramp_up_time_2nd;
      }
      else if( delta_t > ramp_up_time_1st + delay_effect + ramp_up_time_2nd ){
        // probability of protection
        prob_Ia = individuals->vaccine_efficacy_2nd_Ia[a[j]];
        prob_I  = individuals->vaccine_efficacy_2nd_I[a[j]];
      }
    } // vaccinated

    double prob_asymp = individuals->age_prob_asymp[a[j]];
    if( gsl_ran_bernoulli(R_GLOBAL, prob_asymp) == 0 ){ // symptomatic (S -> E1)

      // probability of protection by vaccination
      if( gsl_ran_bernoulli(R_GLOBAL, prob_I) == 1 ) // not infected
      continue;

      // higher susceptibility means less likely to 'continue' over that ite
      if( gsl_ran_bernoulli(R_GLOBAL, individuals->age_sus[a[j]]) == 0 ) // not infected
      continue;

      individuals->status[a[j]] = E1;
      next_trans_func(a[j],individuals,t,param.E1_GAMMA_ALPHA,param.E1_GAMMA_BETA,1);  // next_trans
      individuals->time_of_infection[a[j]] = t;

      if( VERBOSE != -1 )
      fprintf( fiww, "%d\t%d\t%s\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%s\t%d\t%d\t%d\t%d\t%lf\t%lf\t%d\t%d\n",t, a[j], "at_home", "symptomat", 9999,
      idx_i,individuals->cell[idx_i],individuals->mun[idx_i],individuals->cell[a[j]],individuals->age[idx_i],individuals->idx_h[idx_i],
      individuals->idx_h[a[j]],"redun_home_type",individuals->status[a[j]], individuals->age[a[j]],
      individuals->status[idx_i], individuals->county[idx_i], individuals->x[idx_i], individuals->y[idx_i],
      individuals->vaccinated[idx_i], individuals->vaccinated[a[j]]);

      // output conuter
      int age_i       = individuals->age[a[j]];                          // age       of the individual
      int county_i    = individuals->county[a[j]];                       // county    of the individual
      int mun_i       = individuals->mun[a[j]] - 1;                      // municip   of the individual
      int cell_i      = individuals->cell[a[j]] - 1;                     // cell      of the individual
      counter->cum_I[age_i]++;
      counter->cum_I_county[county_i]++;
      counter->cum_I_mun[mun_i]++;
      counter->cum_I_cell[cell_i]++;

    } else { // asymptomatic (S -> E1a)

      // probability of protection by vaccination
      if( gsl_ran_bernoulli(R_GLOBAL, prob_Ia) == 1 ) // not infected
      continue;

      // higher susceptibility means less likely to 'continue' over that ite
      if( gsl_ran_bernoulli(R_GLOBAL, individuals->age_sus[a[j]]) == 0 ) // not infected
      continue;

      individuals->status[a[j]] = E1a;
      next_trans_func(a[j],individuals,t,param.E1a_GAMMA_ALPHA,param.E1a_GAMMA_BETA,1);  // next_trans
      individuals->time_of_infection[a[j]] = t;

      if( VERBOSE != -1 )
      fprintf( fiww, "%d\t%d\t%s\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%s\t%d\t%d\t%d\t%d\t%lf\t%lf\t%d\t%d\n",t, a[j], "at_home", "not_symptomat", 9999,
      idx_i,individuals->cell[idx_i],individuals->mun[idx_i],individuals->cell[a[j]],individuals->age[idx_i],individuals->idx_h[idx_i],
      individuals->idx_h[a[j]],"redun_home_type",individuals->status[a[j]], individuals->age[a[j]],
      individuals->status[idx_i], individuals->county[idx_i], individuals->x[idx_i], individuals->y[idx_i],
      individuals->vaccinated[idx_i], individuals->vaccinated[a[j]]);

      // output conuter
      int age_i       = individuals->age[a[j]];                          // age       of the individual
      int county_i    = individuals->county[a[j]];                       // county    of the individual
      int mun_i       = individuals->mun[a[j]] - 1;                      // municip   of the individual
      int cell_i      = individuals->cell[a[j]] - 1;                     // cell      of the individual
      counter->cum_I[age_i]++;
      counter->cum_I_county[county_i]++;
      counter->cum_I_mun[mun_i]++;
      counter->cum_I_cell[cell_i]++;

    }

    individuals->no_of_infected[idx_i]++;
    counter->inf_household++;

  }
  free(a);
  free(b);

  return;
} // static void h_transmission

static void p_transmission( Grid *grid, unsigned int idx_i, unsigned int idx_p, Places *places, Individuals *individuals, Prob_trans *pt, unsigned int r, Counter *counter, int t,FILE * fiww){

  register int i,j;
  int contacts;
  double prob_trans = 0;
  register int index;
  unsigned int *a, *b;
  int doit;
  int maxcontacts;
  char *place_string;
  int n = 0;
  int county = grid->cell[ places->place[idx_p].idx_c ].mun_ID - 1;

  if( individuals->hospitalized[idx_i] != NO ){
    return;
  }

  if( individuals->home_office[idx_i] == 1 ){
    return;
  }

  doit=0;

  for( i = 1; i <= places->place[idx_p].n_i; i++ ){
    // workplaces
    if( places->place[idx_p].type == P1 ){
      if( individuals->status[places->place[idx_p].idx_i[i]] == S ){
        doit=1;
      }
      if( individuals->home_office[  places->place[idx_p].idx_i[i] ] == 0 && individuals->hospitalized[ places->place[idx_p].idx_i[i] ] == NO ){
        n++;
      }
    }

    // schools
    if( places->place[idx_p].type == P00 || places->place[idx_p].type == P01 || places->place[idx_p].type == P03 || places->place[idx_p].type == P04  ){
      if( individuals->status[places->place[idx_p].idx_i[i]] == S ){
        doit=1;
      }
      if( individuals->hospitalized[ places->place[idx_p].idx_i[i] ] == NO ){
        n++;
      }
    }
  }

  if(doit==0)
  return;

  maxcontacts = n - 1;

  if( maxcontacts <= 0 )
  return;

  if(       individuals->employment[idx_i] == P1 ){
    place_string = "at_workplace";
  }else if( individuals->employment[idx_i] == P04 ){
    place_string = "at_uni";
  }else{
    place_string = "at_school";
  }

  // Vaccine protection against transmission
  double VE_V = 0;
  int vac_i = individuals->vaccinated[idx_i];
  if( vac_i > 0 ){ // vaccinated
    int ramp_up_time_1st = param.RAMP_UP_TIME[0][vac_i-1];
    int delay_effect     = param.DELAY_EFFECT[vac_i-1];
    int ramp_up_time_2nd = param.RAMP_UP_TIME[1][vac_i-1];

    int delta_t = t - individuals->time_vaccination[idx_i];
    if( delta_t <= 0 ){
      VE_V = 0;
    }else if( delta_t > 0 && delta_t <= ramp_up_time_1st ){
      VE_V = individuals->vaccine_efficacy_1st_V[idx_i] * delta_t / ramp_up_time_1st;
    }else if( delta_t > ramp_up_time_1st && delta_t <= ramp_up_time_1st + delay_effect){
      VE_V = individuals->vaccine_efficacy_1st_V[idx_i];
    }else if( delta_t > ramp_up_time_1st + delay_effect && delta_t <= ramp_up_time_1st + delay_effect + ramp_up_time_2nd ){
      VE_V = individuals->vaccine_efficacy_1st_V[idx_i] + (individuals->vaccine_efficacy_2nd_V[idx_i] - individuals->vaccine_efficacy_1st_V[idx_i]) * (delta_t - ramp_up_time_1st - delay_effect) / ramp_up_time_2nd;
    }else if( delta_t > ramp_up_time_1st + delay_effect + ramp_up_time_2nd ){
      VE_V = individuals->vaccine_efficacy_2nd_V[idx_i];
    }
  } // vaccinated

  switch(places->place[idx_p].type){
    case P00:
    case P01:
    case P02:
    case P03:
    switch(individuals->status[idx_i]){
      case I:
      prob_trans = 1.-exp( -                        param.BETA_P0*param.RELATIVE_TRANS_MUN[county]*param.DELTAT/n * (1.0 - VE_V));
      break;
      case Ia:
      prob_trans = 1.-exp( -param.RELATIVE_INF_AS  *param.BETA_P0*param.RELATIVE_TRANS_MUN[county]*param.DELTAT/n * (1.0 - VE_V));
      break;
      case E2:
      prob_trans = 1.-exp( -param.RELATIVE_INF_PRES*param.BETA_P0*param.RELATIVE_TRANS_MUN[county]*param.DELTAT/n * (1.0 - VE_V));
      break;
      default:
      fprintf(stderr,"error5");
      exit(1);
      break;
    }
    break;

    case P04:
    case P1:
    switch(individuals->status[idx_i]){
      case I:
      prob_trans = 1.-exp( -                        param.BETA_P1*param.RELATIVE_TRANS_MUN[county]*param.DELTAT/n * (1.0 - VE_V));
      break;
      case Ia:
      prob_trans = 1.-exp( -param.RELATIVE_INF_AS  *param.BETA_P1*param.RELATIVE_TRANS_MUN[county]*param.DELTAT/n * (1.0 - VE_V));
      break;
      case E2:
      prob_trans = 1.-exp( -param.RELATIVE_INF_PRES*param.BETA_P1*param.RELATIVE_TRANS_MUN[county]*param.DELTAT/n * (1.0 - VE_V));
      break;
      default:
      fprintf(stderr,"error5");
      exit(1);
      break;
    }
    break;

    default:
    fprintf(stderr,"error7");
    exit(1);
    break;
  }

  if(maxcontacts<=20)
  contacts=gsl_ran_binomial(R_GLOBAL, prob_trans, maxcontacts);
  else
  contacts=gsl_ran_poisson( R_GLOBAL, prob_trans * maxcontacts);

  if(contacts==0)
  return;

  a = ( unsigned int* )calloc( maxcontacts+1, sizeof(unsigned int));
  b = ( unsigned int* )calloc( maxcontacts+1, sizeof(unsigned int));

  index=0;

  for( i=1; i <= places->place[idx_p].n_i; i++ ){

    if(places->place[idx_p].idx_i[i] == idx_i)
    continue;

    if( individuals->hospitalized[ places->place[idx_p].idx_i[i] ] != NO )
    continue;

    if( individuals->home_office[ places->place[idx_p].idx_i[i] ]  == 1 )
    continue;

    b[index++] = places->place[idx_p].idx_i[i];

  }

  gsl_ran_choose(R_GLOBAL, a, contacts, b, maxcontacts, sizeof(unsigned int));

  for( j=0; j<contacts; j++ ){

    if( individuals->status[a[j]] != S )
    continue;

    // Vaccine protection against infection
    double prob_Ia = 0; // probability of protection against asymptomatic
    double prob_I  = 0; // probability of protection against  symptomatic

    int vac_i = individuals->vaccinated[a[j]];
    if( vac_i > 0 ){ // vaccinated
      int ramp_up_time_1st = param.RAMP_UP_TIME[0][vac_i-1];
      int delay_effect     = param.DELAY_EFFECT[vac_i-1];
      int ramp_up_time_2nd = param.RAMP_UP_TIME[1][vac_i-1];

      int delta_t = t - individuals->time_vaccination[a[j]];
      if( delta_t <= 0 ){
        // probability of protection
        prob_Ia = 0;
        prob_I  = 0;
      }else if( delta_t > 0 && delta_t <= ramp_up_time_1st ){
        // probability of protection
        prob_Ia = individuals->vaccine_efficacy_1st_Ia[a[j]] * delta_t / ramp_up_time_1st;
        prob_I  = individuals->vaccine_efficacy_1st_I[a[j]]  * delta_t / ramp_up_time_1st;
      }else if( delta_t > ramp_up_time_1st && delta_t <= ramp_up_time_1st + delay_effect){
        // probability of protection
        prob_Ia = individuals->vaccine_efficacy_1st_Ia[a[j]];
        prob_I  = individuals->vaccine_efficacy_1st_I[a[j]];
      }
      else if( delta_t > ramp_up_time_1st + delay_effect && delta_t <= ramp_up_time_1st + delay_effect + ramp_up_time_2nd ){
        // probability of protection
        prob_Ia = individuals->vaccine_efficacy_1st_Ia[a[j]] + (individuals->vaccine_efficacy_2nd_Ia[a[j]] - individuals->vaccine_efficacy_1st_Ia[a[j]]) * (delta_t - ramp_up_time_1st - delay_effect) / ramp_up_time_2nd;
        prob_I  = individuals->vaccine_efficacy_1st_I[a[j]]  + (individuals->vaccine_efficacy_2nd_I[a[j]]  - individuals->vaccine_efficacy_1st_I[a[j]])  * (delta_t - ramp_up_time_1st - delay_effect) / ramp_up_time_2nd;
      }
      else if( delta_t > ramp_up_time_1st + delay_effect + ramp_up_time_2nd ){
        // probability of protection
        prob_Ia = individuals->vaccine_efficacy_2nd_Ia[a[j]];
        prob_I  = individuals->vaccine_efficacy_2nd_I[a[j]];
      }
    } // vaccinated

    double prob_asymp = individuals->age_prob_asymp[a[j]];
    if( gsl_ran_bernoulli(R_GLOBAL, prob_asymp) == 0 ){ // symptomatic (S -> E1)

      // probability of protection by vaccination
      if( gsl_ran_bernoulli(R_GLOBAL, prob_I) == 1 ) // not infected
      continue;

      // higher susceptibility means less likely to 'continue' over that ite
      if( gsl_ran_bernoulli(R_GLOBAL, individuals->age_sus[a[j]]) == 0 ) // not infected
      continue;

      individuals->status[a[j]] = E1;  // come back
      next_trans_func(a[j],individuals,t,param.E1_GAMMA_ALPHA,param.E1_GAMMA_BETA,1);  // next_trans
      individuals->time_of_infection[a[j]] = t;

      if( VERBOSE != -1 )
      fprintf( fiww, "%d\t%d\t%s\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%lf\t%lf\t%d\t%d\n", t, a[j], place_string, "symptomat", idx_p, idx_i,
      individuals->cell[idx_i],individuals->mun[idx_i],individuals->cell[a[j]],
      individuals->age[idx_i],individuals->idx_h[idx_i],individuals->idx_h[a[j]],
      places->place[idx_p].type,individuals->status[a[j]], individuals->age[a[j]],
      individuals->status[idx_i], individuals->county[idx_i], individuals->x[idx_i], individuals->y[idx_i],
      individuals->vaccinated[idx_i], individuals->vaccinated[a[j]]);

      // output conuter
      int age_i       = individuals->age[a[j]];                          // age       of the individual
      int county_i    = individuals->county[a[j]];                       // county    of the individual
      int mun_i       = individuals->mun[a[j]] - 1;                      // municip   of the individual
      int cell_i      = individuals->cell[a[j]] - 1;                     // cell      of the individual
      counter->cum_I[age_i]++;
      counter->cum_I_county[county_i]++;
      counter->cum_I_mun[mun_i]++;
      counter->cum_I_cell[cell_i]++;

    } else { // asymptomatic (S -> E1a)

      // probability of protection by vaccination
      if( gsl_ran_bernoulli(R_GLOBAL, prob_Ia) == 1 ) // not infected
      continue;

      // higher susceptibility means less likely to 'continue' over that ite
      if( gsl_ran_bernoulli(R_GLOBAL, individuals->age_sus[a[j]]) == 0 ) // not infected
      continue;

      individuals->status[a[j]] = E1a;
      next_trans_func(a[j],individuals,t,param.E1a_GAMMA_ALPHA,param.E1a_GAMMA_BETA,1);  // next_trans
      individuals->time_of_infection[a[j]] = t;

      if( VERBOSE != -1 )
      fprintf( fiww, "%d\t%d\t%s\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%lf\t%lf\t%d\t%d\n",t, a[j], place_string, "not_symptomat", idx_p, idx_i,
      individuals->cell[idx_i],individuals->mun[idx_i],individuals->cell[a[j]],
      individuals->age[idx_i],individuals->idx_h[idx_i],individuals->idx_h[a[j]],
      places->place[idx_p].type,individuals->status[a[j]], individuals->age[a[j]],
      individuals->status[idx_i], individuals->county[idx_i], individuals->x[idx_i], individuals->y[idx_i],
      individuals->vaccinated[idx_i], individuals->vaccinated[a[j]]);

      // output conuter
      int age_i       = individuals->age[a[j]];                          // age       of the individual
      int county_i    = individuals->county[a[j]];                       // county    of the individual
      int mun_i       = individuals->mun[a[j]] - 1;                      // municip   of the individual
      int cell_i      = individuals->cell[a[j]] - 1;                     // cell      of the individual
      counter->cum_I[age_i]++;
      counter->cum_I_county[county_i]++;
      counter->cum_I_mun[mun_i]++;
      counter->cum_I_cell[cell_i]++;

    }

    individuals->no_of_infected[idx_i]++;

    if( individuals->employment[idx_i] == P1  )
    counter->inf_work++;
    if( individuals->employment[idx_i] == P00 || individuals->employment[idx_i] == P01 || individuals->employment[idx_i] == P02 || individuals->employment[idx_i] == P03 )
    counter->inf_school++;
    if( individuals->employment[idx_i] == P04 )
    counter->inf_uni++;

  }

  free(a);
  free(b);

  return;
} // static void p_transmission

static void r_transmission_individual_level(unsigned int r,unsigned int c, Grid *grid,Individuals *individuals, Counter *counter, Probability *probability, int t, FILE *fiww, gsl_ran_discrete_t* dist_sub){

  int ncontacts = 0;
  int idx_i, idx_c, id_x;
  double dist_between;
  double *RELATIVE_COMM_CONTACTS;
  double mean_contacts;
  unsigned short int id_x_stat;
  int quar_logic;
  double size_par= 0.1; // Akira Endo et al., Wellcome Oper Research 2020.
  double prob;
  int neg_binomial = 1;
  int age_cat;
  int cat_age, cat_age2;
  gsl_ran_discrete_t **comm_contact;
  int found , count;
  int  idx_age_group;
  int n_age_groups = 9;
  int mun;

  RELATIVE_COMM_CONTACTS = (double *) calloc( 11, sizeof(double));
  RELATIVE_COMM_CONTACTS[0]  = 1.;
  RELATIVE_COMM_CONTACTS[1]  = 1.366;
  RELATIVE_COMM_CONTACTS[2]  = 1.844;
  RELATIVE_COMM_CONTACTS[3]  = 1.551;
  RELATIVE_COMM_CONTACTS[4]  = 1.539;
  RELATIVE_COMM_CONTACTS[5]  = 1.171;
  RELATIVE_COMM_CONTACTS[6]  = 1.726;
  RELATIVE_COMM_CONTACTS[7]  = 1.114;
  RELATIVE_COMM_CONTACTS[8]  = 0.926;
  RELATIVE_COMM_CONTACTS[9]  = 0.926;
  RELATIVE_COMM_CONTACTS[10] = 0.926;

  comm_contact = (gsl_ran_discrete_t **) calloc( n_age_groups, sizeof( gsl_ran_discrete_t* ) );

  for( int i=0; i<n_age_groups; i++ ){ // 9 age groups
    comm_contact[i] = (gsl_ran_discrete_t *) calloc( n_age_groups, sizeof( gsl_ran_discrete_t ) );
    comm_contact[i] = gsl_ran_discrete_preproc( n_age_groups, probability->contact_matrix[i] );
  } // 9 age groups

  for( id_x = grid->cell[c].idx_i_ini; id_x <= grid->cell[c].idx_i_fin; id_x++ ){ // all individuals in the cell

    id_x_stat = individuals->status[id_x]; // status of the individual
    mun = individuals->mun[id_x] - 1;      // municipality of the individual

    if( id_x_stat != I && id_x_stat != Ia && id_x_stat != E2 ) // skip non-infectious individuals
    continue;

    // Note: 'quar_logic' changed to be infeffective for the variable home office (it is always TRUE)
    // quar_logic = ( individuals->home_office[id_x]  == 1 );
    quar_logic = ( individuals->home_office[id_x]  == 1 || individuals->home_office[id_x]  == 0 );

    age_cat = floor( individuals->age[id_x] / 10.0); // 11 age groups

    // Vaccine protection against transmission
    double VE_V = 0;
    int vac_i = individuals->vaccinated[id_x];
    if( vac_i > 0 ){ // vaccinated
      int ramp_up_time_1st = param.RAMP_UP_TIME[0][vac_i-1];
      int delay_effect     = param.DELAY_EFFECT[vac_i-1];
      int ramp_up_time_2nd = param.RAMP_UP_TIME[1][vac_i-1];

      int delta_t = t - individuals->time_vaccination[id_x];
      if( delta_t <= 0 ){
        VE_V = 0;
      }else if( delta_t > 0 && delta_t <= ramp_up_time_1st ){
        VE_V = individuals->vaccine_efficacy_1st_V[id_x] * delta_t / ramp_up_time_1st;
      }else if( delta_t > ramp_up_time_1st && delta_t <= ramp_up_time_1st + delay_effect){
        VE_V = individuals->vaccine_efficacy_1st_V[id_x];
      }else if( delta_t > ramp_up_time_1st + delay_effect && delta_t <= ramp_up_time_1st + delay_effect + ramp_up_time_2nd ){
        VE_V = individuals->vaccine_efficacy_1st_V[id_x] + (individuals->vaccine_efficacy_2nd_V[id_x] - individuals->vaccine_efficacy_1st_V[id_x]) * (delta_t - ramp_up_time_1st - delay_effect) / ramp_up_time_2nd;
      }else if( delta_t > ramp_up_time_1st + delay_effect + ramp_up_time_2nd ){
        VE_V = individuals->vaccine_efficacy_2nd_V[id_x];
      }
    } // vaccinated

    mean_contacts = param.BETA_R*param.DELTAT*RELATIVE_COMM_CONTACTS[age_cat]*param.RELATIVE_TRANS_MUN[mun]*(1.0 - VE_V);

    if( quar_logic ){
      switch( id_x_stat ){
        case I:
        ncontacts = gsl_ran_poisson( R_GLOBAL, mean_contacts );
        if( neg_binomial == 1 ){
          prob = size_par / ( mean_contacts + size_par );
          ncontacts = gsl_ran_negative_binomial( R_GLOBAL, prob, size_par );
        }
        break;

        case Ia:
        ncontacts = gsl_ran_poisson( R_GLOBAL, param.RELATIVE_INF_AS*mean_contacts );
        if( neg_binomial == 1 ){
          prob = size_par / ( param.RELATIVE_INF_AS*mean_contacts + size_par );
          ncontacts = gsl_ran_negative_binomial( R_GLOBAL, prob, size_par );
        }
        break;

        case E2:
        ncontacts = gsl_ran_poisson( R_GLOBAL, param.RELATIVE_INF_PRES*mean_contacts );
        if( neg_binomial == 1 ){
          prob = size_par / ( param.RELATIVE_INF_PRES*mean_contacts + size_par );
          ncontacts = gsl_ran_negative_binomial( R_GLOBAL, prob, size_par );
        }
        break;
      } // id_x_stat

      idx_c = gsl_ran_discrete(R_GLOBAL, dist_sub);

      if(idx_c == 0){ // wrong cell selected (index of cell: from 1 to 4978)
        fprintf(stderr, "Hey selected cell id = 0\n"  );
        exit(1);
      }

      dist_between = haversine_dist(grid->cell[c].x, grid->cell[c].y, grid->cell[idx_c].x, grid->cell[idx_c].y);

      cat_age = floor( individuals->age[id_x]/10.);
      if( cat_age >= 8 ) cat_age = 8; // 9 age groups

      for( int n = 0; n < ncontacts; n++ ){
        idx_age_group = gsl_ran_discrete( R_GLOBAL, comm_contact[cat_age] ); // select an age-group

        found = 0;
        count = 0;

        if( idx_age_group >= n_age_groups){ // wrong age group selected (9 age groups)
          fprintf(stderr, "Error in r_transmission_individual_level(): sampled age group larger then max (%d) \n", n_age_groups );
          exit(1);
        }

        while(found == 0){

          idx_i = grid->cell[idx_c].idx_i_ini + gsl_rng_uniform_int( R_GLOBAL, grid->cell[idx_c].n_i ); // potential individual

          cat_age2 =  (int) floor(individuals->age[idx_i] / 10) ;
          if( cat_age2 > 8 ) cat_age2 = 8; // 9 age groups

          if( cat_age2 == idx_age_group ) // match with targeted age group
          found = 1;

          count++;
          if( count == 100 ){ // cannot find individual in targeted age group
            idx_c = gsl_ran_discrete(R_GLOBAL, dist_sub); // switch to other cell
            count=0;
          } // count
        } // found

        if(individuals->status[idx_i] == S){

          // Vaccine protection against infection
          double prob_Ia = 0; // probability of protection against asymptomatic
          double prob_I  = 0; // probability of protection against  symptomatic
          int vac_i = individuals->vaccinated[idx_i];
          if( vac_i > 0 ){ // vaccinated
            int ramp_up_time_1st = param.RAMP_UP_TIME[0][vac_i-1];
            int delay_effect     = param.DELAY_EFFECT[vac_i-1];
            int ramp_up_time_2nd = param.RAMP_UP_TIME[1][vac_i-1];

            int delta_t = t - individuals->time_vaccination[idx_i];
            if( delta_t <= 0 ){
              // probability of protection
              prob_Ia = 0;
              prob_I  = 0;
            }else if( delta_t > 0 && delta_t <= ramp_up_time_1st ){
              // probability of protection
              prob_Ia = individuals->vaccine_efficacy_1st_Ia[idx_i] * delta_t / ramp_up_time_1st;
              prob_I  = individuals->vaccine_efficacy_1st_I[idx_i]  * delta_t / ramp_up_time_1st;
            }else if( delta_t > ramp_up_time_1st && delta_t <= ramp_up_time_1st + delay_effect){
              // probability of protection
              prob_Ia = individuals->vaccine_efficacy_1st_Ia[idx_i];
              prob_I  = individuals->vaccine_efficacy_1st_I[idx_i];
            }
            else if( delta_t > ramp_up_time_1st + delay_effect && delta_t <= ramp_up_time_1st + delay_effect + ramp_up_time_2nd ){
              // probability of protection
              prob_Ia = individuals->vaccine_efficacy_1st_Ia[idx_i] + (individuals->vaccine_efficacy_2nd_Ia[idx_i] - individuals->vaccine_efficacy_1st_Ia[idx_i]) * (delta_t - ramp_up_time_1st - delay_effect) / ramp_up_time_2nd;
              prob_I  = individuals->vaccine_efficacy_1st_I[idx_i]  + (individuals->vaccine_efficacy_2nd_I[idx_i]  - individuals->vaccine_efficacy_1st_I[idx_i])  * (delta_t - ramp_up_time_1st - delay_effect) / ramp_up_time_2nd;
            }
            else if( delta_t > ramp_up_time_1st + delay_effect + ramp_up_time_2nd ){
              // probability of protection
              prob_Ia = individuals->vaccine_efficacy_2nd_Ia[idx_i];
              prob_I  = individuals->vaccine_efficacy_2nd_I[idx_i];
            }
          } // vaccinated

          double prob_asymp = individuals->age_prob_asymp[idx_i];
          if( gsl_ran_bernoulli( R_GLOBAL, prob_asymp ) == 0 ){ // symptomatic (S -> E1)

            // probability of protection by vaccination
            if( gsl_ran_bernoulli(R_GLOBAL, prob_I) == 1 ) // not infected
            continue;

            // higher susceptibility means less likely to 'continue' over that ite
            if( gsl_ran_bernoulli(R_GLOBAL, individuals->age_sus[idx_i] ) == 0) // not infected
            continue;

            individuals->status[idx_i] = E1;
            next_trans_func( idx_i, individuals, t, param.E1_GAMMA_ALPHA, param.E1_GAMMA_BETA, 1 );
            individuals->time_of_infection[idx_i] = t;

            if( VERBOSE != -1 )
            fprintf( fiww, "%d\t%d\t%s\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%lf\t%d\t%d\t%d\t%d\t%lf\t%lf\t%d\t%d\n",
            t, idx_i, "community", "symptomat", 9999, id_x, individuals->cell[id_x],
            individuals->mun[id_x],individuals->cell[idx_i], individuals->age[id_x],
            individuals->idx_h[id_x],individuals->idx_h[idx_i], dist_between ,
            individuals->status[idx_i], individuals->age[idx_i], individuals->status[id_x], individuals->county[id_x], individuals->x[id_x], individuals->y[id_x],
            individuals->vaccinated[id_x], individuals->vaccinated[idx_i]);

            // output conuter
            int age_i       = individuals->age[idx_i];                          // age       of the individual
            int county_i    = individuals->county[idx_i];                       // county    of the individual
            int mun_i       = individuals->mun[idx_i] - 1;                      // municip   of the individual
            int cell_i      = individuals->cell[idx_i] - 1;                     // cell      of the individual
            counter->cum_I[age_i]++;
            counter->cum_I_county[county_i]++;
            counter->cum_I_mun[mun_i]++;
            counter->cum_I_cell[cell_i]++;

          } else { // asymptomatic (S -> E1a)

            // probability of protection by vaccination
            if( gsl_ran_bernoulli(R_GLOBAL, prob_Ia) == 1 ) // not infected
            continue;

            // higher susceptibility means less likely to 'continue' over that ite
            if( gsl_ran_bernoulli(R_GLOBAL, individuals->age_sus[idx_i] ) == 0) // not infected
            continue;

            individuals->status[idx_i] = E1a;
            next_trans_func(idx_i,individuals,t,param.E1a_GAMMA_ALPHA,param.E1a_GAMMA_BETA,1);  // next_trans
            individuals->time_of_infection[idx_i] = t;

            if( VERBOSE != -1 )
            fprintf( fiww, "%d\t%d\t%s\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%lf\t%d\t%d\t%d\t%d\t%lf\t%lf\t%d\t%d\n",
            t, idx_i, "community", "not_symptomat", 9999, id_x, individuals->cell[id_x],
            individuals->mun[id_x],individuals->cell[idx_i], individuals->age[id_x],
            individuals->idx_h[id_x],individuals->idx_h[idx_i], dist_between ,
            individuals->status[idx_i], individuals->age[idx_i], individuals->status[id_x], individuals->county[id_x], individuals->x[id_x], individuals->y[id_x],
            individuals->vaccinated[id_x], individuals->vaccinated[idx_i]);

            // output conuter
            int age_i       = individuals->age[idx_i];                          // age       of the individual
            int county_i    = individuals->county[idx_i];                       // county    of the individual
            int mun_i       = individuals->mun[idx_i] - 1;                      // municip   of the individual
            int cell_i      = individuals->cell[idx_i] - 1;                     // cell      of the individual
            counter->cum_I[age_i]++;
            counter->cum_I_county[county_i]++;
            counter->cum_I_mun[mun_i]++;
            counter->cum_I_cell[cell_i]++;
          }

          individuals->no_of_infected[id_x]++;
          counter->inf_community++;

        }
      }
    } // quar_logic
  } // id_x: all individuals in the cell

  free(RELATIVE_COMM_CONTACTS);
  for( int i = 0; i < n_age_groups; i++ ) gsl_ran_discrete_free(comm_contact[i]);
  free(comm_contact);

  return;
} // static void r_transmission_individual_level

static void define_risk_groups(Grid *grid, Individuals *individuals, Households *households, Counter *counter, Matrix *matrix, Probability *probability, Vaccination *vaccination){

  // define risk category for each individuals: ########################################

  //  1. people with risk factors
  for( int idx_i = 1; idx_i <= individuals->n_i; idx_i++ ){

    int age_group = floor( individuals->age[idx_i] / 10. );
    if( age_group > 8 ) // we have a larger group for elderly: 80+
    age_group = 8;

    if( gsl_ran_bernoulli( R_GLOBAL, param.FRACTION_RG[age_group] ) == 1  ){ // risk group or not
      individuals->risk_category[idx_i] = 1; // define as risk group
    }
  }

  //  2. heatlhcare workers
  for( int k = 0; k < 9; k++ ){ // loop over age groups

    int n_hws_tmp = 0;
    while ( n_hws_tmp < param.N_HWS[k] ){ // number of healthcare workers

      int idx_i = 1 + gsl_rng_uniform_int( R_GLOBAL, individuals->n_i );
      int age_group = floor( individuals->age[idx_i] / 10. );
      if( age_group > 8 ) age_group = 8;

      // at work & non risk group & targeted age group
      if( individuals->employment[ idx_i ] == P1 && individuals->risk_category[ idx_i ] == 0 && age_group == k ){
        individuals->risk_category[ idx_i ] = 2; // define as healthcare workers
        n_hws_tmp++;
      }
    } // number of healthcare workers
  } // age groups
} // static void define_risk_groups

void correct_efficacy_param(){

  for(int dose = 0; dose < 2; dose++){
    for(int vac = 0; vac < 3; vac++){

      // against hospitalization
      fprintf(stderr, "param.VACCINE_EFF_S[%d][%d] = %lf >>>\t", dose, vac, param.VACCINE_EFF_S[dose][vac]);
      param.VACCINE_EFF_S[dose][vac] = 1 - ( ( 1 - param.VACCINE_EFF_S[dose][vac]  ) / ( 1 - param.VACCINE_EFF_I[dose][vac]   ) );
      fprintf(stderr, "%lf\n", param.VACCINE_EFF_S[dose][vac]);

      // against death
      fprintf(stderr, "param.VACCINE_EFF_D[%d][%d] = %lf >>>\t", dose, vac, param.VACCINE_EFF_D[dose][vac]);
      param.VACCINE_EFF_D[dose][vac] = 1 - ( ( 1 - param.VACCINE_EFF_D[dose][vac]  ) / ( 1 - param.VACCINE_EFF_I[dose][vac]   ) );
      fprintf(stderr, "%lf\n", param.VACCINE_EFF_D[dose][vac]);

    }
  }
} // void correct_efficacy_param

void vaccination_initialized(Individuals *individuals, Vaccination *vaccination, FILE *file, Grid *grid){

  // read data (VaksinertPerKommune.txt) #######################################
  // file name
  char *filename_vpk;                                                           // pointer
  filename_vpk = (char*) calloc(1000, sizeof(char));                            // contiguous allocation
  sprintf(filename_vpk, "%s/input_files/VaksinertPerKommune.txt", EXP_DIR);     // file location
  // read txt file
  FILE *file_vpk;                                                               // pointer
  file_vpk = fopen(filename_vpk, "r");                                          // read file
  // number of rows in txt_row
  int txt_row_N = 0;
  char txt_line;
  for(txt_line = getc(file_vpk); txt_line != EOF; txt_line = getc(file_vpk)){
    if(txt_line == '\n') txt_row_N = txt_row_N + 1;
  }
  fclose(file_vpk);
  // close file
  // 7 columns in vpk (VaksinertPerKommune.txt)

  int *vpk_time;                                                             // pointer: time
  int *vpk_county;                                                           // pointer: county (and NA)
  int *vpk_age_group;                                                        // pointer: age group
  int *vpk_dose;                                                             // pointer: dose number
  int *vpk_vaccine;                                                          // pointer: vaccine (and NA)
  int *vpk_risk;                                                             // pointer: risk category (0 or 1)
  int *vpk_count;                                                            // pointer: count number

  vpk_time      = (int*) calloc(txt_row_N, sizeof(int));                     // contiguous allocation
  vpk_county    = (int*) calloc(txt_row_N, sizeof(int));                     // contiguous allocation
  vpk_age_group = (int*) calloc(txt_row_N, sizeof(int));                     // contiguous allocation
  vpk_dose      = (int*) calloc(txt_row_N, sizeof(int));                     // contiguous allocation
  vpk_vaccine   = (int*) calloc(txt_row_N, sizeof(int));                     // contiguous allocation
  vpk_risk      = (int*) calloc(txt_row_N, sizeof(int));                     // contiguous allocation
  vpk_count     = (int*) calloc(txt_row_N, sizeof(int));                     // contiguous allocation
  file_vpk = fopen(filename_vpk, "r"); // read file

  for(int txt_row = 0; txt_row < txt_row_N; txt_row++){ // all rows in txt
    // read value from file
    fscanf(file_vpk, "%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
    &vpk_time[txt_row],
    &vpk_county[txt_row],
    &vpk_age_group[txt_row],
    &vpk_dose[txt_row],
    &vpk_vaccine[txt_row],
    &vpk_risk[txt_row],
    &vpk_count[txt_row]);
  } // all rows in txt

  fclose(file_vpk);                                                             // close file
  // count vaccinated population for half 18-44 (or 30+)
  for( int mun_n = 0; mun_n < N_MUN ; mun_n++ ){ // 356 municipalities
    vaccination->half_vaccinated[mun_n] = 0;
  } // 356 municipalities

  int ****mun_age_rg_idxi;  // store idx of individuals for each municipality, age-group and risk group
  int ***n_mun_age_rg_idxi; // store the number of individuals in each municipality, age-group and risk group

  int ***age_rg_idmun;
  int **n_age_rg_idmun;

  int *n_ppl_rg;            // number of individuals in each risk group

  int N_age_groups  = 101;
  int N_risk_groups = 3;

  // Allocate memory
  mun_age_rg_idxi   = (int****) calloc( grid->n_mun, sizeof(int***) );
  n_mun_age_rg_idxi = (int***)  calloc( grid->n_mun, sizeof(int**) );

  age_rg_idmun = (int***) calloc( N_age_groups, sizeof(int**));
  n_age_rg_idmun = (int**) calloc( N_age_groups, sizeof(int*));

  n_ppl_rg = (int*)  calloc( N_risk_groups, sizeof(int) );

  for( int mun_i = 0; mun_i < grid->n_mun; mun_i++ ){
    mun_age_rg_idxi[mun_i]   = (int***) calloc( N_age_groups, sizeof(int**));// <----CHECK
    n_mun_age_rg_idxi[mun_i] = (int**)  calloc( N_age_groups, sizeof(int*));

    for( int age_group_i = 0; age_group_i < N_age_groups; age_group_i++ ){
      mun_age_rg_idxi[mun_i][age_group_i]   = (int**) calloc( N_risk_groups, sizeof(int*));
      n_mun_age_rg_idxi[mun_i][age_group_i] = (int*)  calloc( N_risk_groups, sizeof(int));

      if( mun_i == (grid->n_mun - 1) ){
        age_rg_idmun[age_group_i]   = (int**) calloc( N_risk_groups, sizeof(int*) );
        n_age_rg_idmun[age_group_i] = (int*) calloc( N_risk_groups, sizeof(int) );
      }
      for( int risk_i = 0; risk_i < N_risk_groups; risk_i++ ){
        mun_age_rg_idxi[mun_i][age_group_i][risk_i] = (int*) calloc( 1, sizeof(int) );

        if( mun_i == ( grid->n_mun - 1 ) ){
          n_age_rg_idmun[age_group_i][risk_i] = 0;
          age_rg_idmun[age_group_i][risk_i] = (int*) calloc( 1, sizeof(int) );
        }
        n_mun_age_rg_idxi[mun_i][age_group_i][risk_i] = 0;
      }
    }
  }

  // Store idx of people and id of municipalities
  for( int mun_i = 0; mun_i < grid->n_mun; mun_i++ ){
    int found_mx[ 101 ][3] = {0};

    for( int idx_mun_i = 0; idx_mun_i < grid->municipality[mun_i].n_i; idx_mun_i++ ){
      int idx_i = grid->municipality[mun_i].individuals[idx_mun_i];

      if( individuals->vaccinated[idx_i] == 0 ){
        int age_group_i = individuals->age[idx_i] ;
        int risk_i  = individuals->risk_category[idx_i];

        int n     = n_mun_age_rg_idxi[mun_i][age_group_i][risk_i];
        int n_mun = n_age_rg_idmun[age_group_i][risk_i];

        // idx of people
        mun_age_rg_idxi[mun_i][age_group_i][risk_i][n] = idx_i;
        n_mun_age_rg_idxi[mun_i][age_group_i][risk_i]++;
        mun_age_rg_idxi[mun_i][age_group_i][risk_i] = ( int* ) realloc( mun_age_rg_idxi[mun_i][age_group_i][risk_i], ( n_mun_age_rg_idxi[mun_i][age_group_i][risk_i] + 1 )*sizeof( int ));

        // id of mun
        if( found_mx[age_group_i][risk_i] == 0 ){
          age_rg_idmun[age_group_i][risk_i][n_mun] = mun_i;
          n_age_rg_idmun[age_group_i][risk_i]++;
          age_rg_idmun[age_group_i][risk_i] = ( int* ) realloc( age_rg_idmun[age_group_i][risk_i], ( n_age_rg_idmun[age_group_i][risk_i] + 1 )*sizeof( int ));
          found_mx[age_group_i][risk_i] = 1;
        }
        n_ppl_rg[risk_i]++;
      }
    }
  }

  // Age profile vaccinated
  gsl_ran_discrete_t *count_vx_age_gsl;
  double count_vx_age[ 101 ] = {0}; // count vaccine doses by age

  for(int txt_row = 0; txt_row < txt_row_N; txt_row++){   // all rows in txt
    if( vpk_dose[txt_row] == 1 ){  // only the 1st dose
      int group = vpk_age_group[txt_row];
      count_vx_age[group] += vpk_count[txt_row];
    }
  }

  count_vx_age_gsl = (gsl_ran_discrete_t *) calloc( N_age_groups, sizeof(gsl_ran_discrete_t));       // contiguous allocation
  count_vx_age_gsl = gsl_ran_discrete_preproc( N_age_groups, count_vx_age);                          // preprocessor

  // select individuals ########################################################
  fprintf( stderr, "select individuals\n" );

  for(int txt_row = 0; txt_row < txt_row_N; txt_row++){   // all rows in txt
    if( vpk_time[txt_row] <= 0 || param.KEEP_R_CONSTANT == 0){
      if(vpk_dose[txt_row] == 1){ // only the 1st dose

        int vpk_count_vaccinated = 0;     // counter of found individuals
        double adherence = 0.9;

        // update count_vx_age_gsl
        count_vx_age_gsl = gsl_ran_discrete_preproc( N_age_groups, count_vx_age); // preprocessor

        while( vpk_count_vaccinated < (int) round( vpk_count[txt_row] / adherence ) ){
          int idx_mun_i;
          int idx_i = -1;
          int mun_i       = vpk_county[txt_row];     // targeted municipality (or NA)
          int age_group_i = vpk_age_group[txt_row];  // targeted age group of the individual
          int risk_i      = vpk_risk[txt_row];       // targeted risk category of the individual
          int risk = -9999;

          int sample = 0;
          if( mun_i != 9999 && risk_i == 2 && ( n_mun_age_rg_idxi[mun_i][age_group_i][2] > 0 || n_mun_age_rg_idxi[mun_i][age_group_i][0] > 0 ))
          sample = 1;

          if( mun_i != 9999 && risk_i == 1 && n_mun_age_rg_idxi[mun_i][age_group_i][1] > 0 )
          sample = 1;

          // draw from a municipality, if there are available people
          if( mun_i != 9999 && sample == 1 ){
            if ( risk_i == 2 ){ // In the data file 2 includes hws and normal people

              // if there are hws available first sample from them
              if ( n_mun_age_rg_idxi[mun_i][age_group_i][2] > 0 ){
                risk = 2;
              }else{
                risk = 0;
              }

              int n_max = n_mun_age_rg_idxi[mun_i][age_group_i][risk];
              idx_mun_i = gsl_rng_uniform_int( R_GLOBAL, n_max );           // index of the vector
              idx_i = mun_age_rg_idxi[mun_i][age_group_i][risk][idx_mun_i]; // index of a hw

            }else{ // risk_i == 1 ( person in a risk group )

              risk = 1;
              int n_max = n_mun_age_rg_idxi[mun_i][age_group_i][risk];
              idx_mun_i = gsl_rng_uniform_int( R_GLOBAL, n_max );          // index of the vector
              idx_i = mun_age_rg_idxi[mun_i][age_group_i][risk][idx_mun_i]; // index of an individual
            }

          }else{ // draw from another municipality

            int found = 0;
            int count = 0;
            int n_max = 0;
            risk = vpk_risk[txt_row];

            while( n_max == 0 ){
              // Select the municipality
              if( n_age_rg_idmun[age_group_i][risk] >= 1 && risk == 2 ){ // check if there are available hws in the age-group

                int idx_m = gsl_rng_uniform_int(R_GLOBAL, n_age_rg_idmun[age_group_i][risk] );
                mun_i = age_rg_idmun[age_group_i][risk][idx_m];

              }else if( n_age_rg_idmun[age_group_i][0] >= 1  && risk == 2 ){ // check if there are ppl available with risk 0

                risk = 0;
                int idx_m = gsl_rng_uniform_int(R_GLOBAL, n_age_rg_idmun[age_group_i][risk] );
                mun_i = age_rg_idmun[age_group_i][risk][idx_m];

              }else if( n_age_rg_idmun[age_group_i][risk] >= 1  && risk == 1 ){ // check if there are ppl available with risk 1

                int idx_m = gsl_rng_uniform_int(R_GLOBAL, n_age_rg_idmun[age_group_i][risk] );
                mun_i = age_rg_idmun[age_group_i][risk][idx_m];

              }else{ // if there are not available people in that age-group

                int found_p = 0;
                int switch_to_uniform = 0;

                while( found_p == 0 ){
                  age_group_i = 0;

                  while( age_group_i < 12 ){

                    if( switch_to_uniform < 50000 )
                    age_group_i = gsl_ran_discrete(R_GLOBAL, count_vx_age_gsl );
                    else
                    age_group_i = gsl_rng_uniform_int(R_GLOBAL, N_age_groups  );
                  }

                  count++;
                  switch_to_uniform++;

                  if( risk == 2 ){ // hws + normal
                    if( n_age_rg_idmun[age_group_i][2] < 1 && n_age_rg_idmun[age_group_i][0] >= 1  )
                    risk = 0;
                  }

                  if ( count == 1000 && risk == 1 ){
                    risk = 0;
                    count=0;
                  }

                  if( n_age_rg_idmun[age_group_i][risk] >= 1 ){
                    int idx_m = gsl_rng_uniform_int(R_GLOBAL, n_age_rg_idmun[age_group_i][risk] );
                    mun_i = age_rg_idmun[age_group_i][risk][idx_m];
                    found_p = 1;
                  }
                }
              }

              n_max = n_mun_age_rg_idxi[mun_i][age_group_i][risk];
            }

            idx_mun_i = gsl_rng_uniform_int( R_GLOBAL, n_max );            // index of the vector
            idx_i = mun_age_rg_idxi[mun_i][age_group_i][risk][idx_mun_i]; // index of an individual
            found = 0;

            // identify position of sampled indivdudal in the matrix called mun_age_rg_idxi[][][]
            for( int idx = 0; idx < n_mun_age_rg_idxi[mun_i][age_group_i][risk]; idx++ ){
              int idx_i_2 = mun_age_rg_idxi[mun_i][age_group_i][risk][idx]; // index of an individual

              if( idx_i == idx_i_2 ){
                idx_mun_i = idx;
                found = 1;
              }
            }
            if( found == 0 ){
              fprintf(stderr, "Error: person not fount in vaccination_initialized() \n" );
              exit(1);
            }
          }

          // identify position of sampled municipality in the matrix called age_rg_idmun[][]
          int idx_mun;
          int found = 0;

          for( int idx = 0; idx < n_age_rg_idmun[age_group_i][risk]; idx++ ){
            int idx_mun_2 = age_rg_idmun[age_group_i][risk][idx];

            if( idx_mun_2 == mun_i){
              idx_mun = idx;
              found = 1;
            }
          }
          if( found == 0 ){
            fprintf(stderr, "Error: municipality not fount in vaccination_initialized() \n" );
            exit(1);
          }

          // Update matrix with indeces of people needing a vaccine
          int n_max = n_mun_age_rg_idxi[mun_i][age_group_i][risk];
          int n_max_mun = n_age_rg_idmun[age_group_i][risk];

          // id of people
          mun_age_rg_idxi[mun_i][age_group_i][risk][idx_mun_i] = mun_age_rg_idxi[mun_i][age_group_i][risk][n_max - 1];
          n_mun_age_rg_idxi[mun_i][age_group_i][risk]--;
          mun_age_rg_idxi[mun_i][age_group_i][risk] = ( int* ) realloc( mun_age_rg_idxi[mun_i][age_group_i][risk], ( n_mun_age_rg_idxi[mun_i][age_group_i][risk] )*sizeof( int ));

          // id of municipalities
          if( n_mun_age_rg_idxi[mun_i][age_group_i][risk] < 1 )
          {
            age_rg_idmun[age_group_i][risk][idx_mun] = age_rg_idmun[age_group_i][risk][n_max_mun - 1];
            n_age_rg_idmun[age_group_i][risk]--;
            age_rg_idmun[age_group_i][risk] = ( int* ) realloc( age_rg_idmun[age_group_i][risk], ( n_age_rg_idmun[age_group_i][risk] )*sizeof( int ));
          }

          // vaccinate individuals -----------------------
          vpk_count_vaccinated += 1;                                          // found an individual

          vaccination->half_vaccinated[mun_i]++;                              // count one individual
          individuals->time_vaccination[idx_i] = vpk_time[txt_row];           // time of vaccination
          individuals->vaccinated[idx_i]       = vpk_vaccine[txt_row];        // took vaccine 1,2,3

          if( individuals->vaccinated[idx_i] == 9999 ){                         // took vaccine NA
            individuals->vaccinated[idx_i] = (rand() % 2) + 1;                // took vaccine 1 or 2
          }

          if( vpk_count_vaccinated > vpk_count[txt_row] ){ // rejected vaccination
            individuals->vaccinated[idx_i] *= -1;
          }else{ // accepted vaccination
            // vaccine efficacy -----------------------
            int vac_i = individuals->vaccinated[idx_i] - 1; // vaccine type

            individuals->vaccine_efficacy_1st_Ia[idx_i]  = param.VACCINE_EFF_Ia[0][vac_i]; // # VE against asymptomatic
            individuals->vaccine_efficacy_1st_I[idx_i]   = param.VACCINE_EFF_I[0][vac_i];  // # VE against  symptomatic
            individuals->vaccine_efficacy_1st_sd[idx_i]  = param.VACCINE_EFF_S[0][vac_i];  // # VE against hospitalization
            individuals->vaccine_efficacy_1st_d[idx_i]   = param.VACCINE_EFF_D[0][vac_i];  // # VE against death
            individuals->vaccine_efficacy_1st_V[idx_i]   = 1 - param.VACCINE_EFF_V[0][vac_i];  // # VE against transmissibility

            individuals->vaccine_efficacy_2nd_Ia[idx_i]  = param.VACCINE_EFF_Ia[1][vac_i]; // # VE against asymptomatic
            individuals->vaccine_efficacy_2nd_I[idx_i]   = param.VACCINE_EFF_I[1][vac_i];  // # VE against  symptomatic
            individuals->vaccine_efficacy_2nd_sd[idx_i]  = param.VACCINE_EFF_S[1][vac_i];  // # VE against hospitalization
            individuals->vaccine_efficacy_2nd_d[idx_i]   = param.VACCINE_EFF_D[1][vac_i];  // # VE against death
            individuals->vaccine_efficacy_2nd_V[idx_i]   = 1 - param.VACCINE_EFF_V[1][vac_i];  // # VE against transmissibility

            // write into file (fvax = vaccinations.txt) #######################
            if( VERBOSE > 5 )
            fprintf(file, "%d\t%d\t%d\t%d\t%d\n",
            individuals->time_vaccination[idx_i],
            individuals->mun[idx_i] - 1,
            individuals->age[idx_i],
            individuals->vaccinated[idx_i],
            idx_i);

            count_vx_age[ individuals->age[idx_i] ]--;
            if ( count_vx_age[ individuals->age[idx_i] ] < 0)
            count_vx_age[ individuals->age[idx_i] ] = 0;

          } // accepted vaccination
        } // found enough individuals with targeted age group and county
      } // only the 1st dose
    } // only before Day 1 in alternative scenarios & all rows in baseline scenario
  } // all rows in txt

  fprintf(stderr, "select individuals ... done \n" );

  free(vpk_time);
  free(vpk_county);
  free(vpk_age_group);
  free(vpk_dose);
  free(vpk_vaccine);
  free(vpk_risk);
  free(vpk_count);
  gsl_ran_discrete_free(count_vx_age_gsl);

  for( int mun_i = 0; mun_i <  grid->n_mun; mun_i++ ){
    mun_age_rg_idxi[mun_i]   = (int***) calloc( N_age_groups, sizeof(int**));
    n_mun_age_rg_idxi[mun_i] = (int** ) calloc( N_age_groups, sizeof(int* ));

    for( int age_group_i = 0; age_group_i < N_age_groups; age_group_i++ ){
      mun_age_rg_idxi[mun_i][age_group_i]   = (int**) calloc( N_risk_groups, sizeof(int*));
      free(n_mun_age_rg_idxi[mun_i][age_group_i]);

      for( int risk_i = 0; risk_i < N_risk_groups; risk_i++ ){
        free(mun_age_rg_idxi[mun_i][age_group_i][risk_i]);
      }
    }
  }

  // check total number
  int counter_v = 0;
  int counter_r = 0;
  int counter_vr = 0;

  for( int idx_i = 1; idx_i <= individuals->n_i; idx_i++ ){ // all individuals
    if( individuals->vaccinated[idx_i] != 0 && individuals->vaccinated[idx_i] != -1  ) counter_v++;  // vaccinated
    if( individuals->vaccinated[idx_i] == -1  ) counter_r++;  // rejected
    if( individuals->vaccinated[idx_i] != 0   ) counter_vr++; // vaccinated + rejected
  } // all individuals
  fprintf(stderr, "accepted (%d) + rejected number (%d) = %d\n", counter_v, counter_r, counter_vr);

  return;
} // void vaccination_initialized

void vaccination_prioritization(Grid *grid, Individuals *individuals, Households *households, Counter *counter, Matrix *matrix, Probability *probability, Vaccination *vaccination){

  // Stop here the function if there are not available vaccine doses ###########
  if( strcmp( param.PRIORITIZATION_1, "Base" ) == 0 ){ // baseline
    fprintf(stderr, "Baseline: skipped definition of vax priority scenario \n" );
    return;
  } // baseline

  // define vaccination list ###################################################
  if( param.REGIONAL < 0 ){ // national prioritization (* param.REGIONAL == 0 *)

    // line up all individuals nationally
    vaccination->vaccination_list    = (int***) calloc(1, sizeof(int**));
    vaccination->element_vax_list    = (int** ) calloc(1, sizeof(int* ));
    // line up all individuals for each vaccine
    vaccination->vaccination_list[0] = (int** ) calloc(3, sizeof(int* ));
    vaccination->element_vax_list[0] = (int*  ) calloc(3, sizeof(int  ));
    for(int vac = 0; vac < 3; vac++){ // 3 vaccines
      vaccination->vaccination_list[0][vac] = (int*) calloc(individuals->n_i+1, sizeof(int));
      vaccination->element_vax_list[0][vac] = 0;
    } // 3 vaccines

  } else { // regional prioritization

    // line up all individuals for each vaccine and each mun
    vaccination->vaccination_list           = (int***) calloc(N_MUN, sizeof(int**));
    vaccination->element_vax_list           = (int** ) calloc(N_MUN, sizeof(int* ));
    for( int mun = 0; mun < N_MUN ; mun++ ){ // 356 mun
      vaccination->vaccination_list[mun] = (int** ) calloc(3, sizeof(int* ));
      vaccination->element_vax_list[mun] = (int*  ) calloc(3, sizeof(int  ));
      for(int vac = 0; vac < 3; vac++){ // 3 vaccines
        vaccination->vaccination_list[mun][vac] = (int*) calloc(grid->municipality[mun].n_i, sizeof(int));
        vaccination->element_vax_list[mun][vac] = 0;
      } // 3 vaccines
    } // 356 mun

  } // param.REGIONAL

  // assign prioritization to each individual ##################################
  char PRIORITY_SCEN[100]; // name of prioritization file
  int N_col = 110; // age from 0 to 109
  char input_matrix_vax_priority[1000];
  for(int vac = 0; vac < 3; vac++){ // 3 vaccines

    // name of prioritization file
    if(vac == 0){ sscanf(param.NAME_VAX_PRIORITY_SCEN_1, "%s", PRIORITY_SCEN); }
    if(vac == 1){ sscanf(param.NAME_VAX_PRIORITY_SCEN_2, "%s", PRIORITY_SCEN); }
    if(vac == 2){ sscanf(param.NAME_VAX_PRIORITY_SCEN_3, "%s", PRIORITY_SCEN); }

    // Read the input matrix for the vaccine strategy
    sprintf( input_matrix_vax_priority, "%s/input_files/scenarios_txt/%s", EXP_DIR, PRIORITY_SCEN );
    if( read_matrix( input_matrix_vax_priority, N_col, '\t', matrix ) != 0 ){ // cannot read
      fprintf(stderr, "error in read_matrix\n");
      exit(1);
    } else { // can read
      fprintf(stderr, "Read prioritization file: %s\n", PRIORITY_SCEN);
      //
      for( int p = 0; p < 3; p++ ){
        for( int u = 0; u < N_col; u++){
          fprintf(stdout, "%lf\t", matrix->matrix[p][u]);
        }
        fprintf(stdout, "\n");
      }
    } // input_matrix_vax_priority

    for( int mun = 0; mun < N_MUN ; mun++ ){ // 356 mun
      param.COUNTY_PRIORITY[mun] = 0; // not used
    } // 356 mun

    for( int id = 1; id <= individuals->n_i; id++ ){ // all individuals

      // Assign the mun priority to each individual
      int mun = individuals->mun[id] - 1;
      individuals->county_priority[id] = param.COUNTY_PRIORITY[mun];

      // Assign the vaccine_priority to each individual based on age and risk category
      int row = individuals->risk_category[id];
      int col = individuals->age[id];

      if(vac == 0){ individuals->vaccine_priority_1[id] = matrix->matrix[row][col]; }
      if(vac == 1){ individuals->vaccine_priority_2[id] = matrix->matrix[row][col]; }
      if(vac == 2){ individuals->vaccine_priority_3[id] = matrix->matrix[row][col]; }
    } // all individuals

    // define a reshuffled and sorted vector of vaccine priority ###############
    if( param.REGIONAL < 0 ){ // national prioritization (* param.REGIONAL == 0 *)

      gsl_permutation * p = gsl_permutation_alloc( individuals->n_i + 1 );
      gsl_vector      * v = gsl_vector_alloc(      individuals->n_i + 1 );
      double priority;
      for( int i = 1; i <= individuals->n_i; i++ ){ // each individual
        // priority of each individual i
        if(vac == 0){ priority = individuals->vaccine_priority_1[i] + individuals->county_priority[i] + gsl_ran_flat(R_GLOBAL, 0, 0.99); }
        if(vac == 1){ priority = individuals->vaccine_priority_2[i] + individuals->county_priority[i] + gsl_ran_flat(R_GLOBAL, 0, 0.99); }
        if(vac == 2){ priority = individuals->vaccine_priority_3[i] + individuals->county_priority[i] + gsl_ran_flat(R_GLOBAL, 0, 0.99); }
        // remove those vaccinated individuals from the line
        if(individuals->vaccinated[i] != 0) priority = 9999;
        // define v
        gsl_vector_set(v, i, priority);
      } // each individual

      // sorting v and write the ascending ordered index into p
      gsl_sort_vector_index(p, v);

      // create vaccination->vaccination_list as the main output
      int j = 0;
      for( int i = 0; i < individuals->n_i ; i++ ){ // each individual
        if( p->data[i] == 0 ){ // avoid i = 0
          j++;
          continue;
        } // avoid i = 0
        vaccination->vaccination_list[0][vac][i] = p->data[j];
        j++;
      } // each individual

      gsl_permutation_free(p);
      gsl_vector_free(v);

    } else { // regional prioritization

      for( int mun = 0; mun < N_MUN ; mun++ ){ // 356 mun

        gsl_permutation * p = gsl_permutation_alloc( grid->municipality[mun].n_i );
        gsl_vector      * v = gsl_vector_alloc(      grid->municipality[mun].n_i );
        double priority;
        for( int id_ind_co = 0; id_ind_co < grid->municipality[mun].n_i; id_ind_co++ ){ // individuals in the mun
          // index of individual in the mun
          int i = grid->municipality[mun].individuals[ id_ind_co ];
          // priority of each individual i
          if(vac == 0){ priority = individuals->vaccine_priority_1[i] + individuals->county_priority[i] + gsl_ran_flat(R_GLOBAL, 0, 0.99); }
          if(vac == 1){ priority = individuals->vaccine_priority_2[i] + individuals->county_priority[i] + gsl_ran_flat(R_GLOBAL, 0, 0.99); }
          if(vac == 2){ priority = individuals->vaccine_priority_3[i] + individuals->county_priority[i] + gsl_ran_flat(R_GLOBAL, 0, 0.99); }
          // remove those vaccinated individuals from the line
          if(individuals->vaccinated[i] != 0) priority = 9999;
          // define v
          gsl_vector_set(v, id_ind_co, priority);
        } // individuals in the mun

        // sorting v and write the ascending ordered index into p
        gsl_sort_vector_index(p, v);

        for( int id_ind_co = 0; id_ind_co < grid->municipality[mun].n_i; id_ind_co++ ){ // individuals in the mun
          vaccination->vaccination_list[mun][vac][id_ind_co] = grid->municipality[mun].individuals[ p->data[ id_ind_co ] ];
        } // individuals in the mun

        gsl_permutation_free(p);
        gsl_vector_free(v);
      } // 356 mun

    } // param.REGIONAL
  } // 3 vaccines

  // the number of population (18+) for vaccination in each mun ################
  int regional_priority; // vaccination population
  for(int vac = 0; vac < 3; vac++){ // 3 vaccines

    // targeted vaccination number in Norway
    vaccination->vaccination_total_N[vac] = 0;
    // targeted vaccination number in each region
    for( int mun = 0; mun < N_MUN ; mun++ ){vaccination->vaccination_region_N[vac][mun] = 0;}

    for( int idx_i = 1; idx_i <= individuals->n_i; idx_i++ ){ // each individual
      // individual in targeted groups
      if(vac == 0) regional_priority = (individuals->vaccine_priority_1[idx_i] < 9999); // < 9999 for (18+) / <= 9999 for (0+)
      if(vac == 1) regional_priority = (individuals->vaccine_priority_2[idx_i] < 9999);
      if(vac == 2) regional_priority = (individuals->vaccine_priority_3[idx_i] < 9999);
      // mun of the individual
      int mun_i = individuals->mun[idx_i] - 1;
      // count the number of individuals
      if(regional_priority) vaccination->vaccination_region_N[vac][mun_i]++;
      if(regional_priority) vaccination->vaccination_total_N[vac]++;
    } // each individual

    // the fracion of population for vaccination in each mun ###################
    for( int mun = 0; mun < N_MUN ; mun++ ){ // 356 mun
      vaccination->vaccination_region_frac[vac][mun] = (double) vaccination->vaccination_region_N[0][mun] / vaccination->vaccination_total_N[0];
      if( (double) vaccination->vaccination_region_N[vac][mun] / vaccination->vaccination_total_N[vac] * 100 > 1 )
      fprintf(stderr, "The fracion of (18+) population (vaccination->vaccination_region_frac[%d][%d]) = %lf %% (%d / %d)\n",
      vac,
      mun,
      (double) vaccination->vaccination_region_N[vac][mun] / vaccination->vaccination_total_N[vac] * 100,
      vaccination->vaccination_region_N[vac][mun],
      vaccination->vaccination_total_N[vac]);
    } // 356 mun
  } // 3 vaccines

  // the (30+) population for vaccination in each municipality #################
  for( int mun_n = 0; mun_n < N_MUN ; mun_n++ ){ // 356 municipalities
    vaccination->half_vaccination[mun_n] = 0;
  } // 356 municipalities
  for( int idx_i = 1; idx_i <= individuals->n_i; idx_i++ ){       // each individual
    int age_i = individuals->age[idx_i];                          // age group of the individual
    int mun_i = individuals->mun[idx_i] - 1;                      // municipality of the individual
    if(age_i > 30) vaccination->half_vaccination[mun_i]++;        // count regardless of vaccine
  } // each individual

} // void vaccination_prioritization

void vaccination_strategy(FILE *file, Individuals *individuals, Households *households, Counter *counter, int doses_day[], int t, Vaccination *vaccination, Grid *grid, Matrix *matrix, Probability *probability){

  int id;                           // index of individual
  int doses_day_counter[3] = {0};   // number of distributed doses
  int priority[3][N_MUN] = {{ [0 ... (N_MUN-1) ] = 9999 }, { [0 ... (N_MUN-1) ] = 9999 }, { [0 ... (N_MUN-1) ] = 9999 }};     // vaccine priority of an individual
  int priority_stop[3] = {0}; // stop regional prioritization

  double *region_frac;                 // probability of each municipality
  region_frac = (double*) calloc(N_MUN, sizeof(double));

  for(int vac = 0; vac < 3; vac++){ // 3 vaccines
    while( doses_day_counter[vac] < doses_day[vac] ){ // doses_day[vac] = daily doses available for vaccine 1

      // reached the total number of doses
      if(vac == 0) if( counter->vaccinated_1 == param.NR_VAX_DOSES_1 ) break;
      if(vac == 1) if( counter->vaccinated_2 == param.NR_VAX_DOSES_2 ) break;
      if(vac == 2) if( counter->vaccinated_3 == param.NR_VAX_DOSES_3 ) break;
      // finished testing everyone in Norway
      double region_frac_total = 0;
      for( int mun = 0; mun < N_MUN ; mun++ ) region_frac_total += vaccination->vaccination_region_frac[vac][mun];
      if(region_frac_total == 0) break;

      // finish regional prioritization
      priority_stop[vac] = 0;
      for( int mun = 0; mun < N_MUN ; mun++ ){
        if(param.mun_priority[mun] == 1){ // prioritized municipalities
          if(priority[vac][mun] <= param.REGIONAL_PRIORITY){ // still in prioritized groups
            priority_stop[vac]++; // keep regional prioritization
          }
        }
      }

      // probability of each municipality
      memcpy(region_frac, vaccination->vaccination_region_frac[vac], sizeof(vaccination->vaccination_region_frac[vac]));
      // regional prioritization
      if(t >= param.REGIONAL_START){ // from 1st day of a month
        if(priority_stop[vac] > 0){ // not finished prioritized groups
          // calculate the extra doses for prioritized areas
          double region_frac_plus  = 0;
          double region_frac_minus = 0;
          for( int mun = 0; mun < N_MUN ; mun++ ){
            if(param.mun_priority[mun] == 1){ // plus group
              region_frac_plus  += region_frac[mun] * param.REGIONAL; // total prioritized percentage
            }
            if(param.mun_priority[mun] == -1){ // minus group
              region_frac_minus += region_frac[mun];
            }
          }
          // distribute the extra doses for prioritized areas
          for( int mun = 0; mun < N_MUN ; mun++ ){
            if(param.mun_priority[mun] == 1){ // plus group
              region_frac[mun] *= (1 + param.mun_priority[mun] * param.REGIONAL); // prioritized percentage
            }
            if(param.mun_priority[mun] == -1){ // minus group
              region_frac[mun] -= region_frac[mun]/region_frac_minus*region_frac_plus;
            }
          }
        }else{ // finished prioritized groups
          if(doses_day_counter[vac] == doses_day[vac] - 1){
            fprintf(stderr, "No more regional prioritization for vaccine %d\n", vac);
          }
        }
      }

      // random draw a mun by gsl of distribution
      gsl_ran_discrete_t *county_frac_gsl;                                                          // pointer
      county_frac_gsl = (gsl_ran_discrete_t *) calloc(N_MUN, sizeof(gsl_ran_discrete_t));           // contiguous allocation
      county_frac_gsl = gsl_ran_discrete_preproc(N_MUN, region_frac);                               // preprocessor
      int mun = gsl_ran_discrete(R_GLOBAL, county_frac_gsl);                                        // targeted mun
      gsl_ran_discrete_free(county_frac_gsl);                                                       // de-allocation

      int found_mun = 0;
      while(found_mun == 0) { // until found someone in the mun

        // select from the list the id of the person to be vaccinated
        id = vaccination->vaccination_list[mun][vac][ vaccination->element_vax_list[mun][vac] ];
        // keep track of waiting list
        if(vaccination->element_vax_list[mun][vac] + 1 < vaccination->vaccination_region_N[vac][mun]){ // there are 18+ people
          vaccination->element_vax_list[mun][vac]++;                                                      // next index of vaccination waiting list
        }else{                                                                                               // the rest are 9999
          vaccination->vaccination_region_frac[vac][mun] = 0;                                             // remove the region from random draw
          found_mun = 1;                                                                                  // not found but move on
        }

        // exclude the person does not meet the requirements for the vaccination
        if(vac == 0) priority[vac][mun] = individuals->vaccine_priority_1[id] + individuals->county_priority[id];
        if(vac == 1) priority[vac][mun] = individuals->vaccine_priority_2[id] + individuals->county_priority[id];
        if(vac == 2) priority[vac][mun] = individuals->vaccine_priority_3[id] + individuals->county_priority[id];

        if(priority[vac][mun] < 9999){ // vaccine priority
          if( individuals->vaccinated[id] == 0 ){ // non-vaccinated individuals
            if( individuals->status[id] != I ){ // including currently recovered (R) and asymptomatic (Ia) are vaccinated

              // update rate
              double adherence = 0;
              // age groups
              if( individuals->age[id] >= 0 && individuals->age[id] <= 11 ){ // down to 12+ instead of 18+
                if(vac == 0) adherence = param.ADHERENCE_1_0_11;
                if(vac == 1) adherence = param.ADHERENCE_2_0_11;
                if(vac == 2) adherence = param.ADHERENCE_3_0_11;
              }
              if( individuals->age[id] >= 12 && individuals->age[id] <= 15 ){
                if(vac == 0) adherence = param.ADHERENCE_1_12_15;
                if(vac == 1) adherence = param.ADHERENCE_2_12_15;
                if(vac == 2) adherence = param.ADHERENCE_3_12_15;
              }
              if( individuals->age[id] >= 16 && individuals->age[id] <= 17 ){
                if(vac == 0) adherence = param.ADHERENCE_1_16_17;
                if(vac == 1) adherence = param.ADHERENCE_2_16_17;
                if(vac == 2) adherence = param.ADHERENCE_3_16_17;
              }
              if( individuals->age[id] >= 18 && individuals->age[id] <= 24 ){
                if(vac == 0) adherence = param.ADHERENCE_1_18_24;
                if(vac == 1) adherence = param.ADHERENCE_2_18_24;
                if(vac == 2) adherence = param.ADHERENCE_3_18_24;
              }
              if( individuals->age[id] >= 25 && individuals->age[id] <= 39 ){
                if(vac == 0) adherence = param.ADHERENCE_1_25_39;
                if(vac == 1) adherence = param.ADHERENCE_2_25_39;
                if(vac == 2) adherence = param.ADHERENCE_3_25_39;
              }
              if( individuals->age[id] >= 40 && individuals->age[id] <= 44 ){
                if(vac == 0) adherence = param.ADHERENCE_1_40_44;
                if(vac == 1) adherence = param.ADHERENCE_2_40_44;
                if(vac == 2) adherence = param.ADHERENCE_3_40_44;
              }
              if( individuals->age[id] >= 45 && individuals->age[id] <= 54 ){
                if(vac == 0) adherence = param.ADHERENCE_1_45_54;
                if(vac == 1) adherence = param.ADHERENCE_2_45_54;
                if(vac == 2) adherence = param.ADHERENCE_3_45_54;
              }
              if( individuals->age[id] >= 55 && individuals->age[id] <= 64 ){
                if(vac == 0) adherence = param.ADHERENCE_1_55_64;
                if(vac == 1) adherence = param.ADHERENCE_2_55_64;
                if(vac == 2) adherence = param.ADHERENCE_3_55_64;
              }
              if( individuals->age[id] >= 65 && individuals->age[id] <= 74 ){
                if(vac == 0) adherence = param.ADHERENCE_1_65_74;
                if(vac == 1) adherence = param.ADHERENCE_2_65_74;
                if(vac == 2) adherence = param.ADHERENCE_3_65_74;
              }
              if( individuals->age[id] >= 75 && individuals->age[id] <= 84 ){
                if(vac == 0) adherence = param.ADHERENCE_1_75_84;
                if(vac == 1) adherence = param.ADHERENCE_2_75_84;
                if(vac == 2) adherence = param.ADHERENCE_3_75_84;
              }
              if(                               individuals->age[id] >= 85 ){
                if(vac == 0) adherence = param.ADHERENCE_1_85p;
                if(vac == 1) adherence = param.ADHERENCE_2_85p;
                if(vac == 2) adherence = param.ADHERENCE_3_85p;
              }

              if( gsl_ran_bernoulli( R_GLOBAL, adherence ) == 1 ){ // vaccinate or not
                individuals->vaccinated[id] = (vac+1);       // took vaccine 1,2,3
                individuals->time_vaccination[id] = t;       // time of vaccination

                // vaccine efficacy
                individuals->vaccine_efficacy_1st_Ia[id]  = param.VACCINE_EFF_Ia[0][vac];     // # VE against asymptomatic
                individuals->vaccine_efficacy_1st_I[id]   = param.VACCINE_EFF_I[0][vac];      // # VE against  symptomatic
                individuals->vaccine_efficacy_1st_sd[id]  = param.VACCINE_EFF_S[0][vac];      // # VE against hospitalization
                individuals->vaccine_efficacy_1st_d[id]   = param.VACCINE_EFF_D[0][vac];      // # VE against  death
                individuals->vaccine_efficacy_1st_V[id]   = 1 - param.VACCINE_EFF_V[0][vac];  // # VE against transmissibility

                individuals->vaccine_efficacy_2nd_Ia[id]  = param.VACCINE_EFF_Ia[1][vac];     // # VE against asymptomatic
                individuals->vaccine_efficacy_2nd_I[id]   = param.VACCINE_EFF_I[1][vac];      // # VE against  symptomatic
                individuals->vaccine_efficacy_2nd_sd[id]  = param.VACCINE_EFF_S[1][vac];      // # VE against hospitalization
                individuals->vaccine_efficacy_2nd_d[id]   = param.VACCINE_EFF_D[1][vac];      // # VE against death
                individuals->vaccine_efficacy_2nd_V[id]   = 1 - param.VACCINE_EFF_V[1][vac];  // # VE against transmissibility

                found_mun = 1; // found someone in the mun
                doses_day_counter[vac]++; // distributed doses of vaccine
                if(vac == 0) counter->vaccinated_1++; // distributed doses of both vaccines
                if(vac == 1) counter->vaccinated_2++; // distributed doses of both vaccines
                if(vac == 2) counter->vaccinated_3++; // distributed doses of both vaccines

                int age_i       =individuals->age[id];                           // age       of the individual
                int county_i    = individuals->county[id];                       // county    of the individual
                int mun_i       = individuals->mun[id] - 1;                      // municip   of the individual
                int cell_i      = individuals->cell[id] - 1;                     // cell      of the individual
                if(vac == 0) counter->cum_V1[age_i]++; // age groups
                if(vac == 1) counter->cum_V2[age_i]++; // age groups
                if(vac == 2) counter->cum_V3[age_i]++; // age groups
                if(vac == 0) counter->cum_V1_county[county_i]++;         // counties
                if(vac == 1) counter->cum_V2_county[county_i]++;         // counties
                if(vac == 2) counter->cum_V3_county[county_i]++;         // counties
                if(vac == 0) counter->cum_V1_mun[mun_i]++;               // municipalities
                if(vac == 1) counter->cum_V2_mun[mun_i]++;               // municipalities
                if(vac == 2) counter->cum_V3_mun[mun_i]++;               // municipalities
                if(vac == 0) counter->cum_V1_cell[cell_i]++;             // cells
                if(vac == 1) counter->cum_V2_cell[cell_i]++;             // cells
                if(vac == 2) counter->cum_V3_cell[cell_i]++;             // cells
                vaccination->half_vaccinated[mun_i]++;                   // count regardless of vaccine

                // write into file (fvax = vaccinations.txt)
                if( VERBOSE > 5 )
                fprintf(file, "%d\t%d\t%d\t%d\t%d\n",
                individuals->time_vaccination[id],
                individuals->mun[id] - 1,
                individuals->age[id],
                individuals->vaccinated[id],
                id);

              } else { // adherence
                individuals->vaccinated[id] = -(vac+1);       // did not take vaccine 1,2,3
              } // adherence

            } // individuals->status
          } // individuals->vaccinated
        } // vaccine priority
      } // found someone in the mun
    } // doses_day_counter[vac] < doses_day[vac]
  } // 3 vaccines

  // print vaccination info
  fprintf(stderr, "Daily doses %d + %d + %d = %d\n",
  doses_day[0],
  doses_day[1],
  doses_day[2],
  doses_day[0] + doses_day[1] + doses_day[2]);
  fprintf(stderr, "People vaccinated %d + %d + %d = %d \n",
  counter->vaccinated_1,
  counter->vaccinated_2,
  counter->vaccinated_3,
  counter->vaccinated_1 + counter->vaccinated_2 + counter->vaccinated_3);

  // free memory
  free(region_frac);

} // void vaccination_strategy

void initialization( Grid *grid, Individuals *individuals, char file[], char sep, int t, FILE* fiww, Hosp_param *hosp_param, Counter *counter ){

  char *line;
  int out_get_line = 2;
  FILE *fp;
  char string_who[100];
  char string_value[100];

  int N_E[N_MUN][9]  = {0};
  int N_I[N_MUN][9]  = {0};
  int N_Ia[N_MUN][9] = {0};
  int N_E2[N_MUN][9] = {0};
  int Rec[N_MUN][9]  = {0};
  int N_H[N_MUN][9]   = {0};
  int N_ICU[N_MUN][9] = {0};

  if( !( fp = fopen(file, "r") ) ){
    fprintf(stderr,"initialization: error opening file %s for reading\n",file);
  }

  while(out_get_line>=2){
    out_get_line = get_line( &line, fp );
    if( out_get_line < 3 ){
      switch( out_get_line ){

        case 2:
        fprintf(stderr,"initialization: line of file %s does not end in newline\n",file);
        break;

        case 1:
        fprintf(stderr,"initialization: file %s contains an empty line\n",file);
        break;

        case 0:
        for(int j = 0; j < N_MUN; j++){
          for( int age = 0; age < 9; age++ ){

            // E1 individuals
            int  sampled = 0;
            int  sampled_tested = 0;
            while( sampled < N_E[j][age] && sampled_tested < 10 * grid->municipality[j].n_i ){
              sampled_tested++;
              int idx = gsl_rng_uniform_int( R_GLOBAL, grid->municipality[j].n_i );
              int id = grid->municipality[j].individuals[idx];
              int age_i = floor( individuals->age[id] / 10. );
              if( age_i > 8 ) age_i = 8;
              if( age_i == age && individuals->status[id] == S ){
                individuals->status[id] = E1;
                next_trans_func( id, individuals, t, param.E1_GAMMA_ALPHA,param.E1_GAMMA_BETA, 1 );  // next_trans
                if( gsl_ran_bernoulli( R_GLOBAL, param.PROB_ASYMP ) == 1 ){
                  individuals->status[id] = E1a;
                  next_trans_func( id, individuals, t, param.E1a_GAMMA_ALPHA,param.E1a_GAMMA_BETA, 1 );  // next_trans
                }
                sampled++;
              }
            }

            // E2 individuals
            sampled = 0;
            sampled_tested = 0;
            while( sampled < N_E2[j][age] && sampled_tested < 10 * grid->municipality[j].n_i ){
              sampled_tested++;
              int idx = gsl_rng_uniform_int( R_GLOBAL, grid->municipality[j].n_i );
              int id = grid->municipality[j].individuals[idx];
              int age_i = floor( individuals->age[id] / 10. );
              if( age_i > 8 ) age_i = 8;
              if( age_i == age && individuals->status[id] == S ){
                individuals->status[id] = E2;
                next_trans_func( id, individuals, t, param.E2_GAMMA_ALPHA,param.E2_GAMMA_BETA, 1 );  // next_trans
                sampled++;
              }
            }

            // Ia individuals
            sampled = 0;
            sampled_tested = 0;
            while( sampled < N_Ia[j][age] && sampled_tested < 10 * grid->municipality[j].n_i ){
              sampled_tested++;
              int idx = gsl_rng_uniform_int( R_GLOBAL, grid->municipality[j].n_i );
              int id = grid->municipality[j].individuals[idx];
              int age_i = floor( individuals->age[id] / 10. );
              if( age_i > 8 ) age_i = 8;
              if( age_i == age && individuals->status[id] == S ){
                individuals->status[id] = Ia;
                next_trans_func( id, individuals, t, param.Ia_GAMMA_ALPHA, param.Ia_GAMMA_BETA, 1 );  // next_trans
                sampled++;
              }
            }

            // H and ICU Individuals
            int sampled_H_ICU = 0;
            int sampled_ICU   = 0;

            // I Individuals
            sampled = 0;
            sampled_tested = 0;
            while( sampled < N_I[j][age] && sampled_tested < 10 * grid->municipality[j].n_i ){
              sampled_tested++;
              int idx = gsl_rng_uniform_int( R_GLOBAL, grid->municipality[j].n_i );
              int id = grid->municipality[j].individuals[idx];
              int age_i = floor( individuals->age[id] / 10. );
              if( age_i > 8 ) age_i = 8;
              if( age_i == age && individuals->status[id] == S ){
                individuals->status[id] = I;
                next_trans_func( id, individuals, t, param.I_GAMMA_ALPHA, param.I_GAMMA_BETA, 1 );  // next_trans
                hospitalization( id, grid, individuals, counter, t, fiww, hosp_param);
                death( individuals, id, t );
                sampled++; // I Individuals

                if(sampled_H_ICU < N_H[j][age] + N_ICU[j][age]){ // prev_H_ICU
                  int time_to_hosp = 0;
                  int LOS_hosp =  0;
                  while( LOS_hosp == 0 ) LOS_hosp = gsl_ran_negative_binomial(R_GLOBAL, hosp_param->LOS_hosp[age][0], hosp_param->LOS_hosp[age][1]);
                  LOS_hosp = ( rand() % LOS_hosp ) + 1;
                  individuals->hosp_entrance[id] = t + time_to_hosp;
                  individuals->hosp_leaving[id]  = individuals->hosp_entrance[id] + LOS_hosp;

                  if( gsl_ran_bernoulli( R_GLOBAL, hosp_param->ICU_admission[ age ] ) == 1 ){ //probability->icu[ age ] ) == 1 )
                    int time_to_ICU = 0; // LOS in hosp before ICU
                    time_to_ICU = gsl_ran_negative_binomial(R_GLOBAL, hosp_param->time_to_ICU[age][0], hosp_param->time_to_ICU[age][1]);

                    int LOS_ICU = 0; // LOS in ICU
                    LOS_ICU = gsl_ran_negative_binomial(R_GLOBAL, hosp_param->LOS_ICU[age][0],  hosp_param->LOS_ICU[age][1]);

                    int LOS_hosp_ICU = 0; // LOS after ICU
                    LOS_hosp_ICU = gsl_ran_negative_binomial( R_GLOBAL, hosp_param->LOS_hosp_ICU[age][0],  hosp_param->LOS_hosp_ICU[age][1]);

                    individuals->icu_entrance[id] = individuals->hosp_entrance[id] + time_to_ICU;
                    individuals->icu_leaving[id]  = individuals->icu_entrance[id]  + LOS_ICU;
                    individuals->hosp_leaving[id] = individuals->icu_leaving[id]   + LOS_hosp_ICU;
                  }

                  individuals->hospitalized[id] = H;

                  if(sampled_ICU < N_ICU[j][age]){ // prev_ICU: icu_entrance on Day 1
                    int time_to_ICU = 0; // LOS in hosp before ICU
                    // while( time_to_ICU == 0 ) time_to_ICU = gsl_ran_negative_binomial(R_GLOBAL, hosp_param->time_to_ICU[age][0], hosp_param->time_to_ICU[age][1]);
                    int LOS_ICU = 0; // LOS in ICU
                    // while( LOS_ICU == 0 )
                    LOS_ICU = gsl_ran_negative_binomial(R_GLOBAL, hosp_param->LOS_ICU[age][0],  hosp_param->LOS_ICU[age][1]);
                    // LOS_ICU = ( rand() % 14 ) + LOS_ICU;
                    int LOS_hosp_ICU = 0; // LOS after ICU
                    // while( LOS_hosp_ICU == 0 )
                    LOS_hosp_ICU = gsl_ran_negative_binomial( R_GLOBAL, hosp_param->LOS_hosp_ICU[age][0],  hosp_param->LOS_hosp_ICU[age][1]);

                    individuals->icu_entrance[id] = t + time_to_ICU;
                    individuals->icu_leaving[id]  = individuals->icu_entrance[id]  + LOS_ICU;
                    individuals->hosp_leaving[id] = individuals->icu_leaving[id]   + LOS_hosp_ICU;

                    individuals->hospitalized[id] = H_ICU;

                    sampled_ICU++;
                  } // prev_ICU

                  sampled_H_ICU++;
                } // prev_H_ICU
              }
            }

            // R Individuals
            sampled = 0;
            sampled_tested = 0;
            while( sampled < Rec[j][age] && sampled_tested < 10 * grid->municipality[j].n_i ){
              sampled_tested++;
              int idx = gsl_rng_uniform_int( R_GLOBAL, grid->municipality[j].n_i );
              int id = grid->municipality[j].individuals[idx];
              int age_i = floor( individuals->age[id] / 10. );
              if( age_i > 8 ) age_i = 8;
              if( age_i == age && individuals->status[id] == S ){
                individuals->status[id] = R;
                sampled++;
              }
            }
          }
        }
        fclose(fp);
        return;
        break;

        case -1:
        fprintf(stderr,"initialization: get_line error on file %s\n", file);
        default:
        fprintf(stderr,"initialization: unrecognized exit status of get_line on file %s\n",file);
        break;
      }
    }

    sscanf(line,"%s", string_who);

    line = (char *)strchr(line, sep);

    line++;
    sscanf(line,"%s", string_value);

    char * var_name; // name of variable on 1st column

    for( int j = 0; j < N_MUN; j++){ // 356 municipalities
      for( int i = 0; i < 9; i++){ // 9 age groups

        var_name = (char*)calloc( 1000, sizeof(char) );
        sprintf( var_name, "E1_%d_%d", i, j );
        if(strcmp( string_who, var_name )==0)
        N_E[j][i]=atof(string_value);
        free(var_name);

        var_name = (char*)calloc( 1000, sizeof(char) );
        sprintf( var_name, "E2_%d_%d", i, j );
        if(strcmp( string_who, var_name )==0)
        N_E2[j][i]=atof(string_value);
        free(var_name);

        var_name = (char*)calloc( 1000, sizeof(char) );
        sprintf( var_name, "I_%d_%d", i, j );
        if(strcmp( string_who, var_name )==0)
        N_I[j][i]=atof(string_value);
        free(var_name);

        var_name = (char*)calloc( 1000, sizeof(char) );
        sprintf( var_name, "Ia_%d_%d", i, j );
        if(strcmp( string_who, var_name )==0)
        N_Ia[j][i]=atof(string_value);
        free(var_name);

        var_name = (char*)calloc( 1000, sizeof(char) );
        sprintf( var_name, "R_%d_%d", i, j );
        if(strcmp( string_who, var_name )==0)
        Rec[j][i]=atof(string_value);
        free(var_name);

        var_name = (char*)calloc( 1000, sizeof(char) );
        sprintf( var_name, "prev_H_%d_%d", i, j );
        if(strcmp( string_who, var_name )==0)
        N_H[j][i]=atof(string_value);
        free(var_name);

        var_name = (char*)calloc( 1000, sizeof(char) );
        sprintf( var_name, "prev_ICU_%d_%d", i, j );
        if(strcmp( string_who, var_name )==0)
        N_ICU[j][i]=atof(string_value);
        free(var_name);

      } // 9 age groups

      var_name = (char*)calloc( 1000, sizeof(char) );
      sprintf( var_name, "relative_reproduction_number_%d", j );
      if(strcmp( string_who, var_name )==0)
      param.RELATIVE_TRANS_MUN[j] = atof(string_value);
      free(var_name);

    } // 356 municipalities
  } // while

  return;
} // void initialization

void print_initial_conditions( Individuals * individuals ){

  int N_E1a = 0, N_E1 = 0, N_E2 = 0, N_I = 0, N_Ia = 0, N_R = 0;
  int N_H = 0;
  int N_ICU = 0;

  for( int id = 1; id <= individuals->n_i; id++){
    if( individuals->status[id] == E1a ) N_E1a++;
    if( individuals->status[id] == E1  ) N_E1++;
    if( individuals->status[id] == E2  ) N_E2++;
    if( individuals->status[id] == I   ) N_I++;
    if( individuals->status[id] == Ia  ) N_Ia++;
    if( individuals->status[id] == R   ) N_R++;
  }

  for( int id = 1; id <= individuals->n_i; id++){
    if( individuals->hospitalized[id] == H || individuals->hospitalized[id] == H_ICU ) N_H++;
    if( individuals->hospitalized[id] == H_ICU ) N_ICU++;
  }

  fprintf(stderr, "\n");
  fprintf(stderr, "Starting simulation with:\n");
  fprintf(stderr, "E1a: %d\nE1: %d\nE2: %d\nIa: %d\nI: %d\nR: %d\n", N_E1a, N_E1, N_E2, N_Ia, N_I, N_R);
  fprintf(stderr, "prev_H: %d\nprev_ICU: %d\n", N_H, N_ICU);
  fprintf(stderr, "\n");

} // void print_initial_conditions

void free_prob_transmissions(Prob_trans *pt){

  for( int j=0; j < N_MUN; j++ ){
    free( pt->H_P[j] );
    free( pt->H_A[j] );
    free( pt->H_S[j] );
    free( pt->P0_P[j] );
    free( pt->P0_A[j] );
    free( pt->P0_S[j] );
    free( pt->P1_P[j] );
    free( pt->P1_A[j] );
    free( pt->P1_S[j] );
  }

  free( pt->H_P );
  free( pt->H_A );
  free( pt->H_S );
  free( pt->P0_P);
  free( pt->P0_A );
  free( pt->P0_S );
  free( pt->P1_P );
  free( pt->P1_A );
  free( pt->P1_S );

} // void free_prob_transmissions

void seasonality(Prob_trans *pt, int t, int doy_start){

  fprintf(stderr, "param.SEASON_DAILY[%d] = %lf\n", t,         param.SEASON_DAILY[t]);
  fprintf(stderr, "param.SEASON_DAILY[%d] = %lf\n", doy_start, param.SEASON_DAILY[doy_start]);

  double factor       = 1 - param.SEASON_DAILY[t]         * (double) param.SEASONALITY / 100.;
  double factor_day_1 = 1 - param.SEASON_DAILY[doy_start] * (double) param.SEASONALITY / 100.;

  fprintf(stderr, "factor %lf, factor day 1 %lf, %lf\n", factor, factor_day_1, factor/factor_day_1);

  param.BETA_R  = param.BETA_R_CAL  * factor/factor_day_1;
  param.BETA_H  = param.BETA_H_CAL  * factor/factor_day_1;
  param.BETA_P0 = param.BETA_P0_CAL * factor/factor_day_1;
  param.BETA_P1 = param.BETA_P1_CAL * factor/factor_day_1;

  fprintf(stderr, "seasonality factor = %lf\n",   factor/factor_day_1);
  fprintf(stderr, "param.BETA_R  %lf\n",   param.BETA_R  );
  fprintf(stderr, "param.BETA_H  %lf\n",   param.BETA_H  );
  fprintf(stderr, "param.BETA_P0 %lf\n",   param.BETA_P0 );
  fprintf(stderr, "param.BETA_P1 %lf\n\n", param.BETA_P1 );

} // void seasonality

void importation(int t, Individuals *individuals, Grid *grid, Counter *counter, FILE* fiww, Hosp_param *hosp_param){

  // county distribution of imported cases #####################################
  // file name
  char *file_name;                                                           // pointer
  file_name = (char*) calloc(1000, sizeof(char));                            // contiguous allocation
  sprintf(file_name, "%s/input_files/importation_county_3871.txt", EXP_DIR); // file location
  // read txt file
  FILE *file_county_import_prob;                                     // pointer
  file_county_import_prob = fopen(file_name, "r");                   // read file
  // county distribution of imported cases
  double *county_import_prob;                                        // pointer
  county_import_prob = (double*) calloc(11, sizeof(double));         // contiguous allocation
  for(int county_row = 0; county_row < 11; county_row++){ // 11 counties
    fscanf(file_county_import_prob, "%lf\n", &county_import_prob[county_row]);                     // read value from file
    // fprintf(stderr, "county_import_prob[%d] = %lf\n", county_row, county_import_prob[county_row]); // print to terminal
  } // 11 counties
  fclose(file_county_import_prob);                                   // close file
  // gsl of distribution
  gsl_ran_discrete_t *county_import_prob_gsl;                                             // pointer
  county_import_prob_gsl = (gsl_ran_discrete_t *) calloc(11, sizeof(gsl_ran_discrete_t)); // contiguous allocation
  county_import_prob_gsl = gsl_ran_discrete_preproc(11, county_import_prob);              // preprocessor

  // age distribution of imported cases ########################################
  // file name
  // char *file_name;                                                        // pointer
  file_name = (char*) calloc(1000, sizeof(char));                         // contiguous allocation
  sprintf(file_name, "%s/input_files/importation_age_3937.txt", EXP_DIR); // file location
  // read txt file
  FILE *file_age_group_import_prob;                                     // pointer
  file_age_group_import_prob = fopen(file_name, "r");                   // read file
  // age distribution of imported cases
  double *age_group_import_prob;                                        // pointer
  age_group_import_prob = (double*) calloc(10, sizeof(double));         // contiguous allocation
  for(int age_row = 0; age_row < 10; age_row++){ // 10 age groups
    fscanf(file_age_group_import_prob, "%lf\n", &age_group_import_prob[age_row]);                  // read value from file
    // fprintf(stderr, "age_group_import_prob[%d] = %lf\n", age_row, age_group_import_prob[age_row]); // print to terminal
  } // 10 age groups
  fclose(file_age_group_import_prob);                                   // close file
  // gsl of distribution
  gsl_ran_discrete_t *age_group_import_prob_gsl;                                             // pointer
  age_group_import_prob_gsl = (gsl_ran_discrete_t *) calloc(10, sizeof(gsl_ran_discrete_t)); // contiguous allocation
  age_group_import_prob_gsl = gsl_ran_discrete_preproc(10, age_group_import_prob);           // preprocessor

  // number of imported cases per day ##########################################
  int N_import   = gsl_ran_poisson(R_GLOBAL, param.import_daily[t-1]);                     // target imported number (param.IMPORT)
  int N_imported = 0;                                                           // counter to stop
  while(N_imported < N_import){                                                 // until N_imported == N_import

    int age_group_import = gsl_ran_discrete(R_GLOBAL, age_group_import_prob_gsl); // targeted age group
    int county_import    = gsl_ran_discrete(R_GLOBAL,    county_import_prob_gsl); // targeted county
    int found_import = 0;                                                         // counter of targeted age group and county
    while(found_import == 0){                                                     // found targeted importation

      int idx_i = gsl_rng_uniform_int(R_GLOBAL, individuals->n_i) + 1;          // index of an individual
      if(individuals->status[idx_i] == S){                                      // only S

        int age_group_i = floor(individuals->age[idx_i] / 10);                  // age group of the individual
        if(age_group_i > 9) age_group_i = 9;                                    // for age 100+
        if(age_group_i == age_group_import){                                    // according to age distribution

          int county_i = individuals->county[idx_i];                            // county of the individual
          if(county_i == county_import){                                        // according to county distribution

            found_import = 1;               // counter of targeted age group and county
            N_imported++;                   // successful importation

            if( gsl_ran_bernoulli(R_GLOBAL, param.PROB_ASYMP) == 0 ){ // symptomatic (S -> I)

              individuals->status[idx_i] = I;                                                      // update (S -> I)
              next_trans_func(idx_i, individuals, t, param.I_GAMMA_ALPHA, param.I_GAMMA_BETA, 1);  // determine the transition time of (I -> R)
              hospitalization(idx_i, grid, individuals, counter, t, fiww, hosp_param);             // probability and timing of hospitalization
              death(individuals, idx_i, t);                                                        // probability of death

              // output conuter
              counter->I_r[1]++;
              counter->cum_inc_I_r[1]++;
              counter->inc_I_r[1]++;

            } else { // asymptomatic (S -> Ia)

              individuals->status[idx_i] = Ia;                                                       // update (S -> Ia)
              next_trans_func(idx_i, individuals, t, param.Ia_GAMMA_ALPHA, param.Ia_GAMMA_BETA, 1);  // determine the transition time of (Ia -> R)

              // output conuter
              counter->Ia_r[1]++;
              counter->cum_inc_Ia_r[1]++;
              counter->inc_Ia_r[1]++;

            } // symptomatic or asymptomatic

            // output conuter
            int age_i       = individuals->age[idx_i];                          // age       of the individual
            int county_i    = individuals->county[idx_i];                       // county    of the individual
            int mun_i       = individuals->mun[idx_i] - 1;                      // municip   of the individual
            int cell_i      = individuals->cell[idx_i] - 1;                     // cell      of the individual
            counter->cum_I[age_i]++;
            counter->cum_I_county[county_i]++;
            counter->cum_I_mun[mun_i]++;
            counter->cum_I_cell[cell_i]++;

            // write file
            fprintf(fimp, "%d\t%d\t%d\t%d\t%d\n", t, idx_i, individuals->county[idx_i], individuals->age[idx_i], individuals->status[idx_i]);

          } // according to county distribution
        } // according to age distribution
      } // only S
    } // found targeted importation
  } // until N_imported == N_import

  return;
} // void importation

int day_of_the_year( int day, int month, int idx_1st_month, int print ){

  int days_of_the_month[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  int doy = 0;

  if ( month != 1 ){
    for( int i = 0; i < month; i++){
      doy += days_of_the_month[i];
    }
  }

  doy += day;

  if( print ){
    fprintf(stderr, "day of the year = %d\n", doy );
  }

  return doy;
} // int day_of_the_year
