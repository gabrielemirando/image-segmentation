CC = gcc
CFLAGS = -Wall
OMPFLAG = -fopenmp

all: serial.out omp.out

clean:
	rm serial.out omp.out

serial.out: src/main_serial.c src/image_io.h src/image_io.c src/kmeans.h src/kmeans_serial.c
	$(CC) $(CFLAGS) $(OMPFLAG) -o serial.out src/main_serial.c src/image_io.c src/kmeans_serial.c -lm

omp.out: src/main_omp.c src/image_io.h src/image_io.c src/kmeans.h src/kmeans_omp.c
	$(CC) $(CFLAGS) $(OMPFLAG) -o omp.out src/main_omp.c src/image_io.c src/kmeans_omp.c -lm
