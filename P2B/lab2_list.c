/* NAME:  Kyle Leong
   EMAIL: redacted@example.com
   ID:    123456789           */

#include <errno.h>
#include <getopt.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "SortedList.h"

#define __DEBUG__
#ifdef __DEBUG__
#define DBG(...) do { fprintf(stderr, __VA_ARGS__); } while (0)
#else
#define DBG(...)
#endif

#define ERROR_AND_DIE(syscall) do {					\
		fprintf(stderr, "%s: " syscall " error (line %d): %s\n", argv[0], \
			__LINE__, strerror(errno)); exit(1); } while (0)

#define ERROR_AND_DIE2(syscall) do {					\
		fprintf(stderr, syscall " error (line %d): %s\n",	\
			__LINE__, strerror(errno)); exit(1); } while (0)

#define KEY_SIZE (6)
#define USAGE_STR "Usage: %s [--threads=] [--iterations=] [--sync=] [--yield=] [--lists=]\n"

int opt_yield = 0;

const struct option long_options[] = {
	{ "threads", required_argument, NULL, 't' },
	{ "iterations", required_argument, NULL, 'i' },
	{ "yield", required_argument, NULL, 'y' },
	{ "sync", required_argument, NULL, 's' },
	{ "lists", required_argument, NULL, 'l' },
	{ NULL, 0, NULL, 0 }
};

struct thread_args_t {
	int iter_count, lists_count;
	SortedList_t **lists;
	SortedListElement_t **nodes;
	char lock_type;
	unsigned long long lock_time;
	pthread_mutex_t *mut_locks;
	volatile int *test_locks;
};

/* Hashing function to turn node key into a list number. */
int list_hash(const char * key, const int num_lists)
{
	return (key[0] % num_lists);
}

#define TRY_RESPECTIVE_LOCK(idx) do {					\
		if (args->lock_type == 's')				\
			while (__sync_lock_test_and_set(&args->test_locks[idx], 1) == 1); \
		else if (args->lock_type == 'm')			\
			pthread_mutex_lock(&args->mut_locks[idx]);	\
	} while (0)

#define TRY_RESPECTIVE_UNLOCK(idx) do {				\
		if (args->lock_type == 's')			\
			__sync_lock_release(&args->test_locks[idx]);	\
		else if (args->lock_type == 'm')		\
			pthread_mutex_unlock(&args->mut_locks[idx]);	\
	} while (0)

#define START_CLOCK() do {					\
		rc = clock_gettime(CLOCK_MONOTONIC, &start);	\
		if (rc < 0)					\
			ERROR_AND_DIE2("clock_gettime");	\
	} while (0)

#define STOP_CLOCK() do {					\
		rc = clock_gettime(CLOCK_MONOTONIC, &end);	\
		if (rc < 0)					\
			ERROR_AND_DIE2("clock_gettime");	\
	} while (0)

#define ADD_ELAPSED_TIME() do {						\
		args->lock_time += ((1000000000 * end.tv_sec) + end.tv_nsec) - \
			((1000000000 * start.tv_sec) + start.tv_nsec);	\
	} while (0)

#define F() list_hash(args->nodes[i]->key, args->lists_count)

/* This will be excuted by p_threads. */
void *thread(void *blob)
{
	struct timespec start, end;
	struct thread_args_t *args;
	SortedListElement_t *elem;
	int i, lists_len, rc;

	args = (struct thread_args_t*) blob;
	args->lock_time = 0;

	/* Insert nodes into list. */
	for (i = 0; i < args->iter_count; i++) {
		START_CLOCK();
		TRY_RESPECTIVE_LOCK(F());
		STOP_CLOCK();
		ADD_ELAPSED_TIME();
		SortedList_insert(args->lists[F()], args->nodes[i]);
		TRY_RESPECTIVE_UNLOCK(F());
	}

	/* Check for corruption. */
	lists_len = 0;
	for (i = 0; i < args->lists_count; i++) {
		START_CLOCK();
		TRY_RESPECTIVE_LOCK(i);
		STOP_CLOCK();
		ADD_ELAPSED_TIME();
		rc = SortedList_length(args->lists[i]);
		if (rc < 0) {
			fprintf(stderr, "Bad sub-list length.\n");
			exit(2);
		}
		lists_len += rc;
		TRY_RESPECTIVE_UNLOCK(i);
	}
	if (lists_len < 0) {
		fprintf(stderr, "Bad list length.\n");
		exit(2);
	}

	/* Delete elements from list. */
	for (i = 0; i < args->iter_count; i++) {
		START_CLOCK();
		TRY_RESPECTIVE_LOCK(F());
		STOP_CLOCK();
		ADD_ELAPSED_TIME();
		elem = SortedList_lookup(args->lists[F()], args->nodes[i]->key);
		if (elem == NULL) {
			fprintf(stderr, "Could not find inserted key.\n");
			exit(2);
		}
		rc = SortedList_delete(elem);
		if (rc != 0)
			break;
		TRY_RESPECTIVE_UNLOCK(F());
	}

	pthread_exit((void*)(intptr_t) rc);
}

