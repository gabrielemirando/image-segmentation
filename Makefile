CC = gcc
CFLAG = -Wall
LIBS = -fopenmp -lm

imgseg: main.o img_io.o kmeans.o utils.o
	${CC} ${CFLAG} main.o img_io.o kmeans.o utils.o -o imgseg ${LIBS}

main.o: src/main.c src/img_io.c src/img_io.h src/kmeans.c src/kmeans.h src/utils.c src/utils.h
	${CC} ${CFLAG} -c src/main.c ${LIBS}

img_io.o: src/img_io.c src/img_io.h src/utils.c src/utils.h
	${CC} ${CFLAG} -c src/img_io.c ${LIBS}

kmeans.o: src/img_io.c src/img_io.h src/kmeans.c src/kmeans.h src/utils.c src/utils.h
	${CC} ${CFLAG} -c src/kmeans.c ${LIBS}

utils.o: src/utils.c src/utils.h
	${CC} ${CFLAG} -c src/utils.c ${LIBS}

clean:
	-rm *.o imgseg
