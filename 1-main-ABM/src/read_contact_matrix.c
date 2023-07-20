#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

extern char *EXP_DIR;
extern Param param;

int read_contact_matrix(char file[],char sep, Probability *probability ){
  /*
  Read a "community contact matrix" file with columns separated by sep

  x_1 y_1 n_1
  x_2 y_2 n_2
  ...
  x_r  y_r n_r
  Return value: 0 in case of no errors
  1 column: municipality
  2 column: time
  3 column: no. of infections
  */

  char *line;
  int out_get_line = 3;
  FILE *fp;
  int n_rows;
  int n_age_groups = 9;
  //int tmp;

  if(!(fp = fopen(file, "r"))){
    fprintf(stderr,"ERROR: file %s not found\n",file);
    return 1;
  }

  n_rows=0;

  probability->contact_matrix = ( double** ) calloc( n_age_groups , sizeof( double* ) );

  for( int j = 0; j < n_age_groups; j++)
  probability->contact_matrix[j] = ( double* ) calloc( 1, sizeof( double ) );

  while(out_get_line>=2){
    out_get_line=get_line(&line,fp);
    if(out_get_line<3){
      switch(out_get_line){
        case 2:
        fprintf(stderr, "read_contact_matrix: line %d of file %s does not end in newline\n",n_rows,file);
        break;
        case 1:
        fprintf(stderr, "read_contact_matrix: file %s contains an empty line\n",file);
        return 1;
        break;
        case 0:
        fclose(fp);
        return 0;
        break;
        case -1:
        fprintf(stderr, "read_contact_matrix: get_line error on file %s\n",
        file);
        return 1;
        default:
        fprintf(stderr, "read_contact_matrix: unrecognized exit status of get_line on file %s\n",file);
        return 1;
        break;
      }
    }

    n_rows++;

    // seeding = ( Seeding* ) realloc( seeding, ( n_rows + 1 )*sizeof( Seeding ));
    for( int j = 0; j < n_age_groups; j++ )
    {
      probability->contact_matrix[j]  = ( double* ) realloc( probability->contact_matrix[j]  , ( n_rows + 1 )*sizeof( double ));

      sscanf(line,"%lf", &( probability->contact_matrix[j][n_rows-1] ));
      line = (char *)strchr(line, sep);
      line++;

    }

  }
  return 2;
}

int read_matrix_v2( char file[], int ncol, char sep, Matrix *matrix ){
  /*

  Read a matrix file with columns separated by sep

  x_1 y_1 n_1
  x_2 y_2 n_2
  ...
  x_r  y_r n_r

  Return value: 0 in case of no errors

  */

  char *line;
  int out_get_line = 3;
  FILE *fp;
  int n_rows;

  matrix->n_col = ncol;

  if(!(fp = fopen(file, "r"))){
    fprintf(stderr,"ERROR: file %s not found\n",file);
    return 1;
  }

  n_rows=0;

  matrix->matrix = ( double** ) calloc( ncol , sizeof( double* ) );

  for( int j = 0; j < ncol; j++)
  matrix->matrix[j] = ( double* ) calloc( 1, sizeof( double ) );

  while( out_get_line >= 2 )
  {
    out_get_line = get_line( &line, fp );

    if( out_get_line < 3 )
    {
      switch( out_get_line )
      {
        case 2:

        fprintf( stderr, "read_matrix: line %d of file %s does not end in newline\n",n_rows,file );
        break;

        case 1:

        fprintf( stderr, "read_matrix: file %s contains an empty line\n",file );
        return 1;
        break;

        case 0:

        fclose(fp);
        matrix->n_row = n_rows;
        return 0;
        break;

        case -1:

        fprintf( stderr, "read_matrix: get_line error on file %s\n", file );
        return 1;

        default:

        fprintf( stderr, "read_matrix: unrecognized exit status of get_line on file %s\n", file );
        return 1;
        break;
      }
    }

    n_rows++;

    for( int j = 0; j < ncol; j++ )
    {
      matrix->matrix[ j ] = ( double* ) realloc ( matrix->matrix[ j ] , ( n_rows + 1 )*sizeof( double ));

      sscanf( line, "%lf", &( matrix->matrix[j][n_rows-1] ));
      line = (char *) strchr(line, sep);
      line++;
    }
  }
  return 2;
}

int read_matrix( char file[], int ncol, char sep, Matrix *matrix ){
  /*
  Read a matrix file with columns separated by sep
  x_1 y_1 n_1
  x_2 y_2 n_2
  ...
  x_r  y_r n_r
  Return value: 0 in case of no errors
  */

  char *line;
  int out_get_line = 3;
  FILE *fp;
  int n_rows;

  matrix->n_col = ncol;

  if(!(fp = fopen(file, "r"))){
    fprintf(stderr,"ERROR: file %s not found\n",file);
    return 1;
  }

  n_rows=0;

  matrix->matrix = ( double** ) calloc( ncol , sizeof( double* ) );

  for( int j = 0; j < ncol; j++)
  matrix->matrix[j] = ( double* ) calloc( 1, sizeof( double ) );

  while( out_get_line >= 2 )
  {
    out_get_line = get_line( &line, fp );

    if( out_get_line < 3 )
    {
      switch( out_get_line )
      {
        case 2:

        fprintf( stderr, "read_matrix: line %d of file %s does not end in newline\n",n_rows,file );
        break;

        case 1:

        fprintf( stderr, "read_matrix: file %s contains an empty line\n",file );
        return 1;
        break;

        case 0:

        fclose(fp);
        matrix->n_row = n_rows;
        return 0;
        break;

        case -1:

        fprintf( stderr, "read_matrix: get_line error on file %s\n", file );
        return 1;

        default:

        fprintf( stderr, "read_matrix: unrecognized exit status of get_line on file %s\n", file );
        return 1;
        break;
      }
    }

    n_rows++;

    for( int j = 0; j < ncol; j++ )
    {
      matrix->matrix[ n_rows - 1 ] = ( double* ) realloc ( matrix->matrix[ n_rows - 1 ] , ( j + 1 )*sizeof( double ));

      sscanf( line, "%lf", &( matrix->matrix[n_rows-1][j] ));
      //fprintf(stderr, "%lf\n", matrix->matrix[n_rows-1][j] );
      line = (char *) strchr(line, sep);
      line++;
    }
  }
  return 2;
}
