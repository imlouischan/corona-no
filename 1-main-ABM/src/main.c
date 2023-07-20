#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include "header.h"
#include "global.h"
#include <math.h>
#include <time.h>
#include <sys/stat.h>

int simulate( Grid *grid, Households *households, Individuals *individuals, Places *places, Hospitals *hospitals, Prob_trans *pt ,int ni, Counter *counter, Seeding *seeding, Mobility *mobility, Probability *probability );

int main( int argc, char *argv[] ){

	int nflags;
	int exit_parser;
	int ni;

	double BETA_R=0.001, BETA_P=0.001, BETA_P0=0.001, BETA_H=0.001;
	double BETA_CPT[2]; // factor of beta at change points
	double SUS[9]; // susceptibility of 9 age groups

	char *char_seed    = NULL;
	char *char_VERBOSE = NULL;
	char *char_ni      = NULL;
	char *char_betaR   = NULL;
	char *char_betaH   = NULL;
	char *char_betaP   = NULL;
	char *char_betaS   = NULL;

	char **flag, **value;
	char pop_file[1000];
	char mobility_file[1000];
	char area_file[1000];
	char matrix_file[1000];
	char param_file[1000];
	char param_file_2[1000];
	char vacc_param_file[1000];
	char names_file[1000];
	char input_adherence_scenario[1000];
	char input_doses_scenario[1000];
	char input_municip_prior[1000];
	char input_importation[1000];
	char input_seasonality[1000];
	char input_control[1000];
	char input_relative_reopen[1000];
	char input_time[1000];
	char states_file[1000];
	char help[]="NORWEGIAN IBM\n -e [exp_dir] -i [initial_infected:0] -v [verbose:0]  -s [seed:0] \n";
	char outdir[10000];

	Grid grid;
	Individuals individuals;
	Households households;
	Places places;
	Hospitals hospitals;
	Prob_trans pt;
	Counter counter;
	Seeding seeding;
	Probability probability;
	Mobility mobility;

	// read command line parameters
	if(argc<7){
		fprintf(stderr,"HELP:\n %s\n", help);
		exit(1);
	}

	if(argc>=7 && (strcmp(argv[1],"-h")==0 || strcmp(argv[1],"--help")==0 || strcmp(argv[1],"-help")==0)){
		fprintf(stderr,"%s\n",help);
		exit(1);
	}

	if((exit_parser=parser(argc,argv,&flag,&value,&nflags))!=0){
		fprintf(stderr,"parser: syntax error\n");
		fprintf(stderr,"%s\n",help);
		exit(1);
	}

	if((char_seed=get_value(flag,value,nflags,"-s"))==NULL){
		char_seed="0";
	}

	if((char_VERBOSE=get_value(flag,value,nflags,"-v"))!=NULL){
		sscanf(char_VERBOSE,"%d",&VERBOSE);
	}else{
		VERBOSE=0;
	}

	if((char_ni=get_value(flag,value,nflags,"-i"))!=NULL){
		sscanf(char_ni,"%d",&ni);
	}else{
		ni=0;
	}

	if((char_betaR=get_value(flag, value,nflags,"-bR"))!=NULL){
		sscanf(char_betaR,"%lf", &BETA_R);
	}else{
		fprintf(stderr,"WARNING: parameter -bR undefined\n assigned default value \n");
	}

	if((char_betaP=get_value(flag, value,nflags,"-bP"))!=NULL){
		sscanf(char_betaP,"%lf", &BETA_P);
	}else{
		fprintf(stderr,"WARNING: parameter -bP undefined\n assigned default value \n");
	}

	if((char_betaS=get_value(flag, value,nflags,"-bS"))!=NULL){
		sscanf(char_betaS,"%lf", &BETA_P0);
	}else{
		fprintf(stderr,"WARNING: parameter -bS undefined\n assigned default value \n");
	}

	if((char_betaH=get_value(flag, value,nflags,"-bH"))!=NULL){
		sscanf(char_betaH,"%lf", &BETA_H);
	}else{
		fprintf(stderr,"WARNING: parameter -bH undefined\n assigned default value \n");
	}

	// add change point
	if((char_betaR=get_value(flag, value,nflags,"-beta_cpt1"))!=NULL){
		sscanf(char_betaR,"%lf", &BETA_CPT[1-1]);
	} else {
		fprintf(stderr,"WARNING: parameter -beta_cpt1 undefined\n assigned default value \n");
	}

	if((char_betaR=get_value(flag, value,nflags,"-beta_cpt2"))!=NULL){
		sscanf(char_betaR,"%lf", &BETA_CPT[2-1]);
	} else {
		fprintf(stderr,"WARNING: parameter -beta_cpt2 undefined\n assigned default value \n");
	}

	// add susceptibility
	if((char_betaR=get_value(flag, value,nflags,"-sus1"))!=NULL){
		sscanf(char_betaR,"%lf", &SUS[1-1]);
	} else {
		fprintf(stderr,"WARNING: parameter -sus1 undefined\n assigned default value \n");
	}

	if((char_betaR=get_value(flag, value,nflags,"-sus2"))!=NULL){
		sscanf(char_betaR,"%lf", &SUS[2-1]);
	} else {
		fprintf(stderr,"WARNING: parameter -sus2 undefined\n assigned default value \n");
	}

	if((char_betaR=get_value(flag, value,nflags,"-sus3"))!=NULL){
		sscanf(char_betaR,"%lf", &SUS[3-1]);
	} else {
		fprintf(stderr,"WARNING: parameter -sus3 undefined\n assigned default value \n");
	}

	if((char_betaR=get_value(flag, value,nflags,"-sus4"))!=NULL){
		sscanf(char_betaR,"%lf", &SUS[4-1]);
	} else {
		fprintf(stderr,"WARNING: parameter -sus4 undefined\n assigned default value \n");
	}

	if((char_betaR=get_value(flag, value,nflags,"-sus5"))!=NULL){
		sscanf(char_betaR,"%lf", &SUS[5-1]);
	} else {
		fprintf(stderr,"WARNING: parameter -sus5 undefined\n assigned default value \n");
	}

	if((char_betaR=get_value(flag, value,nflags,"-sus6"))!=NULL){
		sscanf(char_betaR,"%lf", &SUS[6-1]);
	} else {
		fprintf(stderr,"WARNING: parameter -sus6 undefined\n assigned default value \n");
	}

	if((char_betaR=get_value(flag, value,nflags,"-sus7"))!=NULL){
		sscanf(char_betaR,"%lf", &SUS[7-1]);
	} else {
		fprintf(stderr,"WARNING: parameter -sus7 undefined\n assigned default value \n");
	}

	if((char_betaR=get_value(flag, value,nflags,"-sus8"))!=NULL){
		sscanf(char_betaR,"%lf", &SUS[8-1]);
	} else {
		fprintf(stderr,"WARNING: parameter -sus8 undefined\n assigned default value \n");
	}

	if((char_betaR=get_value(flag, value,nflags,"-sus9"))!=NULL){
		sscanf(char_betaR,"%lf", &SUS[9-1]);
	} else {
		fprintf(stderr,"WARNING: parameter -sus9 undefined\n assigned default value \n");
	}

	if((EXP_DIR=get_value(flag, value, nflags, "-e" ))==NULL){
		fprintf(stderr,"Error reading -e option\n");
		fprintf(stderr,"%s\n",help);
		exit(1);
	}

	// initialize gsl
	fprintf(stderr, "\n");
	setenv("GSL_RNG_SEED",  char_seed, 1);

	gsl_rng_env_setup();
	R_GLOBAL = gsl_rng_alloc (gsl_rng_default);

	// create output files dir
	sprintf(outdir,"%s/output_files/",EXP_DIR);
	mkdir(outdir,S_IRWXU);

	// read parameters.txt (old parameters)
	sprintf(param_file,"%s/input_files/parameters.txt",EXP_DIR);
	fprintf(stderr, "\nRead parameters (parameters.txt)");
	if(read_param(param_file,'\t')!=0){
		fprintf(stderr,"error in read_param\n");
		exit(1);
	}
	fprintf(stderr, " ... done\n");

	// read time
	sprintf(input_time, "%s/input_files/scenarios_txt/%s", EXP_DIR, "time.txt");
	fprintf(stderr, "Read time from %s", input_time );
	if( read_time(input_time, '\t') != 0 ){
		fprintf(stderr,"error in read_time()\n");
		exit(1);
	}
	fprintf(stderr, " ... done\n");

	// read parameters_vaccination.txt
	sprintf( param_file_2, "%s/input_files/parameters_vaccination.txt", EXP_DIR);
	fprintf( stderr, "Read parameters 2 (parameters_vaccination.txt)");
	if(read_param_2( param_file_2,'\t')!=0){
		fprintf(stderr,"error in read_param\n");
		exit(1);
	}
	fprintf(stderr, " ... done\n");

	if(VERBOSE>4)
	print_parameters();

	// Read read scenario names
	sprintf( names_file, "%s/input_files/names_scenario.txt", EXP_DIR );
	fprintf(stderr, "Read names scenario (names_scenario.txt)");
	if(read_scenario_names ( names_file,'\t')!=0){
		fprintf(stderr,"error in read_scenario_names()\n");
		exit(1);
	}
	fprintf(stderr, " ... done\n");
	print_scenario_names();

	// Read adherence parameters
	sprintf( input_adherence_scenario, "%s/input_files/scenarios_txt/%s", EXP_DIR, param.NAME_ADHERENCE_SCEN );
	fprintf(stderr, "Read param adherence from %s", input_adherence_scenario);
	if( read_adherence_param( input_adherence_scenario,'\t') != 0 ){
		fprintf(stderr,"error in read_adherence_param()\n");
		exit(1);
	}
	fprintf(stderr, " ... done\n");

	// Read vaccine profile from file
	sprintf( vacc_param_file, "%s/input_files/scenarios_txt/%s", EXP_DIR , param.NAME_VAX_PROFILE_SCEN );
	fprintf(stderr, "Read vaccine parameters from %s", vacc_param_file);
	if(read_vaccine_param ( vacc_param_file,'\t')!=0){
		fprintf(stderr,"error in read_vaccine_param()\n");
		exit(1);
	}
	fprintf(stderr, " ... done\n");

	if(VERBOSE>4)
	print_vaccine_parameters();

	// Read daily doses
	sprintf(input_doses_scenario, "%s/input_files/scenarios_txt/%s", EXP_DIR, param.NAME_DOSES_SCEN);
	fprintf(stderr, "Read daily doses from %s", input_doses_scenario );
	if( read_daily_doses(input_doses_scenario, '\t') != 0 ){
		fprintf(stderr,"error in read_daily_doses()\n");
		exit(1);
	}
	fprintf(stderr, " ... done\n");

	// read municip prior
	sprintf(input_municip_prior, "%s/input_files/scenarios_txt/%s", EXP_DIR, param.NAME_GEO);
	fprintf(stderr, "Read municip prior from %s", input_municip_prior );
	if( read_municip_prior(input_municip_prior, '\t') != 0 ){
		fprintf(stderr,"error in read_municip_prior()\n");
		exit(1);
	}
	fprintf(stderr, " ... done\n");

	// read importation
	sprintf(input_importation, "%s/input_files/scenarios_txt/%s", EXP_DIR, param.NAME_IMPORT_SCEN);
	fprintf(stderr, "Read importation from %s", input_importation );
	if( read_importation(input_importation, '\t') != 0 ){
		fprintf(stderr,"error in read_importation()\n");
		exit(1);
	}
	fprintf(stderr, " ... done\n");

	// read seasonality
	sprintf(input_seasonality, "%s/input_files/seasonality.txt", EXP_DIR);
	fprintf(stderr, "Read seasonality from %s", input_seasonality );
	if( read_seasonality(input_seasonality, '\t') != 0 ){
		fprintf(stderr,"error in read_seasonality()\n");
		exit(1);
	}
	fprintf(stderr, " ... done\n");

	// read control
	sprintf(input_control, "%s/input_files/scenarios_txt/%s", EXP_DIR, param.NAME_CONTROL_SCEN);
	fprintf(stderr, "Read control from %s", input_control );
	if( read_control_prev_H(input_control, '\t') != 0 ){
		fprintf(stderr,"error in read_control_prev_H()\n");
		exit(1);
	}
	fprintf(stderr, " ... done\n");

	// read relative_reopen
	sprintf(input_relative_reopen, "%s/input_files/scenarios_txt/%s", EXP_DIR, param.NAME_REOPEN_SCEN);
	fprintf(stderr, "Read municip prior from %s", input_relative_reopen );
	if( read_relative_reopen(input_relative_reopen, '\t') != 0 ){
		fprintf(stderr,"error in read_relative_reopen()\n");
		exit(1);
	}
	fprintf(stderr, " ... done\n");

	// read population data
	sprintf(pop_file,"%s/input_files/population_grunnkretser_2021-20210427_grunnkrets_column_and_new_abm_muni_codes.txt",EXP_DIR);
	if( read_population_data(pop_file,'\t',&grid) != 0 ){
		fprintf(stderr,"error in read_population_data_pop\n");
		exit(1);
	}

	// fill the regions
	regions( &grid );

	// read states data
	sprintf(states_file,"%s/input_files/states.txt",EXP_DIR);
	if( read_states_data(states_file,'\t',&grid)!=0){
		fprintf(stderr,"error in read_states_data_pop (states.txt)\n");
		exit(1);
	}

	// read contact matrix
	fprintf(stderr, "Read contact matrix (community_contact_matrix_NORWAY.txt)");
	sprintf( matrix_file, "%s/input_files/community_contact_matrix_NORWAY.txt", EXP_DIR );
	if( read_contact_matrix( matrix_file, '\t', &probability ) != 0 ){
		fprintf(stderr,"error in read_contact_matrix\n");
		exit(1);
	}
	fprintf(stderr, " ... done\n");

	// read mobility data
	fprintf(stderr, "Read mobility data ");
	sprintf( mobility_file, "%s/input_files/mobility_data-municipality-2021-01-25-356mun.txt", EXP_DIR );
	if( read_mobility_data( mobility_file, '\t', &mobility, &grid ) != 0 ){
		fprintf(stderr,"error in read_mobility_data (mobility_data.txt)\n");
		exit(1);
	}
	fprintf(stderr, " ... done\n");

	// read area data
	fprintf(stderr, "Read area data");
	sprintf( area_file, "%s/input_files/area_data.txt", EXP_DIR );
	if( read_area_data( area_file, '\t', &grid ) != 0 ){
		fprintf(stderr,"error in read_area_data()\n");
		exit(1);
	}
	fprintf(stderr, " ... done\n");

	// beta as an input in seasonality()
	param.BETA_R_CAL  = BETA_R  * 0.24;
	param.BETA_H_CAL  = BETA_H  * 1.00;
	param.BETA_P0_CAL = BETA_P0 * 0.166;
	param.BETA_P1_CAL = BETA_P  * 0.188;

	// beta as an output in seasonality()
	param.BETA_R  = BETA_R  * 0.24;
	param.BETA_H  = BETA_H  * 1.00;
	param.BETA_P0 = BETA_P0 * 0.166;
	param.BETA_P1 = BETA_P  * 0.188;

	fprintf(stderr,"\n----------\n");
	fprintf(stderr, "param.BETA_R            %lf\n", param.BETA_R );
	fprintf(stderr, "param.BETA_H            %lf\n", param.BETA_H );
	fprintf(stderr, "param.BETA_P0 (School)  %lf\n", param.BETA_P0 );
	fprintf(stderr, "param.BETA_P1 (Work)    %lf\n", param.BETA_P1 );

	// beta at change points
	for( int cpt = 1; cpt <= 2; cpt++ ){
		param.BETA_CPT[cpt-1] = BETA_CPT[cpt-1];
		fprintf(stderr, "param.BETA_CPT[%d] = %lf\n", cpt, param.BETA_CPT[cpt-1]);
	}
	// index of change point
	param.CPT = 0;

	// overwrite susceptibility of 9 age groups
	param.SUS_0_9   = SUS[1-1];
	param.SUS_10_19 = SUS[2-1];
	param.SUS_20_29 = SUS[3-1];
	param.SUS_30_39 = SUS[4-1];
	param.SUS_40_49 = SUS[5-1];
	param.SUS_50_59 = SUS[6-1];
	param.SUS_60_69 = SUS[7-1];
	param.SUS_70_79 = SUS[8-1];
	param.SUS_80p   = SUS[9-1];
	for( int age_group = 1; age_group <= 9; age_group++ ){
		fprintf(stderr, "SUS[%d] = %lf\n", age_group, SUS[age_group-1]);
	}

	simulate( &grid, &households, &individuals, &places, &hospitals, &pt, ni, &counter, &seeding, &mobility, &probability );

	return(0);
} // int main

