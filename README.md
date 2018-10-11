# Color-based Segmentation
The objective of the project was to develop a C language program for color-based
image segmentation using the standard k-means clustering algorithm and to speed
up the execution by parallelizing the code using OpenMP.

## Repository
The repository of the project is organized in folders:
* **docs**: contains the report, results and presentation of the project
* **imgs**: contains the program test images
* **libs**: contains the external libraries used for the project
* **src**: contains the source code files of the program

## Compiling
The program can be compiled using the make command. Launching ```make all``` from 
command line will produce:
* **serial.out**: it's the serial version of the program
* **omp.out**: it's the parallel version implemented using OpenMP (requires 
OpenMP 4.5, supported from GCC 6.1)

## Usage
The detailed information about the program usage can be obtained by specifying
the -h flag, when launching the program from command line. Common usages are:

* ```./serial.out -k 4 imgs/test_s.jpg```: to execute the serial program with 
four clusters on the test image provided
 
* ```./omp.out -k 4 -t 4 imgs/test_s.jpg```: to execute the parallel program with 
four clusters using 4 CPU threads

## Credits
External help have been used for handling the I/O of images.

* stb_image (v2.19): to load the color values of the pixels of the images
* stb_image_write (v1.09): to create images from pixel values

These libraries are part of an amazing project that can be found
[here](https://github.com/nothings/stb).
