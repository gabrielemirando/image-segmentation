CC = gcc
CC_OMP = -fopenmp
CC_FLAGS = -Wall

all: serial.out omp.out

clean:
	rm serial.out serial_gprof.out omp.out result.jpg

serial.out: src/main_serial.c src/image_io.h src/image_io.c src/segmentation.h src/segmentation_serial.c
	$(CC) $(CC_OMP) $(CC_FLAGS) -o serial.out src/main_serial.c src/image_io.c src/segmentation_serial.c -lm

omp.out: src/main_omp.c src/image_io.h src/image_io.c src/segmentation.h src/segmentation_omp.c
	$(CC) $(CC_OMP) $(CC_FLAGS) -o omp.out src/main_omp.c src/image_io.c src/segmentation_omp.c -lm
