/* NAME:  Kyle Leong
 * EMAIL: redacted@example.com
 * ID:    123456789           */

#include <errno.h>
#include <getopt.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>

#define BUFSZ 256

#define ERROR_AND_DIE(syscall) do { \
	fprintf(stderr, "%s: " syscall " error (line %d): %s\r\n", \
	argv[0], __LINE__, strerror(errno)); _exit(1); } while (0)

int main(int argc, char* argv[])
{
	struct termios origSettings, newSettings;
	struct pollfd polls[2];
	char buf[BUFSZ], bufFixed[BUFSZ * 2];
	int doShell = 0, rc, bufLen, bufFixedLen, i, j;
	int endRead = 0, shouldDie = 0;
	int lab2bash[2], bash2lab[2], childPID = 0, waitStatus = 0;
	
	static const struct option longOptions[] = {
		{ "shell", no_argument, NULL, 's' },
		{ NULL, 0, NULL, 0 }
	};

	while (1)
	{
		rc = getopt_long(argc, argv, "", longOptions, NULL);
		if (rc < 0)
			break;

		switch (rc)
		{
		case 's':
			doShell = 1;
			break;
		case '?':
			fprintf(stderr, "Usage: %s [--shell]\n", argv[0]);
			_exit(1);
			break;
		}
	}

	/* Handle unknown arguments. */
	if (optind < argc)
	{
		fprintf(stderr, "Usage: %s [--shell]\n", argv[0]);
		_exit(1);
	}

	/* Save original terminal settings, and set new ones. */
	rc = tcgetattr(STDIN_FILENO, &origSettings);
	if (rc == -1)
		ERROR_AND_DIE("tcgetattr");
	newSettings.c_iflag = ISTRIP;
	newSettings.c_oflag = 0;
	newSettings.c_lflag = 0;
	tcsetattr(STDIN_FILENO, TCSANOW, &newSettings);
	if (rc == -1)
		ERROR_AND_DIE("tcsetattr");

	/* Set up a pipe in each direction. */
	rc = pipe(lab2bash);
	if (rc == -1)
		ERROR_AND_DIE("pipe");
	rc = pipe(bash2lab);
	if (rc == -1)
		ERROR_AND_DIE("pipe");
	
	/* Set up poll structures for keyboard and shell output. */
	polls[0].events = polls[1].events = POLLIN;
	polls[0].fd = STDIN_FILENO;
	polls[1].fd = bash2lab[0];
	
	if (doShell)
	{
		/* Fork the process, close unused pipes. */
		childPID = fork();
		if (childPID < 0)
		{
			ERROR_AND_DIE("fork");
		} else if (childPID == 0) {
			/* Child's execution path.
			   Close parent to child write pipe.
			   Close child to parent read pipe. */
			close(lab2bash[1]);
			close(bash2lab[0]);

			/* Make stdin a pipe from the terminal process. */
			close(STDIN_FILENO);
			dup(lab2bash[0]);

			/* Make stdout and stderr a pipe to the term. proc. */
			close(STDOUT_FILENO);
			dup(bash2lab[1]);
			close(STDERR_FILENO);
			dup(bash2lab[1]);

			close(lab2bash[0]);
			close(lab2bash[1]);

			rc = execl("/bin/bash", "/bin/bash", NULL);
			// rc = execlp("./sigint", "./sigint", NULL);
			if (rc == -1)
				ERROR_AND_DIE("execlp");
		} else {
			/* Parent's execution path.
			   Close parent to child read pipe.
			   Close child to parent write pipe. */
			close(lab2bash[0]);
			close(bash2lab[1]);
		}
	}

	while (1)
	{
		/* Call poll for two items in list of FDs, with no timeout. */
		rc = poll(polls, 2, 0);
		if (rc == -1)
			ERROR_AND_DIE("poll");

		i = j = 0;
		bufLen = bufFixedLen = 0;
		if (polls[0].revents & POLLIN)
		{
			bufLen = bufFixedLen = read(STDIN_FILENO, &buf, BUFSZ);
		}
		if (bufLen == -1)
			ERROR_AND_DIE("read");

		/* Originally, butFixedLength is at same offset of bufLen.
		 * Handle input typed into the keyboard.                   */
		for (; i < bufLen; i++, j++)
		{
			switch (buf[i])
			{
			case '\r':
				/* CR or LF should map to shell as LF. */				
				if (doShell)
					buf[i] = '\n';
				__attribute__ ((fallthrough));
			case '\n':
				bufFixed[j]   = '\r';
				bufFixed[++j] = '\n';
				break;
			case '\003': /* Handle ^C */
				if (doShell)
				{
					write(STDOUT_FILENO, "^C", 2);
					rc = kill(childPID, SIGINT);
					if (rc == -1)
						ERROR_AND_DIE("kill");
					// endRead = 1;
					shouldDie = 1;
				}
				bufFixed[j] = buf[i];
				break;
			case '\004': /* Handle ^D.*/
				write(STDOUT_FILENO, "^D", 2);
				endRead = 1;
				break;
			default:
				bufFixed[j] = buf[i];
			}

			if (endRead || shouldDie)
				break;
		}

		/* If we have a bash shell, sanitize input and send to it. */
		if (doShell)
		{
			rc = poll(&polls[1], 1, 0);
			if (rc == -1)
				ERROR_AND_DIE("poll");
			/* If the pipe is closed, don't write to it. */
			if (!(polls[1].revents & (POLLERR | POLLHUP)) && !endRead)
			{
				rc = write(lab2bash[1], buf, bufLen);
				if (rc == -1)
					ERROR_AND_DIE("write");
			}		       
		}

		/* Write the modified output back to the terminal screen. */
		rc = write(STDOUT_FILENO, bufFixed, j);
		if (rc == -1)
			ERROR_AND_DIE("write");

		/* Get output of the bash shell and write it to terminal. */
		if (doShell)
		{
			i = j = 0;
			bufLen = bufFixedLen = 0;
			if (polls[1].revents & POLLIN)
				bufLen = bufFixedLen = read(bash2lab[0], buf, BUFSZ);
			if (bufLen == -1)
				ERROR_AND_DIE("read");

			for (; i < bufLen; i++, j++)
			{
				switch (buf[i])
				{
				case '\n':
					bufFixed[j]   = '\r';
					bufFixed[++j] = '\n';
					break;
				default:
					bufFixed[j] = buf[i];
				}
			}

			rc = write(STDOUT_FILENO, bufFixed, j);
			if (rc == -1)
				ERROR_AND_DIE("write");
		}

		/* Check if the bash shell exited without blocking. */
		if (doShell)
		{
			rc = waitpid(childPID, &waitStatus, WNOHANG);
			if (rc == -1)
				ERROR_AND_DIE("waitpid");
			else if (rc == childPID)
			{
				close(lab2bash[1]);
				close(bash2lab[0]);
				endRead = 1;
			}
		}
		
		if (endRead)
			break;
	}

	/* Restore original terminal settings. */
	rc = tcsetattr(STDIN_FILENO, TCSANOW, &origSettings);
        if (rc == -1)
		ERROR_AND_DIE("tcsetattr");

	if (doShell)
		fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n",
			/* According to kernel source, these are equivalent
			   to what was asked for in the spec.               */
			(WTERMSIG(waitStatus)),
			(WEXITSTATUS(waitStatus)));

	_exit(0);
}
