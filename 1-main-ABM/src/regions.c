#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <sqlite3.h>
#include <math.h>
#include "header.h"

extern int VERBOSE;
extern char *EXP_DIR;
extern Param param;

void regions( Grid *grid ){

	int i, j;
	register int r, c;
	int IDmun, IDcounty;

	/* Determine ID of the country */
	grid->n_r = 1;
	grid->region = (Region *) calloc ( grid->n_r + 1, sizeof( Region ) );
	grid->region[1].r = grid->cell[1].r; // take value from the first cell (the value is the same for each cell)

	/* Determine initial and final ID of the cells in the grid */
	grid->region[grid->n_r].idx_c_ini = 1;
	grid->region[grid->n_r].idx_c_fin = grid->n_c;

	/* Compute population size */
	for( i = 1; i <= grid->n_r; i++ )
	{
		grid->region[i].n_i = 0;

		for( j = grid->region[i].idx_c_ini; j <= grid->region[i].idx_c_fin; j++ )
		{
			grid->region[i].n_i += grid->cell[j].n_i;
		}
	}

	grid->n_mun    = N_MUN;
	grid->n_county = N_COUNTIES;

	grid->municipality = ( Municipality* ) calloc( N_MUN, sizeof(Municipality) );
	grid->county = ( County* ) calloc( N_COUNTIES, sizeof(County) );

	for( i = 0; i < N_MUN; i++ )
	{
		grid->municipality[i].n_cells = 0;
		grid->municipality[i].cells = ( unsigned int* ) calloc( 1, sizeof(unsigned int) );
	}

	for( i = 0; i < N_COUNTIES; i++ )
	{
		grid->county[i].n_cells = 0;
		grid->county[i].cells = ( unsigned int* ) calloc( 1, sizeof(unsigned int) );
	}

	for( r = 1; r <= grid->n_r; r++ )
	{
		for( c = grid->region[r].idx_c_ini; c <= grid->region[r].idx_c_fin; c++ )
		{
			IDmun = grid->cell[c].mun_ID - 1;
			grid->municipality[IDmun].cells[ grid->municipality[IDmun].n_cells ] = c;
			grid->municipality[IDmun].n_cells++;
			grid->municipality[IDmun].cells = (unsigned int*) realloc( grid->municipality[IDmun].cells, (grid->municipality[IDmun].n_cells+1)*sizeof(unsigned int));
		}
	}

	for( r = 1; r <= grid->n_r; r++ )
	{
		for( c = grid->region[r].idx_c_ini; c <= grid->region[r].idx_c_fin; c++ )
		{
			IDcounty = grid->cell[c].county_ID - 1;
			grid->county[IDcounty].cells[ grid->county[IDcounty].n_cells ] = c;
			grid->county[IDcounty].n_cells++;
			grid->county[IDcounty].cells = (unsigned int*) realloc( grid->county[IDcounty].cells, (grid->county[IDcounty].n_cells+1)*sizeof(unsigned int));
		}
	}

	return;

}
