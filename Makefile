CC = gcc
CFLAG = -Wall
LIBS = -fopenmp -lm

imgseg: main.o img_io.o kmeans.o utils.o
	${CC} ${CFLAG} ${LIBS} main.o img_io.o kmeans.o utils.o -o imgseg

main.o: src/main.c src/img_io.c src/img_io.h src/kmeans.c src/kmeans.h src/utils.c src/utils.h
	${CC} ${CFLAG} ${LIBS} -c src/main.c

img_io.o: src/img_io.c src/img_io.h src/utils.c src/utils.h
	${CC} ${CFLAG} ${LIBS} -c src/img_io.c

kmeans.o: src/img_io.c src/img_io.h src/kmeans.c src/kmeans.h src/utils.c src/utils.h
	${CC} ${CFLAG} ${LIBS} -c src/kmeans.c

utils.o: src/utils.c src/utils.h
	${CC} ${CFLAG} ${LIBS} -c src/utils.c

clean:
	-rm *.o imgseg
