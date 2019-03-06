/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

****************************************************************************/

#include <stdio.h>
#include <math.h>

#include "wnlib.h"
#include "wnsll.h"
#include "wnasrt.h"
#include "wnhash.h"
#include "wnrndd.h"
#include "wnswap.h"
#include "wnsqr.h"
#include "wnvect.h"

#include "wnconj.h"


#define NUM_VARS   10000
#define NUM_EQ     NUM_VARS
/*
#define NUM_EQ     96
*/

#define NUM_COEFS    4



typedef struct equation_type_struct
{
  int indexes[NUM_COEFS];
  double coefs[NUM_COEFS];
  double right_side;
} *equation_type;


local equation_type equation_array[NUM_EQ];
local wn_sll equation_list;
local int count;
local double save_correct_vect[NUM_VARS];



local bool index_repeated(indexes,i)

int indexes[NUM_COEFS];
int i;

{
  int j,index;

  index = indexes[i];

  for(j=0;j<i;++j)
  {
    if(indexes[j] == index)
    {
      return(TRUE);
    }
  }

  return(FALSE);
}


local void generate_random_equation(pequation)

equation_type *pequation;

{
  extern double wn_normal_distribution();
  int i;

  *pequation = wn_zalloc(sizeof(struct equation_type_struct));

  (*pequation)->right_side = wn_normal_distribution();
  /*
  (*pequation)->right_side = 0.0;
  */

  for(i=0;i<NUM_COEFS;++i)
  {
    ((*pequation)->coefs)[i] = wn_normal_distribution();

    do
    {
      ((*pequation)->indexes)[i] = wn_random_mod_int(NUM_VARS);
    } 
    while(index_repeated((*pequation)->indexes,i));
  }
}


local bool indexes_repeated(indexes)

int indexes[NUM_COEFS];

{
  int i,j;

  for(i=0;i<NUM_COEFS;++i)
  for(j=0;j<i;++j)
  {
    if(indexes[i] == indexes[j])
    {
      return(TRUE);
    }
  }

  return(FALSE);
}


local void random_swap_indexes()
{
  int count,e1,i1,e2,i2;
  equation_type eq1,eq2;

  for(count=0;count<10*NUM_EQ*NUM_COEFS;++count)
  {
    e1 = wn_random_mod_int(NUM_EQ);
    i1 = wn_random_mod_int(NUM_COEFS);

    e2 = wn_random_mod_int(NUM_EQ);
    i2 = wn_random_mod_int(NUM_COEFS);

    eq1 = equation_array[e1];
    eq2 = equation_array[e2];

    wn_assert(!(indexes_repeated(eq1->indexes)));
    wn_assert(!(indexes_repeated(eq2->indexes)));

    wn_swap((eq1->indexes)[i1],(eq2->indexes)[i2],int);

    if(indexes_repeated(eq1->indexes)||indexes_repeated(eq2->indexes))
    {
      wn_swap((eq1->indexes)[i1],(eq2->indexes)[i2],int);
    }
  }
}


local void generate_equations()
{
  int i,j,count;
  equation_type equation;

  equation_list = NULL;

  for(i=0;i<NUM_EQ;++i)
  {
    generate_random_equation(&equation);
    
    wn_sllins(&equation_list,equation);
    equation_array[i] = equation;
  }

#if 0
  count = 0;

  for(i=0;i<NUM_EQ;++i)
  {
    equation = equation_array[i];

    for(j=0;j<NUM_COEFS;++j)
    {
      (equation->indexes)[j] = count;

      ++count;
      if(count >= NUM_VARS)
      {
	count = 0;
      }
    }
  }

  random_swap_indexes();
#endif
}


local void print_equation(equation,vect)

equation_type equation;
double vect[NUM_VARS];

{
  double equation_diff();
  bool first_time;
  int var;

  first_time = TRUE;

  printf("%lf = ",equation->right_side);

  for(var=0;var<NUM_COEFS;++var)
  {
    if(first_time)
    {
      first_time = FALSE;
    }
    else
    {
      printf(" + ");
    }

    printf("%lf v%d",(equation->coefs)[var],(equation->indexes)[var]);
  }

  printf(";   diff = %lf",equation_diff(equation,vect));

  printf("\n");
}


local void print_equations(vect)

double vect[NUM_VARS];

{
  int i;

  printf("----------------------\n");

  for(i=0;i<NUM_EQ;++i)
  {
    print_equation(equation_array[i],vect);
  }

  printf("----------------------\n");
}


local double equation_diff(equation,vect)

equation_type equation;
double vect[NUM_VARS];

{
  double *coefs;
  int *indexes;
  double sum;
  int i;

  coefs = equation->coefs;
  indexes = equation->indexes;

  sum = 0.0;

  for(i=0;i<NUM_COEFS;++i)
  {
    sum += coefs[i]*vect[indexes[i]];
    /*
    sum = fprec(sum);
    */
  }

  sum -= equation->right_side;

  /*
  sum = fprec(sum);
  */

  return(sum);
}


local double function(vect)

double vect[NUM_VARS];

