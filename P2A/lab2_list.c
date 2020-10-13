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

#define ERROR_AND_DIE(syscall) do { \
		fprintf(stderr, "%s: " syscall " error (line %d): %s\n", argv[0], \
			__LINE__, strerror(errno)); exit(1); } while (0)

#define KEY_SIZE (6)
#define USAGE_STR "Usage: %s [--threads=] [--iterations=] [--sync=] [--yield=]\n"

int opt_yield = 0;

const struct option long_options[] = {
	{ "threads", required_argument, NULL, 't' },
	{ "iterations", required_argument, NULL, 'i' },
	{ "yield", required_argument, NULL, 'y' },
	{ "sync", required_argument, NULL, 's' },
	{ NULL, 0, NULL, 0 }
};

struct thread_args_t {
	int iter_count;
	SortedList_t *list;
	SortedListElement_t **nodes;
};

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

/* This will be excuted by p_threads. */
void *thread(void *blob)
{
	struct thread_args_t *args;
	SortedListElement_t *elem;
	int i, rc;

	args = (struct thread_args_t*) blob;

	for (i = 0; i < args->iter_count; i++)
		SortedList_insert(args->list, args->nodes[i]);
	for (i = 0; i < args->iter_count; i++) {
		elem = SortedList_lookup(args->list, args->nodes[i]->key);
		if (elem == NULL) {
			fprintf(stderr, "Could not find inserted key.\n");
			exit(2);
		}
		rc = SortedList_delete(elem);
		if (rc != 0)
			break;
	}

	pthread_exit((void*)(intptr_t) rc);
}

pthread_mutex_t mut_lock = PTHREAD_MUTEX_INITIALIZER;
void *thread_mutex(void *blob)
{
	struct thread_args_t *args;
	SortedListElement_t *elem;
	int i, rc;

	args = (struct thread_args_t*) blob;

	pthread_mutex_lock(&mut_lock);

	for (i = 0; i < args->iter_count; i++)
		SortedList_insert(args->list, args->nodes[i]);
	for (i = 0; i < args->iter_count; i++) {
		elem = SortedList_lookup(args->list, args->nodes[i]->key);
		if (elem == NULL) {
			fprintf(stderr, "Could not find inserted key.\n");
			exit(2);
		}
		rc = SortedList_delete(elem);
		if (rc != 0)
			break;
	}

	pthread_mutex_unlock(&mut_lock);

	pthread_exit((void*)(intptr_t) rc);
}

volatile int test_lock = 0;
void *thread_test(void *blob)
{
	struct thread_args_t *args;
	SortedListElement_t *elem;
	int i, rc;

	args = (struct thread_args_t*) blob;

	while (__sync_lock_test_and_set(&test_lock, 1) == 1);

	for (i = 0; i < args->iter_count; i++)
		SortedList_insert(args->list, args->nodes[i]);
	for (i = 0; i < args->iter_count; i++) {
		elem = SortedList_lookup(args->list, args->nodes[i]->key);
		if (elem == NULL) {
			fprintf(stderr, "Could not find inserted key.\n");
			exit(2);
		}
		rc = SortedList_delete(elem);
		if (rc != 0)
			break;
	}

	__sync_lock_release(&test_lock);

	pthread_exit((void*)(intptr_t) rc);
}

int main(int argc, char *argv[])
{
	char *sync_str = NULL, *yield_str = NULL;
	char test_name[20] = "list", lock_type;
	int sync_flag = 0, yield_flag = 0, i, j, rc;
	int thread_count = 1, iter_count = 1;
	struct timespec start_clock, stop_clock;
	unsigned long long start_ns, end_ns, time_diff, op_count;
	SortedList_t *list;
	SortedListElement_t *node;

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

	/* Initialize the sorted list. */
	list = (SortedList_t*) malloc(sizeof(SortedList_t));
	if (list == NULL)
		ERROR_AND_DIE("malloc");
	list->key = "[!] DUMMY [!]";
	list->next = list;
	list->prev = list;

	/* Set up thread meta-data. */
	threads = (pthread_t*) malloc(sizeof(pthread_t) * thread_count);
	if (threads == NULL)
		ERROR_AND_DIE("malloc");
	thread_args = (struct thread_args_t*) malloc(sizeof(struct thread_args_t) * thread_count);
	if (thread_args == NULL)
		ERROR_AND_DIE("malloc");
	thread_ret_vals = (void*) malloc(sizeof(int) * thread_count);
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

	/* Seed PRNG deterministically for debugging purposes. */
	srand(0);
	for (i = 0; i < thread_count; i++) {
		thread_args[i].iter_count = iter_count;
		thread_args[i].list = list;
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

	/* Set up the lock type: m, s, or null. */
	lock_type = (sync_flag) ? (sync_str[0]) : ('\0');

	/* Set up seg-fault handler. */
	rc = (int) (intptr_t) signal(SIGSEGV, segfault_handler);
	if (rc == EINVAL)
		ERROR_AND_DIE("signal");

	/* Start clock. */
	rc = clock_gettime(CLOCK_MONOTONIC, &start_clock);
	if (rc < 0)
		ERROR_AND_DIE("clock_gettime");

	/* Create the threads. */
	switch (lock_type)
	{
	case 's':
		for (i = 0; i < thread_count; i++) {
			rc = pthread_create(&threads[i], NULL, &thread_test, &thread_args[i]);
			if (rc != 0)
				ERROR_AND_DIE("pthread_create");
		}
		break;
	case 'm':
		for (i = 0; i < thread_count; i++) {
			rc = pthread_create(&threads[i], NULL, &thread_mutex, &thread_args[i]);
			if (rc != 0)
				ERROR_AND_DIE("pthread_create");
		}
		break;
	default:
		for (i = 0; i < thread_count; i++) {
			rc = pthread_create(&threads[i], NULL, &thread, &thread_args[i]);
			if (rc != 0)
				ERROR_AND_DIE("pthread_create");
		}
		break;
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
	rc = SortedList_length(list);
	if (rc != 0) {
		fprintf(stderr, "List length not zero.\n");
		return 2;
	}

	/* Calculate the time difference in nanoseconds. */
	start_ns = (1000000000 * start_clock.tv_sec) + start_clock.tv_nsec;
	end_ns = (1000000000 * stop_clock.tv_sec) + stop_clock.tv_nsec;
	time_diff = end_ns - start_ns;

	/* Print the formatted data for use in CSV. */
	op_count = 3 * (unsigned long long) thread_count * (unsigned long long) iter_count;
	printf("%s,%d,%d,1,%llu,%llu,%llu\n", test_name, thread_count, iter_count,
	       op_count, time_diff, time_diff / (unsigned long long) op_count);

	return 0;
}
