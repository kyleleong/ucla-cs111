/* NAME:  Kyle Leong
 * EMAIL: redacted@example.com
 * ID:    123456789           */

#include <errno.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#include <zlib.h>

#define BUFSZ 256
#define CHUNK 16384

/* This will generate an appropriate, descriptive error message. */
#define ERROR_AND_DIE(syscall) do { \
	fprintf(stderr, "%s: " syscall " error (line %d): %s\r\n", \
		argv[0], __LINE__, strerror(errno)); _exit(1); } while (0)

/* Does the same as ERROR_AND_DIE() except it also resets term. to default. */
#define ERROR_DIE_RESET(syscall) do { \
	int olderr = errno; \
	rc = tcsetattr(STDIN_FILENO, TCSANOW, &old_term); \
	if (rc == -1) ERROR_AND_DIE("tcsetattr"); else { \
	fprintf(stderr, "%s: " syscall " error (line %d): %s\n", \
		argv[0], __LINE__, strerror(olderr)); _exit(1); }} while (0)

#define DEBUG_ENABLE
#ifdef DEBUG_ENABLE
#define DEBUG(msg) do { fprintf(stderr, "[DBG] " msg "\n"); } while (0)
#else
#define DEBUG(msg) ((void) (msg))
#endif

int def(char* in_buf, int in_len, char* out_buf, int out_len)
{
	z_stream strm;
	int rc;

	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;

	strm.total_in = strm.avail_in = in_len;
	strm.total_out = strm.avail_out = out_len;
	strm.next_in = (Bytef *) in_buf;
	strm.next_out = (Bytef *) out_buf;

	rc = deflateInit(&strm, Z_DEFAULT_COMPRESSION);
	if (rc != Z_OK)
		goto DEFLATE_CLEANUP;

	rc = deflate(&strm, Z_FINISH);
	if (rc == Z_STREAM_END)
		rc = strm.total_out;

DEFLATE_CLEANUP:
	(void) deflateEnd(&strm);
	return rc;
}

int inf(char* in_buf, int in_len, char* out_buf, int out_len)
{
	z_stream strm;
	int rc;

	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;

	strm.total_in = strm.avail_in = in_len;
	strm.total_out = strm.avail_out = out_len;
	strm.next_in = (Bytef *) in_buf;
	strm.next_out = (Bytef *) out_buf;

	rc = inflateInit(&strm);
	if (rc != Z_OK)
		goto INFLATE_CLEANUP;

	rc = inflate(&strm, Z_FINISH);
	if (rc == Z_STREAM_END)
		rc = strm.total_out;	

INFLATE_CLEANUP:
	(void) inflateEnd(&strm);
	return rc;
}

