/* NAME:  Kyle Leong
 * EMAIL: redacted@example.com
 * ID:    123456789           */

#include <errno.h>
#include <getopt.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define __DEBUG__
#ifdef __DEBUG__
#define DBG(...) do { fprintf(stderr, __VA_ARGS__); } while (0)
#else
#define DBG(...)
#endif

#define USAGE_STR "Usage: %s [--threads=] [--iterations=] [--sync=] [--yield]\n"

#define ERROR_AND_DIE(syscall) do { \
		fprintf(stderr, "%s: " syscall " error (line %d): %s\n", argv[0], \
			__LINE__, strerror(errno)); exit(1); } while (0)

struct thread_args_t {
	void (*add_fn)(long long*, long long);
	long long *ptr;
	int num_iters, thread_no;
};

const struct option long_options[] = {
	{ "threads", required_argument, NULL, 't' },
	{ "iterations", required_argument, NULL, 'i' },
	{ "sync", required_argument, NULL, 's' },
	{ "yield", no_argument, NULL, 'y' },
	{ NULL, 0, NULL, 0 }
};

int opt_yield = 0;
void add(long long *ptr, long long val)
{
	long long sum = *ptr + val;
	if (opt_yield)
		sched_yield();
	*ptr = sum;
}

pthread_mutex_t mut_lock = PTHREAD_MUTEX_INITIALIZER;
void add_mutex(long long *ptr, long long val)
{
	pthread_mutex_lock(&mut_lock);
	long long sum = *ptr + val;
	if (opt_yield)
		sched_yield();
	*ptr = sum;
	pthread_mutex_unlock(&mut_lock);
}

volatile int test_lock = 0;
void add_test(long long *ptr, long long val)
{
	while (__sync_lock_test_and_set(&test_lock, 1) == 1);
	long long sum = *ptr + val;
	if (opt_yield)
		sched_yield();
	*ptr = sum;
	__sync_lock_release(&test_lock);
}

volatile int comp_lock = 0;
void add_comp(long long *ptr, long long val)
{
	long long old_sum;
	do {
		old_sum = *ptr;
		if (opt_yield)
			sched_yield();
	} while (__sync_val_compare_and_swap(ptr, old_sum, old_sum + val) != old_sum);
}

void *thread(void *blob)
{
	struct thread_args_t *args;
	int i;

	args = (struct thread_args_t*) blob;
	for (i = 0; i < args->num_iters; i++)
		args->add_fn(args->ptr, 1);
	for (i = 0; i < args->num_iters; i++)
		args->add_fn(args->ptr, -1);

	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	char *sync_opt, test_name[20] = "add";
	int rc, i, thread_count = 1, iter_count = 1, flag_sync = 0;
	long long counter = 0;
	pthread_t* threads;
	struct thread_args_t *thread_args;	
	struct timespec tests_start, tests_end;
	unsigned long long start_ns, end_ns, time_diff, op_count;

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
		case 's':
			if (strlen(optarg) != 1 || strstr("msc", optarg) == NULL) {
				fprintf(stderr, "Bad sync option.\n" \
					USAGE_STR, argv[0]);
				return 1;
			}
			sync_opt = optarg;
			flag_sync = 1;
			break;
		case 'y':
			opt_yield = 1;
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

	/* Create the test name accordingly. */
	if (opt_yield)
		strcat(test_name, "-yield");
	strcat(test_name, "-");
	if (flag_sync)
		strcat(test_name, sync_opt);
	else
		strcat(test_name, "none");

	/* Allocate memory for and set up thread meta-data. */
	threads = (pthread_t*) malloc(sizeof(pthread_t) * thread_count);
	if (threads == NULL)
		ERROR_AND_DIE("malloc");
	thread_args = (struct thread_args_t*) malloc(sizeof(struct thread_args_t) * thread_count);
	if (thread_args == NULL)
		ERROR_AND_DIE("malloc");
	for (i = 0; i < thread_count; i++) {
		thread_args[i].ptr = &counter;
		thread_args[i].num_iters = iter_count;
		thread_args[i].thread_no = i;
		if (flag_sync) {
			switch (sync_opt[0]) {
			case 'm':
				thread_args[i].add_fn = add_mutex;
				break;
			case 's':
				thread_args[i].add_fn = add_test;
				break;
			case 'c':
				thread_args[i].add_fn = add_comp;
				break;
			}
		} else {
			thread_args[i].add_fn = add;
		}
	}

	/* Start the clock. */
	rc = clock_gettime(CLOCK_MONOTONIC, &tests_start);
	if (rc < 0)
		ERROR_AND_DIE("clock_gettime");

	/* Run the threads. */
	for (i = 0; i < thread_count; i++) {
		rc = pthread_create(&threads[i], NULL, &thread, &thread_args[i]);
		if (rc != 0)
			ERROR_AND_DIE("pthread_create");
	}

	/* Wait for all the threads to finish. */
	for (i = 0; i < thread_count; i++) {
		rc = pthread_join(threads[i], NULL);
		if (rc != 0)
			ERROR_AND_DIE("pthread_join");
	}

	/* Stop the clock. */
	rc = clock_gettime(CLOCK_MONOTONIC, &tests_end);
	if (rc < 0)
		ERROR_AND_DIE("clock_gettime");

	/* Calculate the timing. */
	start_ns = (1000000000 * tests_start.tv_sec) + tests_start.tv_nsec;
	end_ns = (1000000000 * tests_end.tv_sec) + tests_end.tv_nsec;
	time_diff = end_ns - start_ns;

	/* Printf formatted data for use in CSV. */
	op_count = 2 * (unsigned long long) thread_count * (unsigned long long) iter_count;
	printf("%s,%d,%d,%llu,%llu,%lld,%lld\n", test_name, thread_count, iter_count,
	       op_count, time_diff, time_diff / (unsigned long long) op_count, counter);

	return 0;
}
