#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

extern char *EXP_DIR;
extern Param param;

int read_population_data(char file[],char sep,Grid *grid)
/*
Read a "population" file with columns separated by sep

x_1 y_1 n_1
x_2 y_2 n_2
...
x_r  y_r n_r
Return value: 0 in case of no errors
1 column: longitude
2 column: latitude
3 column: ID cell (from 1 to number of rows)
4 column: ID state
5 column: number of inhabitants
6 column: ID municipality
7 column: ID county
8 column: unique ID for the municipalities

*/
{
  char *line;
  int out_get_line=3;
  FILE *fp;
  int n_rows;
  int tmp;

  if(!(fp = fopen(file, "r"))){
    fprintf(stderr,"ERROR: file %s not found\n",file);
    return 1;
  }

  n_rows=0;

  grid->cell = (Cell*) calloc(1,sizeof(Cell));

  while(out_get_line>=2){
    out_get_line=get_line(&line,fp);
    if(out_get_line<3){
      switch(out_get_line){
        case 2:
        fprintf(stderr,"read_population_data: line %d of file %s does not end in newline\n",n_rows,file);
        break;
        case 1:
        fprintf(stderr,"read_population_data: file %s contains an empty line\n",file);
        return 1;
        break;
        case 0:
        grid->n_c = n_rows;
        fclose(fp);
        return 0;
        break;
        case -1:
        fprintf(stderr,"read_population_data: get_line error on file %s\n",
        file);
        return 1;
        default:
        fprintf(stderr,"read_population_data: unrecognized exit status of get_line on file %s\n",file);
        return 1;
        break;
      }
    }

    n_rows++;
    grid->cell=(Cell*)realloc(grid->cell,(n_rows+1)*sizeof(Cell));

    sscanf(line,"%lf", &(grid->cell[n_rows].x));
    line = (char *)strchr(line, sep);
    line++;

    sscanf(line,"%lf", &(grid->cell[n_rows].y));
    line = (char *)strchr(line, sep);
    line++;

    sscanf(line,"%d", &tmp);
    if(tmp<0){
      fprintf(stderr,"ERROR: cell name at row %d of population data is negative\n", n_rows);
      exit(1);
    }
    grid->cell[n_rows].c=tmp;
    line = (char *)strchr(line, sep);
    line++;

    sscanf(line,"%d", &tmp);
    if(tmp<0){
      fprintf(stderr,"ERROR: region name at row %d of population data is negative\n", n_rows);
      exit(1);
    }
    if(n_rows>1)
    if(tmp<grid->cell[n_rows-1].r){
      fprintf(stderr,"ERROR: region name at row %d of population data is less than that at row %d\n",n_rows,n_rows-1);
      exit(1);
    }

    grid->cell[n_rows].r=tmp;
    line = (char *)strchr(line, sep);
    line++;

    sscanf(line,"%d", &tmp);
    if(tmp<=0){
      fprintf(stderr,"ERROR: population size at row %d of population data is less or equal than zero\n", n_rows);
      exit(1);
    }
    grid->cell[n_rows].n_i_orig=tmp;
    line = (char *)strchr(line, sep);
    line++;

    sscanf(line,"%d", &tmp);
    if(tmp<=0){
      fprintf(stderr,"ERROR: municipality name at row %d of population data is less or equal than zero\n", n_rows);
      exit(1);
    }
    grid->cell[n_rows].mun = tmp;
    line = (char *)strchr(line, sep);
    line++;

    sscanf(line,"%d", &tmp);
    if(tmp<=0){
      fprintf(stderr,"ERROR: county name at row %d of population data is less or equal than zero\n", n_rows);
      exit(1);
    }
    grid->cell[n_rows].mun_ID = tmp;
    line = (char *)strchr(line, sep);
    line++;

    sscanf(line,"%d", &tmp);
    if(tmp<=0){
      fprintf(stderr,"ERROR: ID municipality name at row %d of population data is less or equal than zero\n", n_rows);
      exit(1);
    }
    grid->cell[n_rows].county = tmp;
    line = (char *)strchr(line, sep);
    line++;

    sscanf(line,"%d", &tmp);
    if(tmp<=0){
      fprintf(stderr,"ERROR: ID municipality name at row %d of population data is less or equal than zero\n", n_rows);
      exit(1);
    }
    grid->cell[n_rows].county_ID = tmp;
    line = (char *)strchr(line, sep);
    line++;

  }
  return 2;
}

