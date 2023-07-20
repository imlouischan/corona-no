#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <sqlite3.h>
#include <math.h>
#include "header.h"

extern Param param;

void kernel( Grid *grid ){

	register int r,c,d;
	int idx_dc;
	int maxdist=0;

	while(1)
	{
		if( round( MAXSHORTINT * pow( maxdist + param.G0, -param.BETA ) * exp( -maxdist / param.KAPPA )) == 0 )
		{
			break;
		}
		maxdist++;
	}

	for( r = 1; r <= grid->n_r; r++ )
	{
		for( c = grid->region[r].idx_c_ini; c <= grid->region[r].idx_c_fin; c++ )
		{
			grid->cell[c].nk = .0;

			/* Loop over cells */

			for( d = grid->region[r].idx_c_ini; d <= grid->region[r].idx_c_fin; d++ )
			{
				idx_dc = d - grid->region[r].idx_c_ini + 1;

				grid->cell[c].dist[idx_dc] = round( MAXSHORTINT*pow(grid->cell[c].dist[idx_dc] + param.G0,-param.BETA )*exp(-grid->cell[c].dist[idx_dc]/param.KAPPA));

				if( grid->cell[c].dist[idx_dc]	>	0	)
				grid->cell[c].nk += grid->cell[d].n_i * grid->cell[c].dist[idx_dc];
			}
		}
	}
	return;
} // void kernel

int read_mobility_data(char file[], char sep, Mobility *mobility, Grid *grid){

	// Read the file (mobility_data.txt) with columns separated by sep
	// Return value: 0 in case of no errors
	// 1 column: maximum distance travelled (Km)
	// 2 column: proportion ( March 11 )
	// 3 column: proportion ( March 18 )
	// 4 column: proportion ( March 25 )
	// 5 column: proportion ( March 28 )
	// 6 column: proportion ( March 29 )

	// Read the file (mobility_data-municipality-2021-01-25.txt) The date is 25th Jan (Monday).
	// 1st column: "radius of gyration, time weighted" (instead of max distance travelled from home).
	// other columns: 351 municipalities (5 missing) using grid->n_mun

	char *line;
	int out_get_line = 3;
	FILE *fp;
	int n_rows;

	if( !( fp = fopen( file, "r" ) ) ){
		fprintf(stderr,"ERROR: file %s not found\n",file);
		return 1;
	}

	n_rows=0;

	mobility->distance   = ( int*     ) calloc( 1          , sizeof (int    ) ); // 1st column
	mobility->proportion = ( double** ) calloc( grid->n_mun, sizeof (double*) ); // other columns
	for( int i = 0; i < grid->n_mun; i++ ){ // 351 municipalities
		mobility->proportion[i] = ( double* ) calloc( 1, sizeof( double ));
	}

	while( out_get_line >= 2 ){
		out_get_line = get_line( &line, fp );
		if( out_get_line < 3 ){
			switch( out_get_line ){
				case 2:
				fprintf(stderr, "read_mobility_data: line %d of file %s does not end in newline\n",n_rows,file);
				break;
				case 1:
				fprintf(stderr, "read_mobility_data: file %s contains an empty line\n",file);
				return 1;
				break;
				case 0:
				mobility->max_distance = mobility->distance[ n_rows - 1 ];
				//fprintf(stderr, "max_distance: %d\n", mobility->max_distance );
				fclose(fp);
				return 0;
				break;
				case -1:
				fprintf(stderr, "read_mobility_data: get_line error on file %s\n",
				file);
				return 1;
				default:
				fprintf(stderr, "read_mobility_data: unrecognized exit status of get_line on file %s\n",file);
				return 1;
				break;
			}
		}

		sscanf(line, "%d", &(mobility->distance[n_rows]));
		line = (char *)strchr(line, sep);
		line++;

		for( int i = 0; i < grid->n_mun; i++ ){ // // 351 municipalities
			sscanf(line, "%lf", &(mobility->proportion[i][n_rows]));
			line = (char *)strchr(line, sep);
			line++;
		}

		n_rows++;

		mobility->distance = ( int* ) realloc( mobility->distance, ( n_rows + 1 ) * sizeof( int ));

		for( int i = 0; i < grid->n_mun; i++ ){ // 351 municipalities
			mobility->proportion[i] = ( double* ) realloc( mobility->proportion[i], ( n_rows + 1 ) * sizeof( double ) );
		}
	}
	return 2;
} // int read_mobility_data
