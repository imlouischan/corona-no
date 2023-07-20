#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "header.h"

extern Param param;

int read_param_2(char file[],char sep){

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
			switch(out_get_line)
			{
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

		sscanf( line, "%s", string_who );
		line = (char *)strchr(line, sep);
		line++;
		sscanf(line,"%s", string_value);

		if(strcmp(string_who,"SUS_0_9")==0)
		param.SUS_0_9 =atof(string_value);
		if(strcmp(string_who,"SUS_10_19")==0)
		param.SUS_10_19 =atof(string_value);
		if(strcmp(string_who,"SUS_20_29")==0)
		param.SUS_20_29 =atof(string_value);
		if(strcmp(string_who,"SUS_30_39")==0)
		param.SUS_30_39 =atof(string_value);
		if(strcmp(string_who,"SUS_40_49")==0)
		param.SUS_40_49 =atof(string_value);
		if(strcmp(string_who,"SUS_50_59")==0)
		param.SUS_50_59 =atof(string_value);
		if(strcmp(string_who,"SUS_60_69")==0)
		param.SUS_60_69 =atof(string_value);
		if(strcmp(string_who,"SUS_70_79")==0)
		param.SUS_70_79=atof(string_value);
		if(strcmp(string_who,"SUS_80p")==0)
		param.SUS_80p=atof(string_value);
		if(strcmp(string_who,"PROB_ASYMP")==0)
		param.PROB_ASYMP=atof(string_value);

		if(strcmp(string_who,"RELATIVE_INF_PRES")==0)
		param.RELATIVE_INF_PRES=atof(string_value);
		if(strcmp(string_who,"RELATIVE_INF_AS")==0)
		param.RELATIVE_INF_AS=atof(string_value);

		if(strcmp(string_who,"RELATIVE_COMM_CONTACTS_0_9")==0)
		param.RELATIVE_COMM_CONTACTS_0_9=atof(string_value);
		if(strcmp(string_who,"RELATIVE_COMM_CONTACTS_10_19")==0)
		param.RELATIVE_COMM_CONTACTS_10_19=atof(string_value);
		if(strcmp(string_who,"RELATIVE_COMM_CONTACTS_20_29")==0)
		param.RELATIVE_COMM_CONTACTS_20_29=atof(string_value);
		if(strcmp(string_who,"RELATIVE_COMM_CONTACTS_30_39")==0)
		param.RELATIVE_COMM_CONTACTS_30_39=atof(string_value);
		if(strcmp(string_who,"RELATIVE_COMM_CONTACTS_40_49")==0)
		param.RELATIVE_COMM_CONTACTS_40_49=atof(string_value);
		if(strcmp(string_who,"RELATIVE_COMM_CONTACTS_50_59")==0)
		param.RELATIVE_COMM_CONTACTS_50_59=atof(string_value);
		if(strcmp(string_who,"RELATIVE_COMM_CONTACTS_60_69")==0)
		param.RELATIVE_COMM_CONTACTS_60_69=atof(string_value);
		if(strcmp(string_who,"RELATIVE_COMM_CONTACTS_70_79")==0)
		param.RELATIVE_COMM_CONTACTS_70_79=atof(string_value);
		if(strcmp(string_who,"RELATIVE_COMM_CONTACTS_80p")==0)
		param.RELATIVE_COMM_CONTACTS_80p=atof(string_value);

		if(strcmp(string_who,"FRACTION_RG_0_9")==0)
		param.FRACTION_RG[0]=atof(string_value);
		if(strcmp(string_who,"FRACTION_RG_10_19")==0)
		param.FRACTION_RG[1]=atof(string_value);
		if(strcmp(string_who,"FRACTION_RG_20_29")==0)
		param.FRACTION_RG[2]=atof(string_value);
		if(strcmp(string_who,"FRACTION_RG_30_39")==0)
		param.FRACTION_RG[3]=atof(string_value);
		if(strcmp(string_who,"FRACTION_RG_40_49")==0)
		param.FRACTION_RG[4]=atof(string_value);
		if(strcmp(string_who,"FRACTION_RG_50_59")==0)
		param.FRACTION_RG[5]=atof(string_value);
		if(strcmp(string_who,"FRACTION_RG_60_69")==0)
		param.FRACTION_RG[6]=atof(string_value);
		if(strcmp(string_who,"FRACTION_RG_70_79")==0)
		param.FRACTION_RG[7]=atof(string_value);
		if(strcmp(string_who,"FRACTION_RG_80p")==0)
		param.FRACTION_RG[8]=atof(string_value);

		if(strcmp(string_who,"E1_GAMMA_ALPHA")==0)
		param.E1_GAMMA_ALPHA = atof(string_value);
		if(strcmp(string_who,"E1_GAMMA_BETA")==0)
		param.E1_GAMMA_BETA = atof(string_value);
		if(strcmp(string_who,"E1a_GAMMA_ALPHA")==0)
		param.E1a_GAMMA_ALPHA = atof(string_value);
		if(strcmp(string_who,"E1a_GAMMA_BETA")==0)
		param.E1a_GAMMA_BETA = atof(string_value);
		if(strcmp(string_who,"E2_GAMMA_ALPHA")==0)
		param.E2_GAMMA_ALPHA = atof(string_value);
		if(strcmp(string_who,"E2_GAMMA_BETA")==0)
		param.E2_GAMMA_BETA = atof(string_value);
		if(strcmp(string_who,"I_GAMMA_ALPHA")==0)
		param.I_GAMMA_ALPHA = atof(string_value);
		if(strcmp(string_who,"I_GAMMA_BETA")==0)
		param.I_GAMMA_BETA = atof(string_value);
		if(strcmp(string_who,"Ia_GAMMA_ALPHA")==0)
		param.Ia_GAMMA_ALPHA = atof(string_value);
		if(strcmp(string_who,"Ia_GAMMA_BETA")==0)
		param.Ia_GAMMA_BETA = atof(string_value);

		if(strcmp(string_who,"PROB_HOSP_0_9")==0)
		param.PROB_HOSP_0_9 = atof(string_value);
		if(strcmp(string_who,"PROB_HOSP_10_19")==0)
		param.PROB_HOSP_10_19 = atof(string_value);
		if(strcmp(string_who,"PROB_HOSP_20_29")==0)
		param.PROB_HOSP_20_29 = atof(string_value);
		if(strcmp(string_who,"PROB_HOSP_30_39")==0)
		param.PROB_HOSP_30_39 = atof(string_value);
		if(strcmp(string_who,"PROB_HOSP_40_49")==0)
		param.PROB_HOSP_40_49 = atof(string_value);
		if(strcmp(string_who,"PROB_HOSP_50_59")==0)
		param.PROB_HOSP_50_59 = atof(string_value);
		if(strcmp(string_who,"PROB_HOSP_60_69")==0)
		param.PROB_HOSP_60_69 = atof(string_value);
		if(strcmp(string_who,"PROB_HOSP_70_79")==0)
		param.PROB_HOSP_70_79 = atof(string_value);
		if(strcmp(string_who,"PROB_HOSP_80p")==0)
		param.PROB_HOSP_80p = atof(string_value);
		if(strcmp(string_who,"PROB_HOSP_RG_0_9")==0)
		param.PROB_HOSP_RG_0_9 = atof(string_value);

		if(strcmp(string_who,"PROB_HOSP_RG_10_19")==0)
		param.PROB_HOSP_RG_10_19 = atof(string_value);
		if(strcmp(string_who,"PROB_HOSP_RG_20_29")==0)
		param.PROB_HOSP_RG_20_29 = atof(string_value);
		if(strcmp(string_who,"PROB_HOSP_RG_30_39")==0)
		param.PROB_HOSP_RG_30_39 = atof(string_value);
		if(strcmp(string_who,"PROB_HOSP_RG_40_49")==0)
		param.PROB_HOSP_RG_40_49 = atof(string_value);
		if(strcmp(string_who,"PROB_HOSP_RG_50_59")==0)
		param.PROB_HOSP_RG_50_59 = atof(string_value);
		if(strcmp(string_who,"PROB_HOSP_RG_60_69")==0)
		param.PROB_HOSP_RG_60_69 = atof(string_value);
		if(strcmp(string_who,"PROB_HOSP_RG_70_79")==0)
		param.PROB_HOSP_RG_70_79 = atof(string_value);
		if(strcmp(string_who,"PROB_HOSP_RG_80p")==0)
		param.PROB_HOSP_RG_80p = atof(string_value);

		if(strcmp(string_who,"PROB_DEATH_0_9")==0)
		param.PROB_DEATH_0_9 = atof(string_value);
		if(strcmp(string_who,"PROB_DEATH_10_19")==0)
		param.PROB_DEATH_10_19 = atof(string_value);
		if(strcmp(string_who,"PROB_DEATH_20_29")==0)
		param.PROB_DEATH_20_29 = atof(string_value);
		if(strcmp(string_who,"PROB_DEATH_30_39")==0)
		param.PROB_DEATH_30_39 = atof(string_value);
		if(strcmp(string_who,"PROB_DEATH_40_49")==0)
		param.PROB_DEATH_40_49 = atof(string_value);
		if(strcmp(string_who,"PROB_DEATH_50_59")==0)
		param.PROB_DEATH_50_59 = atof(string_value);
		if(strcmp(string_who,"PROB_DEATH_60_69")==0)
		param.PROB_DEATH_60_69 = atof(string_value);
		if(strcmp(string_who,"PROB_DEATH_70_79")==0)
		param.PROB_DEATH_70_79 = atof(string_value);
		if(strcmp(string_who,"PROB_DEATH_80p")==0)
		param.PROB_DEATH_80p = atof(string_value);

		if(strcmp(string_who,"PROB_DEATH_RG_0_9")==0)
		param.PROB_DEATH_RG_0_9 = atof(string_value);
		if(strcmp(string_who,"PROB_DEATH_RG_10_19")==0)
		param.PROB_DEATH_RG_10_19 = atof(string_value);
		if(strcmp(string_who,"PROB_DEATH_RG_20_29")==0)
		param.PROB_DEATH_RG_20_29 = atof(string_value);
		if(strcmp(string_who,"PROB_DEATH_RG_30_39")==0)
		param.PROB_DEATH_RG_30_39 = atof(string_value);
		if(strcmp(string_who,"PROB_DEATH_RG_40_49")==0)
		param.PROB_DEATH_RG_40_49 = atof(string_value);
		if(strcmp(string_who,"PROB_DEATH_RG_50_59")==0)
		param.PROB_DEATH_RG_50_59 = atof(string_value);
		if(strcmp(string_who,"PROB_DEATH_RG_60_69")==0)
		param.PROB_DEATH_RG_60_69 = atof(string_value);
		if(strcmp(string_who,"PROB_DEATH_RG_70_79")==0)
		param.PROB_DEATH_RG_70_79 = atof(string_value);
		if(strcmp(string_who,"PROB_DEATH_RG_80p")==0)
		param.PROB_DEATH_RG_80p = atof(string_value);

		if(strcmp(string_who,"TIME_FROM_I_TO_DEATH")==0)
		param.TIME_FROM_I_TO_DEATH = atof(string_value);
		if(strcmp(string_who,"TIME_FROM_H_TO_DEATH")==0)
		param.TIME_FROM_H_TO_DEATH = atof(string_value);
		if(strcmp(string_who,"TIME_FROM_ICU_TO_DEATH")==0)
		param.TIME_FROM_ICU_TO_DEATH = atof(string_value);

		if(strcmp(string_who,"ONSET_TO_HOSP_PROB_0_9")==0)
		param.ONSET_TO_HOSP_PROB_0_9 = atof(string_value);
		if(strcmp(string_who,"ONSET_TO_HOSP_SIZE_0_9")==0)
		param.ONSET_TO_HOSP_SIZE_0_9 = atof(string_value);
		if(strcmp(string_who,"ONSET_TO_HOSP_PROB_10_19")==0)
		param.ONSET_TO_HOSP_PROB_10_19 = atof(string_value);
		if(strcmp(string_who,"ONSET_TO_HOSP_SIZE_10_19")==0)
		param.ONSET_TO_HOSP_SIZE_10_19 = atof(string_value);
		if(strcmp(string_who,"ONSET_TO_HOSP_PROB_20_29")==0)
		param.ONSET_TO_HOSP_PROB_20_29 = atof(string_value);
		if(strcmp(string_who,"ONSET_TO_HOSP_SIZE_20_29")==0)
		param.ONSET_TO_HOSP_SIZE_20_29 = atof(string_value);
		if(strcmp(string_who,"ONSET_TO_HOSP_PROB_30_39")==0)
		param.ONSET_TO_HOSP_PROB_30_39 = atof(string_value);
		if(strcmp(string_who,"ONSET_TO_HOSP_SIZE_30_39")==0)
		param.ONSET_TO_HOSP_SIZE_30_39 = atof(string_value);
		if(strcmp(string_who,"ONSET_TO_HOSP_PROB_40_49")==0)
		param.ONSET_TO_HOSP_PROB_40_49 = atof(string_value);
		if(strcmp(string_who,"ONSET_TO_HOSP_SIZE_40_49")==0)
		param.ONSET_TO_HOSP_SIZE_40_49 = atof(string_value);
		if(strcmp(string_who,"ONSET_TO_HOSP_PROB_50_59")==0)
		param.ONSET_TO_HOSP_PROB_50_59 = atof(string_value);
		if(strcmp(string_who,"ONSET_TO_HOSP_SIZE_50_59")==0)
		param.ONSET_TO_HOSP_SIZE_50_59 = atof(string_value);
		if(strcmp(string_who,"ONSET_TO_HOSP_PROB_60_69")==0)
		param.ONSET_TO_HOSP_PROB_60_69 = atof(string_value);
		if(strcmp(string_who,"ONSET_TO_HOSP_SIZE_60_69")==0)
		param.ONSET_TO_HOSP_SIZE_60_69 = atof(string_value);
		if(strcmp(string_who,"ONSET_TO_HOSP_PROB_70_79")==0)
		param.ONSET_TO_HOSP_PROB_70_79 = atof(string_value);
		if(strcmp(string_who,"ONSET_TO_HOSP_SIZE_70_79")==0)
		param.ONSET_TO_HOSP_SIZE_70_79 = atof(string_value);
		if(strcmp(string_who,"ONSET_TO_HOSP_PROB_80_89")==0)
		param.ONSET_TO_HOSP_PROB_80_89 = atof(string_value);
		if(strcmp(string_who,"ONSET_TO_HOSP_SIZE_80_89")==0)
		param.ONSET_TO_HOSP_SIZE_80_89 = atof(string_value);
		if(strcmp(string_who,"ONSET_TO_HOSP_PROB_90p")==0)
		param.ONSET_TO_HOSP_PROB_90p = atof(string_value);
		if(strcmp(string_who,"ONSET_TO_HOSP_SIZE_90p")==0)
		param.ONSET_TO_HOSP_SIZE_90p = atof(string_value);

		if(strcmp(string_who,"LOS_HOSP_PROB_0_9")==0)
		param.LOS_HOSP_PROB_0_9 = atof(string_value);
		if(strcmp(string_who,"LOS_HOSP_SIZE_0_9")==0)
		param.LOS_HOSP_SIZE_0_9 = atof(string_value);
		if(strcmp(string_who,"LOS_HOSP_PROB_10_19")==0)
		param.LOS_HOSP_PROB_10_19 = atof(string_value);
		if(strcmp(string_who,"LOS_HOSP_SIZE_10_19")==0)
		param.LOS_HOSP_SIZE_10_19 = atof(string_value);
		if(strcmp(string_who,"LOS_HOSP_PROB_20_29")==0)
		param.LOS_HOSP_PROB_20_29 = atof(string_value);
		if(strcmp(string_who,"LOS_HOSP_SIZE_20_29")==0)
		param.LOS_HOSP_SIZE_20_29 = atof(string_value);
		if(strcmp(string_who,"LOS_HOSP_PROB_30_39")==0)
		param.LOS_HOSP_PROB_30_39 = atof(string_value);
		if(strcmp(string_who,"LOS_HOSP_SIZE_30_39")==0)
		param.LOS_HOSP_SIZE_30_39 = atof(string_value);
		if(strcmp(string_who,"LOS_HOSP_PROB_40_49")==0)
		param.LOS_HOSP_PROB_40_49 = atof(string_value);
		if(strcmp(string_who,"LOS_HOSP_SIZE_40_49")==0)
		param.LOS_HOSP_SIZE_40_49 = atof(string_value);
		if(strcmp(string_who,"LOS_HOSP_PROB_50_59")==0)
		param.LOS_HOSP_PROB_50_59 = atof(string_value);
		if(strcmp(string_who,"LOS_HOSP_SIZE_50_59")==0)
		param.LOS_HOSP_SIZE_50_59 = atof(string_value);
		if(strcmp(string_who,"LOS_HOSP_PROB_60_69")==0)
		param.LOS_HOSP_PROB_60_69 = atof(string_value);
		if(strcmp(string_who,"LOS_HOSP_SIZE_60_69")==0)
		param.LOS_HOSP_SIZE_60_69 = atof(string_value);
		if(strcmp(string_who,"LOS_HOSP_PROB_70_79")==0)
		param.LOS_HOSP_PROB_70_79 = atof(string_value);
		if(strcmp(string_who,"LOS_HOSP_SIZE_70_79")==0)
		param.LOS_HOSP_SIZE_70_79 = atof(string_value);
		if(strcmp(string_who,"LOS_HOSP_PROB_80_89")==0)
		param.LOS_HOSP_PROB_80_89 = atof(string_value);
		if(strcmp(string_who,"LOS_HOSP_SIZE_80_89")==0)
		param.LOS_HOSP_SIZE_80_89 = atof(string_value);
		if(strcmp(string_who,"LOS_HOSP_PROB_90p")==0)
		param.LOS_HOSP_PROB_90p = atof(string_value);
		if(strcmp(string_who,"LOS_HOSP_SIZE_90p")==0)
		param.LOS_HOSP_SIZE_90p = atof(string_value);

		if(strcmp(string_who,"PROB_ICU_0_9")==0)
		param.PROB_ICU_0_9 = atof(string_value);
		if(strcmp(string_who,"PROB_ICU_10_19")==0)
		param.PROB_ICU_10_19 = atof(string_value);
		if(strcmp(string_who,"PROB_ICU_20_29")==0)
		param.PROB_ICU_20_29 = atof(string_value);
		if(strcmp(string_who,"PROB_ICU_30_39")==0)
		param.PROB_ICU_30_39 = atof(string_value);
		if(strcmp(string_who,"PROB_ICU_40_49")==0)
		param.PROB_ICU_40_49 = atof(string_value);
		if(strcmp(string_who,"PROB_ICU_50_59")==0)
		param.PROB_ICU_50_59 = atof(string_value);
		if(strcmp(string_who,"PROB_ICU_60_69")==0)
		param.PROB_ICU_60_69 = atof(string_value);
		if(strcmp(string_who,"PROB_ICU_70_79")==0)
		param.PROB_ICU_70_79 = atof(string_value);
		if(strcmp(string_who,"PROB_ICU_80p")==0)
		param.PROB_ICU_80p = atof(string_value);

		if(strcmp(string_who,"TIME_TO_ICU_PROB_0_9")==0)
		param.TIME_TO_ICU_PROB_0_9 = atof(string_value);
		if(strcmp(string_who,"TIME_TO_ICU_SIZE_0_9")==0)
		param.TIME_TO_ICU_SIZE_0_9 = atof(string_value);
		if(strcmp(string_who,"TIME_TO_ICU_PROB_10_19")==0)
		param.TIME_TO_ICU_PROB_10_19 = atof(string_value);
		if(strcmp(string_who,"TIME_TO_ICU_SIZE_10_19")==0)
		param.TIME_TO_ICU_SIZE_10_19 = atof(string_value);
		if(strcmp(string_who,"TIME_TO_ICU_PROB_20_29")==0)
		param.TIME_TO_ICU_PROB_20_29 = atof(string_value);
		if(strcmp(string_who,"TIME_TO_ICU_SIZE_20_29")==0)
		param.TIME_TO_ICU_SIZE_20_29 = atof(string_value);
		if(strcmp(string_who,"TIME_TO_ICU_PROB_30_39")==0)
		param.TIME_TO_ICU_PROB_30_39 = atof(string_value);
		if(strcmp(string_who,"TIME_TO_ICU_SIZE_30_39")==0)
		param.TIME_TO_ICU_SIZE_30_39 = atof(string_value);
		if(strcmp(string_who,"TIME_TO_ICU_PROB_40_49")==0)
		param.TIME_TO_ICU_PROB_40_49 = atof(string_value);
		if(strcmp(string_who,"TIME_TO_ICU_SIZE_40_49")==0)
		param.TIME_TO_ICU_SIZE_40_49 = atof(string_value);
		if(strcmp(string_who,"TIME_TO_ICU_PROB_50_59")==0)
		param.TIME_TO_ICU_PROB_50_59 = atof(string_value);
		if(strcmp(string_who,"TIME_TO_ICU_SIZE_50_59")==0)
		param.TIME_TO_ICU_SIZE_50_59 = atof(string_value);
		if(strcmp(string_who,"TIME_TO_ICU_PROB_60_69")==0)
		param.TIME_TO_ICU_PROB_60_69 = atof(string_value);
		if(strcmp(string_who,"TIME_TO_ICU_SIZE_60_69")==0)
		param.TIME_TO_ICU_SIZE_60_69 = atof(string_value);
		if(strcmp(string_who,"TIME_TO_ICU_PROB_70_79")==0)
		param.TIME_TO_ICU_PROB_70_79 = atof(string_value);
		if(strcmp(string_who,"TIME_TO_ICU_SIZE_70_79")==0)
		param.TIME_TO_ICU_SIZE_70_79 = atof(string_value);
		if(strcmp(string_who,"TIME_TO_ICU_PROB_80p")==0)
		param.TIME_TO_ICU_PROB_80p = atof(string_value);
		if(strcmp(string_who,"TIME_TO_ICU_SIZE_80p")==0)
		param.TIME_TO_ICU_SIZE_80p = atof(string_value);

		if(strcmp(string_who,"LOS_ICU_PROB_0_9")==0)
		param.LOS_ICU_PROB_0_9 = atof(string_value);
		if(strcmp(string_who,"LOS_ICU_SIZE_0_9")==0)
		param.LOS_ICU_SIZE_0_9 = atof(string_value);
		if(strcmp(string_who,"LOS_ICU_PROB_10_19")==0)
		param.LOS_ICU_PROB_10_19 = atof(string_value);
		if(strcmp(string_who,"LOS_ICU_SIZE_10_19")==0)
		param.LOS_ICU_SIZE_10_19 = atof(string_value);
		if(strcmp(string_who,"LOS_ICU_PROB_20_29")==0)
		param.LOS_ICU_PROB_20_29 = atof(string_value);
		if(strcmp(string_who,"LOS_ICU_SIZE_20_29")==0)
		param.LOS_ICU_SIZE_20_29 = atof(string_value);
		if(strcmp(string_who,"LOS_ICU_PROB_30_39")==0)
		param.LOS_ICU_PROB_30_39 = atof(string_value);
		if(strcmp(string_who,"LOS_ICU_SIZE_30_39")==0)
		param.LOS_ICU_SIZE_30_39 = atof(string_value);
		if(strcmp(string_who,"LOS_ICU_PROB_40_49")==0)
		param.LOS_ICU_PROB_40_49 = atof(string_value);
		if(strcmp(string_who,"LOS_ICU_SIZE_40_49")==0)
		param.LOS_ICU_SIZE_40_49 = atof(string_value);
		if(strcmp(string_who,"LOS_ICU_PROB_50_59")==0)
		param.LOS_ICU_PROB_50_59 = atof(string_value);
		if(strcmp(string_who,"LOS_ICU_SIZE_50_59")==0)
		param.LOS_ICU_SIZE_50_59 = atof(string_value);
		if(strcmp(string_who,"LOS_ICU_PROB_60_69")==0)
		param.LOS_ICU_PROB_60_69 = atof(string_value);
		if(strcmp(string_who,"LOS_ICU_SIZE_60_69")==0)
		param.LOS_ICU_SIZE_60_69 = atof(string_value);
		if(strcmp(string_who,"LOS_ICU_PROB_70_79")==0)
		param.LOS_ICU_PROB_70_79 = atof(string_value);
		if(strcmp(string_who,"LOS_ICU_SIZE_70_79")==0)
		param.LOS_ICU_SIZE_70_79 = atof(string_value);
		if(strcmp(string_who,"LOS_ICU_PROB_80p")==0)
		param.LOS_ICU_PROB_80p = atof(string_value);
		if(strcmp(string_who,"LOS_ICU_SIZE_80p")==0)
		param.LOS_ICU_SIZE_80p = atof(string_value);

		if(strcmp(string_who,"LOS_HOSP_ICU_PROB_0_9")==0)
		param.LOS_HOSP_ICU_PROB_0_9 = atof(string_value);
		if(strcmp(string_who,"LOS_HOSP_ICU_SIZE_0_9")==0)
		param.LOS_HOSP_ICU_SIZE_0_9 = atof(string_value);
		if(strcmp(string_who,"LOS_HOSP_ICU_PROB_10_19")==0)
		param.LOS_HOSP_ICU_PROB_10_19 = atof(string_value);
		if(strcmp(string_who,"LOS_HOSP_ICU_SIZE_10_19")==0)
		param.LOS_HOSP_ICU_SIZE_10_19 = atof(string_value);
		if(strcmp(string_who,"LOS_HOSP_ICU_PROB_20_29")==0)
		param.LOS_HOSP_ICU_PROB_20_29 = atof(string_value);
		if(strcmp(string_who,"LOS_HOSP_ICU_SIZE_20_29")==0)
		param.LOS_HOSP_ICU_SIZE_20_29 = atof(string_value);
		if(strcmp(string_who,"LOS_HOSP_ICU_PROB_30_39")==0)
		param.LOS_HOSP_ICU_PROB_30_39 = atof(string_value);
		if(strcmp(string_who,"LOS_HOSP_ICU_SIZE_30_39")==0)
		param.LOS_HOSP_ICU_SIZE_30_39 = atof(string_value);
		if(strcmp(string_who,"LOS_HOSP_ICU_PROB_40_49")==0)
		param.LOS_HOSP_ICU_PROB_40_49 = atof(string_value);
		if(strcmp(string_who,"LOS_HOSP_ICU_SIZE_40_49")==0)
		param.LOS_HOSP_ICU_SIZE_40_49 = atof(string_value);
		if(strcmp(string_who,"LOS_HOSP_ICU_PROB_50_59")==0)
		param.LOS_HOSP_ICU_PROB_50_59 = atof(string_value);
		if(strcmp(string_who,"LOS_HOSP_ICU_SIZE_50_59")==0)
		param.LOS_HOSP_ICU_SIZE_50_59 = atof(string_value);
		if(strcmp(string_who,"LOS_HOSP_ICU_PROB_60_69")==0)
		param.LOS_HOSP_ICU_PROB_60_69 = atof(string_value);
		if(strcmp(string_who,"LOS_HOSP_ICU_SIZE_60_69")==0)
		param.LOS_HOSP_ICU_SIZE_60_69 = atof(string_value);
		if(strcmp(string_who,"LOS_HOSP_ICU_PROB_70_79")==0)
		param.LOS_HOSP_ICU_PROB_70_79 = atof(string_value);
		if(strcmp(string_who,"LOS_HOSP_ICU_SIZE_70_79")==0)
		param.LOS_HOSP_ICU_SIZE_70_79 = atof(string_value);
		if(strcmp(string_who,"LOS_HOSP_ICU_PROB_80p")==0)
		param.LOS_HOSP_ICU_PROB_80p = atof(string_value);
		if(strcmp(string_who,"LOS_HOSP_ICU_SIZE_80p")==0)
		param.LOS_HOSP_ICU_SIZE_80p = atof(string_value);

		if(strcmp(string_who,"N_HWS_0_9")==0)
		param.N_HWS[0] = atof(string_value);
		if(strcmp(string_who,"N_HWS_10_19")==0)
		param.N_HWS[1] = atof(string_value);
		if(strcmp(string_who,"N_HWS_20_29")==0)
		param.N_HWS[2] = atof(string_value);
		if(strcmp(string_who,"N_HWS_30_39")==0)
		param.N_HWS[3] = atof(string_value);
		if(strcmp(string_who,"N_HWS_40_49")==0)
		param.N_HWS[4] = atof(string_value);
		if(strcmp(string_who,"N_HWS_50_59")==0)
		param.N_HWS[5] = atof(string_value);
		if(strcmp(string_who,"N_HWS_60_69")==0)
		param.N_HWS[6] = atof(string_value);
		if(strcmp(string_who,"N_HWS_70_79")==0)
		param.N_HWS[7] = atof(string_value);
		if(strcmp(string_who,"N_HWS_80p")==0)
		param.N_HWS[8] = atof(string_value);

	}

	return 2;
} // int read_param_2

