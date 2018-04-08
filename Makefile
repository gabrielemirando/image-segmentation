CC = gcc

# SERIAL VERSION

serial.out: main_serial.o kmeans_serial.o image_io.o
	${CC} -Wall -fopenmp -o serial.out main_serial.o kmeans_serial.o image_io.o -lm

main_serial.o: src/main_serial.c src/image_io.h src/image_io.c src/kmeans.h src/kmeans_serial.c
	${CC} -Wall -c src/main_serial.c

kmeans_serial.o: src/image_io.h src/kmeans.h src/kmeans_serial.c
	${CC} -Wall -c src/kmeans_serial.c

image_io.o: src/image_io.c src/image_io.h
	${CC} -Wall -c src/image_io.c

# OMP VERSION

omp.out: main_omp.o kmeans_omp.o image_io.o
	${CC} -Wall -fopenmp -o omp.out main_omp.o kmeans_omp.o image_io.o -lm

main_omp.o: src/main_omp.c src/image_io.h src/image_io.c src/kmeans.h src/kmeans_omp.c
	${CC} -Wall -c src/main_omp.c

kmeans_omp.o: src/image_io.h src/kmeans.h src/kmeans_omp.c
	${CC} -Wall -fopenmp -c src/kmeans_omp.c

# CLEAN

clean:
	-rm *.o serial.out omp.out
