# NAME:  Kyle Leong
# EMAIL: redacted@example.com
# ID:    123456789

.PHONY: check clean dist

lab1a: lab1a.c
	gcc -g -Wall -Wextra lab1a.c -o lab1a

clean:
	rm lab1a lab1a-123456789.tar.gz

dist:
	tar czf lab1a-123456789.tar.gz lab1a.c Makefile README
