CC = gcc
CC_FLAGS = -Wall
CC_OMP = -fopenmp

NVCC = nvcc
NVCC_FLAGS = -Wno-deprecated-gpu-targets

all: serial.out omp.out cuda.out

clean:
	rm serial.out omp.out cuda.out result.jpg

serial.out: src/main_serial.c src/image_io.h src/image_io.c src/segmentation.h src/segmentation_serial.c
	$(CC) $(CC_FLAGS) -o serial.out src/main_serial.c src/image_io.c src/segmentation_serial.c -lm

omp.out: src/main_omp.c src/image_io.h src/image_io.c src/segmentation.h src/segmentation_omp.c
	$(CC) $(CC_FLAGS) $(CC_OMP) -o omp.out src/main_omp.c src/image_io.c src/segmentation_omp.c -lm

cuda.out: src/main_cuda.cu src/image_io.h src/image_io.c src/segmentation.h src/segmentation_cuda.cu
	$(NVCC) $(NVCC_FLAGS) -o cuda.out src/main_cuda.cu src/image_io.c src/segmentation_cuda.cu -lm