int simulate( Grid *grid, Households *households, Individuals *individuals, Places *places, Hospitals *hospitals, Prob_trans *pt ,int ni, Counter *counter, Seeding *seeding, Mobility *mobility, Probability *probability ){

	register int j;
	int t ;

	// read_structures_from_file
	read_structures_from_file( grid, households, individuals, places, hospitals, pt , ni, counter, seeding, mobility, probability );

	// Transmission rates
	prob_transmission( pt );

	// Transform distances in kernel values and normalize the sum for each cell ( to be inserted into places.c )
	kernel( grid );

	// start from all S
	individuals->status = (unsigned short int *) calloc( individuals->n_i + 1, sizeof(unsigned short int) );
	for( j=1; j<=individuals->n_i; j++ ){
		individuals->status[j]=S;
		individuals->next_transition[j]=-1;
	}

	// start from ni persons infected
	int tot_inf = 0;
	while(tot_inf < ni){
		int inf_p = 1 + gsl_rng_uniform_int( R_GLOBAL, individuals->n_i);

		if(individuals->status[inf_p] == S){
			individuals->status[inf_p] = I;
		}
		tot_inf++;
	}

	// main part: transmission
	t = 0;
	t = SEIR_model( grid, households, individuals, places, hospitals, pt, counter, seeding, mobility, probability );

	free( individuals->employment );

	return t;
} // int simulate
