# NAME:  Kyle Leong
# EMAIL: redacted@example.com
# ID:    123456789

IMAGES = lab2_add-1.png lab2_add-2.png lab2_add-3.png \
	 lab2_add-4.png lab2_add-5.png lab2_list-1.png \
	 lab2_list-2.png lab2_list-3.png lab2_list-4.png
GRAPHS = lab2_add.csv lab2_list.csv
SOURCE = lab2_add.c lab2_list.c SortedList.h SortedList.c

.PHONY: build tests graphs dist clean

build: lab2_add lab2_list

lab2_add: lab2_add.c
	gcc -g -Wall -Wextra -o lab2_add lab2_add.c -lpthread

lab2_list: lab2_list.c
	gcc -g -Wall -Wextra -o lab2_list lab2_list.c SortedList.c \
		SortedList.h -lpthread

clean:
	-rm lab2_add lab2_list lab2a-123456789.tar.gz

dist: graphs
	tar czf lab2a-123456789.tar.gz $(SOURCE) $(IMAGES) $(GRAPHS) \
		Makefile README lab2_add.gp lab2_list.gp

graphs: tests
	gnuplot lab2_add.gp
	gnuplot lab2_list.gp

tests: clean build
	-rm $(GRAPHS)
	./lab2_add --threads=1  --iterations=10     >> lab2_add.csv
	./lab2_add --threads=1  --iterations=100    >> lab2_add.csv
	./lab2_add --threads=1  --iterations=1000   >> lab2_add.csv
	./lab2_add --threads=1  --iterations=10000  >> lab2_add.csv
	./lab2_add --threads=1  --iterations=100000 >> lab2_add.csv
	./lab2_add --threads=2  --iterations=100    >> lab2_add.csv
	./lab2_add --threads=2  --iterations=1000   >> lab2_add.csv
	./lab2_add --threads=2  --iterations=10000  >> lab2_add.csv
	./lab2_add --threads=2  --iterations=100000 >> lab2_add.csv
	./lab2_add --threads=4  --iterations=100    >> lab2_add.csv
	./lab2_add --threads=4  --iterations=1000   >> lab2_add.csv
	./lab2_add --threads=4  --iterations=10000  >> lab2_add.csv
	./lab2_add --threads=4  --iterations=100000 >> lab2_add.csv
	./lab2_add --threads=8  --iterations=100    >> lab2_add.csv
	./lab2_add --threads=8  --iterations=1000   >> lab2_add.csv
	./lab2_add --threads=8  --iterations=10000  >> lab2_add.csv
	./lab2_add --threads=8  --iterations=100000 >> lab2_add.csv
	./lab2_add --threads=16 --iterations=100    >> lab2_add.csv
	./lab2_add --threads=16 --iterations=1000   >> lab2_add.csv
	./lab2_add --threads=16 --iterations=10000  >> lab2_add.csv
	./lab2_add --threads=16 --iterations=100000 >> lab2_add.csv

	./lab2_add --threads=1  --iterations=10     --yield >> lab2_add.csv
	./lab2_add --threads=1  --iterations=20     --yield >> lab2_add.csv
	./lab2_add --threads=1  --iterations=40     --yield >> lab2_add.csv
	./lab2_add --threads=1  --iterations=80     --yield >> lab2_add.csv
	./lab2_add --threads=1  --iterations=100    --yield >> lab2_add.csv
	./lab2_add --threads=1  --iterations=1000   --yield >> lab2_add.csv
	./lab2_add --threads=1  --iterations=10000  --yield >> lab2_add.csv
	./lab2_add --threads=1  --iterations=100000 --yield >> lab2_add.csv
	./lab2_add --threads=2  --iterations=10     --yield >> lab2_add.csv
	./lab2_add --threads=2  --iterations=20     --yield >> lab2_add.csv
	./lab2_add --threads=2  --iterations=40     --yield >> lab2_add.csv
	./lab2_add --threads=2  --iterations=80     --yield >> lab2_add.csv
	./lab2_add --threads=2  --iterations=100    --yield >> lab2_add.csv
	./lab2_add --threads=2  --iterations=1000   --yield >> lab2_add.csv
	./lab2_add --threads=2  --iterations=10000  --yield >> lab2_add.csv
	./lab2_add --threads=2  --iterations=100000 --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=10     --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=20     --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=40     --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=80     --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=100    --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=1000   --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=10000  --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=100000 --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=10     --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=20     --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=40     --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=80     --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=100    --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=1000   --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=10000  --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=100000 --yield >> lab2_add.csv
	./lab2_add --threads=12 --iterations=10     --yield >> lab2_add.csv
	./lab2_add --threads=12 --iterations=20     --yield >> lab2_add.csv
	./lab2_add --threads=12 --iterations=40     --yield >> lab2_add.csv
	./lab2_add --threads=12 --iterations=80     --yield >> lab2_add.csv
	./lab2_add --threads=12 --iterations=100    --yield >> lab2_add.csv
	./lab2_add --threads=12 --iterations=1000   --yield >> lab2_add.csv
	./lab2_add --threads=12 --iterations=10000  --yield >> lab2_add.csv
	./lab2_add --threads=12 --iterations=100000 --yield >> lab2_add.csv

	./lab2_add --threads=2  --iterations=10000 --sync=m --yield >> lab2_add.csv
	./lab2_add --threads=2  --iterations=10000 --sync=c --yield >> lab2_add.csv
	./lab2_add --threads=2  --iterations=1000  --sync=s --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=10000 --sync=m --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=10000 --sync=c --yield >> lab2_add.csv
	./lab2_add --threads=4  --iterations=1000  --sync=s --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=10000 --sync=m --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=10000 --sync=c --yield >> lab2_add.csv
	./lab2_add --threads=8  --iterations=1000  --sync=s --yield >> lab2_add.csv
	./lab2_add --threads=12 --iterations=10000 --sync=m --yield >> lab2_add.csv
	./lab2_add --threads=12 --iterations=10000 --sync=c --yield >> lab2_add.csv
	./lab2_add --threads=12 --iterations=1000  --sync=s --yield >> lab2_add.csv

	./lab2_add --threads=1  --iterations=10000 --sync=m >> lab2_add.csv
	./lab2_add --threads=1  --iterations=10000 --sync=c >> lab2_add.csv
	./lab2_add --threads=1  --iterations=10000 --sync=s >> lab2_add.csv
	./lab2_add --threads=2  --iterations=10000 --sync=m >> lab2_add.csv
	./lab2_add --threads=2  --iterations=10000 --sync=c >> lab2_add.csv
	./lab2_add --threads=2  --iterations=10000 --sync=s >> lab2_add.csv
	./lab2_add --threads=4  --iterations=10000 --sync=m >> lab2_add.csv
	./lab2_add --threads=4  --iterations=10000 --sync=c >> lab2_add.csv
	./lab2_add --threads=4  --iterations=10000 --sync=s >> lab2_add.csv
	./lab2_add --threads=8  --iterations=10000 --sync=m >> lab2_add.csv
	./lab2_add --threads=8  --iterations=10000 --sync=c >> lab2_add.csv
	./lab2_add --threads=8  --iterations=10000 --sync=s >> lab2_add.csv
	./lab2_add --threads=12 --iterations=10000 --sync=m >> lab2_add.csv
	./lab2_add --threads=12 --iterations=10000 --sync=c >> lab2_add.csv
	./lab2_add --threads=12 --iterations=10000 --sync=s >> lab2_add.csv

	./lab2_list --threads=1 --iterations=10    >> lab2_list.csv
	./lab2_list --threads=1 --iterations=100   >> lab2_list.csv
	./lab2_list --threads=1 --iterations=1000  >> lab2_list.csv
	./lab2_list --threads=1 --iterations=10000 >> lab2_list.csv
	./lab2_list --threads=1 --iterations=20000 >> lab2_list.csv

	-./lab2_list --threads=2  --iterations=1    >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=10   >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=100  >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=1000 >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=1    >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=10   >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=100  >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=1000 >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=1    >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=10   >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=100  >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=1000 >> lab2_list.csv
	-./lab2_list --threads=16 --iterations=1    >> lab2_list.csv
	-./lab2_list --threads=16 --iterations=10   >> lab2_list.csv
	-./lab2_list --threads=16 --iterations=100  >> lab2_list.csv
	-./lab2_list --threads=16 --iterations=1000 >> lab2_list.csv

	-./lab2_list --threads=2  --iterations=1  --yield=i >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=2  --yield=i >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=4  --yield=i >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=8  --yield=i >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=16 --yield=i >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=32 --yield=i >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=1  --yield=i >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=2  --yield=i >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=4  --yield=i >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=8  --yield=i >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=16 --yield=i >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=32 --yield=i >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=1  --yield=i >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=2  --yield=i >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=4  --yield=i >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=8  --yield=i >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=16 --yield=i >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=32 --yield=i >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=1  --yield=i >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=2  --yield=i >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=4  --yield=i >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=8  --yield=i >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=16 --yield=i >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=32 --yield=i >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=1  --yield=d >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=2  --yield=d >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=4  --yield=d >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=8  --yield=d >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=16 --yield=d >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=32 --yield=d >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=1  --yield=d >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=2  --yield=d >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=4  --yield=d >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=8  --yield=d >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=16 --yield=d >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=32 --yield=d >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=1  --yield=d >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=2  --yield=d >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=4  --yield=d >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=8  --yield=d >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=16 --yield=d >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=32 --yield=d >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=1  --yield=d >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=2  --yield=d >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=4  --yield=d >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=8  --yield=d >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=16 --yield=d >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=32 --yield=d >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=1  --yield=il >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=2  --yield=il >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=4  --yield=il >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=8  --yield=il >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=16 --yield=il >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=32 --yield=il >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=1  --yield=il >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=2  --yield=il >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=4  --yield=il >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=8  --yield=il >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=16 --yield=il >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=32 --yield=il >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=1  --yield=il >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=2  --yield=il >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=4  --yield=il >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=8  --yield=il >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=16 --yield=il >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=32 --yield=il >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=1  --yield=il >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=2  --yield=il >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=4  --yield=il >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=8  --yield=il >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=16 --yield=il >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=32 --yield=il >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=1  --yield=dl >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=2  --yield=dl >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=4  --yield=dl >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=8  --yield=dl >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=16 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=32 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=1  --yield=dl >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=2  --yield=dl >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=4  --yield=dl >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=8  --yield=dl >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=16 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=32 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=1  --yield=dl >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=2  --yield=dl >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=4  --yield=dl >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=8  --yield=dl >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=16 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=32 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=1  --yield=dl >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=2  --yield=dl >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=4  --yield=dl >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=8  --yield=dl >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=16 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=32 --yield=dl >> lab2_list.csv

	./lab2_list --threads=2  --iterations=1  --yield=i  --sync=m >> lab2_list.csv
	./lab2_list --threads=2  --iterations=2  --yield=i  --sync=m >> lab2_list.csv
	./lab2_list --threads=2  --iterations=4  --yield=i  --sync=m >> lab2_list.csv
	./lab2_list --threads=2  --iterations=8  --yield=i  --sync=m >> lab2_list.csv
	./lab2_list --threads=2  --iterations=16 --yield=i  --sync=m >> lab2_list.csv
	./lab2_list --threads=2  --iterations=32 --yield=i  --sync=m >> lab2_list.csv
	./lab2_list --threads=4  --iterations=1  --yield=i  --sync=m >> lab2_list.csv
	./lab2_list --threads=4  --iterations=2  --yield=i  --sync=m >> lab2_list.csv
	./lab2_list --threads=4  --iterations=4  --yield=i  --sync=m >> lab2_list.csv
	./lab2_list --threads=4  --iterations=8  --yield=i  --sync=m >> lab2_list.csv
	./lab2_list --threads=4  --iterations=16 --yield=i  --sync=m >> lab2_list.csv
	./lab2_list --threads=4  --iterations=32 --yield=i  --sync=m >> lab2_list.csv
	./lab2_list --threads=8  --iterations=1  --yield=i  --sync=m >> lab2_list.csv
	./lab2_list --threads=8  --iterations=2  --yield=i  --sync=m >> lab2_list.csv
	./lab2_list --threads=8  --iterations=4  --yield=i  --sync=m >> lab2_list.csv
	./lab2_list --threads=8  --iterations=8  --yield=i  --sync=m >> lab2_list.csv
	./lab2_list --threads=8  --iterations=16 --yield=i  --sync=m >> lab2_list.csv
	./lab2_list --threads=8  --iterations=32 --yield=i  --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=1  --yield=i  --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=2  --yield=i  --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=4  --yield=i  --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=8  --yield=i  --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=16 --yield=i  --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=i  --sync=m >> lab2_list.csv
	./lab2_list --threads=2  --iterations=1  --yield=d  --sync=m >> lab2_list.csv
	./lab2_list --threads=2  --iterations=2  --yield=d  --sync=m >> lab2_list.csv
	./lab2_list --threads=2  --iterations=4  --yield=d  --sync=m >> lab2_list.csv
	./lab2_list --threads=2  --iterations=8  --yield=d  --sync=m >> lab2_list.csv
	./lab2_list --threads=2  --iterations=16 --yield=d  --sync=m >> lab2_list.csv
	./lab2_list --threads=2  --iterations=32 --yield=d  --sync=m >> lab2_list.csv
	./lab2_list --threads=4  --iterations=1  --yield=d  --sync=m >> lab2_list.csv
	./lab2_list --threads=4  --iterations=2  --yield=d  --sync=m >> lab2_list.csv
	./lab2_list --threads=4  --iterations=4  --yield=d  --sync=m >> lab2_list.csv
	./lab2_list --threads=4  --iterations=8  --yield=d  --sync=m >> lab2_list.csv
	./lab2_list --threads=4  --iterations=16 --yield=d  --sync=m >> lab2_list.csv
	./lab2_list --threads=4  --iterations=32 --yield=d  --sync=m >> lab2_list.csv
	./lab2_list --threads=8  --iterations=1  --yield=d  --sync=m >> lab2_list.csv
	./lab2_list --threads=8  --iterations=2  --yield=d  --sync=m >> lab2_list.csv
	./lab2_list --threads=8  --iterations=4  --yield=d  --sync=m >> lab2_list.csv
	./lab2_list --threads=8  --iterations=8  --yield=d  --sync=m >> lab2_list.csv
	./lab2_list --threads=8  --iterations=16 --yield=d  --sync=m >> lab2_list.csv
	./lab2_list --threads=8  --iterations=32 --yield=d  --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=1  --yield=d  --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=2  --yield=d  --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=4  --yield=d  --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=8  --yield=d  --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=16 --yield=d  --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=d  --sync=m >> lab2_list.csv
	./lab2_list --threads=2  --iterations=1  --yield=il --sync=m >> lab2_list.csv
	./lab2_list --threads=2  --iterations=2  --yield=il --sync=m >> lab2_list.csv
	./lab2_list --threads=2  --iterations=4  --yield=il --sync=m >> lab2_list.csv
	./lab2_list --threads=2  --iterations=8  --yield=il --sync=m >> lab2_list.csv
	./lab2_list --threads=2  --iterations=16 --yield=il --sync=m >> lab2_list.csv
	./lab2_list --threads=2  --iterations=32 --yield=il --sync=m >> lab2_list.csv
	./lab2_list --threads=4  --iterations=1  --yield=il --sync=m >> lab2_list.csv
	./lab2_list --threads=4  --iterations=2  --yield=il --sync=m >> lab2_list.csv
	./lab2_list --threads=4  --iterations=4  --yield=il --sync=m >> lab2_list.csv
	./lab2_list --threads=4  --iterations=8  --yield=il --sync=m >> lab2_list.csv
	./lab2_list --threads=4  --iterations=16 --yield=il --sync=m >> lab2_list.csv
	./lab2_list --threads=4  --iterations=32 --yield=il --sync=m >> lab2_list.csv
	./lab2_list --threads=8  --iterations=1  --yield=il --sync=m >> lab2_list.csv
	./lab2_list --threads=8  --iterations=2  --yield=il --sync=m >> lab2_list.csv
	./lab2_list --threads=8  --iterations=4  --yield=il --sync=m >> lab2_list.csv
	./lab2_list --threads=8  --iterations=8  --yield=il --sync=m >> lab2_list.csv
	./lab2_list --threads=8  --iterations=16 --yield=il --sync=m >> lab2_list.csv
	./lab2_list --threads=8  --iterations=32 --yield=il --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=1  --yield=il --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=2  --yield=il --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=4  --yield=il --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=8  --yield=il --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=16 --yield=il --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=il --sync=m >> lab2_list.csv
	./lab2_list --threads=2  --iterations=1  --yield=dl --sync=m >> lab2_list.csv
	./lab2_list --threads=2  --iterations=2  --yield=dl --sync=m >> lab2_list.csv
	./lab2_list --threads=2  --iterations=4  --yield=dl --sync=m >> lab2_list.csv
	./lab2_list --threads=2  --iterations=8  --yield=dl --sync=m >> lab2_list.csv
	./lab2_list --threads=2  --iterations=16 --yield=dl --sync=m >> lab2_list.csv
	./lab2_list --threads=2  --iterations=32 --yield=dl --sync=m >> lab2_list.csv
	./lab2_list --threads=4  --iterations=1  --yield=dl --sync=m >> lab2_list.csv
	./lab2_list --threads=4  --iterations=2  --yield=dl --sync=m >> lab2_list.csv
	./lab2_list --threads=4  --iterations=4  --yield=dl --sync=m >> lab2_list.csv
	./lab2_list --threads=4  --iterations=8  --yield=dl --sync=m >> lab2_list.csv
	./lab2_list --threads=4  --iterations=16 --yield=dl --sync=m >> lab2_list.csv
	./lab2_list --threads=4  --iterations=32 --yield=dl --sync=m >> lab2_list.csv
	./lab2_list --threads=8  --iterations=1  --yield=dl --sync=m >> lab2_list.csv
	./lab2_list --threads=8  --iterations=2  --yield=dl --sync=m >> lab2_list.csv
	./lab2_list --threads=8  --iterations=4  --yield=dl --sync=m >> lab2_list.csv
	./lab2_list --threads=8  --iterations=8  --yield=dl --sync=m >> lab2_list.csv
	./lab2_list --threads=8  --iterations=16 --yield=dl --sync=m >> lab2_list.csv
	./lab2_list --threads=8  --iterations=32 --yield=dl --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=1  --yield=dl --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=2  --yield=dl --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=4  --yield=dl --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=8  --yield=dl --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=16 --yield=dl --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=dl --sync=m >> lab2_list.csv

	./lab2_list --threads=2  --iterations=1  --yield=i  --sync=s >> lab2_list.csv
	./lab2_list --threads=2  --iterations=2  --yield=i  --sync=s >> lab2_list.csv
	./lab2_list --threads=2  --iterations=4  --yield=i  --sync=s >> lab2_list.csv
	./lab2_list --threads=2  --iterations=8  --yield=i  --sync=s >> lab2_list.csv
	./lab2_list --threads=2  --iterations=16 --yield=i  --sync=s >> lab2_list.csv
	./lab2_list --threads=2  --iterations=32 --yield=i  --sync=s >> lab2_list.csv
	./lab2_list --threads=4  --iterations=1  --yield=i  --sync=s >> lab2_list.csv
	./lab2_list --threads=4  --iterations=2  --yield=i  --sync=s >> lab2_list.csv
	./lab2_list --threads=4  --iterations=4  --yield=i  --sync=s >> lab2_list.csv
	./lab2_list --threads=4  --iterations=8  --yield=i  --sync=s >> lab2_list.csv
	./lab2_list --threads=4  --iterations=16 --yield=i  --sync=s >> lab2_list.csv
	./lab2_list --threads=4  --iterations=32 --yield=i  --sync=s >> lab2_list.csv
	./lab2_list --threads=8  --iterations=1  --yield=i  --sync=s >> lab2_list.csv
	./lab2_list --threads=8  --iterations=2  --yield=i  --sync=s >> lab2_list.csv
	./lab2_list --threads=8  --iterations=4  --yield=i  --sync=s >> lab2_list.csv
	./lab2_list --threads=8  --iterations=8  --yield=i  --sync=s >> lab2_list.csv
	./lab2_list --threads=8  --iterations=16 --yield=i  --sync=s >> lab2_list.csv
	./lab2_list --threads=8  --iterations=32 --yield=i  --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=1  --yield=i  --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=2  --yield=i  --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=4  --yield=i  --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=8  --yield=i  --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=16 --yield=i  --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=i  --sync=s >> lab2_list.csv
	./lab2_list --threads=2  --iterations=1  --yield=d  --sync=s >> lab2_list.csv
	./lab2_list --threads=2  --iterations=2  --yield=d  --sync=s >> lab2_list.csv
	./lab2_list --threads=2  --iterations=4  --yield=d  --sync=s >> lab2_list.csv
	./lab2_list --threads=2  --iterations=8  --yield=d  --sync=s >> lab2_list.csv
	./lab2_list --threads=2  --iterations=16 --yield=d  --sync=s >> lab2_list.csv
	./lab2_list --threads=2  --iterations=32 --yield=d  --sync=s >> lab2_list.csv
	./lab2_list --threads=4  --iterations=1  --yield=d  --sync=s >> lab2_list.csv
	./lab2_list --threads=4  --iterations=2  --yield=d  --sync=s >> lab2_list.csv
	./lab2_list --threads=4  --iterations=4  --yield=d  --sync=s >> lab2_list.csv
	./lab2_list --threads=4  --iterations=8  --yield=d  --sync=s >> lab2_list.csv
	./lab2_list --threads=4  --iterations=16 --yield=d  --sync=s >> lab2_list.csv
	./lab2_list --threads=4  --iterations=32 --yield=d  --sync=s >> lab2_list.csv
	./lab2_list --threads=8  --iterations=1  --yield=d  --sync=s >> lab2_list.csv
	./lab2_list --threads=8  --iterations=2  --yield=d  --sync=s >> lab2_list.csv
	./lab2_list --threads=8  --iterations=4  --yield=d  --sync=s >> lab2_list.csv
	./lab2_list --threads=8  --iterations=8  --yield=d  --sync=s >> lab2_list.csv
	./lab2_list --threads=8  --iterations=16 --yield=d  --sync=s >> lab2_list.csv
	./lab2_list --threads=8  --iterations=32 --yield=d  --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=1  --yield=d  --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=2  --yield=d  --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=4  --yield=d  --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=8  --yield=d  --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=16 --yield=d  --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=d  --sync=s >> lab2_list.csv
	./lab2_list --threads=2  --iterations=1  --yield=il --sync=s >> lab2_list.csv
	./lab2_list --threads=2  --iterations=2  --yield=il --sync=s >> lab2_list.csv
	./lab2_list --threads=2  --iterations=4  --yield=il --sync=s >> lab2_list.csv
	./lab2_list --threads=2  --iterations=8  --yield=il --sync=s >> lab2_list.csv
	./lab2_list --threads=2  --iterations=16 --yield=il --sync=s >> lab2_list.csv
	./lab2_list --threads=2  --iterations=32 --yield=il --sync=s >> lab2_list.csv
	./lab2_list --threads=4  --iterations=1  --yield=il --sync=s >> lab2_list.csv
	./lab2_list --threads=4  --iterations=2  --yield=il --sync=s >> lab2_list.csv
	./lab2_list --threads=4  --iterations=4  --yield=il --sync=s >> lab2_list.csv
	./lab2_list --threads=4  --iterations=8  --yield=il --sync=s >> lab2_list.csv
	./lab2_list --threads=4  --iterations=16 --yield=il --sync=s >> lab2_list.csv
	./lab2_list --threads=4  --iterations=32 --yield=il --sync=s >> lab2_list.csv
	./lab2_list --threads=8  --iterations=1  --yield=il --sync=s >> lab2_list.csv
	./lab2_list --threads=8  --iterations=2  --yield=il --sync=s >> lab2_list.csv
	./lab2_list --threads=8  --iterations=4  --yield=il --sync=s >> lab2_list.csv
	./lab2_list --threads=8  --iterations=8  --yield=il --sync=s >> lab2_list.csv
	./lab2_list --threads=8  --iterations=16 --yield=il --sync=s >> lab2_list.csv
	./lab2_list --threads=8  --iterations=32 --yield=il --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=1  --yield=il --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=2  --yield=il --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=4  --yield=il --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=8  --yield=il --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=16 --yield=il --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=il --sync=s >> lab2_list.csv
	./lab2_list --threads=2  --iterations=1  --yield=dl --sync=s >> lab2_list.csv
	./lab2_list --threads=2  --iterations=2  --yield=dl --sync=s >> lab2_list.csv
	./lab2_list --threads=2  --iterations=4  --yield=dl --sync=s >> lab2_list.csv
	./lab2_list --threads=2  --iterations=8  --yield=dl --sync=s >> lab2_list.csv
	./lab2_list --threads=2  --iterations=16 --yield=dl --sync=s >> lab2_list.csv
	./lab2_list --threads=2  --iterations=32 --yield=dl --sync=s >> lab2_list.csv
	./lab2_list --threads=4  --iterations=1  --yield=dl --sync=s >> lab2_list.csv
	./lab2_list --threads=4  --iterations=2  --yield=dl --sync=s >> lab2_list.csv
	./lab2_list --threads=4  --iterations=4  --yield=dl --sync=s >> lab2_list.csv
	./lab2_list --threads=4  --iterations=8  --yield=dl --sync=s >> lab2_list.csv
	./lab2_list --threads=4  --iterations=16 --yield=dl --sync=s >> lab2_list.csv
	./lab2_list --threads=4  --iterations=32 --yield=dl --sync=s >> lab2_list.csv
	./lab2_list --threads=8  --iterations=1  --yield=dl --sync=s >> lab2_list.csv
	./lab2_list --threads=8  --iterations=2  --yield=dl --sync=s >> lab2_list.csv
	./lab2_list --threads=8  --iterations=4  --yield=dl --sync=s >> lab2_list.csv
	./lab2_list --threads=8  --iterations=8  --yield=dl --sync=s >> lab2_list.csv
	./lab2_list --threads=8  --iterations=16 --yield=dl --sync=s >> lab2_list.csv
	./lab2_list --threads=8  --iterations=32 --yield=dl --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=1  --yield=dl --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=2  --yield=dl --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=4  --yield=dl --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=8  --yield=dl --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=16 --yield=dl --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=dl --sync=s >> lab2_list.csv

	./lab2_list --threads=1  --iterations=1000 --sync=m >> lab2_list.csv
	./lab2_list --threads=2  --iterations=1000 --sync=m >> lab2_list.csv
	./lab2_list --threads=4  --iterations=1000 --sync=m >> lab2_list.csv
	./lab2_list --threads=8  --iterations=1000 --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=m >> lab2_list.csv
	./lab2_list --threads=16 --iterations=1000 --sync=m >> lab2_list.csv
	./lab2_list --threads=24 --iterations=1000 --sync=m >> lab2_list.csv

	./lab2_list --threads=1  --iterations=1000 --sync=s >> lab2_list.csv
	./lab2_list --threads=2  --iterations=1000 --sync=s >> lab2_list.csv
	./lab2_list --threads=4  --iterations=1000 --sync=s >> lab2_list.csv
	./lab2_list --threads=8  --iterations=1000 --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=s >> lab2_list.csv
	./lab2_list --threads=16 --iterations=1000 --sync=s >> lab2_list.csv
	./lab2_list --threads=24 --iterations=1000 --sync=s >> lab2_list.csv
