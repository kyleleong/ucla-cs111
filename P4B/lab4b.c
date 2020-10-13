/* NAME:  Kyle Leong
   EMAIL: redacted@example.com
   ID:    123456789           */

#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <math.h>
#include <mraa/aio.h>
#include <mraa/gpio.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define BUFSZ 256

#define USAGE "Usage: %s [--period=secs] [--scale=(C|F)] [--log=file]\n"

#define ERROR_AND_DIE(syscall) do {					\
		fprintf(stderr, "%s: " syscall "error (line %d): %s\n", \
			argv[0], __LINE__, strerror(errno)); exit(1);	\
	} while (0)

#define PRINTF_AND_LOG(fmt, ...) do {					\
		printf("%02d:%02d:%02d " fmt, ctm->tm_hour,		\
		       ctm->tm_min, ctm->tm_sec, __VA_ARGS__);		\
		if (log_name) fprintf(fp_log, "%02d:%02d:%02d " fmt,	\
				      ctm->tm_hour, ctm->tm_min,	\
				      ctm->tm_sec, __VA_ARGS__);	\
	} while (0)

#define PUTS_AND_LOG(str) do {						\
		printf("%02d:%02d:%02d " str, ctm->tm_hour,		\
		       ctm->tm_min, ctm->tm_sec);			\
		if (log_name) fprintf(fp_log, "%02d:%02d:%02d " str,	\
				      ctm->tm_hour, ctm->tm_min,	\
				      ctm->tm_sec);			\
	} while (0)

static const struct option long_options[] = {
	{ "period", required_argument, NULL, 'p' },
	{ "scale", required_argument, NULL, 's' },
	{ "log", required_argument, NULL, 'l' },
	{ NULL, 0, NULL, 0 }
};

volatile sig_atomic_t f_shutdown = 0;
volatile int f_start = 1;
volatile int period = 1;
volatile char scale = 'F';
FILE *fp_log = NULL;

/* Will be called when the button is pressed. */
void on_btn_down()
{
	f_shutdown = 1;
}

/* Will be called when Ctrl+C is pressed. */
void sigint_handler(int signum)
{
	(void) signum;
	f_shutdown = 1;
}

/* Convert the raw data from temp to real. */
float get_real_temp(int a)
{
	float b = 4275, r0 = 100000, r;
	r = 1023.0/a-1.0;
	r = r0 * r;
	return 1.0/(log(r/r0)/b + 1/298.15) - 273.15;
}

/* Takes Celsius and converts to Fahrenheit. */
float c_to_f(float c)
{
	return (c * 9) / 5 + 32;
}

/* Parse and execute each string, where each string is a command. */
void parse_and_exec(char *cmd)
{
	int rc;

	/* Parse the commands for scale. */
	rc = strncmp("SCALE=", cmd, 6);
	if (rc == 0 && strlen(cmd) == 7) {
		if (cmd[6] == 'C')
			scale = 'C';
		else if (cmd[6] == 'F')
			scale = 'F';
	}

	rc = strncmp("PERIOD=", cmd, 7);
	if (rc == 0 && atoi(cmd + 7) > 0)
		period = atoi(cmd + 7);

	rc = strcmp("STOP", cmd);
	if (rc == 0)
		f_start = 0;

	rc = strcmp("START", cmd);
	if (rc == 0)
		f_start = 1;

	// TODO: Isn't the log command unnecessary to be handled
	// considering that we're ordered to log all commands,
	// even if they're invalid?
	rc = strcmp("LOG", cmd);

	rc = strcmp("OFF", cmd);
	if (rc == 0)
		f_shutdown = 1;

	/* Log the command to the log file. */
	if (fp_log != NULL)
		fprintf(fp_log, "%s\n", cmd);
}

