#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "header.h"

extern Param param;
extern char *EXP_DIR;

int read_structures_from_file( Grid *grid, Households *households, Individuals *individuals, Places *places, Hospitals *hospitals, Prob_trans *pt ,int ni, Counter *counter, Seeding *seeding, Mobility *mobility, Probability *probability ){

  int i = 1;
  FILE *fp;
  int i_ind, age, idx_h, idx_p, employment, school_grade, risk_group, cell, mun, county, home_office;
  int i_h, idx_c, n_i, idx_i_ini, idx_i_fin, idx_i;
  int i_p, cat_n_i, type, P00_i_r, P00_f_r, P01_i_r, P01_f_r, P02_i_r, P02_f_r;
  int P03_i_r, P03_f_r, P04_i_r, P04_f_r, P1_i_r, P1_f_r;
  double x, y;
  time_t time_start, time_end; // Timing the reading
  char *filename = NULL;

  // Individuals ###############################################################
  // Memory allocation
  // Allocate arrays
  individuals->age          = ( short int *)          calloc ( 2, sizeof( short int));
  individuals->idx_h        = ( unsigned int *)       calloc ( 2, sizeof( unsigned int));
  individuals->idx_p        = ( unsigned int *)       calloc ( 2, sizeof( unsigned int));
  individuals->employment   = ( unsigned short int *) calloc ( 2, sizeof( unsigned short int));
  individuals->school_grade = ( int *)                calloc ( 2, sizeof( int));
  individuals->risk_group   = ( int *)                calloc ( 2, sizeof( int));
  individuals->cell         = ( unsigned int *)       calloc ( 2, sizeof( unsigned int));
  individuals->mun          = ( unsigned int *)       calloc ( 2, sizeof( unsigned int));
  individuals->county       = ( unsigned int *)       calloc ( 2, sizeof( unsigned int));
  individuals->x            = ( double *)             calloc ( 2, sizeof( double));
  individuals->y            = ( double *)             calloc ( 2, sizeof( double));
  individuals->home_office  = ( int *)                calloc ( 2, sizeof( int));

  time_start = time(NULL);
  filename = (char*) calloc(1000, sizeof(char));
  sprintf( filename, "%s/input_files/individuals.txt", EXP_DIR );
  fprintf(stderr, "Reading file %s\n", filename);

  if( !(fp = fopen(filename, "r")) ){
    fprintf(stderr,"read_structures_from_file(): error opening file %s for reading\n", filename);
    return 1;
  }

  while( fscanf(fp, "%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%lf\t%lf\t%d\t%d\t%d\n",
  &i_ind, &age, &idx_h, &idx_p, &employment, &school_grade,
  &risk_group, &cell, &mun, &county, &x, &y, &home_office,
  &idx_i_ini, &idx_i_fin )!=EOF ){

    individuals->age[i] = age;
    individuals->idx_h[i] = idx_h;
    individuals->idx_p[i] = idx_p;
    individuals->employment[i] = employment;
    individuals->school_grade[i] = school_grade;
    individuals->risk_group[i] = risk_group;
    individuals->cell[i] = cell;
    individuals->mun[i] = mun;
    individuals->county[i] = county;
    individuals->x[i] = x;
    individuals->y[i] = y;
    individuals->home_office[i] = home_office;
    individuals->n_i = i;

    if( idx_i_ini == 1 )
    grid->cell[cell].idx_i_ini = i;

    if( idx_i_fin == 1 )
    grid->cell[cell].idx_i_fin = i;

    grid->cell[cell].n_i += 1;
    i++;

    // Reallocation of memory
    individuals->age          = (short int *)          realloc( individuals->age,          (i+1) * sizeof( short int));
    individuals->idx_h        = (unsigned int *)       realloc( individuals->idx_h,        (i+1) * sizeof( unsigned  int));
    individuals->idx_p        = (unsigned int *)       realloc( individuals->idx_p,        (i+1) * sizeof( unsigned  int));
    individuals->employment   = (unsigned short int *) realloc( individuals->employment,   (i+1) * sizeof( unsigned short int));
    individuals->school_grade = (int *)                realloc( individuals->school_grade, (i+1) * sizeof( int));
    individuals->risk_group   = (int *)                realloc( individuals->risk_group,   (i+1) * sizeof( int));
    individuals->cell         = (unsigned int *)       realloc( individuals->cell,         (i+1) * sizeof( unsigned int));
    individuals->mun          = (unsigned int *)       realloc( individuals->mun,          (i+1) * sizeof( unsigned int));
    individuals->county       = (unsigned int *)       realloc( individuals->county,       (i+1) * sizeof( unsigned int));
    individuals->x            = (double *)             realloc( individuals->x,            (i+1) * sizeof( double));
    individuals->y            = (double *)             realloc( individuals->y,            (i+1) * sizeof( double));
    individuals->home_office  = (int *)                realloc( individuals->home_office,  (i+1) * sizeof( int));
  }
  fclose(fp);

  // Allocate additional arrays for the structure Individuals
  individuals->hosp_entrance            = (int*)                calloc( individuals->n_i+1, sizeof(int));
  individuals->hosp_leaving             = (int*)                calloc( individuals->n_i+1, sizeof(int));
  individuals->icu_entrance             = (int*)                calloc( individuals->n_i+1, sizeof(int));
  individuals->icu_leaving              = (int*)                calloc( individuals->n_i+1, sizeof(int));
  individuals->vaccinated               = (int*)                calloc( individuals->n_i+1, sizeof(int));
  individuals->risk_group               = (int*)                calloc( individuals->n_i+1, sizeof(int));
  individuals->time_vaccination         = (int*)                calloc( individuals->n_i+1, sizeof(int));
  individuals->risk_category            = (int*)                calloc( individuals->n_i+1, sizeof(int));
  individuals->vaccine_efficacy_1st_Ia  = (double*)             calloc( individuals->n_i+1, sizeof(double));
  individuals->vaccine_efficacy_1st_I   = (double*)             calloc( individuals->n_i+1, sizeof(double));
  individuals->vaccine_efficacy_1st_sd  = (double*)             calloc( individuals->n_i+1, sizeof(double));
  individuals->vaccine_efficacy_1st_d   = (double*)             calloc( individuals->n_i+1, sizeof(double));
  individuals->vaccine_efficacy_1st_V   = (double*)             calloc( individuals->n_i+1, sizeof(double));
  individuals->vaccine_efficacy_2nd_Ia  = (double*)             calloc( individuals->n_i+1, sizeof(double));
  individuals->vaccine_efficacy_2nd_I   = (double*)             calloc( individuals->n_i+1, sizeof(double));
  individuals->vaccine_efficacy_2nd_sd  = (double*)             calloc( individuals->n_i+1, sizeof(double));
  individuals->vaccine_efficacy_2nd_d   = (double*)             calloc( individuals->n_i+1, sizeof(double));
  individuals->vaccine_efficacy_2nd_V   = (double*)             calloc( individuals->n_i+1, sizeof(double));
  individuals->vaccine_priority_1       = (int*)                calloc( individuals->n_i+1, sizeof(int));
  individuals->vaccine_priority_2       = (int*)                calloc( individuals->n_i+1, sizeof(int));
  individuals->vaccine_priority_3       = (int*)                calloc( individuals->n_i+1, sizeof(int));
  individuals->death                    = (int*)                calloc( individuals->n_i+1, sizeof(int));
  individuals->county_priority          = (int*)                calloc( individuals->n_i+1, sizeof(int));
  individuals->time_of_infection        = (int*)                calloc( individuals->n_i+1, sizeof(int));
  individuals->no_of_infected           = (int*)                calloc( individuals->n_i+1, sizeof(int));
  individuals->hospitalized             = (unsigned short int*) calloc( individuals->n_i+1, sizeof(unsigned short int));
  individuals->next_transition          = (int*)                calloc( individuals->n_i+1, sizeof(int));
  individuals->stay_at_home             = (int*)                calloc( individuals->n_i+1, sizeof(int));
  individuals->quarantine               = (int*)                calloc( individuals->n_i+1, sizeof(int));
  individuals->id_cluster               = (int*)                calloc( individuals->n_i+1, sizeof(int));

  // Initialise the vectors above
  for( i=1; i <= individuals->n_i; i++ ){

    individuals->hosp_entrance[i] = 0;
    individuals->hosp_leaving[i] = 0;
    individuals->icu_entrance[i] = 0;
    individuals->icu_leaving[i] = 0;
    individuals->vaccinated[i] = 0;
    individuals->risk_group[i] = 0;
    individuals->time_vaccination[i] = 0;
    individuals->risk_category[i] = 0;
    individuals->risk_category[i] = 0;
    individuals->vaccine_efficacy_1st_Ia[i] = 0;
    individuals->vaccine_efficacy_1st_I[i] = 0;
    individuals->vaccine_efficacy_1st_sd[i] = 0;
    individuals->vaccine_efficacy_1st_d[i] = 0;
    individuals->vaccine_efficacy_1st_V[i] = 0;
    individuals->vaccine_efficacy_2nd_Ia[i] = 0;
    individuals->vaccine_efficacy_2nd_I[i] = 0;
    individuals->vaccine_efficacy_2nd_sd[i] = 0;
    individuals->vaccine_efficacy_2nd_d[i] = 0;
    individuals->vaccine_efficacy_2nd_V[i] = 0;
    individuals->vaccine_priority_1[i] = 0;
    individuals->vaccine_priority_2[i] = 0;
    individuals->vaccine_priority_3[i] = 0;
    individuals->death[i] = 0;
    individuals->county_priority[i] = 0;
    individuals->time_of_infection[i] = -9999;
    individuals->no_of_infected[i]=0;
    individuals->hospitalized[i] = NO;
    individuals->next_transition[i] = -1;
    individuals->stay_at_home[i] = 0;
    individuals->quarantine[i] = param.MAXT+1;
    individuals->id_cluster[i] = -9999;
  }

  fclose(fp);
  free(filename);

  // Households ###############################################################
  households->household = (Household *) calloc ( 2, sizeof(Household));

  filename = (char*) calloc(1000, sizeof(char));
  sprintf( filename, "%s/input_files/households.txt", EXP_DIR );
  fprintf(stderr, "Reading file %s\n", filename);

  if( !(fp = fopen(filename, "r")) ){
    fprintf(stderr,"read_structures_from_file(): error opening file %s for reading\n", filename);
    return 1;
  }

  int h = 1;

  while( fscanf(fp, "%d\t%d\t%d\t%d\t%d\t%d\n", &i_h, &idx_c, &n_i, &idx_i_ini, &idx_i_fin, &idx_i )!= EOF ){
    if( h == 1 ){
      households->household[i_h].idx_i = (unsigned int *) calloc( n_i + 1, sizeof( unsigned  int ));
    }
    households->household[i_h].idx_c     = idx_c;
    households->household[i_h].n_i       = n_i;
    households->household[i_h].idx_i_ini = idx_i_ini;
    households->household[i_h].idx_i_fin = idx_i_fin;
    households->household[i_h].idx_i[h]  = idx_i;

    if ( h < n_i ){
      h+=1;
    }else{
      h=1;
      households->n_h = i_h;
      households->household = (Household*) realloc (households->household, (households->n_h+2)*sizeof(Household) );
    }
  }

  fclose(fp);
  free(filename);

  // Places ###############################################################
  places->P00_i_r = (int*) calloc ( grid->n_r+1, sizeof(int) );
  places->P00_f_r = (int*) calloc ( grid->n_r+1, sizeof(int) );
  places->P04_i_r = (int*) calloc ( grid->n_r+1, sizeof(int) );
  places->P04_f_r = (int*) calloc ( grid->n_r+1, sizeof(int) );
  places->P01_i_r = (int*) calloc ( grid->n_r+1, sizeof(int) );
  places->P01_f_r = (int*) calloc ( grid->n_r+1, sizeof(int) );
  places->P02_i_r = (int*) calloc ( grid->n_r+1, sizeof(int) );
  places->P02_f_r = (int*) calloc ( grid->n_r+1, sizeof(int) );
  places->P03_i_r = (int*) calloc ( grid->n_r+1, sizeof(int) );
  places->P03_f_r = (int*) calloc ( grid->n_r+1, sizeof(int) );
  places->P1_i_r  = (int*) calloc ( grid->n_r+1, sizeof(int) );
  places->P1_f_r  = (int*) calloc ( grid->n_r+1, sizeof(int) );

  places->place = (Place *) calloc ( 2, sizeof(Place));

  filename = (char*) calloc(1000, sizeof(char));

  sprintf( filename, "%s/input_files/places.txt", EXP_DIR );
  fprintf(stderr, "Reading file %s\n", filename);

  if( !(fp = fopen(filename, "r")) ){
    fprintf(stderr,"read_structures_from_file(): error opening file %s for reading\n", filename);
    return 1;
  }

  int p = 1;

  while( fscanf(fp, "%d\t%d\t%d\t%d\t%d\t%lf\t%lf\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
  &i_p, &idx_c, &n_i, &cat_n_i, &type, &x, &y, &idx_i,
  &P00_i_r, &P00_f_r, &P01_i_r, &P01_f_r, &P02_i_r, &P02_f_r,
  &P03_i_r, &P03_f_r, &P04_i_r, &P04_f_r, &P1_i_r,  &P1_f_r  )!= EOF ){

    if( p == 1 ){
      places->place[i_p].idx_i = (unsigned int *) calloc( n_i+1, sizeof( unsigned  int ));
    }
    places->place[i_p].idx_c    = idx_c;
    places->place[i_p].n_i      = n_i;
    places->place[i_p].cat_n_i  = cat_n_i;
    places->place[i_p].type     = type;
    places->place[i_p].x        = x;
    places->place[i_p].y        = y;
    places->place[i_p].idx_i[p] = idx_i;

    places->P00_i_r[1] = P00_i_r;
    places->P00_f_r[1] = P00_f_r;
    places->P01_i_r[1] = P01_i_r;
    places->P01_f_r[1] = P01_f_r;
    places->P02_i_r[1] = P02_i_r;
    places->P02_f_r[1] = P02_f_r;
    places->P03_i_r[1] = P03_i_r;
    places->P03_f_r[1] = P03_f_r;
    places->P04_i_r[1] = P04_i_r;
    places->P04_f_r[1] = P04_f_r;
    places->P1_i_r [1] = P1_i_r ;
    places->P1_f_r [1] = P1_f_r ;

    if ( p < n_i ){
      p+=1;
      places->place[i_p].idx_i = (unsigned int *) realloc( places->place[i_p].idx_i , (p+1) * sizeof( unsigned  int ));
    }else{
      p=1;
      places->n_p = i_p;
      places->place = (Place*) realloc (places->place, (places->n_p+2)*sizeof(Place) );
    }
  }
  fclose(fp);
  free(filename);

  // Grid distance ###############################################################
  double coeff_distance=COEFF_DISTANCE;
  double diffx;
  double diffy;
  int nc;

  for( int i = 1; i <= grid->n_r; i++ ){
    nc = grid->region[i].idx_c_fin - grid->region[i].idx_c_ini + 1;

    /* For each cell of the region ... */
    for( int j = grid->region[i].idx_c_ini; j <= grid->region[i].idx_c_fin; j++ ){
      /* ...compute probability of commuting to other cells by the gravity model*/
      grid->cell[j].dist = (unsigned short int *) calloc ( nc + 1, sizeof( unsigned short int ) );

      idx_c=1;
      for( int k = grid->region[i].idx_c_ini; k <= grid->region[i].idx_c_fin; k++ ){
        diffx = ( grid->cell[j].x - grid->cell[k].x );
        diffy = ( grid->cell[j].y - grid->cell[k].y );
        grid->cell[j].dist[idx_c] = (int) round ( coeff_distance*sqrt( diffx*diffx+diffy*diffy ));
        idx_c++;
      }
    }
  }

  for( i = 0; i < N_MUN; i++ ){
    grid->municipality[i].n_i = 0;
    grid->municipality[i].individuals = (unsigned int*) calloc( 1, sizeof(unsigned int) );
  }

  int IDmun;
  for( int r = 1;r <= grid->n_r; r++ ){
    for( int c = grid->region[r].idx_c_ini; c <= grid->region[r].idx_c_fin; c++ ){
      IDmun = grid->cell[c].mun_ID - 1;

      for( int i = grid->cell[c].idx_i_ini; i <= grid->cell[c].idx_i_fin; i++ ){
        grid->municipality[IDmun].individuals[ grid->municipality[IDmun].n_i ] = i;
        grid->municipality[IDmun].n_i++;
        grid->municipality[IDmun].individuals = (unsigned int*) realloc( grid->municipality[IDmun].individuals, (grid->municipality[IDmun].n_i+1)*sizeof(unsigned int) );
      }
    }
  }

  for( i = 0; i < N_COUNTIES; i++ ){
    grid->county[i].n_i = 0;
    grid->county[i].individuals = (unsigned int*) calloc( 1, sizeof(unsigned int) );
  }

  int IDcounty;
  for( int r = 1;r <= grid->n_r; r++ ){
    for( int c = grid->region[r].idx_c_ini; c <= grid->region[r].idx_c_fin; c++ ){
      IDcounty = grid->cell[c].county_ID - 1;

      for( int i = grid->cell[c].idx_i_ini; i <= grid->cell[c].idx_i_fin; i++ ){
        grid->county[IDcounty].individuals[ grid->county[IDcounty].n_i ] = i;
        grid->county[IDcounty].n_i++;
        grid->county[IDcounty].individuals = (unsigned int*) realloc( grid->county[IDcounty].individuals, (grid->county[IDcounty].n_i+1)*sizeof(unsigned int) );
        individuals->county[i] = IDcounty;
      }
    }
  }

  // Timing
  time_end = time(NULL);
  fprintf(stdout, "read_structures_from_file() took %ld seconds\n", time_end - time_start);

  return 1;
}
