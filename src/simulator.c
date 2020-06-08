#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>
#include <argp.h>
#include <string.h>
#include "packet.h"
#include "generator.h"
#include "pq.h"
#include "spq.h"

// Argp keys
#define O_STREAM 301
#define O_STATS 302
#define O_ALGSTATS 303
#define O_VALUE 'v'
#define O_BUFFER 'b'
#define O_SEED 's'
#define O_FINISH 'f'
#define O_GEN 304
#define O_ALG 305

// Param states
#define PS_ALG 0
#define PS_DONE 1
#define PS_GEN 2
#define PS_RND_GEN 3
#define PS_SPQ_ALG 4

static struct
{
  bool print_stream;
  bool print_stats;
  bool print_algstats;
  bool print_value;
  bool print_buffer;
  bool finish_stream;
  struct packet_stream (* gen) (void *params);
  void *gen_params;
  struct processing_algo algo;
} state;

static struct argp_option options[] =
  {
    {"stream", O_STREAM, 0, 0,
     "Print generated stream"},
    {"stats", O_STATS, 0, 0,
     "Print stream statistics"},
    {"seed", O_SEED, "SEED", 0,
     "Random seed for a generator"},
    {"algstats", O_ALGSTATS, 0, 0,
     "Print algorithm performance statistics"},
    {"value", O_VALUE, 0, 0,
     "Print total value obtained by the algorithm"},
    {"buffer", O_BUFFER, 0, 0,
     "Print maximal buffer usage of the algorithm"},
    {"finish", O_FINISH, 0, 0,
     "Contininue processing buffered packets when stream ended."},
    {"generators", O_GEN, 0, 0,
     "List generators."},
    {"algo", O_ALG, 0, 0,
     "List algorithms."},
    {0}
  };

struct st_input {
  unsigned int state;
  int p_num;
};

static int
parse (int key, char *arg, struct argp_state *st)
{
  int *param_state = &((struct st_input*)st->input)->state;
  switch (key)
    {
    case O_STREAM: state.print_stream = true;
      break;
    case O_GEN:
      printf("random num max_val max_work c_max c_min congestion\n");
      exit(EXIT_SUCCESS);
      break;
    case O_ALG:
      printf("pq_v\n"
	     "pq_v_w\n"
	     "pq_w\n"
	     "pq_s\n"
	     "pq_v_s\n"
	     "spq a b\n");
      exit(EXIT_SUCCESS);
      break;     
    case O_STATS: state.print_stats = true;
      break;
    case O_SEED: srand(atoi(arg));
      break;
    case O_ALGSTATS: state.print_algstats = true;
      break;
    case O_VALUE: state.print_value = true;
      break;
    case O_BUFFER: state.print_buffer = true;
      break;
    case O_FINISH: state.finish_stream = true;
      break;
    case ARGP_KEY_ARG:
      switch (*param_state)
	{
	case PS_GEN:
	  if (strcmp(arg, "random") == 0)
	    {
	      *param_state = PS_RND_GEN;
	      state.gen_params = malloc(sizeof(struct gen_random_params));
	      state.gen = gen_random;
	      ((struct st_input*)st->input)->p_num = 0;
	    }
	  break;
	case PS_RND_GEN:
	  {
	    struct gen_random_params *params = state.gen_params;;
	    switch (((struct st_input*)st->input)->p_num++)
	      {
	      case 0: params->num = atoi(arg);
		break;
	      case 1: params->max_val = atoi(arg);
		break;
	      case 2: params->max_work = atoi(arg);
		break;
	      case 3: params->c_max = atoi(arg);
		break;
	      case 4: params->c_min = atoi(arg);
		break;
	      case 5: params->congestion = atof(arg);
		*param_state = PS_ALG;
		((struct st_input*)st->input)->p_num = 0;
		break;		
	      }
	  }
	  break;
	case PS_SPQ_ALG:
	  {
	    struct gen_random_params *params = state.gen_params;;
	    struct spq_data *spqd = state.algo.data;
	    switch (((struct st_input*)st->input)->p_num++)
	      {
	      case 0: spqd->a = atof(arg);
		break;
	      case 1: spqd->b = atof(arg);
		*param_state = PS_DONE;
		((struct st_input*)st->input)->p_num = 0;
		break;		
	      }
	  }
	  break;
	case PS_ALG:
	  if (strcmp(arg, "pq_v") == 0)
	    {
	      state.algo = pq_algo(cmp_v);
	    }
	  else if (strcmp(arg, "pq_v_w") == 0)
	    {
	      state.algo = pq_algo(cmp_v_w);
	    }
	  else if (strcmp(arg, "pq_w") == 0)
	    {
	      state.algo = pq_algo(cmp_w);
	    }
	  else if (strcmp(arg, "pq_s") == 0)
	    {
	      state.algo = pq_algo(cmp_s);
	    }
	  else if (strcmp(arg, "pq_v_s") == 0)
	    {
	      state.algo = pq_algo(cmp_v_s);
	    }
	  else if (strcmp(arg, "spq") == 0)
	    {
	      *param_state = PS_SPQ_ALG;
	      state.algo = spq_algo();
	      ((struct st_input*)st->input)->p_num = 0;
	      break;
	    }
	  else 
	    {
	      argp_failure(st, EXIT_FAILURE, 0,
			  "Unknown processing algorithm: %s", arg);
	    }
	  *param_state = PS_DONE;
	  break;
	case PS_DONE:
	  argp_failure(st, EXIT_FAILURE, 0, "Too many arguments");
	  break;
	}
      break;
    case ARGP_KEY_END:
      if (*param_state != PS_DONE)
	{
	  argp_failure(st, EXIT_FAILURE, 0,
		       "Too few arguments");
	}
    }
  return 0;
}

static void
parse_opt(int argc, char **argv)
{
  state.print_stream = false;
  state.print_stats = false;
  state.print_algstats = false;
  state.print_value = false;
  state.print_buffer = false;
  state.finish_stream = false;
  state.gen = 0;
  struct argp argp = { options, parse,
		       "GEN [GEN_OPTIONS] ALG [ALG_OPTIONS]" };
  struct st_input pstate = {PS_GEN, 0};
  argp_parse(&argp, argc, argv, 0, 0, &pstate);
}

struct processing_algo
get_processing_aglo()
{
  return pq_algo(cmp_s);
}

int main(int argc, char **argv)
{
  parse_opt(argc, argv);

  struct packet_stream stream = state.gen(state.gen_params);
  
  if (state.print_stats)
    {
      packet_stream_print_stats(stream);
    }

  if (state.print_stream)
    {
      packet_stream_print_stream(stream);
    }

  struct processing_algo algo = state.algo;

  while (!packet_stream_ended(stream))
    {
      algo.arrival(&algo, packet_stream_get_packets(&stream));
      algo.processing(&algo);
      packet_stream_tick(&stream);
    }

  if (state.finish_stream)
    {
      while (algo.buffer > 0) {
	algo.processing(&algo);    
      }
    }
  
  if (state.print_algstats)
    {
      printf("Algorithm statistics:\n\tValue: %d MaxBuffer: %d\n",
	     algo.val, algo.max_buffer);
    }
  if (state.print_value)
    {
      printf("%f\n", algo.val / (float) stream.val);
    }
  if (state.print_buffer)
    {
      printf("%d\n", algo.max_buffer);
    }
  algo.free(&algo);
  packet_stream_free(&stream);
  return EXIT_SUCCESS;
}
