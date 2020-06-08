#ifndef SPQ_SIM_ALGO
#define SPQ_SIM_ALGO

#include "packet.h"

struct processing_algo
{
  void *data;
  void (*arrival) (struct processing_algo *algo,
		   struct packet_array array);
  void (*processing) (struct processing_algo *algo);
  void (*free) (struct processing_algo *algo);
  unsigned int val;
  unsigned int max_buffer;
  unsigned int buffer;
};

#endif //SPQ_SIM_ALGO
