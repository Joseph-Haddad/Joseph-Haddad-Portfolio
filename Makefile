
all: controller.c overseer.c
	gcc -o controller controller.c -Wall
	gcc -o overseer overseer.c -pthread -Wall


clean:
	rm -f controller *.o
	rm -f overseer *.o

.PHONY: all clean