# NAME:  Kyle Leong
# EMAIL: redacted@example.com
# ID:    123456789

TARCONTENTS = lab0.c Makefile backtrace.png breakpoint.png README

.PHONY: check clean dist

lab0: lab0.c
	gcc -g -Wall -Wextra lab0.c -o lab0

check: lab0
	@./lab0 badword > /dev/null 2>&1; \
		if [ "$$?" == "1" ]; then \
		echo "[PASS] Test 0"; else echo "[FAIL] Test 0"; fi;
	@./lab0 --badarg > /dev/null 2>&1; \
		if [ "$$?" == "1" ]; then \
		echo "[PASS] Test 1"; else echo "[FAIL] Test 1"; fi;
	@./lab0 --input=/etc/shadow > /dev/null 2>&1; \
		if [ "$$?" == "2" ]; then \
		echo "[PASS] Test 2"; else echo "[FAIL] Test 2"; fi;
	@./lab0 --output=/bin/sh > /dev/null 2>&1; \
		if [ "$$?" == "3" ]; then \
		echo "[PASS] Test 3"; else echo "[FAIL] Test 3"; fi;
	@./lab0 --segfault --catch > /dev/null 2>&1; \
		if [ "$$?" == "4" ]; then \
		echo "[PASS] Test 4"; else echo "[FAIL] Test 4"; fi;
	@./lab0 --input=/etc/shadow --segfault --catch > /dev/null 2>&1; \
		if [ "$$?" == "2" ]; then \
		echo "[PASS] Test 5"; else echo "[FAIL] Test 5"; fi;
	@OUTPUT=$$(echo "Pass Me!" | ./lab0); \
		if [ "$$OUTPUT" == "Pass Me!" ]; then \
		echo "[PASS] Test 6"; else echo "[FAIL] Test 6"; fi;
	@OUTPUT=$$(echo "Vibe check." > in.txt; ./lab0 --input=in.txt); \
		rm in.txt; \
		if [ "$$OUTPUT" == "Vibe check." ]; then \
		echo "[PASS] Test 7"; else echo "[FAIL] Test 7"; fi;
	@OUTPUT=$$(echo "Paul Eggert" | ./lab0 --output=out.txt; cat out.txt); \
		rm out.txt; \
		if [ "$$OUTPUT" == "Paul Eggert" ]; then \
		echo "[PASS] Test 8"; else echo "[FAIL] Test 8"; fi;
	@./lab0 --segfault > /dev/null 2>&1; \
		if [ "$$?" == "139" ]; then \
		echo "[PASS] Test 9"; else echo "[FAIL] Test 9"; fi;

clean:
	rm lab0 lab0-123456789.tar.gz

dist:
	tar czf lab0-123456789.tar.gz $(TARCONTENTS)
