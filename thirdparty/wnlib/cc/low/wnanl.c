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

wn_measure_anneal_temperature(&temperature,pevaluate_random_mutation,iterations)

wn_get_anneal_statistics(&mean,&sdev,pevaluate_random_mutation,iterations)

void wn_anneal_linear_temperature(
	       pevaluate_random_mutation,paccept_mutation,preject_mutation,
               pcheckpoint,
               problem_size,stop_run_length,epochs_to_run,
	       start_temperature,fin_temperature)

void wn_anneal_with_sched(
	       pevaluate_random_mutation,paccept_mutation,preject_mutation,
               pcheckpoint,
               problem_size,stop_run_length,epochs_to_run,
	       ptemperature_function,start_temperature)

void wn_anneal_from_temperature(
	       pevaluate_random_mutation,paccept_mutation,preject_mutation,
               pcheckpoint,
               problem_size,stop_run_length,epochs_to_run,
	       start_temperature)

void wn_anneal(pevaluate_random_mutation,paccept_mutation,preject_mutation,
               pcheckpoint,
               problem_size,stop_run_length,epochs_to_run)

void wn_anneal_std_checkpoint_print()

**********************************************************************/

#include "wnlib.h"
#include "wnmax.h"
#include "wninr.h"
#include "wnio.h"
#include "wnsqr.h"



/* export these values */
int wn_anneal_epochs_to_run,wn_anneal_epochs_remaining;
double wn_anneal_temperature,
       wn_anneal_objective,
       wn_anneal_accept_rate;  



void wn_anneal_std_checkpoint_print()
{
  printf("------------------------------------\n");
  printf("%d epochs remaining out of %d\n",
	 wn_anneal_epochs_remaining,wn_anneal_epochs_to_run);
  printf("temperature = %lf\n",wn_anneal_temperature);
  printf("accept_rate = %lf\n",wn_anneal_accept_rate);
  printf("objective = %lf\n",wn_anneal_objective);

  fflush(stdout);
}


void wn_get_anneal_statistics(pmean,psdev,pevaluate_random_mutation,iterations)

double *pmean,*psdev;
double (*pevaluate_random_mutation)();
int iterations;

{
  extern double sqrt();
  int i;
  double sum,square_sum,delta;

  sum = square_sum = 0.0;

  for(i=0;i<iterations;++i)
  {
    delta = (*pevaluate_random_mutation)();

    sum += delta;
    square_sum += wn_square(delta);
  }

  *pmean = sum/iterations;
  *psdev = sqrt((square_sum-wn_square(sum)/iterations)/(iterations-1.0));
}


void wn_measure_anneal_temperature(ptemperature,
				   pevaluate_random_mutation,iterations)

double *ptemperature;
double (*pevaluate_random_mutation)();
int iterations;

{
  double mean,sdev;

  wn_get_anneal_statistics(&mean,&sdev,pevaluate_random_mutation,iterations);

  if(mean <= sdev/2.0)
  {
    mean = sdev/2.0;
  }

  *ptemperature = wn_square(sdev)/(2*mean);
}


void wn_anneal_linear_temperature(
	       pevaluate_random_mutation,paccept_mutation,preject_mutation,
               pcheckpoint,
               problem_size,stop_run_length,epochs_to_run,
	       start_temperature,fin_temperature)

double (*pevaluate_random_mutation)();
void (*paccept_mutation)(),(*preject_mutation)();
void (*pcheckpoint)();
int problem_size,stop_run_length,epochs_to_run;
double start_temperature,fin_temperature;

