/* NAME:  Kyle Leong
   EMAIL: redacted@example.com
   ID:    123456789           */

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <math.h>
#include <mraa/aio.h>
#include <mraa/gpio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define BUFSZ 256

#define USAGE "Usage: %s --log=file --id=UID --host=NAME\n"	\
	"       [--period=secs] [--scale=(C|F)] PORT\n"

#define ERROR_AND_DIE(syscall) do {					\
		fprintf(stderr, "%s: " syscall " error (line %d): "	\
			"%s\n",	argv[0], __LINE__, strerror(errno));	\
		exit(2);						\
	} while (0)

static const struct option long_options[] = {
	{ "period", required_argument, NULL, 'p' },
	{ "scale", required_argument, NULL, 's' },
	{ "log", required_argument, NULL, 'l' },
	{ "id", required_argument, NULL, 'i' },
	{ "host", required_argument, NULL, 'h' },
	{ NULL, 0, NULL, 0 }
};

volatile sig_atomic_t f_shutdown = 0;
volatile int f_start = 1;
volatile int period = 1;
volatile char scale = 'F';
FILE *fp_log = NULL;

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

	rc = strncmp("OFF ", cmd, 4);
	if (rc == 0 && strlen(cmd + 4) > 0)
		(void) 0;

	rc = strcmp("OFF", cmd);
	if (rc == 0)
		f_shutdown = 1;

	/* Log the command to the log file and stdout. */
	fprintf(fp_log, "%s\n", cmd);
}

/* Helper function taken from the OpenSSL example page at
   https://wiki.openssl.org/index.php/Simple_TLS_Server */
SSL_CTX *create_context()
{
	const SSL_METHOD *method;
	SSL_CTX *ctx;

	method = SSLv23_client_method();

	ctx = SSL_CTX_new(method);
	if (ctx == NULL) {
		fprintf(stderr, "Error on SSL_CTX_new.\n");
		exit(2);
	}

	return ctx;
}

int main(int argc, char *argv[])
{
	SSL *ssl;
	SSL_CTX *ctx;
	char *host_name = NULL, *id_name = NULL;
	char master_buf[BUFSZ], buf[BUFSZ], *log_name = NULL;
	float real_temp;
	int i, rc, raw_temp, id, port, sock_fd;
	mraa_aio_context temp;
	struct hostent *srv;
	struct pollfd poll_in;
	struct sockaddr_in srv_addr;
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
		case 'h':
			host_name = optarg;
			break;
		case 'i':
			id_name = optarg;
			break;
		case '?':
			fprintf(stderr, USAGE, argv[0]);
			exit(1);
		}
	}

	/* Check that the port argument is provided. */
	if (optind < argc - 1) {
		fprintf(stderr, "Extraneous argument.\n" USAGE, argv[0]);
		exit(1);
	} else if (optind >= argc) {
		fprintf(stderr, "Must supply a port.\n" USAGE, argv[0]);

		exit(1);
	}
	port = atoi(argv[optind]);
	if (port < 1 || port > 65535) {
		fprintf(stderr, "Bad port supplied.\n" USAGE, argv[0]);
		exit(1);
	}

	/* Ensure that the user supplied a valid id. */
	id = atoi(argv[optind]);
	if (id_name == NULL || strlen(id_name) != 9 ||
	    id < 1 || id > 999999999) {
		fprintf(stderr, "Bad or no ID supplied.\n" USAGE, argv[0]);
		exit(1);
	}

	/* Check that the user supplied a valid host. */
	if (host_name == NULL) {
		fprintf(stderr, "Must supply a host name.\n"
			USAGE, argv[0]);
		exit(1);
	}

	/* Try to conenct to the supplied host. */
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd == -1)
		ERROR_AND_DIE("socket");
	srv = gethostbyname(host_name);
	if (srv == NULL)
		ERROR_AND_DIE("gethostbyname");
	bzero((char*) &srv_addr, sizeof(srv_addr));
	srv_addr.sin_family = AF_INET;
	bcopy((char*) srv->h_addr, (char*) &srv_addr.sin_addr.s_addr,
	      srv->h_length);
	srv_addr.sin_port = htons(port);
	rc = connect(sock_fd, (struct sockaddr *) &srv_addr, sizeof(srv_addr));
	if (rc == -1)
		ERROR_AND_DIE("connect");

	/* Initialize OpenSSL context. These functions either return
	   void or constant values, so we do not check for errors. */
	SSL_load_error_strings();
	OpenSSL_add_ssl_algorithms();
	ctx = create_context();

	/* Set up the OpenSSL file descriptor handlers. */
	ssl = SSL_new(ctx);
	rc = SSL_set_fd(ssl, sock_fd);
	if (rc != 1) {
		ERR_print_errors_fp(stderr);
		exit(2);
	}

	rc = SSL_connect(ssl);
	if (rc <= 0) {
		ERR_print_errors_fp(stderr);
		exit(2);
	}

	/* Send the ID over first. */
	char first_msg[BUFSZ] = { 0 };
	rc = sprintf(first_msg, "ID=%s\n", id_name);
	if (rc < 0)
		ERROR_AND_DIE("sprintf");
	SSL_write(ssl, first_msg, strlen(first_msg));

	/* Register Ctrl+C handler to flush before quitting. */
	signal(SIGINT, &sigint_handler);

	/* Attempt to open the log file. */
	if (log_name == NULL) {
		fprintf(stderr, "No log specified.\n" USAGE, argv[0]);
		exit(1);
	}
	fp_log = fopen(log_name, "w");
	if (fp_log == NULL)
		ERROR_AND_DIE("fopen");

	/* Set up temperature sensor for reading. */
	temp = mraa_aio_init(1);
	if (temp == NULL) {
		fprintf(stderr, "Failed to initialize temperature.\n");
		exit(1);
	}

	/* Set up poll structures. */
	poll_in.events = POLLIN;
	poll_in.fd = sock_fd;

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
			rc = SSL_read(ssl, &buf, BUFSZ - 1);
			buf[rc] = '\0';
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

			char sock_buf[BUFSZ] = { 0 };
			sprintf(sock_buf, "%02d:%02d:%02d %.1f\n",
				ctm->tm_hour, ctm->tm_min, ctm->tm_sec,
				(scale == 'F') ? c_to_f(real_temp) : real_temp);

			fprintf(fp_log, "%s", sock_buf);

			/* Send it to the server as well. */
			SSL_write(ssl, sock_buf, strlen(sock_buf));

			old_tick = new_tick;
		}
	}

	/* Closing ensures it will be flushed, and thus written. */
	char sock_buf[BUFSZ] = { 0 };
	sprintf(sock_buf, "%02d:%02d:%02d SHUTDOWN\n",
		ctm->tm_hour, ctm->tm_min, ctm->tm_sec);
	fprintf(fp_log, sock_buf);
	SSL_write(ssl, sock_buf, strlen(sock_buf));

	/* Close SSL connection. */
	SSL_shutdown(ssl);
	SSL_free(ssl);
	close(sock_fd);
	SSL_CTX_free(ctx);
	EVP_cleanup();

	/* Close access to the GPIO and AIO. */
	mraa_aio_close(temp);

	exit(0);
}
