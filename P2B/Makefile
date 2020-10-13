# NAME:  Kyle Leong
# EMAIL: redacted@example.com
# ID:    123456789

IMAGES = lab2b_1.png lab2b_2.png lab2b_3.png lab2b_4.png lab2b_5.png
SOURCE = lab2_list.c SortedList.h SortedList.c

.PHONY: tests profile graphs dist clean

lab2_list: lab2_list.c
	gcc -lprofiler -g -Wall -Wextra -o lab2_list $(SOURCE) -lpthread

profile: clean lab2_list
	env CPUPROFILE=profile.out.bin ./lab2_list --iterations=1000 --threads=12 --sync=s
	pprof --text ./lab2_list ./profile.out.bin > profile.out
	pprof --list=thread ./lab2_list ./profile.out.bin >> profile.out
	rm profile.out.bin

clean:
	-rm lab2_list lab2b-123456789.tar.gz

dist: graphs profile
	tar czf lab2b-123456789.tar.gz $(IMAGES) $(SOURCE) \
		Makefile README lab2b_list.csv profile.out lab2_list.gp

graphs: tests
	gnuplot lab2_list.gp

tests: clean lab2_list
	./lab2_list --iterations=1000 --threads=1  --sync=m >  lab2b_list.csv
	./lab2_list --iterations=1000 --threads=2  --sync=m >> lab2b_list.csv
	./lab2_list --iterations=1000 --threads=4  --sync=m >> lab2b_list.csv
	./lab2_list --iterations=1000 --threads=8  --sync=m >> lab2b_list.csv
	./lab2_list --iterations=1000 --threads=12 --sync=m >> lab2b_list.csv
	./lab2_list --iterations=1000 --threads=16 --sync=m >> lab2b_list.csv
	./lab2_list --iterations=1000 --threads=24 --sync=m >> lab2b_list.csv

	./lab2_list --iterations=1000 --threads=1  --sync=s >> lab2b_list.csv
	./lab2_list --iterations=1000 --threads=2  --sync=s >> lab2b_list.csv
	./lab2_list --iterations=1000 --threads=4  --sync=s >> lab2b_list.csv
	./lab2_list --iterations=1000 --threads=8  --sync=s >> lab2b_list.csv
	./lab2_list --iterations=1000 --threads=12 --sync=s >> lab2b_list.csv
	./lab2_list --iterations=1000 --threads=16 --sync=s >> lab2b_list.csv
	./lab2_list --iterations=1000 --threads=24 --sync=s >> lab2b_list.csv

	-./lab2_list --yield=id --lists=4 --threads=1  --iterations=1  >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=1  --iterations=2  >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=1  --iterations=4  >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=1  --iterations=8  >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=1  --iterations=16 >> lab2b_list.csv

	-./lab2_list --yield=id --lists=4 --threads=2  --iterations=1  >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=2  --iterations=2  >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=2  --iterations=4  >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=2  --iterations=8  >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=2  --iterations=16 >> lab2b_list.csv

	-./lab2_list --yield=id --lists=4 --threads=4  --iterations=1  >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=4  --iterations=2  >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=4  --iterations=4  >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=4  --iterations=8  >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=4  --iterations=16 >> lab2b_list.csv

	-./lab2_list --yield=id --lists=4 --threads=8  --iterations=1  >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=8  --iterations=2  >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=8  --iterations=4  >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=8  --iterations=8  >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=8  --iterations=16 >> lab2b_list.csv

	-./lab2_list --yield=id --lists=4 --threads=12 --iterations=1  >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=12 --iterations=2  >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=12 --iterations=4  >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=12 --iterations=8  >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=12 --iterations=16 >> lab2b_list.csv

	-./lab2_list --yield=id --lists=4 --threads=16 --iterations=1  >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=16 --iterations=2  >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=16 --iterations=4  >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=16 --iterations=8  >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=16 --iterations=16 >> lab2b_list.csv

	./lab2_list --yield=id --lists=4 --threads=1  --iterations=10 --sync=s >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=2  --iterations=10 --sync=s >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=4  --iterations=10 --sync=s >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=8  --iterations=10 --sync=s >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=12 --iterations=10 --sync=s >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=16 --iterations=10 --sync=s >> lab2b_list.csv

	./lab2_list --yield=id --lists=4 --threads=1  --iterations=20 --sync=s >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=2  --iterations=20 --sync=s >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=4  --iterations=20 --sync=s >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=8  --iterations=20 --sync=s >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=12 --iterations=20 --sync=s >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=16 --iterations=20 --sync=s >> lab2b_list.csv

	./lab2_list --yield=id --lists=4 --threads=1  --iterations=40 --sync=s >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=2  --iterations=40 --sync=s >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=4  --iterations=40 --sync=s >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=8  --iterations=40 --sync=s >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=12 --iterations=40 --sync=s >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=16 --iterations=40 --sync=s >> lab2b_list.csv

	./lab2_list --yield=id --lists=4 --threads=1  --iterations=80 --sync=s >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=2  --iterations=80 --sync=s >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=4  --iterations=80 --sync=s >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=8  --iterations=80 --sync=s >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=12 --iterations=80 --sync=s >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=16 --iterations=80 --sync=s >> lab2b_list.csv

	./lab2_list --yield=id --lists=4 --threads=1  --iterations=10 --sync=m >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=2  --iterations=10 --sync=m >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=4  --iterations=10 --sync=m >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=8  --iterations=10 --sync=m >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=12 --iterations=10 --sync=m >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=16 --iterations=10 --sync=m >> lab2b_list.csv

	./lab2_list --yield=id --lists=4 --threads=1  --iterations=20 --sync=m >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=2  --iterations=20 --sync=m >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=4  --iterations=20 --sync=m >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=8  --iterations=20 --sync=m >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=12 --iterations=20 --sync=m >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=16 --iterations=20 --sync=m >> lab2b_list.csv

	./lab2_list --yield=id --lists=4 --threads=1  --iterations=40 --sync=m >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=2  --iterations=40 --sync=m >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=4  --iterations=40 --sync=m >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=8  --iterations=40 --sync=m >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=12 --iterations=40 --sync=m >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=16 --iterations=40 --sync=m >> lab2b_list.csv

	./lab2_list --yield=id --lists=4 --threads=1  --iterations=80 --sync=m >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=2  --iterations=80 --sync=m >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=4  --iterations=80 --sync=m >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=8  --iterations=80 --sync=m >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=12 --iterations=80 --sync=m >> lab2b_list.csv
	./lab2_list --yield=id --lists=4 --threads=16 --iterations=80 --sync=m >> lab2b_list.csv

	./lab2_list --iterations=1000 --lists=4  --threads=1  --sync=m >> lab2b_list.csv
	./lab2_list --iterations=1000 --lists=4  --threads=2  --sync=m >> lab2b_list.csv
	./lab2_list --iterations=1000 --lists=4  --threads=4  --sync=m >> lab2b_list.csv
	./lab2_list --iterations=1000 --lists=4  --threads=8  --sync=m >> lab2b_list.csv
	./lab2_list --iterations=1000 --lists=4  --threads=12 --sync=m >> lab2b_list.csv

	./lab2_list --iterations=1000 --lists=8  --threads=1  --sync=m >> lab2b_list.csv
	./lab2_list --iterations=1000 --lists=8  --threads=2  --sync=m >> lab2b_list.csv
	./lab2_list --iterations=1000 --lists=8  --threads=4  --sync=m >> lab2b_list.csv
	./lab2_list --iterations=1000 --lists=8  --threads=8  --sync=m >> lab2b_list.csv
	./lab2_list --iterations=1000 --lists=8  --threads=12 --sync=m >> lab2b_list.csv

	./lab2_list --iterations=1000 --lists=16 --threads=1  --sync=m >> lab2b_list.csv
	./lab2_list --iterations=1000 --lists=16 --threads=2  --sync=m >> lab2b_list.csv
	./lab2_list --iterations=1000 --lists=16 --threads=4  --sync=m >> lab2b_list.csv
	./lab2_list --iterations=1000 --lists=16 --threads=8  --sync=m >> lab2b_list.csv
	./lab2_list --iterations=1000 --lists=16 --threads=12 --sync=m >> lab2b_list.csv

	./lab2_list --iterations=1000 --lists=4  --threads=1  --sync=s >> lab2b_list.csv
	./lab2_list --iterations=1000 --lists=4  --threads=2  --sync=s >> lab2b_list.csv
	./lab2_list --iterations=1000 --lists=4  --threads=4  --sync=s >> lab2b_list.csv
	./lab2_list --iterations=1000 --lists=4  --threads=8  --sync=s >> lab2b_list.csv
	./lab2_list --iterations=1000 --lists=4  --threads=12 --sync=s >> lab2b_list.csv

	./lab2_list --iterations=1000 --lists=8  --threads=1  --sync=s >> lab2b_list.csv
	./lab2_list --iterations=1000 --lists=8  --threads=2  --sync=s >> lab2b_list.csv
	./lab2_list --iterations=1000 --lists=8  --threads=4  --sync=s >> lab2b_list.csv
	./lab2_list --iterations=1000 --lists=8  --threads=8  --sync=s >> lab2b_list.csv
	./lab2_list --iterations=1000 --lists=8  --threads=12 --sync=s >> lab2b_list.csv

	./lab2_list --iterations=1000 --lists=16 --threads=1  --sync=s >> lab2b_list.csv
	./lab2_list --iterations=1000 --lists=16 --threads=2  --sync=s >> lab2b_list.csv
	./lab2_list --iterations=1000 --lists=16 --threads=4  --sync=s >> lab2b_list.csv
	./lab2_list --iterations=1000 --lists=16 --threads=8  --sync=s >> lab2b_list.csv
	./lab2_list --iterations=1000 --lists=16 --threads=12 --sync=s >> lab2b_list.csv
