#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

extern Param param;

int read_param(char file[],char sep){

	char *line;
	int out_get_line=2;
	FILE *fp;
	char string_who[100];
	char string_value[100];

	if( !(fp = fopen(file, "r")) )
	{
		fprintf(stderr,"read_param: error opening file %s for reading\n",file);
		return 1;
	}

	while(out_get_line>=2)
	{
		out_get_line=get_line(&line,fp);
		if(out_get_line<3)
		{
			switch(out_get_line){
				case 2:
				fprintf(stderr,"read_param: line of file %s does not end in newline\n",file);
				break;
				case 1:
				fprintf(stderr,"read_param: file %s contains an empty line\n",file);
				return 1;
				break;
				case 0:
				fclose(fp);
				return 0;
				break;
				case -1:
				fprintf(stderr,"read_param: get_line error on file %s\n", file);
				return 1;
				default:
				fprintf(stderr,"read_param: unrecognized exit status of get_line on file %s\n",file);
				return 1;
				break;
			}
		}

		sscanf(line,"%s", string_who);
		line = (char *)strchr(line, sep);
		line++;
		sscanf(line,"%s", string_value);

		if(strcmp(string_who,"TAU_T")==0)
		param.TAU_T=atof(string_value);
		if(strcmp(string_who,"TAU_F")==0)
		param.TAU_F=atof(string_value);
		if(strcmp(string_who,"RHO")==0)
		param.RHO=atof(string_value);
		if(strcmp(string_who,"G0")==0)
		param.G0=atof(string_value);
		if(strcmp(string_who,"KAPPA")==0)
		param.KAPPA=atof(string_value);
		if(strcmp(string_who,"BETA")==0)
		param.BETA=atof(string_value);
		if(strcmp(string_who,"MAXDIST_P01")==0)
		param.MAXDIST_P01=atoi(string_value);
		if(strcmp(string_who,"MAXDIST_P03")==0)
		param.MAXDIST_P03=atoi(string_value);
		if(strcmp(string_who,"MAXDIST_P1")==0)
		param.MAXDIST_P1=atoi(string_value);
		if(strcmp(string_who,"PROB_SAMPLING_WP")==0)
		param.PROB_SAMPLING_WP=atof(string_value);
		if(strcmp(string_who,"PROB_WP")==0)
		param.PROB_WP=atof(string_value);
		if(strcmp(string_who,"BC_DIST")==0)
		param.BC_DIST=atoi(string_value);
		if(strcmp(string_who,"MAX_COMM_DIST")==0)
		param.MAX_COMM_DIST=atoi(string_value);
		if(strcmp(string_who,"DELTAT")==0)
		param.DELTAT=atof(string_value);
		if(strcmp(string_who,"MAXT")==0)
		param.MAXT=atoi(string_value);
		if(strcmp(string_who,"DAY_STR2_INI")==0)
		param.DAY_STR2_INI=atoi(string_value);
		if(strcmp(string_who,"DAY_STR2_FIN")==0)
		param.DAY_STR2_FIN=atoi(string_value);
		if(strcmp(string_who,"RADIUS_STR3")==0)
		param.RADIUS_STR3=atof(string_value);
		if(strcmp(string_who,"XDIM_CELLS")==0)
		param.XDIM_CELLS=atof(string_value);
		if(strcmp(string_who,"YDIM_CELLS")==0)
		param.YDIM_CELLS=atof(string_value);
		if(strcmp(string_who,"RELATIVE_INF_AS")==0)
		param.RELATIVE_INF_AS = atof(string_value);
		if(strcmp(string_who,"RELATIVE_INF_PRES")==0)
		param.RELATIVE_INF_PRES = atof(string_value);
		if(strcmp(string_who,"PROP_HOME_OFFICE")==0)
		param.PROP_HOME_OFFICE = atof(string_value);
		if(strcmp(string_who,"REDUCE_SOC_DIST")==0)
		param.REDUCE_SOC_DIST= atof(string_value);

	}

	return 2;
} // int read_param
