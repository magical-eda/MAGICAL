/*
 * qreig.  This routine computes the eigenvalues of a symmetric tridiagonal
 * matrix using Algorith 464 from CALGO. 
 *
 */

#include <math.h>
#define EPS 1.0e-6

void qreig(n,d,e2)
     int n;
     double *d;
     double *e2;
{
  int i,k,m;
  double b,b2,f,g,h,p2,r2,s2;

  f=0.0;
  b2=0.0;
  b=0.0;
  e2[n]=0.0;

  for (k=1; k<=n; k++)
    {
      h=EPS*EPS*(d[k]*d[k]+e2[k]);
      if (b2 < h)
	{
	  b=sqrt(h);
	  b2=h;
	};

      for (m=k; m<=n; m++)
	{
	  if (e2[m] <= b2) goto cont1;
	};

    cont1: 

      if (m==k) goto root;

    nextit:
      g=d[k]; 
      p2=sqrt(e2[k]);
      h=(d[k+1]-g)/(2.0*p2);
      r2=sqrt(h*h+1.0);
      if (h < 0.0) 
	{
	  h=p2/(h-r2);
	}
      else
	{
	  h=p2/(h+r2);
	};
      d[k]=h;
      h=g-h;
      f=f+h;
      for (i=k+1; i<=n; i++)
	{
	  d[i]=d[i]-h;
	};
      g=d[m];
      if (g==0.0) g=b;
      h=g;
      s2=0.0;
      for (i=m-1; i>=k; i=i-1)
	{
	  p2=g*h;
	  r2=p2+e2[i];
	  e2[i+1]=s2*r2;
	  s2=e2[i]/r2;
	  d[i+1]=h+s2*(h+d[i]);
	  g=d[i]-e2[i]/g;
	  if (g==0.0) g=b;
	  h=g*p2/r2;
	}; /* end for i */

      e2[k]=s2*g*h;
      d[k]=h;
      if (e2[k]>b2) goto nextit;
    root:
      h=d[k]+f;
      for (i=k; i>=2; i=i-1)
	{
	  if (h<d[i-1])
	    {
	      d[i]=d[i-1];
	    }
	  else
	    {
	      goto cont2;
	    };
	}; /* end i */
      i=1;

    cont2:

      d[i]=h;

    }; /* end k */
}
