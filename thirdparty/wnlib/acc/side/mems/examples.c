/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

AUTHOR:

  Bill Chapman

****************************************************************************/

#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>

#include "wnlib.h"
#include "wnabs.h"
#include "wnasrt.h"
#include "wnmem.h"


/* return a random between 0 and 1024 */
int lo_maskrand(void)
{
  int result;

  do
  {
    result = rand();
    result &= 1024-1;
  } while (0 == result);

  return result;
} /* lo_maskrand */


#define LO_BLOCK_COUNT 10000
local ptr lo_blocks[LO_BLOCK_COUNT];
local int lo_allocated_block_sizes[LO_BLOCK_COUNT];
#define LO_SIZE_MAX 500
local int lo_alloc_count;


#define LO_LENGTH(a) (sizeof(a)/sizeof((a)[0]))

local int lo_sizes_to_alloc = 8;

void lo_alloc_a_block()
{
  static int sizes[] = {	5200, 5300, 5400, 5500, 5600,
  /**/				2500, 3000, 3100, 3252, 5140,
  /**/				 500,  700, 1000, 2000, 2400,
  /**/				 100,  200,  260,  301,  405,
  /**/				  96,   92,   88,   84,   80,
  /**/				  76,   72,   68,   64,   60};
  int size;
  int r;
  int i;

  if (lo_alloc_count == LO_BLOCK_COUNT)
  {
    return;
  }

  do
  {
    r = (unsigned) rand() >> 1;
    size = sizes[r % lo_sizes_to_alloc];
  } while (!size);

  do
  {
    r = (unsigned) rand() >> 1;
    i = r % LO_BLOCK_COUNT;
  } while (lo_blocks[i]);

  lo_blocks[i] = wn_alloc(size);
  lo_allocated_block_sizes[i] = size;
  ++ lo_alloc_count;
} /* lo_alloc_a_block */


void lo_free_a_block()
{
  int r;
  int i;

  if (lo_alloc_count == 0)
  {
    return;
  }

  do
  {
    r = (unsigned) rand() >> 1;
    i = r % LO_BLOCK_COUNT;
  } while (!lo_blocks[i]);

  wn_free(lo_blocks[i]);
  lo_blocks[i] = NULL;
  lo_allocated_block_sizes[i] = 0;
  -- lo_alloc_count;
} /* lo_free_a_block */


local clock_t lo_get_time(void)
{
  struct tms time_buf;
  int sts;

  sts = times(&time_buf);
  wn_assert(-1 != sts);
  return time_buf.tms_utime;
} /* lo_get_time */


void lo_memory_speed_trial(void)
{
  clock_t start_time, end_time;
  long int total_allocated, flab, total_flab;
  int i;

  wn_gpmake("general_free");
    wn_gplabel("speed test");

    for (i = 0;  i < LO_BLOCK_COUNT;  ++i)
    {
      lo_blocks[i] = NULL;
      lo_allocated_block_sizes[i] = 0;
    }
    lo_alloc_count = 0;

    for (i = 0;  i < LO_BLOCK_COUNT/2;  ++i)
    {
      lo_alloc_a_block();
    }

    start_time = lo_get_time();

    for (i = 0;  i < 1000000;  ++i)
    {
      if (1 & rand())
      {
	lo_alloc_a_block();
      }
      else
      {
	lo_free_a_block();
      }
    }

    end_time = lo_get_time();

    (void) printf("Speed trial:\n");
    (void) printf("  Elapsed time = %g\n", (end_time - start_time) / 100.0);
    (void) printf("  Memory used == %d, total wn memory == %ld\n",
    /**/			wn_curgp()->mem_used, wn_total_wn_memory());
    (void) printf("  Total memory in group: %ld\n",
    /**/				wn_total_memory_in_group(wn_curgp()));

    total_allocated =
    total_flab = 0;
    for (i = 0;  i < LO_BLOCK_COUNT;  ++i)
    {
      if (lo_blocks[i])
      {
	total_allocated += lo_allocated_block_sizes[i];
	flab = ((int *) lo_blocks[i])[-1] - lo_allocated_block_sizes[i];
	wn_assert(flab >= 0);
	total_flab += flab;
      }
    }
    (void) printf("  Total allocated: %ld, total flab: %ld\n",
    /**/				total_allocated, total_flab);
    (void) printf("  Total free: %ld\n\n",
    /**/		wn_amount_of_free_memory_in_group(wn_curgp()));

/*  wn_print_composition_of_big_blocks_free_list(wn_curgp()); */
  wn_gpfree();
} /* lo_memory_speed_trial */


