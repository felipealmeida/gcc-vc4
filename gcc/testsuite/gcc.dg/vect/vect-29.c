/* { dg-require-effective-target vect_int } */

#include <stdarg.h>
#include "tree-vect.h"

#define N 128
#define OFF 3

/* unaligned load.  */

int main1 (int off)
{
  int i;
  int ia[N];
  int ib[N+OFF];

  for (i = 0; i < N+OFF; i++)
    {
      ib[i] = i;
    }

  for (i = 0; i < N; i++)
    {
      ia[i] = ib[i+off];
    }

  /* check results:  */
  for (i = 0; i < N; i++)
    {
      if (ia[i] != ib[i+off])
        abort ();
    }

  return 0;
}

int main (void)
{ 
  check_vect ();
  
  main1 (0); /* aligned */
  main1 (OFF); /* unaligned */
  return 0;
}

/* These are not yet vectorized on targets that do not model alignment-handling
   mechanisms.  */
/* { dg-final { scan-tree-dump-times "vectorized 1 loops" 1 "vect" { xfail i?86-*-* x86_64-*-* } } } */

