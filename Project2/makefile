all: syn_phistogram syn_thistogram

syn_phistogram: syn_phistogram.c
	gcc -g -Wall syn_phistogram.c -o syn_phistogram -lrt -pthread

syn_thistogram: syn_thistogram.c
	gcc -g -Wall -o syn_thistogram syn_thistogram.c -lrt -pthread
