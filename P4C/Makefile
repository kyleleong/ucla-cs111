# NAME:  Kyle Leong
# EMAIL: redacted@example.com
# ID:    123456789

default: lab4c_tcp lab4c_tls

lab4c_tcp: lab4c_tcp.c
	gcc -g -Wall -Wextra -o lab4c_tcp lab4c_tcp.c -lmraa -lm

lab4c_tls: lab4c_tls.c
	gcc -g -Wall -Wextra -o lab4c_tls lab4c_tls.c -lmraa -lm -lcrypto -lssl

clean:
	-rm lab4c_tcp lab4c_tls lab4c-123456789.tar.gz

dist: lab4c_tcp lab4c_tls
	tar czf lab4c-123456789.tar.gz \
	lab4c_tcp.c lab4c_tls.c Makefile README
