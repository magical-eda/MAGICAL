/*
 *  Setup default values of the parameters.
 */

#include <stdio.h>
#include "declarations.h"

void initparams(params,pprintlevel)
     struct paramstruc *params;
     int *pprintlevel;
{
  FILE *paramfile;

  paramfile=fopen("param.csdp","r");
  if (paramfile == NULL)
    {
      params->axtol=1.0e-8;
      params->atytol=1.0e-8;
      params->objtol=1.0e-8;
      params->pinftol=1.0e8;
      params->dinftol=1.0e8;
      params->maxiter=100;
      params->minstepfrac=0.90;
      params->maxstepfrac=0.97;
      params->minstepp=1.0e-8;
      params->minstepd=1.0e-8;
      params->usexzgap=1;
      params->tweakgap=0;
      params->affine=0;
      params->perturbobj=1;
      params->fastmode=0;
      *pprintlevel=1;
    }
  else
    {
      fscanf(paramfile,"%*[^=]%*c%lf",&(params->axtol));
      fscanf(paramfile,"%*[^=]%*c%lf",&(params->atytol));
      fscanf(paramfile,"%*[^=]%*c%lf",&(params->objtol));
      fscanf(paramfile,"%*[^=]%*c%lf",&(params->pinftol));
      fscanf(paramfile,"%*[^=]%*c%lf",&(params->dinftol));
      fscanf(paramfile,"%*[^=]%*c%d",&(params->maxiter));
      fscanf(paramfile,"%*[^=]%*c%lf",&(params->minstepfrac));
      fscanf(paramfile,"%*[^=]%*c%lf",&(params->maxstepfrac));
      fscanf(paramfile,"%*[^=]%*c%lf",&(params->minstepp));
      fscanf(paramfile,"%*[^=]%*c%lf",&(params->minstepd));
      fscanf(paramfile,"%*[^=]%*c%d",&(params->usexzgap));
      fscanf(paramfile,"%*[^=]%*c%d",&(params->tweakgap));
      fscanf(paramfile,"%*[^=]%*c%d",&(params->affine));
      fscanf(paramfile,"%*[^=]%*c%d",pprintlevel);
      fscanf(paramfile,"%*[^=]%*c%lf",&(params->perturbobj));
      fscanf(paramfile,"%*[^=]%*c%d",&(params->fastmode));
      fclose(paramfile);
    };

  if (*pprintlevel >= 3)
    {
      printf("params->axtol is %e \n",params->axtol);
      printf("params->atytol is %e \n",params->atytol);
      printf("params->objtol is %e \n",params->objtol);
      printf("params->pinftol is %e \n",params->pinftol);
      printf("params->dinftol is %e \n",params->dinftol);
      printf("params->maxiter is %d \n",params->maxiter);
      printf("params->minstepfrac is %e \n",params->minstepfrac);
      printf("params->maxstepfrac is %e \n",params->maxstepfrac);
      printf("params->minstepp is %e \n",params->minstepp);
      printf("params->minstepd is %e \n",params->minstepd);
      printf("params->usexzgap is %d \n",params->usexzgap);
      printf("params->tweakgap is %d \n",params->tweakgap);
      printf("params->affine is %d \n",params->affine);
      printf("params->printlevel is %d \n",*pprintlevel);
      printf("params->perturbobj is %e \n",params->perturbobj);
      printf("params->fastmode is %d \n",params->fastmode);
    };


}