void print_parameters(){

	fprintf( stderr, "SUS_0_9	%lf\n", param.SUS_0_9  );
	fprintf( stderr, "SUS_10_19	%lf\n", param.SUS_10_19   );
	fprintf( stderr, "SUS_20_29	%lf\n", param.SUS_20_29    );
	fprintf( stderr, "SUS_30_39	%lf\n", param.SUS_30_39    );
	fprintf( stderr, "SUS_40_49	%lf\n", param.SUS_40_49   );
	fprintf( stderr, "SUS_50_59	%lf\n", param.SUS_50_59    );
	fprintf( stderr, "SUS_60_69	%lf\n", param.SUS_60_69   );
	fprintf( stderr, "SUS_70_79	%lf\n", param.SUS_70_79   );
	fprintf( stderr, "SUS_80p	%lf\n", param.SUS_80p    );
	fprintf( stderr, "PROB_ASYMP	%lf\n", param.PROB_ASYMP    );
	fprintf( stderr, "\n");
	fprintf( stderr, "SUS_0_9 	%lf\n", param.SUS_0_9  );
	fprintf( stderr, "SUS_10_19	%lf\n", param.SUS_10_19   );
	fprintf( stderr, "SUS_20_29	%lf\n", param.SUS_20_29    );
	fprintf( stderr, "SUS_30_39	%lf\n", param.SUS_30_39    );
	fprintf( stderr, "SUS_40_49	%lf\n", param.SUS_40_49   );
	fprintf( stderr, "SUS_50_59	%lf\n", param.SUS_50_59    );
	fprintf( stderr, "SUS_60_69	%lf\n", param.SUS_60_69   );
	fprintf( stderr, "SUS_70_79	%lf\n", param.SUS_70_79   );
	fprintf( stderr, "SUS_80p	%lf\n", param.SUS_80p    );
	fprintf( stderr, "PROB_ASYMP	%lf\n", param.PROB_ASYMP    );

	fprintf( stderr, "RELATIVE_INF_PRES	%lf\n", param.RELATIVE_INF_PRES ) ;
	fprintf( stderr, "RELATIVE_INF_AS	%lf\n", param.RELATIVE_INF_AS  ) ;

	fprintf( stderr, "RELATIVE_COMM_CONTACTS_0_9	%lf\n", param.RELATIVE_COMM_CONTACTS_0_9 )  ;
	fprintf( stderr, "RELATIVE_COMM_CONTACTS_10_19	%lf\n", param.RELATIVE_COMM_CONTACTS_10_19  ) ;
	fprintf( stderr, "RELATIVE_COMM_CONTACTS_20_29	%lf\n", param.RELATIVE_COMM_CONTACTS_20_29  ) ;
	fprintf( stderr, "RELATIVE_COMM_CONTACTS_30_39	%lf\n", param.RELATIVE_COMM_CONTACTS_30_39  );
	fprintf( stderr, "RELATIVE_COMM_CONTACTS_40_49	%lf\n", param.RELATIVE_COMM_CONTACTS_40_49  ) ;
	fprintf( stderr, "RELATIVE_COMM_CONTACTS_50_59	%lf\n", param.RELATIVE_COMM_CONTACTS_50_59  ) ;
	fprintf( stderr, "RELATIVE_COMM_CONTACTS_60_69	%lf\n", param.RELATIVE_COMM_CONTACTS_60_69   );
	fprintf( stderr, "RELATIVE_COMM_CONTACTS_70_79	%lf\n", param.RELATIVE_COMM_CONTACTS_70_79  ) ;
	fprintf( stderr, "RELATIVE_COMM_CONTACTS_80p	%lf\n", param.RELATIVE_COMM_CONTACTS_80p  ) ;

	fprintf( stderr, "PROB_HOSP_0_9 	%lf\n", param.PROB_HOSP_0_9 ) ;
	fprintf( stderr, "PROB_HOSP_10_19 	%lf\n", param.PROB_HOSP_10_19 ) ;
	fprintf( stderr, "PROB_HOSP_20_29 	%lf\n", param.PROB_HOSP_20_29 ) ;
	fprintf( stderr, "PROB_HOSP_30_39 	%lf\n", param.PROB_HOSP_30_39 ) ;
	fprintf( stderr, "PROB_HOSP_40_49 	%lf\n", param.PROB_HOSP_40_49 ) ;
	fprintf( stderr, "PROB_HOSP_50_59 	%lf\n", param.PROB_HOSP_50_59 ) ;
	fprintf( stderr, "PROB_HOSP_60_69 	%lf\n", param.PROB_HOSP_60_69 ) ;
	fprintf( stderr, "PROB_HOSP_70_79 	%lf\n", param.PROB_HOSP_70_79 ) ;
	fprintf( stderr, "PROB_HOSP_80p 	%lf\n", param.PROB_HOSP_80p ) ;
	fprintf( stderr, "PROB_HOSP_RG_0_9 	%lf\n", param.PROB_HOSP_RG_0_9  );
	fprintf( stderr, "PROB_HOSP_RG_10_19 	%lf\n", param.PROB_HOSP_RG_10_19  );
	fprintf( stderr, "PROB_HOSP_RG_20_29 	%lf\n", param.PROB_HOSP_RG_20_29  );
	fprintf( stderr, "PROB_HOSP_RG_30_39 	%lf\n", param.PROB_HOSP_RG_30_39  );
	fprintf( stderr, "PROB_HOSP_RG_40_49 	%lf\n", param.PROB_HOSP_RG_40_49  );
	fprintf( stderr, "PROB_HOSP_RG_50_59 	%lf\n", param.PROB_HOSP_RG_50_59  );
	fprintf( stderr, "PROB_HOSP_RG_60_69 	%lf\n", param.PROB_HOSP_RG_60_69  );
	fprintf( stderr, "PROB_HOSP_RG_70_79 	%lf\n", param.PROB_HOSP_RG_70_79 ) ;
	fprintf( stderr, "PROB_HOSP_RG_80p 	%lf\n", param.PROB_HOSP_RG_80p ) ;

	fprintf( stderr, "PROB_DEATH_0_9 	%lf\n", param.PROB_DEATH_0_9  );
	fprintf( stderr, "PROB_DEATH_10_19 	%lf\n", param.PROB_DEATH_10_19 );
	fprintf( stderr, "PROB_DEATH_20_29 	%lf\n", param.PROB_DEATH_20_29  );
	fprintf( stderr, "PROB_DEATH_30_39 	%lf\n", param.PROB_DEATH_30_39  );
	fprintf( stderr, "PROB_DEATH_40_49 	%lf\n", param.PROB_DEATH_40_49 );
	fprintf( stderr, "PROB_DEATH_50_59 	%lf\n", param.PROB_DEATH_50_59  );
	fprintf( stderr, "PROB_DEATH_60_69 	%lf\n", param.PROB_DEATH_60_69  );
	fprintf( stderr, "PROB_DEATH_70_79 	%lf\n", param.PROB_DEATH_70_79  );
	fprintf( stderr, "PROB_DEATH_80p 	%lf\n", param.PROB_DEATH_80p  );
	fprintf( stderr, "PROB_DEATH_RG_0_9 	%lf\n", param.PROB_DEATH_RG_0_9  );
	fprintf( stderr, "PROB_DEATH_RG_10_19 	%lf\n", param.PROB_DEATH_RG_10_19  );
	fprintf( stderr, "PROB_DEATH_RG_20_29 	%lf\n", param.PROB_DEATH_RG_20_29  );
	fprintf( stderr, "PROB_DEATH_RG_30_39 	%lf\n", param.PROB_DEATH_RG_30_39  );
	fprintf( stderr, "PROB_DEATH_RG_40_49 	%lf\n", param.PROB_DEATH_RG_40_49  );
	fprintf( stderr, "PROB_DEATH_RG_50_59 	%lf\n", param.PROB_DEATH_RG_50_59  );
	fprintf( stderr, "PROB_DEATH_RG_60_69 	%lf\n", param.PROB_DEATH_RG_60_69  );
	fprintf( stderr, "PROB_DEATH_RG_70_79 	%lf\n", param.PROB_DEATH_RG_70_79  );
	fprintf( stderr, "PROB_DEATH_RG_80p 	%lf\n", param.PROB_DEATH_RG_80p  );

	fprintf( stderr, "TIME_FROM_I_TO_DEATH	%lf\n", param.TIME_FROM_I_TO_DEATH  );
	fprintf( stderr, "TIME_FROM_H_TO_DEATH	%lf\n", param.TIME_FROM_H_TO_DEATH ) ;
	fprintf( stderr, "TIME_FROM_ICU_TO_DEATH	%lf\n", param.TIME_FROM_ICU_TO_DEATH ) ;

	fprintf( stderr, "FRACTION_RG_0_9	%lf\n", param.FRACTION_RG[0]  );
	fprintf( stderr, "FRACTION_RG_10_19	%lf\n", param.FRACTION_RG[1]  );
	fprintf( stderr, "FRACTION_RG_20_29	%lf\n", param.FRACTION_RG[2]  );
	fprintf( stderr, "FRACTION_RG_30_39	%lf\n", param.FRACTION_RG[3]  );
	fprintf( stderr, "FRACTION_RG_40_49	%lf\n", param.FRACTION_RG[4]  );
	fprintf( stderr, "FRACTION_RG_50_59	%lf\n", param.FRACTION_RG[5]  );
	fprintf( stderr, "FRACTION_RG_60_69	%lf\n", param.FRACTION_RG[6]  );
	fprintf( stderr, "FRACTION_RG_70_79	%lf\n", param.FRACTION_RG[7]  );
	fprintf( stderr, "FRACTION_RG_80p	%lf\n", param.FRACTION_RG[8]  );

	fprintf( stderr, "ONSET_TO_HOSP_PROB_0_9	%lf\n",   param.ONSET_TO_HOSP_PROB_0_9  );
	fprintf( stderr, "ONSET_TO_HOSP_SIZE_0_9	%lf\n",   param.ONSET_TO_HOSP_SIZE_0_9  );
	fprintf( stderr, "ONSET_TO_HOSP_PROB_10_19	%lf\n", param.ONSET_TO_HOSP_PROB_10_19  );
	fprintf( stderr, "ONSET_TO_HOSP_SIZE_10_19	%lf\n", param.ONSET_TO_HOSP_SIZE_10_19  );
	fprintf( stderr, "ONSET_TO_HOSP_PROB_20_29	%lf\n", param.ONSET_TO_HOSP_PROB_20_29  );
	fprintf( stderr, "ONSET_TO_HOSP_SIZE_20_29	%lf\n", param.ONSET_TO_HOSP_SIZE_20_29  );
	fprintf( stderr, "ONSET_TO_HOSP_PROB_30_39	%lf\n", param.ONSET_TO_HOSP_PROB_30_39  );
	fprintf( stderr, "ONSET_TO_HOSP_SIZE_30_39	%lf\n", param.ONSET_TO_HOSP_SIZE_30_39  );
	fprintf( stderr, "ONSET_TO_HOSP_PROB_40_49	%lf\n", param.ONSET_TO_HOSP_PROB_40_49  );
	fprintf( stderr, "ONSET_TO_HOSP_SIZE_40_49	%lf\n", param.ONSET_TO_HOSP_SIZE_40_49  );
	fprintf( stderr, "ONSET_TO_HOSP_PROB_50_59	%lf\n", param.ONSET_TO_HOSP_PROB_50_59  );
	fprintf( stderr, "ONSET_TO_HOSP_SIZE_50_59	%lf\n", param.ONSET_TO_HOSP_SIZE_50_59  );
	fprintf( stderr, "ONSET_TO_HOSP_PROB_60_69	%lf\n", param.ONSET_TO_HOSP_PROB_60_69  );
	fprintf( stderr, "ONSET_TO_HOSP_SIZE_60_69	%lf\n", param.ONSET_TO_HOSP_SIZE_60_69  );
	fprintf( stderr, "ONSET_TO_HOSP_PROB_70_79	%lf\n", param.ONSET_TO_HOSP_PROB_70_79  );
	fprintf( stderr, "ONSET_TO_HOSP_SIZE_70_79	%lf\n", param.ONSET_TO_HOSP_SIZE_70_79  );
	fprintf( stderr, "ONSET_TO_HOSP_PROB_80_89	%lf\n", param.ONSET_TO_HOSP_PROB_80_89  );
	fprintf( stderr, "ONSET_TO_HOSP_SIZE_80_89	%lf\n", param.ONSET_TO_HOSP_SIZE_80_89  );
	fprintf( stderr, "ONSET_TO_HOSP_PROB_90p	%lf\n",   param.ONSET_TO_HOSP_PROB_90p  );
	fprintf( stderr, "ONSET_TO_HOSP_SIZE_90p	%lf\n",   param.ONSET_TO_HOSP_SIZE_90p );

	fprintf( stderr, "LOS_HOSP_PROB_0_9	%lf\n", param.LOS_HOSP_PROB_0_9  );
	fprintf( stderr, "LOS_HOSP_SIZE_0_9	%lf\n", param.LOS_HOSP_SIZE_0_9  );
	fprintf( stderr, "LOS_HOSP_PROB_10_19	%lf\n", param.LOS_HOSP_PROB_10_19  );
	fprintf( stderr, "LOS_HOSP_SIZE_10_19	%lf\n", param.LOS_HOSP_SIZE_10_19  );
	fprintf( stderr, "LOS_HOSP_PROB_20_29	%lf\n", param.LOS_HOSP_PROB_20_29  );
	fprintf( stderr, "LOS_HOSP_SIZE_20_29	%lf\n", param.LOS_HOSP_SIZE_20_29  );
	fprintf( stderr, "LOS_HOSP_PROB_30_39	%lf\n", param.LOS_HOSP_PROB_30_39  );
	fprintf( stderr, "LOS_HOSP_SIZE_30_39	%lf\n", param.LOS_HOSP_SIZE_30_39  );
	fprintf( stderr, "LOS_HOSP_PROB_40_49	%lf\n", param.LOS_HOSP_PROB_40_49  );
	fprintf( stderr, "LOS_HOSP_SIZE_40_49	%lf\n", param.LOS_HOSP_SIZE_40_49  );
	fprintf( stderr, "LOS_HOSP_PROB_50_59	%lf\n", param.LOS_HOSP_PROB_50_59  );
	fprintf( stderr, "LOS_HOSP_SIZE_50_59	%lf\n", param.LOS_HOSP_SIZE_50_59  );
	fprintf( stderr, "LOS_HOSP_PROB_60_69	%lf\n", param.LOS_HOSP_PROB_60_69  );
	fprintf( stderr, "LOS_HOSP_SIZE_60_69	%lf\n", param.LOS_HOSP_SIZE_60_69  );
	fprintf( stderr, "LOS_HOSP_PROB_70_79	%lf\n", param.LOS_HOSP_PROB_70_79  );
	fprintf( stderr, "LOS_HOSP_SIZE_70_79	%lf\n", param.LOS_HOSP_SIZE_70_79  );
	fprintf( stderr, "LOS_HOSP_PROB_80_89	%lf\n", param.LOS_HOSP_PROB_80_89  );
	fprintf( stderr, "LOS_HOSP_SIZE_80_89	%lf\n", param.LOS_HOSP_SIZE_80_89  );
	fprintf( stderr, "LOS_HOSP_PROB_90p	%lf\n", param.LOS_HOSP_PROB_90p  );
	fprintf( stderr, "LOS_HOSP_SIZE_90p	%lf\n", param.LOS_HOSP_SIZE_90p  );

	fprintf( stderr, "PROB_ICU_0_9	%lf\n", param.PROB_ICU_0_9  );
	fprintf( stderr, "PROB_ICU_10_19	%lf\n", param.PROB_ICU_10_19  );
	fprintf( stderr, "PROB_ICU_20_29	%lf\n", param.PROB_ICU_20_29  );
	fprintf( stderr, "PROB_ICU_30_39	%lf\n", param.PROB_ICU_30_39  );
	fprintf( stderr, "PROB_ICU_40_49	%lf\n", param.PROB_ICU_40_49  );
	fprintf( stderr, "PROB_ICU_50_59	%lf\n", param.PROB_ICU_50_59  );
	fprintf( stderr, "PROB_ICU_60_69	%lf\n", param.PROB_ICU_60_69  );
	fprintf( stderr, "PROB_ICU_70_79	%lf\n", param.PROB_ICU_70_79  );
	fprintf( stderr, "PROB_ICU_80p	%lf\n", param.PROB_ICU_80p  );

	fprintf( stderr, "TIME_TO_ICU_PROB_0_9	%lf\n", param.TIME_TO_ICU_PROB_0_9  );
	fprintf( stderr, "TIME_TO_ICU_SIZE_0_9	%lf\n", param.TIME_TO_ICU_SIZE_0_9  );
	fprintf( stderr, "TIME_TO_ICU_PROB_10_19	%lf\n", param.TIME_TO_ICU_PROB_10_19  );
	fprintf( stderr, "TIME_TO_ICU_SIZE_10_19	%lf\n", param.TIME_TO_ICU_SIZE_10_19  );
	fprintf( stderr, "TIME_TO_ICU_PROB_20_29	%lf\n", param.TIME_TO_ICU_PROB_20_29  );
	fprintf( stderr, "TIME_TO_ICU_SIZE_20_29	%lf\n", param.TIME_TO_ICU_SIZE_20_29  );
	fprintf( stderr, "TIME_TO_ICU_PROB_30_39	%lf\n", param.TIME_TO_ICU_PROB_30_39  );
	fprintf( stderr, "TIME_TO_ICU_SIZE_30_39	%lf\n", param.TIME_TO_ICU_SIZE_30_39  );
	fprintf( stderr, "TIME_TO_ICU_PROB_40_49	%lf\n", param.TIME_TO_ICU_PROB_40_49  );
	fprintf( stderr, "TIME_TO_ICU_SIZE_40_49	%lf\n", param.TIME_TO_ICU_SIZE_40_49  );
	fprintf( stderr, "TIME_TO_ICU_PROB_50_59	%lf\n", param.TIME_TO_ICU_PROB_50_59  );
	fprintf( stderr, "TIME_TO_ICU_SIZE_50_59	%lf\n", param.TIME_TO_ICU_SIZE_50_59  );
	fprintf( stderr, "TIME_TO_ICU_PROB_60_69	%lf\n", param.TIME_TO_ICU_PROB_60_69  );
	fprintf( stderr, "TIME_TO_ICU_SIZE_60_69	%lf\n", param.TIME_TO_ICU_SIZE_60_69  );
	fprintf( stderr, "TIME_TO_ICU_PROB_70_79	%lf\n", param.TIME_TO_ICU_PROB_70_79  );
	fprintf( stderr, "TIME_TO_ICU_SIZE_70_79	%lf\n", param.TIME_TO_ICU_SIZE_70_79  );
	fprintf( stderr, "TIME_TO_ICU_PROB_80p	%lf\n", param.TIME_TO_ICU_PROB_80p  );
	fprintf( stderr, "TIME_TO_ICU_SIZE_80p	%lf\n", param.TIME_TO_ICU_SIZE_80p  );

	fprintf( stderr, "LOS_ICU_PROB_0_9	%lf\n", param.LOS_ICU_PROB_0_9  );
	fprintf( stderr, "LOS_ICU_SIZE_0_9	%lf\n", param.LOS_ICU_SIZE_0_9  );
	fprintf( stderr, "LOS_ICU_PROB_10_19	%lf\n", param.LOS_ICU_PROB_10_19  );
	fprintf( stderr, "LOS_ICU_SIZE_10_19	%lf\n", param.LOS_ICU_SIZE_10_19  );
	fprintf( stderr, "LOS_ICU_PROB_20_29	%lf\n", param.LOS_ICU_PROB_20_29  );
	fprintf( stderr, "LOS_ICU_SIZE_20_29	%lf\n", param.LOS_ICU_SIZE_20_29  );
	fprintf( stderr, "LOS_ICU_PROB_30_39	%lf\n", param.LOS_ICU_PROB_30_39  );
	fprintf( stderr, "LOS_ICU_SIZE_30_39	%lf\n", param.LOS_ICU_SIZE_30_39  );
	fprintf( stderr, "LOS_ICU_PROB_40_49	%lf\n", param.LOS_ICU_PROB_40_49  );
	fprintf( stderr, "LOS_ICU_SIZE_40_49	%lf\n", param.LOS_ICU_SIZE_40_49  );
	fprintf( stderr, "LOS_ICU_PROB_50_59	%lf\n", param.LOS_ICU_PROB_50_59  );
	fprintf( stderr, "LOS_ICU_SIZE_50_59	%lf\n", param.LOS_ICU_SIZE_50_59  );
	fprintf( stderr, "LOS_ICU_PROB_60_69	%lf\n", param.LOS_ICU_PROB_60_69  );
	fprintf( stderr, "LOS_ICU_SIZE_60_69	%lf\n", param.LOS_ICU_SIZE_60_69  );
	fprintf( stderr, "LOS_ICU_PROB_70_79	%lf\n", param.LOS_ICU_PROB_70_79  );
	fprintf( stderr, "LOS_ICU_SIZE_70_79	%lf\n", param.LOS_ICU_SIZE_70_79  );
	fprintf( stderr, "LOS_ICU_PROB_80p	%lf\n", param.LOS_ICU_PROB_80p  );
	fprintf( stderr, "LOS_ICU_SIZE_80p	%lf\n", param.LOS_ICU_SIZE_80p  );

	fprintf( stderr, "LOS_HOSP_ICU_PROB_0_9	%lf\n", param.LOS_HOSP_ICU_PROB_0_9  );
	fprintf( stderr, "LOS_HOSP_ICU_SIZE_0_9	%lf\n", param.LOS_HOSP_ICU_SIZE_0_9  );
	fprintf( stderr, "LOS_HOSP_ICU_PROB_10_19	%lf\n", param.LOS_HOSP_ICU_PROB_10_19  );
	fprintf( stderr, "LOS_HOSP_ICU_SIZE_10_19	%lf\n", param.LOS_HOSP_ICU_SIZE_10_19  );
	fprintf( stderr, "LOS_HOSP_ICU_PROB_20_29	%lf\n", param.LOS_HOSP_ICU_PROB_20_29  );
	fprintf( stderr, "LOS_HOSP_ICU_SIZE_20_29	%lf\n", param.LOS_HOSP_ICU_SIZE_20_29  );
	fprintf( stderr, "LOS_HOSP_ICU_PROB_30_39	%lf\n", param.LOS_HOSP_ICU_PROB_30_39  );
	fprintf( stderr, "LOS_HOSP_ICU_SIZE_30_39	%lf\n", param.LOS_HOSP_ICU_SIZE_30_39  );
	fprintf( stderr, "LOS_HOSP_ICU_PROB_40_49	%lf\n", param.LOS_HOSP_ICU_PROB_40_49  );
	fprintf( stderr, "LOS_HOSP_ICU_SIZE_40_49	%lf\n", param.LOS_HOSP_ICU_SIZE_40_49  );
	fprintf( stderr, "LOS_HOSP_ICU_PROB_50_59	%lf\n", param.LOS_HOSP_ICU_PROB_50_59  );
	fprintf( stderr, "LOS_HOSP_ICU_SIZE_50_59	%lf\n", param.LOS_HOSP_ICU_SIZE_50_59  );
	fprintf( stderr, "LOS_HOSP_ICU_PROB_60_69	%lf\n", param.LOS_HOSP_ICU_PROB_60_69  );
	fprintf( stderr, "LOS_HOSP_ICU_SIZE_60_69	%lf\n", param.LOS_HOSP_ICU_SIZE_60_69  );
	fprintf( stderr, "LOS_HOSP_ICU_PROB_70_79	%lf\n", param.LOS_HOSP_ICU_PROB_70_79  );
	fprintf( stderr, "LOS_HOSP_ICU_SIZE_70_79	%lf\n", param.LOS_HOSP_ICU_SIZE_70_79  );
	fprintf( stderr, "LOS_HOSP_ICU_PROB_80p	%lf\n", param.LOS_HOSP_ICU_PROB_80p  );
	fprintf( stderr, "LOS_HOSP_ICU_SIZE_80p	%lf\n", param.LOS_HOSP_ICU_SIZE_80p  );

	fprintf( stderr, "N_HWS_0_9	%lf\n", param.N_HWS[0]  );
	fprintf( stderr, "N_HWS_10_19	%lf\n", param.N_HWS[1]  );
	fprintf( stderr, "N_HWS_20_29	%lf\n", param.N_HWS[2]  );
	fprintf( stderr, "N_HWS_30_39	%lf\n", param.N_HWS[3]  );
	fprintf( stderr, "N_HWS_40_49	%lf\n", param.N_HWS[4]  );
	fprintf( stderr, "N_HWS_50_59	%lf\n", param.N_HWS[5]  );
	fprintf( stderr, "N_HWS_60_69	%lf\n", param.N_HWS[6]  );
	fprintf( stderr, "N_HWS_70_79	%lf\n", param.N_HWS[7]  );
	fprintf( stderr, "N_HWS_80p	%lf\n", param.N_HWS[8]  );

} // void print_parameters

