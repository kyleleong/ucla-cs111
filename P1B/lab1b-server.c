/* NAME:  Kyle Leong
 * EMAIL: redacted@example.com
 * ID:    123456789           */

#include <errno.h>
#include <getopt.h>
#include <getopt.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>
#include <zlib.h>

#define BUFSZ 256
#define CHUNK 16384

#define ERROR_AND_DIE(syscall) do { \
	fprintf(stderr, "%s: " syscall " error (line %d): %s\r\n", \
		argv[0], __LINE__, strerror(errno)); _exit(1); } while (0)

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
		{ "compress", no_argument, NULL, 'c' },
		{ NULL, 0, NULL, 0 }
	};
	struct sockaddr_in srv_addr, cli_addr;
	struct pollfd polls[2];

	int rc, port, sock_fd, new_sock_fd, buf_len;
	socklen_t cli_len;
	char *port_str = NULL;
	int given_port = 0, given_compress = 0;
	char buf[BUFSZ], buf_fixed[BUFSZ * 2];
	int stop_writing_to_bash = 0;

	int child_pid;
	int srv2bash[2], bash2srv[2], wait_res = 0;
	int i, j;

	while (1)
	{
		rc = getopt_long(argc, argv, "", long_options, NULL);
		if (rc == -1)
			break;

		switch(rc)
		{
		case 'p':
			given_port = 1;
			port_str = optarg;
			break;
		case 'c':
			given_compress = 1;
			break;
		case '?':
			fprintf(stderr, "Usage: %s --port= [--compress]\n",
				argv[0]);
			_exit(1);
		}
	}

	/* Ensure we are given the mandatory --port argument. */
	if (given_port == 0 || optind < argc)
	{
		fprintf(stderr, "Usage: %s --port= [--compress]\n", argv[0]);
		_exit(1);
	}

	/* Parse an integer form the port's option. atoi() will return 0
	   on failure which is an invalid port anyways. */
	port = atoi(port_str);
	if (port < 1 || port > 65535)
	{
		fprintf(stderr, "Port %d out of range (1-65535).\n", port);
		_exit(1);
	}

	/* Set up the server. */
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd == -1)
		ERROR_AND_DIE("socket");
	bzero((char*) &srv_addr, sizeof(srv_addr));
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_addr.s_addr = INADDR_ANY;
	srv_addr.sin_port = htons(port);
	rc = bind(sock_fd, (struct sockaddr *) &srv_addr, sizeof(srv_addr));
	if (rc == -1)
		ERROR_AND_DIE("bind");
	
	/* Set up listening backlog. */
	rc = listen(sock_fd, 5);
	if (rc == -1)
		ERROR_AND_DIE("listen");

	/* Wait for a client to connect. */
	cli_len = (socklen_t) sizeof(cli_addr);
	new_sock_fd = accept(sock_fd, (struct sockaddr *) &cli_addr, &cli_len);
	if (new_sock_fd == -1)
		ERROR_AND_DIE("accept");

	/* Set up pipes to child. */
	rc = pipe(srv2bash);
	if (rc == -1)
		ERROR_AND_DIE("pipe");
	rc = pipe(bash2srv);
	if (rc == -1)
		ERROR_AND_DIE("pipe");

	/* Create child bash process. */
	child_pid = fork();
	if (child_pid < 0)
		ERROR_AND_DIE("fork");
	else if (child_pid == 0)
	{
		/* Child close unused pipe ends. */
		close(srv2bash[1]);
		close(bash2srv[0]);

		/* Make stdin a pipe to the server process. */
		close(STDIN_FILENO);
		dup(srv2bash[0]);

		/* Make stdout/stderr a pipe to the server process. */
		close(STDOUT_FILENO);
		dup(bash2srv[1]);
		close(STDERR_FILENO);
		dup(bash2srv[1]);

		/* Handles are duplicated so we can close extra ones. */
		close(srv2bash[0]);
		close(bash2srv[1]);

		rc = execl("/bin/bash", "/bin/bash", NULL);
		if (rc == -1)
			ERROR_AND_DIE("execl");
	}

	/* Close unused pipe ends. */
	close(srv2bash[0]);
	close(bash2srv[1]);

	/* Set up poll structures. */
	polls[0].events = polls[1].events = POLLIN | POLLHUP | POLLERR;
	polls[0].fd = new_sock_fd;
	polls[1].fd = bash2srv[0];	
	
	while (1)
	{
		rc = poll(polls, 2, 0);
		if (rc == -1)
			ERROR_AND_DIE("poll");

		/* See if there's input from the socket. */
		if (polls[0].revents & POLLIN)
		{
			buf_len = read(new_sock_fd, buf, BUFSZ - 1);
			if (buf_len == -1)
				ERROR_AND_DIE("read");
			
			if (!given_compress)
			{
				/* Format input before sending it to shell.
				   CR or LF should map to LF. */
				for (i = 0; i < buf_len; i++)
				{
					switch (buf[i])
					{
					case '\r':
						buf[i] = '\n';
						break;
					case '\003':
						rc = kill(child_pid, SIGINT);
						if (rc == -1)
							ERROR_AND_DIE("kill");
						break;
					case '\004':
						close(srv2bash[1]);
						stop_writing_to_bash = 1;
						break;
					}
				}

				if (!stop_writing_to_bash)
				{
					rc = write(srv2bash[1], buf, buf_len);
					if (rc == -1)
						ERROR_AND_DIE("pipe");				
				}				
			} else {
				char decomp[CHUNK];
				int decomp_len;
				decomp_len = inf(buf, buf_len, decomp, CHUNK);
				if (decomp_len < 0)
					ERROR_AND_DIE("inf");

				/* Format input before sending it to shell.
				   CR or LF should map to LF. */
				for (i = 0; i < decomp_len; i++)
				{
					switch (decomp[i])
					{
					case '\r':
						decomp[i] = '\n';
						break;
					case '\003':
						rc = kill(child_pid, SIGINT);
						if (rc == -1)
							ERROR_AND_DIE("kill");
						break;
					case '\004':
						close(srv2bash[1]);
						stop_writing_to_bash = 1;
						break;
					}
				}

				if (!stop_writing_to_bash)
				{
					rc = write(srv2bash[1], decomp, decomp_len);
					if (rc == -1)
						ERROR_AND_DIE("pipe");				
				}				
				
			}
		}

		/* See if there's output from the child shell. */
		if (polls[1].revents & POLLIN)
		{
			buf_len = read(bash2srv[0], buf, BUFSZ - 1);
			if (buf_len == -1)
				ERROR_AND_DIE("read");
			
			/* Format input before sending it back to socket.
			   The recipient will be in non-canonical mode, so
			   CR or LF should map to CRLF, I think. */
			for (i = j = 0; i < buf_len; i++, j++)
			{
				switch (buf[i])
				{
				case '\n':
					__attribute__ ((fallthrough));
				case '\r':
					buf_fixed[j]   = '\r';
					buf_fixed[++j] = '\n';
					break;
				default:
					buf_fixed[j] = buf[i];
					break;
				}
			}

			buf_fixed[j] = '\0';

			if (!given_compress)
			{
				write(new_sock_fd, buf_fixed, j);				
			} else {
				char comp[CHUNK];
				int comp_len;

				comp_len = def(buf_fixed, j, comp, CHUNK);
				if (comp_len < 0)
					ERROR_AND_DIE("def");

				write(new_sock_fd, comp, comp_len);
			}
		}

		/* If the shell is dead and no more is being read. */
		if ((polls[1].revents & (POLLHUP | POLLERR)))
		{
			do {				
				rc = waitpid(child_pid, &wait_res, WNOHANG);
				if (rc == -1)
					ERROR_AND_DIE("waitpid");
			} while (rc != child_pid);
			if (!stop_writing_to_bash)
			{
				rc = close(srv2bash[1]);
				if (rc == -1)
					ERROR_AND_DIE("close");				
			}
			rc = close(bash2srv[0]);
			if (rc == -1)
				ERROR_AND_DIE("close");
			break;
		}

		/* Check if the client disconnected. */
		rc = recv(new_sock_fd, buf, BUFSZ, MSG_PEEK | MSG_DONTWAIT);
		if (rc == -1 && errno != EAGAIN)
			ERROR_AND_DIE("recv");
		if (rc == 0)
			break;
	}

	/* Write termination status to socket. */
	if (!given_compress)
	{
		dprintf(new_sock_fd, "SHELL EXIT SIGNAL=%d STATUS=%d\r\n",
			WTERMSIG(wait_res),
			WEXITSTATUS(wait_res));
	} else {
		char clear_text[BUFSZ];
		char zlibed[CHUNK];
		int clear_len, zlib_len;
		clear_len = snprintf(clear_text, BUFSZ - 1, "SHELL EXIT SIGNAL %d STATUS=%d\r\n",
				     WTERMSIG(wait_res),
				     WEXITSTATUS(wait_res));

		zlib_len = def(clear_text, clear_len, zlibed, CHUNK);
		if (zlib_len < 0)
			ERROR_AND_DIE("def");

		write(new_sock_fd, zlibed, zlib_len);
	}

	/* Per @301 on Piazza, print it out to server's console as well. */
	fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n",
		WTERMSIG(wait_res),
		WEXITSTATUS(wait_res));
	
	/* When we're done, close the socket so we can re-use the port. */
	rc = close(new_sock_fd);
	if (rc == -1)
		ERROR_AND_DIE("close");
	rc = close(sock_fd);
	if (rc == -1)
		ERROR_AND_DIE("close");
	
	_exit(0);
}
