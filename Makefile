CC = gcc
CFLAGS = -Wall -c
OMPFLAG = -fopenmp

all: serial omp

clean:
	-rm *.o serial.out omp.out


serial: main_serial.o kmeans_serial.o image_io.o
	$(CC) $(OMPFLAG) -o serial.out main_serial.o kmeans_serial.o image_io.o -lm

main_serial.o: src/main_serial.c src/image_io.h src/image_io.c src/kmeans.h src/kmeans_serial.c
	$(CC) $(CFLAGS) src/main_serial.c 

kmeans_serial.o: src/image_io.h src/kmeans.h src/kmeans_serial.c
	$(CC) $(CFLAGS) src/kmeans_serial.c

image_io.o: src/image_io.c src/image_io.h
	$(CC) $(CFLAGS) src/image_io.c


omp: main_omp.o kmeans_omp.o image_io.o
	$(CC) $(OMPFLAG) -o omp.out main_omp.o kmeans_omp.o image_io.o -lm

main_omp.o: src/main_omp.c src/image_io.h src/image_io.c src/kmeans.h src/kmeans_omp.c
	$(CC) $(CFLAGS) src/main_omp.c

kmeans_omp.o: src/image_io.h src/kmeans.h src/kmeans_omp.c
	$(CC) $(CFLAGS) $(OMPFLAG) src/kmeans_omp.c