int read_scenario_names(char file[],char sep){

	char *line;
	int out_get_line=2;
	FILE *fp;
	char string_who[100];
	char string_value[100];

	if( !(fp = fopen(file, "r")) ){
		fprintf(stderr,"read_scenario_names: error opening file %s for reading\n",file);
		return 1;
	}

	while(out_get_line>=2){
		out_get_line=get_line(&line,fp);
		if(out_get_line<3){
			switch(out_get_line){
				case 2:
				fprintf(stderr,"read_scenario_names: line of file %s does not end in newline\n",file);
				break;
				case 1:
				fprintf(stderr,"read_scenario_names: file %s contains an empty line\n",file);
				return 1;
				break;
				case 0:
				fclose(fp);
				return 0;
				break;
				case -1:
				fprintf(stderr,"read_scenario_names: get_line error on file %s\n", file);
				return 1;
				default:
				fprintf(stderr,"read_scenario_names: unrecognized exit status of get_line on file %s\n",file);
				return 1;
				break;
			}
		}

		sscanf(line, "%s", string_who);
		line = (char *) strchr(line, sep);
		line++;
		sscanf(line, "%s", string_value);

		if(strcmp(string_who,"NAME_VAX_PRIORITY_SCEN_1")==0) sscanf( string_value, "%s", param.NAME_VAX_PRIORITY_SCEN_1 );
		if(strcmp(string_who,"NAME_VAX_PRIORITY_SCEN_2")==0) sscanf( string_value, "%s", param.NAME_VAX_PRIORITY_SCEN_2 );
		if(strcmp(string_who,"NAME_VAX_PRIORITY_SCEN_3")==0) sscanf( string_value, "%s", param.NAME_VAX_PRIORITY_SCEN_3 );
		if(strcmp(string_who,"NAME_VAX_PROFILE_SCEN"   )==0) sscanf( string_value, "%s", param.NAME_VAX_PROFILE_SCEN );
		if(strcmp(string_who,"NAME_ADHERENCE_SCEN"     )==0) sscanf( string_value, "%s", param.NAME_ADHERENCE_SCEN );
		if(strcmp(string_who,"NAME_DOSES_SCEN"         )==0) sscanf( string_value, "%s", param.NAME_DOSES_SCEN );
		if(strcmp(string_who,"NAME_GEO"                )==0) sscanf( string_value, "%s", param.NAME_GEO );
		if(strcmp(string_who,"NAME_IMPORT_SCEN"        )==0) sscanf( string_value, "%s", param.NAME_IMPORT_SCEN );
		if(strcmp(string_who,"NAME_CONTROL_SCEN"       )==0) sscanf( string_value, "%s", param.NAME_CONTROL_SCEN );
		if(strcmp(string_who,"NAME_REOPEN_SCEN"        )==0) sscanf( string_value, "%s", param.NAME_REOPEN_SCEN );
		if(strcmp(string_who,"NAME_SCENARIO"           )==0) sscanf( string_value, "%s", param.NAME_SCENARIO );
		if(strcmp(string_who,"PRIORITIZATION_1"        )==0) sscanf( string_value, "%s", param.PRIORITIZATION_1 );
		if(strcmp(string_who,"PRIORITIZATION_2"        )==0) sscanf( string_value, "%s", param.PRIORITIZATION_2 );
		if(strcmp(string_who,"PRIORITIZATION_3"        )==0) sscanf( string_value, "%s", param.PRIORITIZATION_3 );
		if(strcmp(string_who,"PROFILE"                 )==0) sscanf( string_value, "%s", param.PROFILE );
		if(strcmp(string_who,"P_TRANS"                 )==0) sscanf( string_value, "%s", param.P_TRANS );
		if(strcmp(string_who,"ADHERENCE"               )==0) sscanf( string_value, "%s", param.ADHERENCE );
		if(strcmp(string_who,"DELAY_SCEN"              )==0) sscanf( string_value, "%s", param.DELAY_SCEN );
		if(strcmp(string_who,"DOSES_SCEN"              )==0) sscanf( string_value, "%s", param.DOSES_SCEN );
		if(strcmp(string_who,"SEASONALITY"             )==0) param.SEASONALITY            = atof(string_value);
		if(strcmp(string_who,"REGIONAL"                )==0) param.REGIONAL               = atof(string_value);
		if(strcmp(string_who,"REGIONAL_PRIORITY"       )==0) param.REGIONAL_PRIORITY      = atof(string_value);
		if(strcmp(string_who,"GEO"                     )==0) param.GEO                    = atof(string_value);
		if(strcmp(string_who,"REGIONAL_START"          )==0) param.REGIONAL_START         = atof(string_value);
		if(strcmp(string_who,"CONTROL_H_REGION"        )==0) param.CONTROL_H_REGION       = atof(string_value);
		if(strcmp(string_who,"KEEP_R_CONSTANT"         )==0) param.KEEP_R_CONSTANT        = atof(string_value);
		if(strcmp(string_who,"IMPORT"                  )==0) sscanf( string_value, "%s", param.IMPORT );
		if(strcmp(string_who,"REPROD_NUM"              )==0) param.REPROD_NUM             = atof(string_value);

		param.CONTROL_Rt_MIN = 0.9; // threshold
		param.CONTROL_Rt_MAX = 1.1; // threshold
	}
	return 2;
} // int read_scenario_names

