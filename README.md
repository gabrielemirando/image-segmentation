# Image segmentation
The objective of the project was to develop a C language program for color-based 
image segmentation using the standard k-means clustering algorithm and to speed 
up the execution by paralleling the code using OpenMP. 

### Compiling
The program can be easily compiled using the make command. Launching  
```
make all 
```
from command line will produce three executables:
* **serial.out**: it's the serial version of the program
* **serial_gprof.out**: will produce profiling infos about the serial code using gprof
* **omp.out**: it's the parallel version implemented using OpenMP

The parallel version requires OpenMP 4.5, which is fully supported starting from
GCC 6.1.

All the infos about the program usage can be obtained by specifying the -h flag 
when launching from command line.

### Credits
External help have been used for handling the I/O of images.

* stb_image (v2.19): to load the color values of the pixels of the images.
* stb_image_write (v1.09): to create images from pixel values.

These libraries are part of an amazing project that can be found 
[here](https://github.com/nothings/stb).
