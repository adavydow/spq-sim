#include "generator.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

struct packet_stream
gen_random (void *params)
{
  struct gen_random_params *p = params;
  assert(p->num > 0);
  assert(p->max_work > 0);
  assert(p->max_val > 0);
  assert(p->congestion > 0.001);
  // assert(p->max_slack >= p->c * p->max_work);
  assert(p->c_max >= p->c_min);

  struct packet_stream stream;
  stream.size = p->num;
  stream.packets = malloc(sizeof(struct packet) * p->num);
  stream.ticks = malloc(sizeof(unsigned int) * p->num);
  stream.val = 0;
  unsigned int tick = 0;
  for (int i = 0; i < p->num; ++i)
    {
      stream.packets[i].work = rand() % p->max_work + 1;
      stream.packets[i].val = rand() % p->max_val + 1;
      stream.val += stream.packets[i].val;
      unsigned int min_slack = stream.packets[i].work * p->c_min;
      assert (min_slack <= p->c_max * stream.packets[i].work);
      stream.packets[i].slack = min_slack +
	rand() % (p->c_max * stream.packets[i].work - min_slack + 1);
      while (rand() % ((unsigned int)((p->congestion + 1) * 10e4)) < 10e4)
	{
	  ++tick;
	}
      stream.ticks[i] = tick;
    }
  stream.tick = 0;
  stream.position = 0;
  return stream;
}


struct packet_stream
gen_sample (void *params)
{
  struct gen_sample_params *p = params;
  assert(p->num > 0);
  assert(p->max_work > 0);
  assert(p->max_val > 0);
  assert(p->congestion > 0.001);
  // assert(p->max_slack >= p->c * p->max_work);
  assert(p->c_max >= p->c_min);

  FILE *f = fopen(p->file, "r");
  int n = 0;
  for (int i = 0; i < p->num; ++i) {
    fscanf(f, "%d", &n);
  }
  double time_scale = p->num / (n * p->congestion);
  rewind(f);
  struct packet_stream stream;
  stream.size = p->num;
  stream.packets = malloc(sizeof(struct packet) * p->num);
  stream.ticks = malloc(sizeof(unsigned int) * p->num);
  stream.val = 0;
  unsigned int tick = 0;
  for (int i = 0; i < p->num; ++i)
    {
      stream.packets[i].work = rand() % p->max_work + 1;
      stream.packets[i].val = rand() % p->max_val + 1;
      stream.val += stream.packets[i].val;
      unsigned int min_slack = stream.packets[i].work * p->c_min;
      assert (min_slack <= p->c_max * stream.packets[i].work);
      stream.packets[i].slack = min_slack +
	rand() % (p->c_max * stream.packets[i].work - min_slack + 1);
      fscanf(f, "%d", &stream.ticks[i]);
      stream.ticks[i] *= time_scale;
    }
  stream.tick = 0;
  stream.position = 0;
  return stream;
}
