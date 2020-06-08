#ifndef SPQ_SIM_GENERATOR
#define SPQ_SIM_GENERATOR

#include "packet.h"

struct gen_random_params {
  unsigned int num;
  unsigned int max_work;
  unsigned int max_val;
  unsigned int c_max;
  unsigned int c_min;
  float congestion;
};

struct gen_sample_params {
  unsigned int num;
  unsigned int max_work;
  unsigned int max_val;
  unsigned int c_max;
  unsigned int c_min;
  char *file;
  float congestion;
};


struct packet_stream
gen_random (void *params);

#endif // SPQ_SIM_GENERATOR