void print_scenario_names(){
	fprintf( stderr, "\n");
	fprintf( stderr, "NAME_VAX_PRIORITY_SCEN_1  %s\n",  param.NAME_VAX_PRIORITY_SCEN_1 );
	fprintf( stderr, "NAME_VAX_PRIORITY_SCEN_2  %s\n",  param.NAME_VAX_PRIORITY_SCEN_2 );
	fprintf( stderr, "NAME_VAX_PRIORITY_SCEN_3  %s\n",  param.NAME_VAX_PRIORITY_SCEN_3 );
	fprintf( stderr, "NAME_VAX_PROFILE_SCEN     %s\n",  param.NAME_VAX_PROFILE_SCEN    );
	fprintf( stderr, "NAME_ADHERENCE_SCEN       %s\n",  param.NAME_ADHERENCE_SCEN      );
	fprintf( stderr, "NAME_DOSES_SCEN           %s\n",  param.NAME_DOSES_SCEN          );
	fprintf( stderr, "NAME_GEO                  %s\n",  param.NAME_GEO                 );
	fprintf( stderr, "NAME_IMPORT_SCEN          %s\n",  param.NAME_IMPORT_SCEN         );
	fprintf( stderr, "NAME_CONTROL_SCEN         %s\n",  param.NAME_CONTROL_SCEN        );
	fprintf( stderr, "NAME_REOPEN_SCEN          %s\n",  param.NAME_REOPEN_SCEN         );
	fprintf( stderr, "NAME_SCENARIO             %s\n",  param.NAME_SCENARIO            );
	fprintf( stderr, "PRIORITIZATION_1          %s\n",  param.PRIORITIZATION_1         );
	fprintf( stderr, "PRIORITIZATION_2          %s\n",  param.PRIORITIZATION_2         );
	fprintf( stderr, "PRIORITIZATION_3          %s\n",  param.PRIORITIZATION_3         );
	fprintf( stderr, "PROFILE                   %s\n",  param.PROFILE                  );
	fprintf( stderr, "P_TRANS                   %s\n",  param.P_TRANS                  );
	fprintf( stderr, "ADHERENCE                 %s\n",  param.ADHERENCE                );
	fprintf( stderr, "DELAY_SCEN                %s\n",  param.DELAY_SCEN               );
	fprintf( stderr, "DOSES_SCEN                %s\n",  param.DOSES_SCEN               );
	fprintf( stderr, "SEASONALITY               %d\n",  param.SEASONALITY              );
	fprintf( stderr, "REGIONAL                  %lf\n", param.REGIONAL                 );
	fprintf( stderr, "REGIONAL_PRIORITY         %d\n",  param.REGIONAL_PRIORITY        );
	fprintf( stderr, "GEO                       %d\n",  param.GEO                      );
	fprintf( stderr, "REGIONAL_START            %d\n",  param.REGIONAL_START           );
	fprintf( stderr, "CONTROL_H_REGION          %d\n",  param.CONTROL_H_REGION         );
	fprintf( stderr, "KEEP_R_CONSTANT           %d\n",  param.KEEP_R_CONSTANT          );
	fprintf( stderr, "IMPORT                    %s\n",  param.IMPORT                   );
	fprintf( stderr, "REPROD_NUM                %lf\n", param.REPROD_NUM               );
	fprintf( stderr, "\n");
} // void print_scenario_names