int main(int argc, char *argv[])
{
  int i;
  bool arg_d_dump_segments = FALSE;
  ptr  arg_trap_address_address = NULL;
  ptr  arg_mem_in_group_address = (ptr) 1;
  bool arg_p_print_all_groups_mem = FALSE;
  ptr  arg_r_mem_to_read_from = (ptr) 1;
  ptr  arg_w_mem_to_write_to = (ptr) 1;
  ptr  arg_array_address = (ptr) 1;
  int  arg_array_index;
  bool arg_speed_test = FALSE;
  void *p;
  char c = '5', d, e;
  int sts;

  for (i = 1;  i < argc;  ++i)
  {
    if (!strcmp("-d", argv[i]))
    {
      arg_d_dump_segments = TRUE;
    }
    else if (!strcmp("-t", argv[i]))
    {
      ++i;
      sts = sscanf(argv[i], "%lx", (long *) &arg_trap_address_address);
      wn_assert(sts == 1);
      wn_gp_trap_address(arg_trap_address_address);
    }
    else if (!strcmp("-m", argv[i]))
    {
      ++i;
      sts = sscanf(argv[i], "%lx", (long *) &arg_mem_in_group_address);
      wn_assert(sts == 1);
    }
    else if (!strcmp("-p", argv[i]))
    {
      arg_p_print_all_groups_mem = TRUE;
    }
    else if (!strcmp("-r", argv[i]))
    {
      ++i;
      sts = sscanf(argv[i], "%lx", (long *) &arg_r_mem_to_read_from);
      wn_assert(sts == 1);
    }
    else if (!strcmp("-w", argv[i]))
    {
      ++i;
      sts = sscanf(argv[i], "%lx", (long *) &arg_w_mem_to_write_to);
      wn_assert(sts == 1);
    }
    else if (!strcmp("-arrayread", argv[i]))
    {
      ++i;
      sts = sscanf(argv[i], "%lx", (long *) &arg_array_address);
      wn_assert(sts == 1);

      ++i;
      sts = sscanf(argv[i], "%d", &arg_array_index);
      wn_assert(sts == 1);
    }
    else if (!strcmp("-speedtest", argv[i]))
    {
      arg_speed_test = TRUE;

      if (argc > i+1)
      {
	++i;
	sts = sscanf(argv[i], "%d", &lo_sizes_to_alloc);
	wn_assert(sts == 1);
	wn_assert(lo_sizes_to_alloc > 0);
	wn_assert(lo_sizes_to_alloc <= 30);
      }
    }
    else
    {
      fprintf(stderr, "Unrecognized arg \"%s\"\n", argv[i]);
      fprintf(stderr, "  Recognized args are:\n");
      fprintf(stderr, "    -d : dump segments\n");
      fprintf(stderr, "    -t <0x address> : trap address\n");
      fprintf(stderr, "    -m <0x address> : is address in group?\n");
      fprintf(stderr, "    -p : print all groups memory\n");
      fprintf(stderr, "    -r <0x address> : read from location\n");
      fprintf(stderr, "    -w <0x address> : write to location\n");
      fprintf(stderr, "    -arrayread <0x array start> <0x array index> :\n");
      fprintf(stderr, "           		array read\n");
      fprintf(stderr, "    -speedtest <# sizes to alloc>\n");
      return 1;
    }
  }

  wn_assert(i == argc);

  p = malloc(31);	/* make a lost segment for leak detection to notice */
  p = NULL;

  wn_gpmake("no_free");

    wn_gplabel("Example group");

    if (arg_d_dump_segments)
    {
      printf("ptr = %8x, size = %d\n", (long) wn_alloc(40), 40);
      printf("ptr = %8x, size = %d\n", (long) wn_alloc(100), 100);
      printf("ptr = %8x, size = %d\n", (long) wn_alloc(50), 50);
      printf("ptr = %8x, size = %d\n", (long) wn_alloc(7), 7);
      printf("ptr = %8x, size = %d\n", (long) wn_alloc(9), 9);
    }

    if (arg_mem_in_group_address != (ptr) 1)
    {
      printf("Testing if address 0x%x is in group\n", arg_mem_in_group_address);
      if (wn_mem_in_group(arg_mem_in_group_address, wn_curgp()))
      {
	printf("  It is.\n");
      }
      else
      {
	printf("  It isn't.\n");
      }
    }

    if (arg_p_print_all_groups_mem)
    {
      wn_print_all_groups_mem_used();
    }

    if ((ptr) 1 != arg_r_mem_to_read_from)
    {
      c = * (char *) arg_r_mem_to_read_from;
      d = c + 5;
      e = d - 5;

      if (e)
      {
	printf("e is not zero\n");
      }
      else
      {
	printf("e is zero\n");
      }
    }

    if ((ptr) 1 != arg_w_mem_to_write_to)
    {
      * (char *) arg_w_mem_to_write_to = c;
    }

    if ((ptr) 1 != arg_array_address)
    {
      c = ((char *) arg_array_address)[arg_array_index];
      d = c + 5;
      e = d - 5;

      if (e)
      {
	printf("e is not zero\n");
      }
      else
      {
	printf("e is zero\n");
      }
    }

    if (arg_speed_test)
    {
      lo_memory_speed_trial();
    }

  wn_gpfree();

  return(0);
} /* main */
