/*
 * File: image_io.h
 * --------------------
 * Headers file containing the declarations of the functions defined in image_io.c
 */

#ifndef IMAGE_IO_H
#define IMAGE_IO_H

/*
 * Type Definition: byte_t
 * --------------------
 * The intensity of each color channel (red, green, blue or gray) of a pixel
 * is a value from 0 to 255. One byte of memory is sufficient to store it and
 * that's why "unsigned char" type is used. For convenience "unsigned char"
 * is renamed to "byte_t".
 */
typedef unsigned char byte_t;

byte_t *img_load(char *img_file, int *width, int *height, int *n_channels);
void img_save(char *img_file, byte_t *data, int width, int height, int n_channels);

#endif
