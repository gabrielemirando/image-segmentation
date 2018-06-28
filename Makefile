CC = gcc
CC_OMP = -fopenmp
CC_FLAGS = -Wall
GPROF = -pg -static-libgcc

all: serial.out serial_prof.out omp.out

clean:
	rm serial.out omp.out result.*

serial.out: src/main_serial.c src/image_io.h src/image_io.c src/segmentation.h src/segmentation_serial.c
	$(CC) $(CC_OMP) $(CC_FLAGS) -o serial.out src/main_serial.c src/image_io.c src/segmentation_serial.c -lm

serial_prof.out: src/main_serial.c src/image_io.h src/image_io.c src/segmentation.h src/segmentation_serial.c
	$(CC) $(GPROF) $(CC_OMP) $(CC_FLAGS) -o serial_prof.out src/main_serial.c src/image_io.c src/segmentation_serial.c -lm

omp.out: src/main_omp.c src/image_io.h src/image_io.c src/segmentation.h src/segmentation_omp.c
	$(CC) $(CC_OMP) $(CC_FLAGS) -o omp.out src/main_omp.c src/image_io.c src/segmentation_omp.c -lm
