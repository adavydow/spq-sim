#ifndef SPQ_SIM_PQ
#define SPQ_SIM_PQ

#include "packet.h"
#include "algo.h"

bool cmp_v(struct packet p1, struct packet p2);

bool cmp_v_w(struct packet p1, struct packet p2);

bool cmp_w(struct packet p1, struct packet p2);

bool cmp_s(struct packet p1, struct packet p2);

bool cmp_v_s(struct packet p1, struct packet p2);

struct processing_algo
pq_algo (bool (*greater) (struct packet p1, struct packet p2));

void
pq_arrival (struct processing_algo *algo,
	    struct packet_array array);

void
pq_processing (struct processing_algo *algo);

void
pq_free (struct processing_algo *algo);

#endif // SPQ_SIM_PQ