int read_adherence_param( char file[], char sep ){

	char *line;
	int out_get_line = 2;
	FILE *fp;
	char string_who[100];
	char string_value[100];

	if( !( fp = fopen( file, "r" ) ) ){
		fprintf( stderr, "read_adherence_param: error opening file %s for reading\n",file);
		return 1;
	}

	while(out_get_line>=2){
		out_get_line=get_line(&line,fp);
		if(out_get_line<3){
			switch(out_get_line){
				case 2:
				fprintf(stderr,"read_adherence_param: line of file %s does not end in newline\n",file);
				break;
				case 1:
				fprintf(stderr,"read_adherence_param: file %s contains an empty line\n",file);
				return 1;
				break;
				case 0:
				fclose(fp);
				return 0;
				break;
				case -1:
				fprintf(stderr,"read_adherence_param: get_line error on file %s\n", file);
				return 1;
				default:
				fprintf(stderr,"read_adherence_param: unrecognized exit status of get_line on file %s\n",file);
				return 1;
				break;
			}
		}

		sscanf( line, "%s", string_who );
		line = (char *)strchr(line, sep);
		line++;

		sscanf(line,"%s", string_value);

		if(strcmp(string_who, "DAY_START_VACCINATION") == 0) param.DAY_START_VACCINATION = atof(string_value);
		if(strcmp(string_who, "WASTE")                 == 0) param.WASTE                 = atof(string_value);

		if(strcmp(string_who, "ADHERENCE_1_0_11") == 0) param.ADHERENCE_1_0_11 = atof(string_value);
		if(strcmp(string_who, "ADHERENCE_1_12_15") == 0) param.ADHERENCE_1_12_15 = atof(string_value);
		if(strcmp(string_who, "ADHERENCE_1_16_17") == 0) param.ADHERENCE_1_16_17 = atof(string_value);
		if(strcmp(string_who, "ADHERENCE_1_18_24") == 0) param.ADHERENCE_1_18_24 = atof(string_value);
		if(strcmp(string_who, "ADHERENCE_1_25_39") == 0) param.ADHERENCE_1_25_39 = atof(string_value);
		if(strcmp(string_who, "ADHERENCE_1_40_44") == 0) param.ADHERENCE_1_40_44 = atof(string_value);
		if(strcmp(string_who, "ADHERENCE_1_45_54") == 0) param.ADHERENCE_1_45_54 = atof(string_value);
		if(strcmp(string_who, "ADHERENCE_1_55_64") == 0) param.ADHERENCE_1_55_64 = atof(string_value);
		if(strcmp(string_who, "ADHERENCE_1_65_74") == 0) param.ADHERENCE_1_65_74 = atof(string_value);
		if(strcmp(string_who, "ADHERENCE_1_75_84") == 0) param.ADHERENCE_1_75_84 = atof(string_value);
		if(strcmp(string_who, "ADHERENCE_1_85p") == 0) param.ADHERENCE_1_85p = atof(string_value);

		if(strcmp(string_who, "ADHERENCE_2_0_11") == 0) param.ADHERENCE_2_0_11 = atof(string_value);
		if(strcmp(string_who, "ADHERENCE_2_12_15") == 0) param.ADHERENCE_2_12_15 = atof(string_value);
		if(strcmp(string_who, "ADHERENCE_2_16_17") == 0) param.ADHERENCE_2_16_17 = atof(string_value);
		if(strcmp(string_who, "ADHERENCE_2_18_24") == 0) param.ADHERENCE_2_18_24 = atof(string_value);
		if(strcmp(string_who, "ADHERENCE_2_25_39") == 0) param.ADHERENCE_2_25_39 = atof(string_value);
		if(strcmp(string_who, "ADHERENCE_2_40_44") == 0) param.ADHERENCE_2_40_44 = atof(string_value);
		if(strcmp(string_who, "ADHERENCE_2_45_54") == 0) param.ADHERENCE_2_45_54 = atof(string_value);
		if(strcmp(string_who, "ADHERENCE_2_55_64") == 0) param.ADHERENCE_2_55_64 = atof(string_value);
		if(strcmp(string_who, "ADHERENCE_2_65_74") == 0) param.ADHERENCE_2_65_74 = atof(string_value);
		if(strcmp(string_who, "ADHERENCE_2_75_84") == 0) param.ADHERENCE_2_75_84 = atof(string_value);
		if(strcmp(string_who, "ADHERENCE_2_85p") == 0) param.ADHERENCE_2_85p = atof(string_value);

		if(strcmp(string_who, "ADHERENCE_3_0_11") == 0) param.ADHERENCE_3_0_11 = atof(string_value);
		if(strcmp(string_who, "ADHERENCE_3_12_15") == 0) param.ADHERENCE_3_12_15 = atof(string_value);
		if(strcmp(string_who, "ADHERENCE_3_16_17") == 0) param.ADHERENCE_3_16_17 = atof(string_value);
		if(strcmp(string_who, "ADHERENCE_3_18_24") == 0) param.ADHERENCE_3_18_24 = atof(string_value);
		if(strcmp(string_who, "ADHERENCE_3_25_39") == 0) param.ADHERENCE_3_25_39 = atof(string_value);
		if(strcmp(string_who, "ADHERENCE_3_40_44") == 0) param.ADHERENCE_3_40_44 = atof(string_value);
		if(strcmp(string_who, "ADHERENCE_3_45_54") == 0) param.ADHERENCE_3_45_54 = atof(string_value);
		if(strcmp(string_who, "ADHERENCE_3_55_64") == 0) param.ADHERENCE_3_55_64 = atof(string_value);
		if(strcmp(string_who, "ADHERENCE_3_65_74") == 0) param.ADHERENCE_3_65_74 = atof(string_value);
		if(strcmp(string_who, "ADHERENCE_3_75_84") == 0) param.ADHERENCE_3_75_84 = atof(string_value);
		if(strcmp(string_who, "ADHERENCE_3_85p") == 0) param.ADHERENCE_3_85p = atof(string_value);

	}
	return 2;
} // int read_adherence_param

