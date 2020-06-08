#ifndef SPQ_SIM_PACKET
#define SPQ_SIM_PACKET
#include <stdbool.h>

struct packet
{
  unsigned int work, val, slack;
};

struct packet_stream
{
  unsigned int size, position, tick, val;
  struct packet *packets;
  unsigned int *ticks;
};

struct packet_array
{
  struct packet *packets;
  unsigned int size;
};

struct packet_stream
gen_random_packets (unsigned int num,
		    unsigned int max_work,
		    unsigned int max_val,
		    unsigned int max_slack,
		    unsigned int c,
		    unsigned int congestion);

void
packet_stream_free (struct packet_stream *stream);

bool
packet_stream_ended (struct packet_stream stream);

void
packet_stream_tick (struct packet_stream *stream);

struct packet_array
packet_stream_get_packets (struct packet_stream *stream);

void
packet_array_print (struct packet_array array);

void
packet_stream_print_stats (struct packet_stream stream);

void
packet_stream_print_stream (struct packet_stream stream);
#endif // SPQ_SIM_PACKET