int read_states_data(char file[],char sep,Grid *grid)
/*
Read a "states" file with columns separated by sep

x_1 y_1 n_1
x_2 y_2 n_2

...

x_r  y_r n_r

Return value: 0 in case of no errors
1 column: ID state
2 column: longitude of the capital
3 column: latitude of the capital
4 column: GDPPPP
5 column: number of yearly passengers from ext-EU countries
6 column: number of days from 2009 Jan 1 for school summer holidays
7 column: number of days from 2009 Jan 1 for school summer reopening
8 column: number of days from 2009 Jan 1 for school fall holidays
9 column: number of days from 2009 Jan 1 for school fall reopening
10 column: number of additional days at the beginning of summer school closure
11 column: number of additional days at the end of summer school closure
12 column: probabilities of having childrend for
*/
{
  char *line;
  int out_get_line=3;
  FILE *fp;
  int n_rows=0;
  int tmp;
  int i;
  int code_r;
  int code_found;

  if(!(fp = fopen(file, "r"))){
    fprintf(stderr,"ERROR: file %s not found\n",file);
    return 1;
  }

  while(out_get_line>=2){
    out_get_line=get_line(&line,fp);
    if(out_get_line<3){
      switch(out_get_line){
        case 2:
        fprintf(stderr,"read_states_data: line %d of file %s does not end in newline\n",n_rows,file);
        break;
        case 1:
        fprintf(stderr,"read_states_data: file %s contains an empty line\n",file);
        return 1;
        break;
        case 0:
        fclose(fp);
        return 0;
        break;
        case -1:
        fprintf(stderr,"read_states_data: get_line error on file %s\n",
        file);
        return 1;
        default:
        fprintf(stderr,"read_states_data: unrecognized exit status of get_line on file %s\n",file);
        return 1;
        break;
      }
    }

    sscanf(line,"%d", &tmp);
    code_found=0;

    for(i=1;i<=grid->n_r;i++)
    if(grid->region[i].r==tmp){
      code_found=1;
      code_r=i;
      break;
    }

    if(!code_found)
    continue;

    line = (char *)strchr(line, sep);
    line++;

    sscanf(line,"%lf", &(grid->region[code_r].x));
    line = (char *)strchr(line, sep);
    line++;

    sscanf(line,"%lf", &(grid->region[code_r].y));
    line = (char *)strchr(line, sep);
    line++;

    //sscanf(line,"%lf", &(grid->region[code_r].gdpppp));
    line = (char *)strchr(line, sep);
    line++;

    //sscanf(line,"%d", &(grid->region[code_r].exteut));
    line = (char *)strchr(line, sep);
    line++;

    //sscanf(line,"%d", &(grid->region[code_r].ssi));
    line = (char *)strchr(line, sep);
    line++;

    //sscanf(line,"%d", &(grid->region[code_r].ssf));
    line = (char *)strchr(line, sep);
    line++;

    //sscanf(line,"%d", &(grid->region[code_r].fsi));
    line = (char *)strchr(line, sep);
    line++;

    //sscanf(line,"%d", &(grid->region[code_r].fsf));
    line = (char *)strchr(line, sep);
    line++;

    //sscanf(line,"%d", &(grid->region[code_r].ssi_suppl));
    line = (char *)strchr(line, sep);
    line++;

    //sscanf(line,"%d", &(grid->region[code_r].ssf_suppl));
    line = (char *)strchr(line, sep);
    line++;

    sscanf(line,"%lf", &(grid->region[code_r].hh2prop));
    line = (char *)strchr(line, sep);
    line++;

    sscanf(line,"%lf", &(grid->region[code_r].hh3prop));
    line = (char *)strchr(line, sep);
    line++;

    sscanf(line,"%lf", &(grid->region[code_r].hh4prop));
    line = (char *)strchr(line, sep);
    line++;


  }

  return 2;
}

