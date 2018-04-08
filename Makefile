CC = gcc

serial.out: main_serial.o kmeans_serial.o image_io.o
	${CC} -Wall -fopenmp -o serial.out main_serial.o kmeans_serial.o image_io.o -lm

main_serial.o: src/main_serial.c src/image_io.h src/image_io.c src/kmeans.h src/kmeans_serial.c
	${CC} -Wall -c src/main_serial.c

kmeans_serial.o: src/image_io.h src/kmeans.h src/kmeans_serial.c
	${CC} -Wall -c src/kmeans_serial.c

image_io.o: src/image_io.c src/image_io.h
	${CC} -Wall -c src/image_io.c

clean:
	-rm *.o serial.out
