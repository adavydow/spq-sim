#include "pq.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

struct pq_data
{
  struct packet_array buffer;
  unsigned int asize;
  bool (*greater) (struct packet p1, struct packet p2);
};


bool cmp_v(struct packet p1, struct packet p2)
{
  return p1.val > p2.val;
}

bool cmp_v_w(struct packet p1, struct packet p2)
{
  return p1.val * p2.work > p2.val * p1.work;
}

bool cmp_w(struct packet p1, struct packet p2)
{
  return p2.work > p1.work;
}

bool cmp_s(struct packet p1, struct packet p2)
{
  return p2.slack > p1.slack;
}

bool cmp_v_s(struct packet p1, struct packet p2)
{
  return p1.val * p2.slack > p2.val * p1.slack;
}


struct processing_algo
pq_algo (bool (*greater) (struct packet p1, struct packet p2))
{
  struct processing_algo algo;
  algo.data = malloc(sizeof(struct pq_data));
  struct pq_data *pqd = (struct pq_data *)algo.data;
  pqd->buffer.packets = 0;
  pqd->buffer.size = 0;
  pqd->asize = 0;
  pqd->greater = greater;
  algo.arrival = pq_arrival;
  algo.processing = pq_processing;
  algo.free = pq_free;
  algo.val = 0;
  algo.buffer = 0;
  algo.max_buffer = 0;
  return algo;
}


void
pq_arrival (struct processing_algo *algo,
	    struct packet_array array)
{
  struct pq_data *pqd = (struct pq_data *)algo->data;
  unsigned int newsize = pqd->buffer.size + array.size;
  if (newsize >= pqd->asize)
    {
      pqd->asize = newsize * 2;
      pqd->buffer.packets =
	realloc(pqd->buffer.packets, pqd->asize * sizeof(struct packet));
    }
  for (unsigned int i = 0; i < array.size; ++i)
    {
      pqd->buffer.packets[pqd->buffer.size++] = array.packets[i];
    }
}

void
pq_processing (struct processing_algo *algo)
{
  struct pq_data *pqd = (struct pq_data *)algo->data;
  if (pqd->buffer.size > 0) {
    struct packet *best_packet = &pqd->buffer.packets[0];
    for (unsigned int i = 1; i < pqd->buffer.size; ++i)
      {
	if (pqd->greater(pqd->buffer.packets[i], *best_packet))
	  {
	    best_packet = &pqd->buffer.packets[i];
	  }
      }
    
    if (--best_packet->work == 0) {
      best_packet->work = 1; // to drop 
      best_packet->slack = 1;// --//--
      algo->val += best_packet->val;
    }

    unsigned int j = 0;
    for (unsigned int i = 0; i < pqd->buffer.size; ++i)
      {
	if (--pqd->buffer.packets[i].slack >= pqd->buffer.packets[i].work)
	  {
	    pqd->buffer.packets[j] = pqd->buffer.packets[i];
	    ++j;
	  }
      }
    pqd->buffer.size = j;
  }

  algo->buffer = pqd->buffer.size;
  algo->max_buffer = fmaxf(algo->max_buffer, algo->buffer);
}

void
pq_free (struct processing_algo *algo)  
{
  struct pq_data *pqd = (struct pq_data *)algo->data;
  free(pqd->buffer.packets);
  free(pqd);
}
