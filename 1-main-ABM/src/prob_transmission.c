#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <sqlite3.h>
#include <math.h>
#include "header.h"

extern Param param;

void prob_transmission( Prob_trans *pt ){

  int i;

  pt->H_P   = (double**) calloc( N_MUN , sizeof(double*) );
  pt->H_A   = (double**) calloc( N_MUN , sizeof(double*) );
  pt->H_S   = (double**) calloc( N_MUN , sizeof(double*) );
  pt->P0_P  = (double**) calloc( N_MUN , sizeof(double*) );
  pt->P0_A  = (double**) calloc( N_MUN , sizeof(double*) );
  pt->P0_S  = (double**) calloc( N_MUN , sizeof(double*) );
  pt->P1_P  = (double**) calloc( N_MUN , sizeof(double*) );
  pt->P1_A  = (double**) calloc( N_MUN , sizeof(double*) );
  pt->P1_S  = (double**) calloc( N_MUN , sizeof(double*) );

  for( int j=0; j < N_MUN; j++ ){
    pt->H_P[j]   = (double*) calloc( N_HS+1, sizeof(double) );
    pt->H_A[j]   = (double*) calloc( N_HS+1, sizeof(double) );
    pt->H_S[j]   = (double*) calloc( N_HS+1, sizeof(double) );
    pt->P0_P[j]  = (double*) calloc( N_PS+1, sizeof(double) );
    pt->P0_A[j]  = (double*) calloc( N_PS+1, sizeof(double) );
    pt->P0_S[j]  = (double*) calloc( N_PS+1, sizeof(double) );
    pt->P1_P[j]  = (double*) calloc( N_PS+1, sizeof(double) );
    pt->P1_A[j]  = (double*) calloc( N_PS+1, sizeof(double) );
    pt->P1_S[j]  = (double*) calloc( N_PS+1, sizeof(double) );
  }

  for( int j = 0; j < N_MUN; j++ ){

    for(i=1;i<=N_HS;i++){
      pt->H_P[j][i] = 1.-exp( -param.RELATIVE_INF_PRES*param.BETA_H*param.RELATIVE_TRANS_MUN[j]*param.DELTAT/i );
      pt->H_A[j][i] = 1.-exp( -param.RELATIVE_INF_AS  *param.BETA_H*param.RELATIVE_TRANS_MUN[j]*param.DELTAT/i );
      pt->H_S[j][i] = 1.-exp( -                        param.BETA_H*param.RELATIVE_TRANS_MUN[j]*param.DELTAT/i );
    }

    for(i=1;i<=N_PS;i++){
      pt->P0_P[j][i] = 1.-exp( -param.RELATIVE_INF_PRES*param.BETA_P0*param.RELATIVE_TRANS_MUN[j]*param.DELTAT/i );
      pt->P0_A[j][i] = 1.-exp( -param.RELATIVE_INF_AS  *param.BETA_P0*param.RELATIVE_TRANS_MUN[j]*param.DELTAT/i );
      pt->P0_S[j][i] = 1.-exp( -                        param.BETA_P0*param.RELATIVE_TRANS_MUN[j]*param.DELTAT/i );

      pt->P1_P[j][i] = 1.-exp( -param.RELATIVE_INF_PRES*param.BETA_P1*param.RELATIVE_TRANS_MUN[j]*param.DELTAT/i );
      pt->P1_A[j][i] = 1.-exp( -param.RELATIVE_INF_AS  *param.BETA_P1*param.RELATIVE_TRANS_MUN[j]*param.DELTAT/i );
      pt->P1_S[j][i] = 1.-exp( -                        param.BETA_P1*param.RELATIVE_TRANS_MUN[j]*param.DELTAT/i );
    }
  }
  return;
}