{
  wn_sll el;
  equation_type equation;
  double error,diff;

  error = 0.0;

#if 0
  el=equation_list;

  equation = (equation_type)(el->contents);

  diff = equation_diff(equation,vect);

  error += diff;

  for(el=equation_list->next;el!=NULL;el=el->next)
  {
    equation = (equation_type)(el->contents);

    diff = equation_diff(equation,vect);

    if(diff > 0.0)
    {
    error += diff*diff;
    }
  }
#endif

  for(el=equation_list;el!=NULL;el=el->next)
  {
    equation = (equation_type)(el->contents);

    diff = equation_diff(equation,vect);

    error += diff*diff;

    /*
    error = fprec(error);
    */
  }

  printf("f = %lg,norm2(vect) = %lg,norm2(vect-correct) = %lg\n",
	 error/NUM_EQ,
	 wn_norm2_vect(vect,NUM_VARS)/NUM_VARS,
	 wn_dist2_vect(vect,save_correct_vect,NUM_VARS)/NUM_VARS);
  /*
  printf("f = %lg,norm2(vect) = %lg\n",
	 error/NUM_EQ,
	 wn_norm2_vect(vect,NUM_VARS)/NUM_VARS);
  */

  return(error/NUM_EQ);
}


local void gradient(grad,vect)

double grad[NUM_VARS],vect[NUM_VARS];

{
  int i,index;
  wn_sll el;
  equation_type equation;
  double *coefs;
  int *indexes;
  double diff;

  for(i=0;i<NUM_VARS;++i)
  {
    grad[i] = 0.0;
  }

#if 0
  el=equation_list;
  equation = (equation_type)(el->contents);

  coefs = equation->coefs;
  indexes = equation->indexes;

  for(i=0;i<NUM_COEFS;++i)
  {
    index = indexes[i];

    grad[index] += coefs[i];
  }

  for(el=equation_list->next;el!=NULL;el=el->next)
  {
    equation = (equation_type)(el->contents);

    diff = equation_diff(equation,vect);

    if(diff > 0.0)
    {
    coefs = equation->coefs;
    indexes = equation->indexes;

    for(i=0;i<NUM_COEFS;++i)
    {
      index = indexes[i];

      grad[index] += 2.0*diff*coefs[i];
    }
    }
  }
#endif

  for(el=equation_list;el!=NULL;el=el->next)
  {
    equation = (equation_type)(el->contents);

    diff = equation_diff(equation,vect);

    coefs = equation->coefs;
    indexes = equation->indexes;

    for(i=0;i<NUM_COEFS;++i)
    {
      index = indexes[i];

      grad[index] += 2.0*diff*coefs[i]/NUM_EQ;

      /*
      grad[index] = fprec(grad[index]);
      */
    }
  }

  printf("iterations = %d,norm(g) = %lg\n",count,wn_norm_vect(grad,NUM_VARS));
  fflush(stdout);
  /*
  */
  ++count;
}


local void funcgrad(pval,grad,vect)

double *pval;
double grad[],vect[];

{
  *pval = function(vect);
  gradient(grad,vect);
}


local void update_right_sides(vect)

double vect[NUM_VARS];

{
  wn_sll el;
  equation_type equation;
  double diff;

  for(el=equation_list;el!=NULL;el=el->next)
  {
    equation = (equation_type)(el->contents);
    equation->right_side = 0.0;

    diff = equation_diff(equation,vect);

    equation->right_side = diff;
  }
}


#if 1
void main(argc,argv)

int argc;
char *argv[];

{
  extern double wn_normal_distribution();
  double vect[NUM_VARS];
  double val_min;
  int i,seed,code;

  if(argc != 2)
  {
    fprintf(stderr,"usage: examples <seed>\n");
    exit(-1);
  }

  /*
  sscanf(argv[1],"%d",&seed);
  wn_seed_random_number_generator(seed);
  */
  wn_seed_random_number_generator(wn_strhash(argv[1]));

  wn_gpmake("no_free");

  generate_equations();

  wn_zero_vect(vect,NUM_VARS);
  /*
  */
  for(i=0;i<NUM_VARS;++i)
  {
    vect[i] = wn_normal_distribution();
  }
  update_right_sides(vect);
  wn_copy_vect(save_correct_vect,vect,NUM_VARS);

  for(i=0;i<NUM_VARS;++i)
  {
    vect[i] = wn_normal_distribution();
  }

  count = 0;

  /*
  print_equations(vect);
  */
  val_min = function(vect);
  printf("objective = %lg\n",val_min);

  /*
  conj_direction_method(&code,&val_min,vect,NUM_VARS,(function),WN_IHUGE);
  */
  wn_conj_gradient_method(&code,&val_min,vect,NUM_VARS,(function),(gradient),WN_IHUGE);

  printf("code = %d\n",code);
  printf("objective = %lg\n",val_min);
  val_min = function(vect);
  printf("actual objective = %lg\n",val_min);
  /*
  wn_print_vect(vect,NUM_VARS);
  */

#if 0
  while(TRUE)
  {
  conj_gradient_method(&code,&val_min,vect,NUM_VARS,(function),(gradient),NUM_VARS);

  printf("---------------------------------------\n");
  printf("objective = %lf\n",val_min);
  val_min = function(vect);
  printf("actual objective = %lf\n",val_min);

  update_right_sides(vect);
  wn_blkzero(vect,NUM_VARS*sizeof(double));
  }
#endif

#if 0
  /*
  for(i=0;i<10;++i)
  */
  while(TRUE)
  {
  conj_gradient_method2(&code,&val_min,vect,NUM_VARS,(function),(gradient),NUM_VARS);

  val_min = function(vect);

  printf("objective = %lf\n",val_min);
  /*
  print_vect(vect,NUM_VARS);
  */
  }
#endif
}
#endif
