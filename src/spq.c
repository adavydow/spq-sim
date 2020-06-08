#include "spq.h"
#include "pq.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

// #define DEBUG_SPQ

struct processing_algo
spq_algo ()
{
  struct processing_algo algo;
  algo.data = malloc(sizeof(struct spq_data));
  struct spq_data *spqd = algo.data;
  spqd->buffer.packets = 0;
  spqd->buffer.size = 0;
  spqd->absize = 0;
  spqd->stack.packets = 0;
  spqd->stack.size = 0;
  spqd->istack.packets = 0;
  spqd->istack.size = 0;
  spqd->assize = 0;
  spqd->a = 1;
  spqd->b = 0;
  algo.arrival = spq_arrival;
  algo.processing = spq_processing;
  algo.free = spq_free;
  algo.buffer = 0;
  algo.max_buffer = 0;
  return algo;
}

void
spq_arrival (struct processing_algo *algo,
	     struct packet_array array)
{
  struct spq_data *spqd = (struct spq_data *)algo->data;
  unsigned int newsize = spqd->buffer.size + array.size;
  if (newsize >= spqd->absize)
    {
      spqd->absize = newsize * 2;
      spqd->buffer.packets =
	realloc(spqd->buffer.packets, spqd->absize * sizeof(struct packet));
    }
  for (unsigned int i = 0; i < array.size; ++i)
    {
      spqd->buffer.packets[spqd->buffer.size++] = array.packets[i];
    }
}

void
spq_processing (struct processing_algo *algo)
{
  struct spq_data *spqd = (struct spq_data *)algo->data;
  if (spqd->buffer.size > 0)
    {
      struct packet *best_packet = &spqd->buffer.packets[0];
      for (unsigned int i = 1; i < spqd->buffer.size; ++i)
	{
	  if (cmp_v_w(spqd->buffer.packets[i], *best_packet))
	    {
	      best_packet = &spqd->buffer.packets[i];
	    }
	}
      
    // a-preemption
      if (spqd->istack.size == 0 ||
	  best_packet->val *
	  spqd->istack.packets[spqd->istack.size - 1].work >
	  spqd->a * spqd->istack.packets[spqd->istack.size - 1].val *
	  best_packet->work)
	{
	  if (spqd->assize < spqd->istack.size + 1) {
	    spqd->assize = (spqd->istack.size + 1) * 2;
	    spqd->stack.packets =
	      realloc(spqd->stack.packets,
		      spqd->assize * sizeof(struct packet));
	    spqd->istack.packets =
	      realloc(spqd->istack.packets,
		      spqd->assize * sizeof(struct packet));	  
	  }
	  spqd->istack.packets[spqd->istack.size++] = *best_packet;
	  spqd->stack.packets[spqd->stack.size++] = *best_packet;
	  best_packet->work = 1; // to drop 
	  best_packet->slack = 1;// --//--
	}
    }
  
  if (spqd->stack.size > 0)
    {
    // work HOL
      if (--spqd->stack.packets[spqd->stack.size - 1].work == 0) {
	spqd->stack.packets[spqd->stack.size - 1].work = 1;  // to drop
	spqd->stack.packets[spqd->stack.size - 1].slack = 1; // --//--
	algo->val += spqd->stack.packets[spqd->stack.size - 1].val;
      }
    }
    // clean stack

  unsigned int j = 0;
  for (unsigned int i = 0; i < spqd->stack.size; ++i)
    {
      if (--spqd->stack.packets[i].slack >= spqd->stack.packets[i].work)
	{
	  spqd->stack.packets[j] = spqd->stack.packets[i];
	  spqd->istack.packets[j] = spqd->istack.packets[i];	    
	  ++j;
	}
    }
  spqd->stack.size = spqd->istack.size = j;
    
  // clean buffer
  j = 0;
  for (unsigned int i = 0; i < spqd->buffer.size; ++i)
    {
      // b-pessimism
      if (--spqd->buffer.packets[i].slack >= // TODO: --s or s--
	  (1 + spqd->b) * spqd->buffer.packets[i].work)
	{
	  spqd->buffer.packets[j] = spqd->buffer.packets[i];
	  ++j;
	}
    }
  spqd->buffer.size = j;

  algo->buffer = spqd->buffer.size + spqd->stack.size;
  algo->max_buffer = fmaxf(algo->max_buffer, algo->buffer);

#ifdef DEBUG_SPQ
  printf("buffer size: %d\n", algo->buffer);
  printf("buffer:\n");
  packet_array_print(spqd->buffer);
  printf("stack:\n");
  packet_array_print(spqd->stack);
  printf("stack:\n");
  packet_array_print(spqd->istack);
#endif
}

void
spq_free (struct processing_algo *algo)  
{
  struct spq_data *spqd = (struct spq_data *)algo->data;
  free(spqd->buffer.packets);
  free(spqd->stack.packets);
  free(spqd->istack.packets);
  free(spqd);
}