int read_county_vax_priority(char file[], char sep){

	char *line;
	int out_get_line=2;
	FILE *fp;
	char string_value[100];
	int count = 0;
	if( !(fp = fopen(file, "r")) )
	{
		fprintf(stderr,"read_scenario_names: error opening file %s for reading\n",file);
		return 1;
	}

	while(out_get_line>=2)
	{
		out_get_line=get_line(&line,fp);
		if(out_get_line<3)
		{
			switch(out_get_line)
			{
				case 2:
				fprintf(stderr,"read_scenario_names: line of file %s does not end in newline\n",file);
				break;
				case 1:
				fprintf(stderr,"read_scenario_names: file %s contains an empty line\n",file);
				return 1;
				break;
				case 0:
				fclose(fp);
				return 0;
				break;
				case -1:
				fprintf(stderr,"read_scenario_names: get_line error on file %s\n", file);
				return 1;
				default:
				fprintf(stderr,"read_scenario_names: unrecognized exit status of get_line on file %s\n",file);
				return 1;
				break;
			}
		}

		sscanf( line, "%s", string_value );
		line = (char *)strchr(line, sep);

		param.COUNTY_PRIORITY[count] = atof(string_value);

		line++;
		count++;

	}

	return 2;
} // int read_county_vax_priority

