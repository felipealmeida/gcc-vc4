/* { dg-require-effective-target vect_int } */

#include <stdarg.h>
#include "tree-vect.h"

#define N 128

/* unaligned load.  */

int main1 ()
{
  int i;
  int ia[N];
  int ib[N+1];

  for (i=0; i < N; i++)
    {
      ib[i] = i;
    }

  for (i = 1; i <= N; i++)
    {
      ia[i-1] = ib[i];
    }

  /* check results:  */
  for (i = 1; i <= N; i++)
    {
      if (ia[i-1] != ib[i])
        abort ();
    }

  return 0;
}

int main (void)
{ 
  check_vect ();
  
  return main1 ();
}

/* These are not yet vectorized on targets that do not model alignment-handling
   mechanisms.  */
/* { dg-final { scan-tree-dump-times "vectorized 1 loops" 1 "vect" { xfail i?86-*-* x86_64-*-* } } } */

