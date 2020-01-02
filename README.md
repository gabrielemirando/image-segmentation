# Image segmentation

The objective of this project is to develop a program for color-based image
segmentation using the standard k-means clustering algorithm and to speed up the
execution by parallelizing the code using OpenMP.

## Getting Started

These instructions will get you a copy of the project up and running on your
local machine.

### Prerequisites

The compilation of the parallel version of the program requires OpenMP 4.5,
which is supported starting from GCC 6.1.

### Installing

The project can be compiled using the make command. Launching ```make all```
will produce:

* *serial.out*: the serial version of the program.

* *omp.out*: the parallel version implemented using OpenMP.

Some examples of usage are:

* ```./serial.out -k 4 imgs/test_s.jpg```: to execute the serial program with
  four clusters on a test image.

* ```./omp.out -k 4 -t 4 imgs/test_s.jpg```: to execute the parallel version
  with four clusters using four CPU threads.

## License

This project is [UNLICENSED](UNLICENSE).

## Acknowledgments

External help have been used for handling the I/O of images:

* *stb_image(v2.19)*: to load the color values of the pixels of the input
  images.

* *stb_image_write(v1.09)*: to create images from pixel values.

These libraries are part of an amazing project that can be found
[here](https://github.com/nothings/stb).