{
  extern double exp();
  double delta,prob,relative_time;
  int count,accept_count,since_checkpoint_count,since_accept_count;

  wn_anneal_epochs_to_run = epochs_to_run;
  wn_anneal_objective = 0.0;
  accept_count = 0;
  since_checkpoint_count = problem_size;
  since_accept_count = 0;

  for(wn_anneal_epochs_remaining=epochs_to_run;
      wn_anneal_epochs_remaining>1;
      --wn_anneal_epochs_remaining)
  for(count=0;count<problem_size;++count)
  {
    if(since_checkpoint_count >= problem_size)
    {
      relative_time = ((double)(wn_anneal_epochs_remaining))*
			    ((double)problem_size)
			     -count;
      if(epochs_to_run > 0)
      {
        relative_time /= ((double)problem_size)*((double)epochs_to_run);
      }
	  
      wn_anneal_temperature = relative_time*start_temperature+
			      (1.0-relative_time)*fin_temperature;
      wn_anneal_accept_rate = ((double)accept_count)/
				 ((double)since_checkpoint_count);

      (*pcheckpoint)();

      accept_count = 0;
      since_checkpoint_count = 0;
    }

    while(TRUE)
    {
      delta = (*pevaluate_random_mutation)();

      ++since_checkpoint_count;

      if(delta < 0.0)  /* mutation is improvement */
      {
	since_accept_count = 0;

        break;  /* accept mutation */
      }
      else if(delta == 0.0)
      {
	++since_accept_count;

        break;  /* accept mutation */
      }
      else 
      {
	relative_time = ((double)(wn_anneal_epochs_remaining))*
			    ((double)problem_size)
			    -count;
	if(epochs_to_run > 0)
	{
          relative_time /= ((double)problem_size)*((double)epochs_to_run);
        }
	  
        wn_anneal_temperature = relative_time*start_temperature+
			        (1.0-relative_time)*fin_temperature;

	if(wn_anneal_temperature > 0.0)
	{
          prob = exp(-delta/wn_anneal_temperature);

          if(wn_true_with_probability(prob))
          {
	    since_accept_count = 0;

            break;  /* accept mutation */
          }
        }
      }

      ++since_accept_count;

      (*preject_mutation)();

      if(since_accept_count >= stop_run_length)
      {
	goto exit;
      }
    } /* while(TRUE) */

    (*paccept_mutation)();

    wn_anneal_objective += delta;
    ++accept_count;
  }

exit:
  relative_time = ((double)(wn_anneal_epochs_remaining))*
			    ((double)problem_size)
			    -count;
  if(epochs_to_run > 0)
  {
    relative_time /= ((double)problem_size)*((double)epochs_to_run);
  }
	  
  wn_anneal_temperature = relative_time*start_temperature+
			   (1.0-relative_time)*fin_temperature;

  if(since_checkpoint_count > 0)
  {
    wn_anneal_accept_rate = ((double)accept_count)/
				 ((double)since_checkpoint_count);
  }
  else
  {
    wn_anneal_accept_rate = 0.0;
  }

  (*pcheckpoint)();
}


void wn_anneal_with_sched(
	       pevaluate_random_mutation,paccept_mutation,preject_mutation,
               pcheckpoint,
               problem_size,stop_run_length,epochs_to_run,
	       ptemperature_function,start_temperature)

double (*pevaluate_random_mutation)();
void (*paccept_mutation)(),(*preject_mutation)();
void (*pcheckpoint)();
int problem_size,stop_run_length,epochs_to_run;
double (*ptemperature_function)();
double start_temperature;

{
  extern double exp();
  double delta,prob,relative_time;
  int count,accept_count,since_checkpoint_count,since_accept_count;

  wn_anneal_epochs_to_run = epochs_to_run;
  wn_anneal_objective = 0.0;
  accept_count = 0;
  since_checkpoint_count = problem_size;
  since_accept_count = 0;

  for(wn_anneal_epochs_remaining=epochs_to_run;;--wn_anneal_epochs_remaining)
  for(count=0;count<problem_size;++count)
  {
    if(since_checkpoint_count >= problem_size)
    {
      relative_time = ((double)(wn_anneal_epochs_remaining))*
			    ((double)problem_size)
			     -count;
      if(relative_time < 0.0)
      {
	relative_time = 0.0;
      }
      if(epochs_to_run > 0)
      {
        relative_time /= ((double)problem_size)*((double)epochs_to_run);
      }
	  
      wn_anneal_temperature = start_temperature*
			           (ptemperature_function)(relative_time);
      wn_anneal_accept_rate = ((double)accept_count)/
				 ((double)since_checkpoint_count);

      (*pcheckpoint)();

      accept_count = 0;
      since_checkpoint_count = 0;
    }

    while(TRUE)
    {
      delta = (*pevaluate_random_mutation)();

      ++since_checkpoint_count;

      if(delta < 0.0)  /* mutation is improvement */
      {
	since_accept_count = 0;

        break;  /* accept mutation */
      }
      else if(delta == 0.0)
      {
	++since_accept_count;

        break;  /* accept mutation */
      }
      else 
      {
	relative_time = ((double)(wn_anneal_epochs_remaining))*
			    ((double)problem_size)
			    -count;
        if(relative_time < 0.0)
        {
  	  relative_time = 0.0;
        }
        if(epochs_to_run > 0)
        {
          relative_time /= ((double)problem_size)*((double)epochs_to_run);
        }
	  
        wn_anneal_temperature = start_temperature*
			           (ptemperature_function)(relative_time);

	if(wn_anneal_temperature > 0.0)
	{
          prob = exp(-delta/wn_anneal_temperature);

          if(wn_true_with_probability(prob))
          {
	    since_accept_count = 0;

            break;  /* accept mutation */
          }
        }
      }

      ++since_accept_count;

      (*preject_mutation)();

      if(since_accept_count >= stop_run_length)
      {
	goto exit;
      }
    } /* while(TRUE) */

    (*paccept_mutation)();

    wn_anneal_objective += delta;
    ++accept_count;
  }

exit:
  relative_time = ((double)(wn_anneal_epochs_remaining))*
			    ((double)problem_size)
			    -count;
  if(relative_time < 0.0)
  {
    relative_time = 0.0;
  }
  if(epochs_to_run > 0)
  {
    relative_time /= ((double)problem_size)*((double)epochs_to_run);
  }
	  
  wn_anneal_temperature = start_temperature*
		           (ptemperature_function)(relative_time);

  if(since_checkpoint_count > 0)
  {
    wn_anneal_accept_rate = ((double)accept_count)/
				 ((double)since_checkpoint_count);
  }
  else
  {
    wn_anneal_accept_rate = 0.0;
  }

  (*pcheckpoint)();
}


