# NAME:  Kyle Leong
# EMAIL: redacted@example.com
# ID:    123456789

.PHONY: all clean dist

all: lab1b-client lab1b-server

lab1b-client: lab1b-client.c
	gcc -g -Wall -Wextra -o lab1b-client $^ -lz

lab1b-server: lab1b-server.c
	gcc -g -Wall -Wextra -o lab1b-server $^ -lz

clean:
	rm lab1b-123456789.tar.gz lab1b-client lab1b-server

dist:
	tar czf lab1b-123456789.tar.gz lab1b-client.c lab1b-server.c Makefile README
