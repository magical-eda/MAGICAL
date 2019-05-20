/*

  This include file contains declarations for a number of parameters that 
  can affect the performance of CSDP.  You can adjust these parameters by 
 
    1. #include "parameters.h" in your code.
    2. Declare struct paramstruc params;
    3. Call init_params(params); to get default values.
    4. Change the value of the parameter that you're interested in.

  */

struct paramstruc {
  double axtol;
  double atytol;
  double objtol;
  double pinftol;
  double dinftol;
  int maxiter;
  double minstepfrac;
  double maxstepfrac;
  double minstepp;
  double minstepd;
  int usexzgap;
  int tweakgap;
  int affine;
  double perturbobj;
  int fastmode;
};