char *random_key(int length)
{
	static const char char_set[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	static const int char_set_len = (sizeof(char_set)/sizeof(char_set[0])) - 1;

	char *key = malloc(sizeof(char) * length);
	for (int i = 0; i < length; i++)
		key[i] = char_set[rand() % char_set_len];

	return key;
}

/* Return 1 if supplied argument to --yield is valid, and return test name
   in test_name. Otherwise, return 0. */
int is_valid_yield_opt(char* s, char *test_name)
{
	int i;
	char idl_found[3] = { 0 };

	for (i = 0; s[i] != '\0'; i++) {
		if (i > 2)
			return 0;

		switch (s[i]) {
		case 'i':
			if (idl_found[0])
				return 0;
			idl_found[0] = 1;
			break;
		case 'd':
			if (idl_found[1])
				return 0;
			idl_found[1] = 1;
			break;
		case 'l':
			if (idl_found[2])
				return 0;
			idl_found[2] = 1;
			break;
		default:
			return 0;
		}
	}

	i = 0;
	test_name[i++] = '-';
	if (idl_found[0])
		test_name[i++] = 'i';
	if (idl_found[1])
		test_name[i++] = 'd';
	if (idl_found[2])
		test_name[i++] = 'l';
	test_name[i++] = '\0';

	return 1;
}

void segfault_handler(int signum)
{
	(void) signum;
	static char msg[] = "Segfault encountered and caught. Exiting.\n";
	static int msg_len = sizeof(msg) / sizeof(msg[0]) - 1;
	write(STDERR_FILENO, msg, msg_len);
	_exit(2);
}

int main(int argc, char *argv[])
{
	char *sync_str = NULL, *yield_str = NULL;
	char test_name[20] = "list", lock_type;
	int sync_flag = 0, yield_flag = 0, i, j, rc;
	int thread_count = 1, iter_count = 1;
	struct timespec start_clock, stop_clock;
	unsigned long long start_ns, end_ns, time_diff, op_count;
	unsigned long long avg_lock_time = 0;
	SortedList_t **lists;
	SortedListElement_t *node;

	int lists_count = 1;

	volatile int *test_locks;
	pthread_mutex_t *mut_locks;

	pthread_t *threads;
	struct thread_args_t *thread_args;
	int *thread_ret_vals;

	while (1) {
		rc = getopt_long(argc, argv, "", long_options, NULL);
		if (rc < 0)
			break;

		switch (rc) {
		case 't':
			thread_count = atoi(optarg);
			if (thread_count < 1) {
				fprintf(stderr, "Bad thread count.\n" \
					USAGE_STR, argv[0]);
				return 1;
			}
			break;
		case 'i':
			iter_count = atoi(optarg);
			if (iter_count < 1) {
				fprintf(stderr, "Bad iteration count.\n" \
					USAGE_STR, argv[0]);
				return 1;
			}
			break;
		case 'y':
			if (!is_valid_yield_opt(optarg, &test_name[strlen(test_name)])) {
				fprintf(stderr, "Bad yield options.\n" \
					USAGE_STR, argv[0]);
				return 1;
			}
			yield_str = optarg;
			yield_flag = 1;
			break;
		case 's':
			if (strlen(optarg) != 1 || strstr("ms", optarg) == NULL) {
				fprintf(stderr, "Bad sync options.\n" \
					USAGE_STR, argv[0]);
				return 1;
			}
			sync_str = optarg;
			sync_flag = 1;
			break;
		case 'l':
			lists_count = atoi(optarg);
			if (lists_count < 1) {
				fprintf(stderr, "Bad lists count.\n" \
					USAGE_STR, argv[0]);
				return 1;
			}
			break;
		case '?':
			fprintf(stderr, USAGE_STR, argv[0]);
			return 1;
		}
	}

	if (optind < argc) {
		fprintf(stderr, "You supplied an extraneous argument.\n"
			USAGE_STR, argv[0]);
		return 1;
	}

	/* Format the rest of the test name. */
	if (!yield_flag)
		strcat(test_name, "-none");
	if (sync_flag) {
		strcat(test_name, "-");
		strcat(test_name, sync_str);
	} else
		strcat(test_name, "-none");

	/* Initialize the sorted lists. */
	lists = (SortedList_t**) malloc(sizeof(SortedList_t*) * lists_count);
	if (lists == NULL)
		ERROR_AND_DIE("malloc");
	for (i = 0; i < lists_count; i++) {
		lists[i] = malloc(sizeof(SortedList_t));
		if (lists[i] == NULL)
			ERROR_AND_DIE("malloc");
		lists[i]->key = "[!] DUMMY [!]";
		lists[i]->next = lists[i];
		lists[i]->prev = lists[i];
	}

	/* Initialize the mutexes and spin locks. */
	test_locks = (volatile int*) malloc(sizeof(int) * lists_count);
	if (test_locks == NULL)
		ERROR_AND_DIE("malloc");
	mut_locks = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t) * lists_count);
	if (mut_locks == NULL)
		ERROR_AND_DIE("malloc");
	for (i = 0; i < lists_count; i++) {
		test_locks[i] = 0;
		rc = pthread_mutex_init(&mut_locks[i], NULL);
		if (rc != 0)
			ERROR_AND_DIE("pthread_mutex_init");
	}

	/* Set up thread meta-data. */
	threads = (pthread_t*) malloc(sizeof(pthread_t) * thread_count);
	if (threads == NULL)
		ERROR_AND_DIE("malloc");
	thread_args = (struct thread_args_t*) malloc(sizeof(struct thread_args_t) * thread_count);
	if (thread_args == NULL)
		ERROR_AND_DIE("malloc");
	thread_ret_vals = (int*) malloc(sizeof(int) * thread_count);
	if (thread_ret_vals == NULL)
		ERROR_AND_DIE("malloc");

	/* Set up the yield bit vector accordingly. */
	if (yield_flag) {
		if (strchr(yield_str, 'i'))
			opt_yield |= INSERT_YIELD;
		if (strchr(yield_str, 'd'))
			opt_yield |= DELETE_YIELD;
		if (strchr(yield_str, 'l'))
			opt_yield |= LOOKUP_YIELD;
	}

	/* Set up the lock type: m, s, or null. */
	lock_type = (sync_flag) ? (sync_str[0]) : ('\0');

	/* Seed PRNG deterministically for debugging purposes. */
	srand(0);
	for (i = 0; i < thread_count; i++) {
		thread_args[i].mut_locks = mut_locks;
		thread_args[i].test_locks = test_locks;
		thread_args[i].lists_count = lists_count;
		thread_args[i].lock_type = lock_type;
		thread_args[i].iter_count = iter_count;
		thread_args[i].lists = lists;
		thread_args[i].nodes = (SortedListElement_t**)
			malloc(sizeof(SortedListElement_t*) * iter_count);
		if (thread_args[i].nodes == NULL)
			ERROR_AND_DIE("malloc");
		for (j = 0; j < iter_count; j++) {
			node = (SortedListElement_t*) malloc(sizeof(SortedListElement_t));
			if (node == NULL)
				ERROR_AND_DIE("malloc");
			node->key = random_key(KEY_SIZE);
			thread_args[i].nodes[j] = node;
		}
	}

	/* Set up seg-fault handler. */
	rc = (int) (intptr_t) signal(SIGSEGV, segfault_handler);
	if (rc == EINVAL)
		ERROR_AND_DIE("signal");

	/* Start clock. */
	rc = clock_gettime(CLOCK_MONOTONIC, &start_clock);
	if (rc < 0)
		ERROR_AND_DIE("clock_gettime");

	/* Create the threads. */
	for (i = 0; i < thread_count; i++) {
		rc = pthread_create(&threads[i], NULL, &thread, &thread_args[i]);
		if (rc != 0)
			ERROR_AND_DIE("pthread_create");
	}

	/* Wait for the threads to finish. */
	for (i = 0; i < thread_count; i++) {
		rc = pthread_join(threads[i], (void**) &thread_ret_vals[i]);
		if (rc != 0)
			ERROR_AND_DIE("pthread_join");
	}

	/* Stop clock. */
	rc = clock_gettime(CLOCK_MONOTONIC, &stop_clock);
	if (rc < 0)
		ERROR_AND_DIE("clock_gettime");

	/* See if any of the threads reported an error. */
	for (i = 0; i < thread_count; i++) {
		rc = (int) thread_ret_vals[i];
		if (rc != 0) {
			fprintf(stderr, "Join Error: Thread %d returned %d.\n", i, rc);
			return 2;
		}
	}

	/* Assert that the length of the list is zero. */
	for (i = 0; i < lists_count; i++) {
		rc = SortedList_length(lists[i]);
		if (rc != 0) {
			fprintf(stderr, "List length is non-zero.\n");
			return 2;
		}
	}

	/* Calculate the time difference in nanoseconds. */
	start_ns = (1000000000 * start_clock.tv_sec) + start_clock.tv_nsec;
	end_ns = (1000000000 * stop_clock.tv_sec) + stop_clock.tv_nsec;
	time_diff = end_ns - start_ns;

	/* Compute average thread lock time. */
	op_count = 3 * (unsigned long long) thread_count * (unsigned long long) iter_count;
	if (lock_type != '\0') {
		for (i = 0; i < thread_count; i++)
			avg_lock_time += thread_args[i].lock_time;
		avg_lock_time /= op_count;
	}

	/* Print the formatted data for use in CSV. */
	printf("%s,%d,%d,%d,%llu,%llu,%llu,%llu\n", test_name, thread_count,
	       iter_count, lists_count, op_count, time_diff,
	       time_diff / (unsigned long long) op_count, avg_lock_time);

	return 0;
}