int main(int argc, char* argv[])
{
	const struct option long_options[] = {
		{ "port", required_argument, NULL, 'p' },
		{ "log", required_argument, NULL, 'l' },
		{ "compress", no_argument, NULL, 'c' },
		{ "host", required_argument, NULL, 'h' },
		{ NULL, 0, NULL, 0 }
	};
	struct termios old_term, new_term = {
		.c_iflag = ISTRIP,
		.c_oflag = 0,
		.c_lflag = 0
	};
	struct pollfd polls[2];
	struct sockaddr_in srv_addr;
	struct hostent* srv;

	int rc, port, sock_fd;
	FILE* f_log = NULL;
	char *port_str = NULL, *log_str = NULL, *host_str = "localhost";
	int given_port = 0, given_log = 0, given_compress = 0;
	char buf[BUFSZ], buf_fixed[BUFSZ * 2];
	int buf_len, i, j, end_rw = 0;

	while (1)
	{
		rc = getopt_long(argc, argv, "", long_options, NULL);
		if (rc < 0)
			break;

		switch (rc)
		{
		case 'p':
			given_port = 1;
			port_str = optarg;
			break;
		case 'l':
			given_log = 1;
			log_str = optarg;
			break;
		case 'c':
			given_compress = 1;
			break;
		case 'h':
			host_str = optarg;
			break;
		case '?':
			fprintf(stderr, "Usage: %s --port= [--log=FILENAME] "
				"[--compress]\n", argv[0]);
			_exit(1);
			break;
		}
	}

	if (given_port == 0 || optind < argc)
	{
		fprintf(stderr, "Usage: %s --port= [--log=FILENAME] "
			"[--compress]\n", argv[0]);
		_exit(1);
	}

	/* Try and parse the port argument's option.
	   Uses the fact that atoi() will return 0, an invalid port, by default. */
	port = atoi(port_str);
	if (port < 1 || port > 65535)
	{
		fprintf(stderr, "Port %d out of range (1-65535.\n", port);
		_exit(1);
	}

	/* If we want a log, then set up the file. */
	if (given_log)
	{
		f_log = fopen(log_str, "wb");
		if (f_log == NULL)
			ERROR_AND_DIE("fopen");
	}

	/* Try and establish a connection. */
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd == -1)
		ERROR_AND_DIE("socket");
	srv = gethostbyname(host_str);
	if (srv == NULL)
		ERROR_AND_DIE("gethostbyname");
	bzero((char*) &srv_addr, sizeof(srv_addr));
	srv_addr.sin_family = AF_INET;
	bcopy((char*) srv->h_addr, (char*) &srv_addr.sin_addr.s_addr,
	      srv->h_length);
	srv_addr.sin_port = htons(port);
	rc = connect(sock_fd, (struct sockaddr*) &srv_addr, sizeof(srv_addr));
	if (rc == -1)
		ERROR_AND_DIE("connect");

	/* Set up poll structures. */
	polls[0].events = polls[1].events = POLLIN;
	polls[0].fd = STDIN_FILENO;
	polls[1].fd = sock_fd;

	/* Change it to non-canonical, no-echo mode. */
	rc = tcgetattr(STDIN_FILENO, &old_term);
	if (rc == -1)
		ERROR_AND_DIE("tcgetattr");
	tcsetattr(STDIN_FILENO, TCSANOW, &new_term);
	if (rc == -1)
		ERROR_AND_DIE("tcsetattr");

	/* Now we can start continually reading/writing to socket. */
	while (!end_rw)
	{
		rc = poll(polls, 2, 0);
		if (rc == -1)
			ERROR_DIE_RESET("poll");
		
		/* Read from stdin, if pending. */
		if (polls[0].revents & POLLIN)
		{
			/* Get input to send to the socket. */
			buf_len = read(STDIN_FILENO, &buf, BUFSZ - 1);
			if (buf_len == -1)
				ERROR_DIE_RESET("read");

			/* We are supposed to just send it to server, but for now,
			   use these control codes to end execution until it works. */
			for (i = j = 0; i < buf_len; i++, j++)
			{
				switch (buf[i])
				{
				case '\003':
					write(STDOUT_FILENO, "^C", 2);
					buf_fixed[j] = buf[i];
					break;
				case '\004':
					// TODO: Replace this behavior with desired.
					write(STDOUT_FILENO, "^D", 2);
					buf_fixed[j] = buf[i];
					break;
				case '\r':
					__attribute__ ((fallthrough));
				case '\n':
					buf_fixed[j]   = '\r';
					buf_fixed[++j] = '\n';
					break;
				default:
					buf_fixed[j] = buf[i];
				}
			}

			/* Echo fixed (i.e. CRLF) input to the terminal. */
			rc = write(STDOUT_FILENO, buf_fixed, j);
			if (rc == -1)
				ERROR_DIE_RESET("write");

			if (!given_compress)
			{
				/* If we're supposed to log, write the "raw" bytes. */
				if (given_log)
				{
					buf[buf_len] = '\0';
					fprintf(f_log, "SENT %d bytes: %s\n", buf_len, buf);
				}

				/* Send it over the socket. */
				rc = write(sock_fd, buf, buf_len);
				if (rc == -1)
					ERROR_DIE_RESET("write");				
			} else {
				char comp[CHUNK];
				int comp_len;
				
				comp_len = def(buf, buf_len, comp, CHUNK - 1);
				if (comp_len < 0)
					ERROR_DIE_RESET("def");

				// Confirmed: The server is receiving/decompressing correctly. 
				// fprintf(stderr, "COMP %d: '%s'", comp_len, comp);
				
				if (given_log)
				{
					comp[comp_len] = '\n';
					fprintf(f_log, "SENT %d bytes: ", comp_len);
					fwrite(comp, 1, comp_len + 1, f_log);
					fflush(f_log);
				}

				rc = write(sock_fd, comp, comp_len);
				if (rc == -1)
					ERROR_DIE_RESET("write");
			}
		}

		/* Read from the socket, if its available. */
		if (polls[1].revents & POLLIN)
		{
			buf_len = read(sock_fd, buf, BUFSZ - 1);
			if (buf_len == -1)
				ERROR_DIE_RESET("read");
			
			/* If we're supposed to log, write the "raw" bytes. */
			if (given_log)
			{
				buf[buf_len] = '\n';
				fprintf(f_log, "RECEIVED %d bytes: ", buf_len);
				fwrite(buf, 1, buf_len + 1, f_log);
				fflush(f_log);
			}			

			if (!given_compress)
			{
				rc = write(STDOUT_FILENO, buf, buf_len);
				if (rc == -1)
					ERROR_DIE_RESET("write");				
			} else {
				char decomp[CHUNK];
				int decomp_len;
				decomp_len = inf(buf, buf_len, decomp, CHUNK);
				if (decomp_len < 0)
					ERROR_DIE_RESET("inf");				
				
				rc = write(STDOUT_FILENO, decomp, decomp_len);
				if (rc == -1)
					ERROR_DIE_RESET("write");
				
			}
		}

		/* See if the server closed the connection.
		   Don't count EAGAIN becuase that means no data, not closed. */
		rc = recv(sock_fd, buf, BUFSZ, MSG_PEEK | MSG_DONTWAIT);
		if (rc == -1 && errno != EAGAIN)
			ERROR_DIE_RESET("recv");
		if (rc == 0)
			goto CLEANUP;
	}

CLEANUP:
	/* Close files and socket. */
	if (f_log)
		fclose(f_log);
	close(sock_fd);
	
	/* Restore the original terminal settings. */
	rc = tcsetattr(STDIN_FILENO, TCSANOW, &old_term); \
	if (rc == -1)
		ERROR_AND_DIE("tcsetattr");

	_exit(0);
}
