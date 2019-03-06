/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

****************************************************************************/

/**********************************************************************

wn_anneal(pevaluate_random_mutation,
	  psave_mutation,paccept_saved_mutation,
	  iterations)

wn_anneal_from_start_temperature(pevaluate_random_mutation,
	                         psave_mutation,paccept_saved_mutation,
	                         iterations,
				 start_temperature)

wn_measure_temperature(&temperature,pevaluate_random_mutation,iterations)

wn_bake_at_temperature(pevaluate_random_mutation,
	               psave_mutation,paccept_saved_mutation,
	               iterations,
		       temperature)

**********************************************************************/

#include "wnlib.h"
#include "wnmax.h"
#include "wninr.h"
#include "wnio.h"



double wn_temperature;  /* export this value */



wn_anneal(pevaluate_random_mutation,
	  psave_mutation,paccept_saved_mutation,
          iterations)

double (*pevaluate_random_mutation)();
void (*psave_mutation)(),(*paccept_saved_mutation)();
double iterations;

{
  extern double sqrt(),wn_floor();
  double start_temperature,anneal_iterations,measure_iterations;

  measure_iterations = wn_floor(sqrt(iterations));
  anneal_iterations = iterations-measure_iterations;

  wn_measure_temperature(&start_temperature,
			 pevaluate_random_mutation,measure_iterations);

  wn_anneal_from_start_temperature(pevaluate_random_mutation,
	                           psave_mutation,paccept_saved_mutation,
	                           anneal_iterations,
				   start_temperature);
}



wn_measure_temperature(ptemperature,pevaluate_random_mutation,iterations)

double *ptemperature;
double (*pevaluate_random_mutation)();
double iterations;

{
  extern double log();
  double i,improve_count,decline_count,delta,
	 improve_fraction,decline_fraction;

  wn_assert(iterations > 0.0);

  improve_count = decline_count = 0.0;

  for(i=0.0;i<iterations;i+=1.0)
  {
    delta = (*pevaluate_random_mutation)();
      
    if(delta < 0.0)
    {
      improve_count+=1.0;
    }
    else if(delta > 0.0)
    {
      decline_count+=1.0;
    }
    /* delta == 0.0 is not counted */
  }

  improve_fraction = improve_count/iterations;
  decline_fraction = decline_count/iterations;

  if(
      (decline_fraction == 0.0)
	||
      (improve_fraction >= decline_fraction)
    )
  {
    *ptemperature = 1.0;
  }
  else if(improve_fraction == 0.0)
  {
    *ptemperature = 0.0;
  }
  else
  {
    *ptemperature = log(decline_fraction)/
	  log(improve_fraction/(1.0+improve_fraction-decline_fraction));

    wn_assert(wn_inrange(0.0,<=,*ptemperature,<=,1.0));
  }
}



wn_anneal_from_start_temperature(pevaluate_random_mutation,
	                         psave_mutation,paccept_saved_mutation,
	                         iterations,
				 start_temperature)

double (*pevaluate_random_mutation)();
void (*psave_mutation)(),(*paccept_saved_mutation)();
double iterations,start_temperature;

{
  double k,delta,best_delta,sample_size,i;

  k = start_temperature/iterations;

  while(iterations > 0.0)
  {
    wn_temperature = k*iterations;
    compute_sample_size(&sample_size,wn_temperature);

    best_delta = WN_FHUGE;

    for(i=0.0;i<sample_size;i+=1.0)
    {
      iterations-=1.0;

      delta = (*pevaluate_random_mutation)();

      if(delta < best_delta)
      {
        (*psave_mutation)();

	best_delta = delta;

	if(best_delta <= 0.0)
	{
	  break;
	}
      }

      if(iterations <= 0.0)
      {
	return;
      }
    }

    (*paccept_saved_mutation)();
  }
}



local compute_sample_size(psample_size,temperature)

double *psample_size,temperature;

{
  extern double wn_floor();
  double run_len,int_part,float_part;

  if(temperature == 0.0)
  {
    *psample_size = WN_FHUGE;
  }
  else
  {
    run_len = 1.0/temperature;

    int_part = wn_floor(run_len);
    float_part = run_len-int_part;

    if(wn_true_with_probability(float_part))
    {
      *psample_size = int_part+1.0;
    }
    else
    {
      *psample_size = int_part;
    }
  }
}



wn_bake_at_temperature(pevaluate_random_mutation,
	               psave_mutation,paccept_saved_mutation,
	               iterations,
		       temperature)

double (*pevaluate_random_mutation)();
void (*psave_mutation)(),(*paccept_saved_mutation)();
double iterations,temperature;

{
  double delta,best_delta,sample_size,i;

  wn_temperature = temperature;

  while(iterations > 0.0)
  {
    compute_sample_size(&sample_size,temperature);

    best_delta = WN_FHUGE;

    for(i=0.0;i<sample_size;i+=1.0)
    {
      iterations-=1.0;

      delta = (*pevaluate_random_mutation)();

      if(delta < best_delta)
      {
        (*psave_mutation)();

	best_delta = delta;

	if(best_delta <= 0.0)
	{
	  break;
	}
      }

      if(iterations <= 0.0)
      {
	return;
      }
    }

    (*paccept_saved_mutation)();
  }
}