int main(int argc, char *argv[])
{
	char master_buf[BUFSZ], buf[BUFSZ], *log_name = NULL;
	float real_temp;
	int i, rc, raw_temp;
	mraa_aio_context temp;
	mraa_gpio_context btn;
	struct pollfd poll_in;
	struct tm *ctm;
	time_t cur_time, new_tick, old_tick;

	/* Parse command line arguments. */
	while (1) {
		rc = getopt_long(argc, argv, "", long_options, NULL);
		if (rc < 0)
			break;

		switch (rc) {
		case 'p':
			period = atoi(optarg);
			if (period < 1) {
				fprintf(stderr, "Bad period length.\n" USAGE,
					argv[0]);
				exit(1);
			}
			break;
		case 's':
			scale = optarg[0];
			if (strlen(optarg) != 1 ||
			    (scale != 'F' && scale != 'C')) {
				fprintf(stderr, "Bad scale supplied.\n" USAGE,
					argv[0]);
				exit(1);
			}
			break;
		case 'l':
			log_name = optarg;
			break;
		case '?':
			fprintf(stderr, USAGE, argv[0]);
			exit(1);
		}
	}

	/* Check for unexpected arguments. */
	if (optind < argc) {
		fprintf(stderr, "Extraneous argument.\n" USAGE, argv[0]);
		exit(1);
	}

	/* Register Ctrl+C handler to flush before quitting. */
	signal(SIGINT, &sigint_handler);

	/* Attempt to open the log file. */
	if (log_name != NULL) {
		fp_log = fopen(log_name, "w");
		if (fp_log == NULL)
			ERROR_AND_DIE("fopen");
	}

	/* Set up button and its handler. */
	btn = mraa_gpio_init(60);
	if (btn == NULL) {
		fprintf(stderr, "Failed to initialize button.\n");
		exit(1);
	}
	rc = mraa_gpio_dir(btn, MRAA_GPIO_IN);
	if (rc != MRAA_SUCCESS) {
		fprintf(stderr, "Could not map button as input.\n");
		exit(1);
	}
	rc = mraa_gpio_isr(btn, MRAA_GPIO_EDGE_RISING, &on_btn_down, NULL);
	if (rc != MRAA_SUCCESS) {
		fprintf(stderr, "Could not register button handler.\n");
		exit(1);
	}

	/* Set up temperature sensor for reading. */
	temp = mraa_aio_init(1);
	if (temp == NULL) {
		fprintf(stderr, "Failed to initialize temperature.\n");
		exit(1);
	}

	/* Set up poll structures. */
	poll_in.events = POLLIN;
	poll_in.fd = STDIN_FILENO;

	/* Set up the buffers. */
	memset(master_buf, '\0', BUFSZ);
	memset(buf, '\0', BUFSZ);

	/* Initialize timers. */
	old_tick = new_tick = time(NULL);
	old_tick--;

	/* Continually loop, sleeping for period amount of seconds. */
	while (!f_shutdown) {
		new_tick = time(NULL);

		/* If we have input. */
		rc = poll(&poll_in, 1, 0);
		if (rc == -1)
			ERROR_AND_DIE("poll");
		if (poll_in.revents & POLLIN) {
			rc = read(STDIN_FILENO, &buf, BUFSZ - 1);
			if (rc == -1)
				ERROR_AND_DIE("read");
			for (i = 0; i < rc; i++) {
				char cat[2] = { 0 };
				if (buf[i] == '\n') {
					strcat(master_buf, cat);
					parse_and_exec(master_buf);
					memset(master_buf, '\0', BUFSZ);
				} else {
					cat[0] = buf[i];
					strcat(master_buf, cat);
				}
			}
		}

		/* If the period has passed, print new temperature. */
		if (difftime(new_tick, old_tick) >= period && f_start) {
			raw_temp = mraa_aio_read(temp);
			real_temp = get_real_temp(raw_temp);
			cur_time = time(NULL);
			ctm = localtime(&cur_time);
			PRINTF_AND_LOG("%.1f\n", (scale == 'F') ?
				       c_to_f(real_temp) : real_temp);
			old_tick = new_tick;
		}
	}

	/* Closing ensures it will be flushed, and thus written. */
	PUTS_AND_LOG("SHUTDOWN\n");
	if (fp_log)
		fclose(fp_log);

	/* Close access to the GPIO and AIO. */
	mraa_aio_close(temp);
	mraa_gpio_close(btn);

	exit(0);
}