int read_vaccine_param( char file[], char sep ){

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

		if( strcmp(string_who,"RAMP_UP_1ST_1"         )==0 ) param.RAMP_UP_TIME[0][0]          = atof(string_value);
		if( strcmp(string_who,"RAMP_UP_1ST_2"         )==0 ) param.RAMP_UP_TIME[0][1]          = atof(string_value);
		if( strcmp(string_who,"RAMP_UP_1ST_3"         )==0 ) param.RAMP_UP_TIME[0][2]          = atof(string_value);
		if( strcmp(string_who,"DELAY_EFFECT_1"         )==0 ) param.DELAY_EFFECT[0]          = atof(string_value);
		if( strcmp(string_who,"DELAY_EFFECT_2"         )==0 ) param.DELAY_EFFECT[1]          = atof(string_value);
		if( strcmp(string_who,"DELAY_EFFECT_3"         )==0 ) param.DELAY_EFFECT[2]          = atof(string_value);
		if( strcmp(string_who,"DELAY_EFFECT_1_LONG"    )==0 ) param.DELAY_EFFECT_1_LONG      = atof(string_value);
		if( strcmp(string_who,"DELAY_EFFECT_1_LONG_t"  )==0 ) param.DELAY_EFFECT_1_LONG_t    = atof(string_value);
		if( strcmp(string_who,"RAMP_UP_2ND_1"         )==0 ) param.RAMP_UP_TIME[1][0]          = atof(string_value);
		if( strcmp(string_who,"RAMP_UP_2ND_2"         )==0 ) param.RAMP_UP_TIME[1][1]          = atof(string_value);
		if( strcmp(string_who,"RAMP_UP_2ND_3"         )==0 ) param.RAMP_UP_TIME[1][2]          = atof(string_value);
		if( strcmp(string_who,"VACCINE_EFF_1ST_1_Ia"         )==0 ) param.VACCINE_EFF_Ia[0][0]          = atof(string_value);
		if( strcmp(string_who,"VACCINE_EFF_1ST_2_Ia"         )==0 ) param.VACCINE_EFF_Ia[0][1]          = atof(string_value);
		if( strcmp(string_who,"VACCINE_EFF_1ST_3_Ia"         )==0 ) param.VACCINE_EFF_Ia[0][2]          = atof(string_value);
		if( strcmp(string_who,"VACCINE_EFF_2ND_1_Ia"         )==0 ) param.VACCINE_EFF_Ia[1][0]          = atof(string_value);
		if( strcmp(string_who,"VACCINE_EFF_2ND_2_Ia"         )==0 ) param.VACCINE_EFF_Ia[1][1]          = atof(string_value);
		if( strcmp(string_who,"VACCINE_EFF_2ND_3_Ia"         )==0 ) param.VACCINE_EFF_Ia[1][2]          = atof(string_value);
		if( strcmp(string_who,"VACCINE_EFF_1ST_1_I"         )==0 ) param.VACCINE_EFF_I[0][0]          = atof(string_value);
		if( strcmp(string_who,"VACCINE_EFF_1ST_2_I"         )==0 ) param.VACCINE_EFF_I[0][1]          = atof(string_value);
		if( strcmp(string_who,"VACCINE_EFF_1ST_3_I"         )==0 ) param.VACCINE_EFF_I[0][2]          = atof(string_value);
		if( strcmp(string_who,"VACCINE_EFF_2ND_1_I"         )==0 ) param.VACCINE_EFF_I[1][0]          = atof(string_value);
		if( strcmp(string_who,"VACCINE_EFF_2ND_2_I"         )==0 ) param.VACCINE_EFF_I[1][1]          = atof(string_value);
		if( strcmp(string_who,"VACCINE_EFF_2ND_3_I"         )==0 ) param.VACCINE_EFF_I[1][2]          = atof(string_value);
		if( strcmp(string_who,"VACCINE_EFF_1ST_1_S"         )==0 ) param.VACCINE_EFF_S[0][0]          = atof(string_value);
		if( strcmp(string_who,"VACCINE_EFF_1ST_2_S"         )==0 ) param.VACCINE_EFF_S[0][1]          = atof(string_value);
		if( strcmp(string_who,"VACCINE_EFF_1ST_3_S"         )==0 ) param.VACCINE_EFF_S[0][2]          = atof(string_value);
		if( strcmp(string_who,"VACCINE_EFF_2ND_1_S"         )==0 ) param.VACCINE_EFF_S[1][0]          = atof(string_value);
		if( strcmp(string_who,"VACCINE_EFF_2ND_2_S"         )==0 ) param.VACCINE_EFF_S[1][1]          = atof(string_value);
		if( strcmp(string_who,"VACCINE_EFF_2ND_3_S"         )==0 ) param.VACCINE_EFF_S[1][2]          = atof(string_value);
		if( strcmp(string_who,"VACCINE_EFF_1ST_1_D"         )==0 ) param.VACCINE_EFF_D[0][0]          = atof(string_value);
		if( strcmp(string_who,"VACCINE_EFF_1ST_2_D"         )==0 ) param.VACCINE_EFF_D[0][1]          = atof(string_value);
		if( strcmp(string_who,"VACCINE_EFF_1ST_3_D"         )==0 ) param.VACCINE_EFF_D[0][2]          = atof(string_value);
		if( strcmp(string_who,"VACCINE_EFF_2ND_1_D"         )==0 ) param.VACCINE_EFF_D[1][0]          = atof(string_value);
		if( strcmp(string_who,"VACCINE_EFF_2ND_2_D"         )==0 ) param.VACCINE_EFF_D[1][1]          = atof(string_value);
		if( strcmp(string_who,"VACCINE_EFF_2ND_3_D"         )==0 ) param.VACCINE_EFF_D[1][2]          = atof(string_value);
		if( strcmp(string_who,"VACCINE_EFF_1ST_1_V"         )==0 ) param.VACCINE_EFF_V[0][0]          = atof(string_value);
		if( strcmp(string_who,"VACCINE_EFF_1ST_2_V"         )==0 ) param.VACCINE_EFF_V[0][1]          = atof(string_value);
		if( strcmp(string_who,"VACCINE_EFF_1ST_3_V"         )==0 ) param.VACCINE_EFF_V[0][2]          = atof(string_value);
		if( strcmp(string_who,"VACCINE_EFF_2ND_1_V"         )==0 ) param.VACCINE_EFF_V[1][0]          = atof(string_value);
		if( strcmp(string_who,"VACCINE_EFF_2ND_2_V"         )==0 ) param.VACCINE_EFF_V[1][1]          = atof(string_value);
		if( strcmp(string_who,"VACCINE_EFF_2ND_3_V"         )==0 ) param.VACCINE_EFF_V[1][2]          = atof(string_value);

	}
	return 2;
} // int read_vaccine_param