int read_home_office_data(char file[],char sep, Individuals *individuals )
/*
Read a "seeding" file with columns separated by sep

x_1 y_1 n_1
x_2 y_2 n_2
...
x_r  y_r n_r
Return value: 0 in case of no errors
1 column: municipality
2 column: time
3 column: no. of infections
*/
{
  char *line;
  int out_get_line = 3;
  FILE *fp;
  int n_rows;
  //int tmp;

  if(!(fp = fopen(file, "r"))){
    fprintf(stderr,"ERROR: file %s not found\n",file);
    return 1;
  }

  n_rows=0;

  individuals->prop_doing_home_office = ( double* ) calloc( 1, sizeof( double ));

  while(out_get_line>=2){
    out_get_line=get_line(&line,fp);
    if(out_get_line<3){
      switch(out_get_line){
        case 2:
        fprintf(stderr, "read_home_office_data: line %d of file %s does not end in newline\n",n_rows,file);
        break;
        case 1:
        fprintf(stderr, "read_home_office_data: file %s contains an empty line\n",file);
        return 1;
        break;
        case 0:
        fclose(fp);
        return 0;
        break;
        case -1:
        fprintf(stderr, "read_home_office_data: get_line error on file %s\n",
        file);
        return 1;
        default:
        fprintf(stderr, "read_home_office_data: unrecognized exit status of get_line on file %s\n",file);
        return 1;
        break;
      }
    }

    n_rows++;
    //eeding = ( Seeding* ) realloc( seeding, ( n_rows + 1 )*sizeof( Seeding ));
    individuals->prop_doing_home_office = ( double* ) realloc( individuals->prop_doing_home_office, ( n_rows + 1 )*sizeof( double ));

    sscanf(line,"%lf", &( individuals->prop_doing_home_office[n_rows]));
    line = (char *)strchr(line, sep);
    line++;


  }
  return 2;
}

int read_area_data(char file[],char sep,Grid *grid)
/*
Read a "area" file with columns separated by sep

1 column: area number id
2 column: municipality id
*/
{
  char *line;
  int out_get_line=3;
  FILE *fp;
  int n_rows;
  int tmp_mun;
  int tmp_area = -9999;
  int tmp_area_prev = -9999;
  int n_mun_in_area = 0;

  grid->n_area = 0;

  if(!(fp = fopen(file, "r")))
  {
    fprintf(stderr,"ERROR: file %s not found\n",file);
    return 1;
  }

  n_rows=0;

  grid->area    = (Area*) calloc( 1, sizeof(Area) );
  grid->area[0].municipalities = (int*)  calloc( 1, sizeof(int) );
  grid->area[0].n_mun = 0;

  while(out_get_line>=2){
    out_get_line=get_line(&line,fp);
    if(out_get_line<3){
      switch(out_get_line){
        case 2:
        fprintf(stderr,"read_area_data: line %d of file %s does not end in newline\n",n_rows,file);
        break;
        case 1:
        fprintf(stderr,"read_area_data: file %s contains an empty line\n",file);
        return 1;
        break;
        case 0:
        // exit
        grid->n_area++;

        if( n_rows != grid->n_mun )
        fprintf(stderr,"Warning! Some municipalities are missing in area_data.txt\n" );

        fclose(fp);
        return 0;
        break;
        case -1:
        fprintf(stderr,"read_population_data: get_line error on file %s\n",
        file);
        return 1;
        default:
        fprintf(stderr,"read_population_data: unrecognized exit status of get_line on file %s\n",file);
        return 1;
        break;
      }
    }

    n_rows++;

    sscanf( line, "%d", &tmp_area );
    if( tmp_area < 0 )
    {
      fprintf(stderr,"ERROR: area code at row %d of the area's data is negative\n", n_rows);
      exit(1);
    }

    // check if the row corresponds to a new area

    if( tmp_area != tmp_area_prev && tmp_area_prev != -9999 )
    {
      grid->n_area++;
      grid->area = (Area*) realloc ( grid->area, ( grid->n_area + 1 )*sizeof(Area) );
      grid->area[tmp_area].municipalities = (int*) calloc( 1, sizeof(int) );
      n_mun_in_area = 0;
    }

    tmp_area_prev = tmp_area;
    line = (char *)strchr(line, sep);
    line++;

    sscanf(line,"%d", &tmp_mun);
    grid->area[tmp_area].municipalities[n_mun_in_area] = tmp_mun;
    n_mun_in_area++;
    grid->area[tmp_area].n_mun = n_mun_in_area;
    grid->area[tmp_area].municipalities = (int*) realloc ( grid->area[tmp_area].municipalities , (n_mun_in_area + 1)*sizeof(int));

    line = (char *)strchr(line, sep);
    line++;

  }
  return 2;
}
