/* NAME:  Kyle Leong
 * EMAIL: redacted@example.com
 * ID:    123456789           */

#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define UNUSED(arg) ((void) (arg))

static const struct option longOptions[] = {
	{ "input", required_argument, NULL, 'i' },
	{ "output", required_argument, NULL, 'o' },
	{ "segfault", no_argument, NULL, 's' },
	{ "catch", no_argument, NULL, 'c' },
	{ NULL, 0, NULL, 0 }
};

void sigsegv_handler(int signum)
{
	UNUSED(signum);
	fprintf(stderr, "Caught SIGSEGV because '--catch' was specified.\n");
	_exit(4);
}

void perform_segfault()
{
	char* segfaultme = NULL;
	*segfaultme = 42;
}

int main(int argc, char *argv[])
{
	int c, tempFd;
	int doInput = 0, doOutput = 0, doSegfault = 0, doCatch = 0;
	char *inputFile = NULL, *outputFile = NULL;
	char b;
	
	while (1) {
		c = getopt_long(argc, argv, "", longOptions, NULL);
		if (c < 0)
			break;

		switch (c) {
		case 'i':
			doInput = 1;
			inputFile = optarg;
			break;
		case 'o':
			doOutput = 1;
			outputFile = optarg;
			break;
		case 's':
			doSegfault = 1;
			break;
		case 'c':
			doCatch = 1;
			break;
		case '?':
			fprintf(stderr, "Usage: %s [--input=INFILE] "
				"[--output=OUTFILE] [--segfault] "
				"[--catch]\n", argv[0]);
			_exit(1);
			break;
		}
	}

	if (optind < argc) {
		fprintf(stderr, "Usage: %s [--input=INFILE] [--output=OUTFILE] "
			"[--segfault] [--catch]\n", argv[0]);
		_exit(1);
	}

	if (doInput)
	{
		tempFd = open(inputFile, O_RDONLY);
		if (tempFd < 0)
		{
			fprintf(stderr, "%s: open err on '--input=%s': %s\n",
				argv[0], inputFile, strerror(errno));
			_exit(2);
		}

		close(STDIN_FILENO);
		dup(tempFd);
		close(tempFd);		
	}

	if (doOutput)
	{
		tempFd = creat(outputFile, 0666);
		if (tempFd < 0)
		{
			fprintf(stderr, "%s: creat err on '--output=%s': %s\n",
				argv[0], outputFile, strerror(errno));
			_exit(3);
		}

		close(STDOUT_FILENO);
		dup(tempFd);
		close(tempFd);
	}

	if (doCatch)
		signal(SIGSEGV, sigsegv_handler);

	if (doSegfault)
		perform_segfault();

        while (1)
	{
		c = read(STDIN_FILENO, &b, 1);
		if (c == 0)
			break;
		if (c < 0)
		{
			fprintf(stderr, "%s: read err on '--input=%s': %s\n",
				argv[0], inputFile, strerror(errno));
			_exit(2);
		}

		c = write(STDOUT_FILENO, &b, 1);
		if (c != 1)
		{
			fprintf(stderr, "%s: write err on '--output=%s': %s\n",
				argv[0], outputFile, strerror(errno));
			_exit(3);
		}
	}
	
	_exit(0);
}
