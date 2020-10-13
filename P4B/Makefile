# NAME:  Kyle Leong
# EMAIL: redacted@example.com
# ID:    123456789

lab4b: lab4b.c
	gcc -g -Wall -Wextra -o lab4b lab4b.c -lmraa -lm

check: lab4b
	@printf "Testing bad long options... "; \
		./lab4b --bogus > /dev/null 2>&1; \
		if [ "$$?" = "1" ]; then \
		echo "OK"; else echo "ERROR"; fi;
	@printf "Testing extraneous options... "; \
		./lab4b extraneous > /dev/null 2>&1; \
		if [ "$$?" = "1" ]; then \
		echo "OK"; else echo "ERROR"; fi;
	@RC=$$(echo "OFF" | ./lab4b | grep -c "SHUTDOWN"); \
		printf "Testing OFF command... "; \
		if [ "$$RC" = "1" ]; then \
		echo "OK"; else echo "ERROR"; fi;
	@echo "OFF" | ./lab4b --log=tmp > /dev/null 2>&1; \
		RC=$$(grep -P '[0-9]{2}:[0-9]{2}:[0-9]{2}' './tmp' | wc -l); \
		printf "Testing creation of log... "; \
		if [ -e "./tmp" ]; then \
		echo "OK"; else echo "ERROR"; fi; \
		printf "Testing proper log format... "; \
		if [ "$$RC" = "2" ]; then \
		echo "OK"; else echo "ERROR"; fi; \
		printf "Testing proper log length... "; \
		RC=$$(wc -l './tmp'); \
		if [ "$$RC" = "3 ./tmp" ]; then \
		echo "OK"; else echo "ERROR"; fi; \
		rm tmp;

clean:
	-rm lab4b lab4b-123456789.tar.gz

dist: lab4b
	tar czf lab4b-123456789.tar.gz lab4b.c Makefile README
