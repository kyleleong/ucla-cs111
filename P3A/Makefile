# NAME:  Kyle Leong, Zachary Finkelstein
# EMAIL: redacted@example.com, redacted@example.com
# ID:    123456789, 123456789

TARBALL = lab3a-123456789.tar.gz
SOURCES = lab3a.c
HEADERS = ext2_fs.h

lab3a: $(SOURCES)
	gcc -g -Wall -Wextra -o lab3a $(SOURCES)

dist: lab3a
	tar czf $(TARBALL) $(SOURCES) $(HEADERS) Makefile README

clean:
	-rm lab3a $(TARBALL)
