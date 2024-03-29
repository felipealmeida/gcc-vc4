/* Machine description for AArch64 architecture.
   Copyright (C) 2012-2013 Free Software Foundation, Inc.
   Contributed by ARM Ltd.

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GCC is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING3.  If not see
   <http://www.gnu.org/licenses/>.  */

void
__aarch64_sync_cache_range (const void *base, const void *end)
{
  unsigned icache_lsize;
  unsigned dcache_lsize;
  static unsigned int cache_info = 0;
  const char *address;

  if (! cache_info)
    /* CTR_EL0 [3:0] contains log2 of icache line size in words.
       CTR_EL0 [19:16] contains log2 of dcache line size in words.  */
    asm volatile ("mrs\t%0, ctr_el0":"=r" (cache_info));

  icache_lsize = 4 << (cache_info & 0xF);
  dcache_lsize = 4 << ((cache_info >> 16) & 0xF);

  /* Loop over the address range, clearing one cache line at once.
     Data cache must be flushed to unification first to make sure the
     instruction cache fetches the updated data.  'end' is exclusive,
     as per the GNU definition of __clear_cache.  */

  /* Make the start address of the loop cache aligned.  */
  address = (const char*) ((__UINTPTR_TYPE__) base
			   & ~ (__UINTPTR_TYPE__) (dcache_lsize - 1));

  for (address; address < (const char *) end; address += dcache_lsize)
    asm volatile ("dc\tcvau, %0"
		  :
		  : "r" (address)
		  : "memory");

  asm volatile ("dsb\tish" : : : "memory");

  /* Make the start address of the loop cache aligned.  */
  address = (const char*) ((__UINTPTR_TYPE__) base
			   & ~ (__UINTPTR_TYPE__) (icache_lsize - 1));

  for (address; address < (const char *) end; address += icache_lsize)
    asm volatile ("ic\tivau, %0"
		  :
		  : "r" (address)
		  : "memory");

  asm volatile ("dsb\tish; isb" : : : "memory");
}
