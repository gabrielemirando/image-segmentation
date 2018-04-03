# image-segmentation
The goal of the project is to achieve image segmentation using the standard 
kmeans clustering algorithm and to speed up the execution exploiting different 
multithreading programming paradigms.

### Usage 
The program can be compiled using gcc and including the OpenMP flag.
```
gcc -fopenmp src/main.c src/img_io.c src/kmeans.c src/utils.c -o img_seg
```

All the infos about the usage can be obtained by launching the
program from command line specifing the -h parameter.
```
./img_seg -h
```

### Credits
External help have been used for handling the i/o of images.

* stb_image (v2.19): to load the pixel values of the images.
* stb_image_write (v1.09): to create images from pixel values.

These libraries are part of an amazing project that
can be found [here](https://github.com/nothings/stb).