local double wn_usual_temperature_function(time)

double time;

{
  return(time);
}


void wn_anneal_from_temperature
	      (pevaluate_random_mutation,paccept_mutation,preject_mutation,
               pcheckpoint,
               problem_size,stop_run_length,epochs_to_run,
	       start_temperature)

double (*pevaluate_random_mutation)();
void (*paccept_mutation)(),(*preject_mutation)();
void (*pcheckpoint)();
int problem_size,stop_run_length,epochs_to_run;
double start_temperature;

{
  wn_assert(start_temperature >= 0.0);

  wn_anneal_with_sched(
	       pevaluate_random_mutation,paccept_mutation,preject_mutation,
               pcheckpoint,
               problem_size,stop_run_length,epochs_to_run,
	       (wn_usual_temperature_function),start_temperature);
}


void wn_anneal(pevaluate_random_mutation,paccept_mutation,preject_mutation,
               pcheckpoint,
               problem_size,stop_run_length,epochs_to_run)

double (*pevaluate_random_mutation)();
void (*paccept_mutation)(),(*preject_mutation)();
void (*pcheckpoint)();
int problem_size,stop_run_length,epochs_to_run;

{
  double samples,i;
  double start_temp,sum,sum2,delta;
  int count;

  wn_anneal_epochs_to_run = epochs_to_run;
  wn_anneal_epochs_remaining = epochs_to_run;
  wn_anneal_temperature = WN_FHUGE;
  wn_anneal_objective = 0.0;
  wn_anneal_accept_rate = 1.0;  

  samples = 100;
  if(samples < problem_size)
  {
    samples = problem_size;
  }
  if(samples < ((double)problem_size)*((double)epochs_to_run)*(1.0/30.0))
  {
    samples = ((double)problem_size)*((double)epochs_to_run)*(1.0/30.0);
  }

  count = problem_size;

  /* randomize problem */
  for(i=0.0;i<samples;i+=1.0)
  {
    if(count >= problem_size)
    {
      (*pcheckpoint)();

      count = 0;
    }
    
    delta = (*pevaluate_random_mutation)();
    (*paccept_mutation)();

    ++count;
  }

  /* randomize problem and measure standard deviation */
  sum = sum2 = 0.0;
  for(i=0.0;i<samples;i+=1.0)
  {
    if(count >= problem_size)
    {
      (*pcheckpoint)();

      count = 0;
    }
    
    delta = (*pevaluate_random_mutation)();
    sum += delta;
    sum2 += delta*delta;
    (*paccept_mutation)();

    ++count;
  }

  sum /= (double)samples;
  sum2 /= (double)samples;

  start_temp = sqrt(sum2-sum*sum);

  wn_anneal_from_temperature(
	       pevaluate_random_mutation,paccept_mutation,preject_mutation,
               pcheckpoint,
               problem_size,stop_run_length,epochs_to_run,
	       start_temp);
}