void print_vaccine_parameters(){

	fprintf(stderr, "\n");

} // void print_vaccine_parameters

int read_daily_doses(char file[], char sep){

	char *line;
	int out_get_line = 2;
	FILE *fp;
	int count = 0;
	double DOSES_DAY[3] = {0};

	// JEM trying to figure out pointers: Dynamically allocate param.DOSES_DAY to be matrix of dim (param.MAXT, 3)
	param.DOSES_DAY = (int **) calloc(param.MAXT, sizeof(int*));
	for (int i = 0; i < param.MAXT; i ++) {
		param.DOSES_DAY[i] = (int *) calloc(3, sizeof(int));
	}

	if( !(fp = fopen(file, "r")) ){
		fprintf(stderr, "read_daily_doses: error opening file %s for reading\n", file);
		return 1;
	}

	while(out_get_line >= 2){
		out_get_line = get_line(&line, fp);
		if(out_get_line < 3){
			switch(out_get_line){
				case 2:
				fprintf(stderr, "read_daily_doses: line of file %s does not end in newline\n", file);
				break;
				case 1:
				fprintf(stderr, "read_daily_doses: file %s contains an empty line\n", file);
				return 1;
				break;
				case 0:
				fclose(fp);
				return 0;
				break;
				case -1:
				fprintf(stderr, "read_daily_doses: get_line error on file %s\n", file);
				return 1;
				default:
				fprintf(stderr, "read_daily_doses: unrecognized exit status of get_line on file %s\n", file);
				return 1;
				break;
			}
		}
		// read numbers in the txt file
		sscanf(line, "%lf\t%lf\t%lf", &DOSES_DAY[0], &DOSES_DAY[1], &DOSES_DAY[2]);

		// convert from %lf to %d
		param.DOSES_DAY[count][0] = DOSES_DAY[0]; // vac 1
		param.DOSES_DAY[count][1] = DOSES_DAY[1]; // vac 2
		param.DOSES_DAY[count][2] = DOSES_DAY[2]; // vac 3
		line++;
		count++;
	}
	return 2;
} // int read_daily_doses

int read_municip_prior(char file[], char sep){

	char *line;
	int out_get_line = 2;
	FILE *fp;
	int count = 0;
	int municip_prior = 9999;
	if( !(fp = fopen(file, "r")) ){
		fprintf(stderr, "read_municip_prior: error opening file %s for reading\n", file);
		return 1;
	}

	while(out_get_line >= 2){
		out_get_line = get_line(&line, fp);
		if(out_get_line < 3){
			switch(out_get_line){
				case 2:
				fprintf(stderr, "read_municip_prior: line of file %s does not end in newline\n", file);
				break;
				case 1:
				fprintf(stderr, "read_municip_prior: file %s contains an empty line\n", file);
				return 1;
				break;
				case 0:
				fclose(fp);
				return 0;
				break;
				case -1:
				fprintf(stderr, "read_municip_prior: get_line error on file %s\n", file);
				return 1;
				default:
				fprintf(stderr, "read_municip_prior: unrecognized exit status of get_line on file %s\n", file);
				return 1;
				break;
			}
		}
		// read numbers in the txt file
		sscanf(line, "%d", &municip_prior);
		// convert from %lf to %d
		param.mun_priority[count] = municip_prior;

		line++;
		count++;
	}
	return 2;
} // int read_municip_prior

int read_importation(char file[], char sep){

	char *line;
	int out_get_line = 2;
	FILE *fp;
	int count = 0;
	double import = 0;
	if( !(fp = fopen(file, "r")) ){
		fprintf(stderr, "read_importation: error opening file %s for reading\n", file);
		return 1;
	}

	while(out_get_line >= 2){
		out_get_line = get_line(&line, fp);
		if(out_get_line < 3){
			switch(out_get_line){
				case 2:
				fprintf(stderr, "read_importation: line of file %s does not end in newline\n", file);
				break;
				case 1:
				fprintf(stderr, "read_importation: file %s contains an empty line\n", file);
				return 1;
				break;
				case 0:
				fclose(fp);
				return 0;
				break;
				case -1:
				fprintf(stderr, "read_importation: get_line error on file %s\n", file);
				return 1;
				default:
				fprintf(stderr, "read_importation: unrecognized exit status of get_line on file %s\n", file);
				return 1;
				break;
			}
		}
		// read numbers in the txt file
		sscanf(line, "%lf", &import);
		// convert from %lf to %d
		param.import_daily[count] = round(import);

		line++;
		count++;
	}
	return 2;
} // int read_importation

int read_seasonality(char file[], char sep){

	char *line;
	int out_get_line = 2;
	FILE *fp;
	int count = 0;
	double seasonality = 0;
	if( !(fp = fopen(file, "r")) ){
		fprintf(stderr, "read_seasonality: error opening file %s for reading\n", file);
		return 1;
	}

	while(out_get_line >= 2){
		out_get_line = get_line(&line, fp);
		if(out_get_line < 3){
			switch(out_get_line){
				case 2:
				fprintf(stderr, "read_seasonality: line of file %s does not end in newline\n", file);
				break;
				case 1:
				fprintf(stderr, "read_seasonality: file %s contains an empty line\n", file);
				return 1;
				break;
				case 0:
				fclose(fp);
				return 0;
				break;
				case -1:
				fprintf(stderr, "read_seasonality: get_line error on file %s\n", file);
				return 1;
				default:
				fprintf(stderr, "read_seasonality: unrecognized exit status of get_line on file %s\n", file);
				return 1;
				break;
			}
		}
		// read numbers in the txt file
		sscanf(line, "%lf", &seasonality);
		// convert from %lf to %d
		param.SEASON_DAILY[count] = seasonality;

		line++;
		count++;
	}
	return 2;
} // int read_seasonality

int read_control_prev_H( char file[], char sep ){

	char *line;
	int out_get_line=2;
	FILE *fp;
	char string_who[100];
	char string_value[100];

	if( !(fp = fopen(file, "r")) )
	{
		fprintf(stderr,"read_control_prev_H: error opening file %s for reading\n",file);
		return 1;
	}

	while(out_get_line>=2)
	{
		out_get_line=get_line(&line,fp);
		if(out_get_line<3)
		{
			switch(out_get_line){
				case 2:
				fprintf(stderr,"read_control_prev_H: line of file %s does not end in newline\n",file);
				break;
				case 1:
				fprintf(stderr,"read_control_prev_H: file %s contains an empty line\n",file);
				return 1;
				break;
				case 0:
				fclose(fp);
				return 0;
				break;
				case -1:
				fprintf(stderr,"read_control_prev_H: get_line error on file %s\n", file);
				return 1;
				default:
				fprintf(stderr,"read_control_prev_H: unrecognized exit status of get_line on file %s\n",file);
				return 1;
				break;
			}
		}

		sscanf(line,"%s", string_who);
		line = (char *)strchr(line, sep);
		line++;
		sscanf(line,"%s", string_value);

		if( strcmp(string_who,"CONTROL_H_1"            )==0 ) param.CONTROL_H[0]             = atof(string_value);
		if( strcmp(string_who,"CONTROL_H_2"            )==0 ) param.CONTROL_H[1]             = atof(string_value);
		if( strcmp(string_who,"CONTROL_H_3"            )==0 ) param.CONTROL_H[2]             = atof(string_value);

		if( strcmp(string_who,"CONTROL_H_Rt_0"         )==0 ) param.CONTROL_H_Rt[0]          = atof(string_value);
		if( strcmp(string_who,"CONTROL_H_Rt_1"         )==0 ) param.CONTROL_H_Rt[1]          = atof(string_value);
		if( strcmp(string_who,"CONTROL_H_Rt_2"         )==0 ) param.CONTROL_H_Rt[2]          = atof(string_value);
		if( strcmp(string_who,"CONTROL_H_Rt_3"         )==0 ) param.CONTROL_H_Rt[3]          = atof(string_value);

	}
	return 2;
} // int read_control_prev_H

int read_relative_reopen(char file[], char sep){

	char *line;
	int out_get_line = 2;
	FILE *fp;
	int count = 0;
	double relative_reopen = 9999.;
	if( !(fp = fopen(file, "r")) ){
		fprintf(stderr, "read_relative_reopen: error opening file %s for reading\n", file);
		return 1;
	}

	while(out_get_line >= 2){
		out_get_line = get_line(&line, fp);
		if(out_get_line < 3){
			switch(out_get_line){
				case 2:
				fprintf(stderr, "read_relative_reopen: line of file %s does not end in newline\n", file);
				break;
				case 1:
				fprintf(stderr, "read_relative_reopen: file %s contains an empty line\n", file);
				return 1;
				break;
				case 0:
				fclose(fp);
				return 0;
				break;
				case -1:
				fprintf(stderr, "read_relative_reopen: get_line error on file %s\n", file);
				return 1;
				default:
				fprintf(stderr, "read_relative_reopen: unrecognized exit status of get_line on file %s\n", file);
				return 1;
				break;
			}
		}
		// read numbers in the txt file

		sscanf(line, "%lf", &relative_reopen);
		param.RELATIVE_TRANS_MUN_OPEN[count] = relative_reopen;

		line++;
		count++;
	}
	return 2;
} // int read_relative_reopen

int read_time( char file[], char sep ){

	char *line;
	int out_get_line=2;
	FILE *fp;
	char string_who[100];
	char string_value[100];

	if( !(fp = fopen(file, "r")) )
	{
		fprintf(stderr,"read_time: error opening file %s for reading\n",file);
		return 1;
	}

	while(out_get_line>=2)
	{
		out_get_line=get_line(&line,fp);
		if(out_get_line<3)
		{
			switch(out_get_line){
				case 2:
				fprintf(stderr,"read_time: line of file %s does not end in newline\n",file);
				break;
				case 1:
				fprintf(stderr,"read_time: file %s contains an empty line\n",file);
				return 1;
				break;
				case 0:
				fclose(fp);
				return 0;
				break;
				case -1:
				fprintf(stderr,"read_time: get_line error on file %s\n", file);
				return 1;
				default:
				fprintf(stderr,"read_time: unrecognized exit status of get_line on file %s\n",file);
				return 1;
				break;
			}
		}

		sscanf(line,"%s", string_who);
		line = (char *)strchr(line, sep);
		line++;
		sscanf(line,"%s", string_value);

		if( strcmp(string_who,"Year"         )==0 ) param.YEAR           = atof(string_value);
		if( strcmp(string_who,"Month"        )==0 ) param.MONTH          = atof(string_value);
		if( strcmp(string_who,"Date"         )==0 ) param.DAY            = atof(string_value);
		if( strcmp(string_who,"maxT"         )==0 ) param.MAXT           = atof(string_value);

	}
	return 2;
} // int read_time
