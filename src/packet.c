#include "packet.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

void
packet_stream_free (struct packet_stream *stream)
{
  free(stream->packets);
  free(stream->ticks);
  stream->packets = 0;
  stream->ticks = 0;
}

bool
packet_stream_ended (struct packet_stream stream)
{
  return stream.position >= stream.size;
}

void
packet_stream_tick (struct packet_stream *stream)
{
  ++stream->tick;
}

struct packet_array
packet_stream_get_packets (struct packet_stream *stream)
{
  struct packet_array array;
  array.packets = stream->packets + stream->position;
  array.size = 0;
  while (stream->position < stream->size
	 && stream->ticks[stream->position] < stream->tick)
    {
      ++stream->position;
      ++array.size;
    }
  return array;
}

void
packet_array_print (struct packet_array array)
{
  printf("Packets: <\n");
  for (unsigned int i = 0; i < array.size; ++i)
    {
      printf("\tVal: %u, Work: %u, Slack: %u\n",
	     array.packets[i].val,
	     array.packets[i].work,
	     array.packets[i].slack);
    }
  printf(">\n");
}

void
packet_stream_print_stats (struct packet_stream stream)
{
  float tot_work = 0;
  float tot_val = 0;
  float tot_slack = 0;
  float c = INFINITY;
  for (unsigned int i = 0; i < stream.size; ++i) {
    tot_work += stream.packets[i].work;
    tot_val += stream.packets[i].val;
    tot_slack += stream.packets[i].slack;
    c = fminf(c,
	      ((float) stream.packets[i].slack) / stream.packets[i].work);
  }
  printf("Stream statistics:\n");
  printf("\tAverage:\n\t\tWork = %f, Val = %f, Slack = %f\n",
	 tot_work / stream.size,
	 tot_val / stream.size,
	 tot_slack / stream.size);
  
  printf("\tTotal:\n\t\tWork = %f, Val = %f, Slack = %f\n",
	 tot_work,
	 tot_val,
	 tot_slack);
  
  printf("\tCongestion = %f, c = %f\n",
	 (stream.size / (float) stream.ticks[stream.size - 1]),
	 c);

}

void
packet_stream_print_stream (struct packet_stream stream)
{
  while (!packet_stream_ended(stream))
    {
      printf("Tick: %u\n", stream.tick);
      packet_array_print(packet_stream_get_packets(&stream));
      packet_stream_tick(&stream);
    }
}
