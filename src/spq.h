#ifndef SPQ_SIM_SPQ
#define SPQ_SIM_SPQ

#include "packet.h"
#include "algo.h"

struct spq_data
{
  struct packet_array buffer;
  unsigned int absize;
  struct packet_array stack;
  struct packet_array istack; // same as stack, but initial slacks and works
  unsigned int assize;
  float a;
  float b;
};

struct processing_algo
spq_algo ();

void
spq_arrival (struct processing_algo *algo,
	     struct packet_array array);

void
spq_processing (struct processing_algo *algo);

void
spq_free (struct processing_algo *algo);

#endif // SPQ_SIM_SPQ
